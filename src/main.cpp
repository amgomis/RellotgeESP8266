//
//  Rellotge HH:MM per ESP8266
//  Permet AHT10 per Humitat i Temperatura
//  Permet sortida OLED I2C
//  Permet sortida 7 segments ws2812
//  Permet sortida matriu 4 mòduls 8x8 de ws2812
//  Permet WebServer que mostra hora
//  
//  Primera versió 10/02/2025   (poden caldre algunes millores)
//  
//
// Definició d'opcions de compilació
//#define AHT       // Sensor AHT10
#define DEBUG     // Mostra per port sèrie
//#define OLED      // Displai PLED I2C
//#define LEDS7S    // Displai 7 segments configurable
#define LEDSMTX   // Displai matriu leds 
#define WEBSRV    // Mostra en web
//
//
//

#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
// ESP8266 WebServer
#ifdef WEBSRV
  #include <ESP8266WebServer.h>
#endif
#include <WiFiUdp.h>
// Rellotge RTC I2C DS3231
#include <RTClib.h>
// Sensor temperatura i humitat AHT10
#ifdef AHT
  #include <AHT10.h>
#endif
// Displai OLED SSD1306
#ifdef OLED
  #include <Adafruit_GFX.h>       // Gràfics per OLED
  #include <Adafruit_SSD1306.h>   // Driver OLED
#endif
// Control zona horària
#include <ezTime.h>

// Inclou la pàgina web
#ifdef WEBSRV
  #include "index.h"
#endif
// Ús de neopixels
#if defined(LEDS7S) || defined(LEDSMTX)
  #include <Adafruit_NeoPixel.h>
#endif

#ifdef LEDS7S
  // Característiques dels Digits
  #define num_led 2    // Nombre de leds per segment
  #define num_seg 7    // Nombre de segments per dígit
  #define num_dis 4    // Nombre de dígits

  // Defineix ordre dels segments
  #define  a   2
  #define  b   4
  #define  c   8
  #define  d  16
  #define  e  32
  #define  f   1
  #define  g  64
  
  // Pin de sortida Leds
  #define PIN 12
  
  // Leds extra per punts i monitoratge
  #define extraLED 8

// Nombre total de Leds
int nleds = (num_seg * num_led * num_dis) + extraLED;

// Espai per dígits, pel rellotge n'utilitzem 4
int dig_rell[10];  // si s'utilitzen més de 10 dígits cal augmentar

// Confecció dels dígits 0123456789-ºCHu
int dseg[20] = {a+b+c+d+e+f, b+c, a+b+d+e+g, a+b+c+d+g, b+c+f+g, a+c+d+f+g,
                a+c+d+e+f+g, a+b+c, a+b+c+d+e+f+g, a+b+c+d+f+g, g, a+b+f+g,
                a+f+e+d, b+c+e+f+g, c+d+e, 0, 0, 0, 0, 0 };

// valor inicial per defecte dels colors
byte vermell    =  20;
byte verd       =  20;
byte blau       = 120;

// Funcionament neopixels, definició de l'objecte
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(nleds, PIN, NEO_GRB + NEO_KHZ800);
#endif

// Definició de variables de temperatura i humitat, aqui per ser reconegudes
float temperature, humidity;

// Inclou funcions de led mtx
#ifdef LEDSMTX
  #include "displaimtx.h"
#endif

// Inclou funcions manegament 7 segments
#ifdef LEDS7S
  #include "displai7s.h"
#endif

// Credencials WiFi
const char* ssid = "ADAMO-xxxx";
const char* password = "xxxxxxxxxx";

// Definició configuració de OLED 
#ifdef OLED
  // Característiques displai OLED
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET    -1
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

// Creació objecte RTC DS3231
RTC_DS3231 rtc;

// Creació objecte WebServer
#ifdef WEBSRV
  ESP8266WebServer server(80);
#endif

// Per displaiar o no temperatura i humitat
bool sensorTH = false;
#ifdef AHT
// Sensor AHT10
AHT10 aht10;
sensorTH = true;
#endif

// Configuració de la zona horària
Timezone myTZ;

// Estat del sistema
bool wifiConnected  = false;
bool rtcAvailable   = false;
bool aht10Available = false;
bool dconnect       = false;
bool inter          = false;

// Variables de temps
int currentHour   = 0;
int currentMinute = 0;
int currentSecond = 0;
String formattedTime = "00:00:00";
bool esHivern = true;  // Indica si és horari d'hivern (true) o d'estiu (false)

int cnt_led;
int dcol;
int tmp1 = 0;

// Funcions visualització Hora/minuts, temperatura i humitat 
#ifdef LEDSMTX
void hominmtx(){
  int hora = currentHour;
  int minuts = currentMinute;

  int hh = hora / 10;
  int hl = hora % 10;
  int mh = minuts / 10;
  int ml = minuts % 10;

  horamingfx(hh, hl, mh, ml, inter);
  pixelIndex = 0;
}

void mtxtemp(){
      hh = int(temperature) / 10;
      hl = int(temperature) % 10;
      tempgfx(hh, hl);
      pixelIndex = 0;
}

void mtxhum(){
      hh = int(humidity) / 10;
      hl = int(humidity) % 10;
      humitgfx(hh, hl);
      pixelIndex = 0;
}
#endif

// Funcions displai set segments
#ifdef LEDS7S
void s7homin(){
  int hora = currentHour;
  int minuts = currentMinute;

  dig_rell[0] = hora / 10;
  dig_rell[1] = hora % 10;
  dig_rell[2] = minuts / 10;
  dig_rell[3] = minuts % 10;
}

void s7temp(){
  dig_rell[0] = int(temperature) / 10;
  dig_rell[1] = int(temperature) % 10;
  dig_rell[2] = 11;
  dig_rell[3] = 12;
}

void s7hum(){
  dig_rell[0] = int(humidity) / 10;
  dig_rell[1] = int(humidity) % 10;
  dig_rell[2] = 13;
  dig_rell[3] = 14;
}
#endif

// Rutines envia dades displai OLED
#ifdef OLED
void envdisplai(){
  display.setTextSize(4);

  int hora = currentHour;
  int minuts = currentMinute;

  display.clearDisplay();
  display.setCursor(0, 0);
  
  // Mostrar les hores amb dos dígits
  if (hora < 10) {
    display.print("0");
  }
  display.print(hora);
  
  // Mostrar els dos punts
  if (inter == 1) display.print(":"); 
  else display.print(" ");
  
  // Mostrar els minuts amb dos dígits
  if (minuts < 10) {
    display.print("0");
  }
  display.print(minuts);

  display.setTextSize(2);
  display.setCursor(0, 32);
  display.print("Tem:");
  display.print(temperature, 1);
  display.print((char)247); // Símbol de grau
  display.print("C");
  display.setCursor(0, 50);
  display.print("Hum: ");
  display.print(int(humidity));
 
  if (inter == 1) display.print("%"); 
  else display.print("% .");

  display.display();
}
#endif


// Rutina que envia dades als displais
void displais() {

#ifdef OLED
  envdisplai();
#endif

#ifdef LEDS7S
  s7homin(); // cal temporitzar correctament les sortides
  #ifdef AHT
    s7temp();
    s7hum();
  #endif
#endif

#ifdef LEDSMTX
if (sensorTH == true) {
  if (tmp1 < 40) {
    if (tmp1 == 0) clrgfx();
      hominmtx(); // cal temporitzar les sortides
  }
  #ifdef AHT
  if (tmp1 >= 40 && tmp1 < 50) {
    if (tmp1 == 40) {clrgfx();}
      mtxtemp();
  }
  if (tmp1 >= 50 && tmp1 < 60) {
    if (tmp1 == 50) {clrgfx();}
      mtxhum();
  }
  #endif
} else {
  if (tmp1 < 60) {
      hominmtx(); // cal temporitzar les sortides
      }
    }
#endif
}

// Connectar-se a la WiFi
void connectToWiFi() {
    Serial.print("Connectant a la WiFi");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();
    // Si en 30 segons no connecta hauria d'agafar els valors de RTC
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
        Serial.print(".");
        delay(500);
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" Connected!");
        wifiConnected = true;
    } else {
        Serial.println(" Failed!");
        wifiConnected = false;
    }
}

// Rutines visualització WEB   (Cal una revisió/actualització)
#ifdef WEBSRV
void handleData() {
    int zh;
    if (esHivern) zh = 1; else zh = 2;
    formattedTime = (currentHour   < 10 ? "0" : "") + String(currentHour) + ":" + 
                    (currentMinute < 10 ? "0" : "") + String(currentMinute);
    String json = "{";
    json += "\"time\":\"" + formattedTime + "\",";
    json += "\"temperature\":" + String(temperature, 1) + ",";
    json += "\"humidity\":" + String(humidity, 1) + ",";
    json += "\"timezone\":" + String(zh) ; // Añade un valor de timezone
    json += "}";
    server.send(200, "application/json", json);
}

void handleData1() {
    formattedTime = (currentHour   < 10 ? "0" : "") + String(currentHour) + ":" + 
                    (currentMinute < 10 ? "0" : "") + String(currentMinute);

    String json = "{";
    json += "\"time\":\"" + formattedTime + "\",";
    json += "\"temperature\":" + String(temperature, 1) + ",";
    json += "\"humidity\":" + String(humidity, 1) + ",";
    json += "}";
    server.send(200, "application/json", json);
}

void handleTimezone() {
    server.send(200, "text/plain", "Fus horari actualitzat");
}
#endif

void syncTimeWithNTP() {
    if (wifiConnected) {
        waitForSync();
        myTZ.setLocation("Europe/Madrid");  // Utilitzem Madrid per a l'hora espanyola
        Serial.println("Hora sincronitzada amb NTP.");

        // Obtenim l'hora local ajustada (tenint en compte l'horari d'estiu/hivern)
        time_t localTime = myTZ.now();

        // Actualitzem el RTC amb l'hora local correcta
        rtc.adjust(DateTime(year(localTime), month(localTime), day(localTime),
                            hour(localTime), minute(localTime), second(localTime)));

        // Comprovem si és horari d'hivern o d'estiu
        esHivern = !myTZ.isDST();
        Serial.print("Actualment estem en horari de ");
        Serial.println(esHivern ? "HIVERN" : "ESTIU");

        // Imprimim l'hora per verificar
        Serial.print("Hora actualitzada: ");
        Serial.println(myTZ.dateTime("Y-m-d H:i:s"));
    } else {
        Serial.println("No es pot sincronitzar amb NTP (WiFi no disponible).");
    }
}

// Llegir l'hora actual del RTC
void readTime() {
    DateTime now = rtc.now();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentSecond = now.second();

    char buffer[15];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", currentHour, currentMinute, currentSecond);
    formattedTime = String(buffer);
}

#ifdef AHT
// Llegir dades del sensor AHT10
void readAHT10Data(float& temperature, float& humidity) {
    if (aht10Available) {
        temperature = aht10.readTemperature();
        humidity = aht10.readHumidity();
    } else {
        temperature = NAN;
        humidity = NAN;
    }
}
#endif

// Setup
void setup() {
    Serial.begin(115200);
    Wire.begin();

#ifdef LEDSMTX
  pixels.begin();
  pixels.setBrightness(brillantor); // Set BRIGHTNESS to about 1/5 (max = 255)
  for (int i = 0; i < nleds; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
  delay(200);
#endif

#ifdef OLED
    dconnect = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // No funciona massa bé
    if (!dconnect) {
      Serial.println("Display OLED no detectat");
    }
#endif

// Connecta a la WiFi
    connectToWiFi();

// Inicialitza RTC
    if (rtc.begin()) {
        rtcAvailable = true;
        if (rtc.lostPower()) {
            Serial.println("RTC sense energia, ajustant l'hora.");
            // Inicialitza amb la data i hora de compilació si no hi ha connexió WiFi
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }
    } else {
        Serial.println("RTC no detectat!");
        rtcAvailable = false;
    }

#ifdef OLED
    display.setRotation(2);    // 180º
    display.clearDisplay();
    display.setTextSize(2);    // Text mida 2
    display.setTextColor(WHITE);
    display.display();
#endif

#ifdef AHT
    // Inicialitza AHT10
    delay(100);
    if (aht10.begin()) {
        aht10Available = true;
    } else {
        Serial.println("AHT10 no detectat!");
        aht10Available = false;
    }
#endif

// Sincronitza hora amb NTP si hi ha connexió WiFi
    if (wifiConnected) {
        syncTimeWithNTP();
    }

#ifdef WEBSRV
    // Configura el servidor
    server.on("/", []() {
        server.send_P(200, "text/html", webpage);
    });
    server.on("/data",     handleData);
    server.on("/timezone", handleTimezone);
    server.begin();
    Serial.println("Servidor web inicialitzat");
#endif
}

// Loop
void loop() {
#ifdef WEBSRV
    server.handleClient();
#endif
// Comprova la connexió WiFi
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconnectat, intentant reconnectar...");
        connectToWiFi();
    }

// Actualitza l'hora amb NTP cada dia si hi ha connexió WiFi
    static unsigned long lastNTPUpdate = 0;
    if (wifiConnected && (millis() - lastNTPUpdate >= 86400000)) {
        syncTimeWithNTP();
        lastNTPUpdate = millis();
    }

// Actualitza l'hora i les dades del sensor cada 2000 ms
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 1000) {
      tmp1++;
      if (tmp1 >= 60) tmp1 = 0;
        readTime();

#ifdef AHT
        readAHT10Data(temperature, humidity);
#endif

#ifdef DEBUG
        Serial.printf("Hora: %s | ", formattedTime.c_str());
        Serial.print("Horari: ");
        Serial.println(esHivern ? "HIVERN" : "ESTIU");
#endif
        
#ifdef AHT
        if (!isnan(temperature) && !isnan(humidity)) {
  #ifdef DEBUG
            Serial.printf("Temperatura: %.2f°C, Humitat: %.2f%%\n", temperature, humidity);
  #endif
        } else {
  #ifdef DEBUG
            Serial.println("Temperatura: N/A, Humitat: N/A");
  #endif
        }
#endif
#ifdef DEBUG
        Serial.println("");
#endif
        displais();
        inter = !inter;
        lastUpdate = millis();
    }
}

