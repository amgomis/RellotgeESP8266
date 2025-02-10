
#include <vector>

#define PIN    12      // Pin de sortida Leds
#define nleds 320      // Nombre de Leds
#define brillantor 10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(nleds, PIN, NEO_GRB + NEO_KHZ800);

int pixelIndex;
bool punts = false;
int hh, hl, mh, ml;
int cont = 0;

unsigned long refresc = 1000;
unsigned long anterior_millis = millis();

int zero[]   = {0x7E, 0xFF, 0x81, 0x81, 0xFF, 0x7E};
int un[]     = {0x00, 0x41, 0xFF, 0xFF, 0x01, 0x00};
int dos[]    = {0x43, 0xC7, 0x8D, 0x99, 0xF1, 0x61};
int tres[]   = {0x42, 0xC3, 0x91, 0x91, 0xFF, 0x6E};
int quatre[] = {0x1C, 0x3C, 0x64, 0xCF, 0x8F, 0x04};
int cinc[]   = {0xF2, 0xF3, 0x91, 0x91, 0x9F, 0x8E};
int sis[]    = {0x7E, 0xFF, 0x89, 0x89, 0xCF, 0x46};
int nset[]   = {0x80, 0x80, 0x8F, 0x9F, 0xF0, 0xE0};
int vuit[]   = {0x6E, 0xFF, 0x91, 0x91, 0xFF, 0x6E};
int nou[]    = {0x72, 0xFB, 0x89, 0x89, 0xFF, 0x7E};

int lla[]    = {0x7F, 0xFF, 0x88, 0x88, 0xFF, 0x7F};
int llb[]    = {0xFF, 0xFF, 0x91, 0x91, 0xFF, 0x6E};
int llc[]    = {0x7E, 0xFF, 0x81, 0x81, 0xC3, 0x42};
int lld[]    = {0xFF, 0xFF, 0x81, 0x81, 0xFF, 0x7E};
int lle[]    = {0xFF, 0xFF, 0x91, 0x91, 0x91, 0x81};
int llf[]    = {0xFF, 0xFF, 0x90, 0x90, 0x90, 0x80};
int llg[]    = {0x7E, 0xFF, 0x81, 0x89, 0x8F, 0x8F};
int llh[]    = {0xFF, 0xFF, 0x10, 0x10, 0xFF, 0xFF};
int lli[]    = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};
int llj[]    = {0x00, 0x02, 0x03, 0x01, 0xFF, 0xFE};
int llk[]    = {0xFF, 0xFF, 0x18, 0x3C, 0xE7, 0xC3};
int lll[]    = {0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03};
int llm[]    = {0xFF, 0xFF, 0xC0, 0x60, 0xC0, 0xFF};
int lln[]    = {0xFF, 0xFF, 0x30, 0x0C, 0xFF, 0xFF};
int llo[]    = {0x7E, 0xFF, 0x81, 0x81, 0xFF, 0x7E};
int llp[]    = {0xFF, 0xFF, 0x88, 0x88, 0xF8, 0x70};
int llq[]    = {0x7E, 0xFF, 0x81, 0x81, 0xFE, 0x7D};
int llr[]    = {0xFF, 0xFF, 0x88, 0x8C, 0xFF, 0x73};
int lls[]    = {0x72, 0xFB, 0x99, 0x99, 0xDF, 0x4E};
int llt[]    = {0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0};
int llu[]    = {0xFE, 0xFF, 0x01, 0x01, 0xFF, 0xFE};
int llv[]    = {0xFC, 0xFE, 0x03, 0x03, 0xFE, 0xFC};
int llw[]    = {0xFF, 0xFF, 0x06, 0x0C, 0x06, 0xFF};
int llx[]    = {0xC3, 0xE7, 0x3C, 0x3C, 0xE7, 0xC3};
int lly[]    = {0xF0, 0xF8, 0x0F, 0x0F, 0xF8, 0xF0};
int llz[]    = {0xC3, 0xC7, 0xCF, 0xDB, 0xF3, 0xE3};

int tpc[]    = {0xC1, 0xC6, 0x1C, 0x38, 0x63, 0x83};
int tpc1[]   = {0xC3, 0xC4, 0x08, 0x10, 0x23, 0xC3};
int tpc2[]   = {0xE3, 0xA4, 0xE8, 0x17, 0x25, 0xC7};
int grau[]   = {0x00, 0xE0, 0xA0, 0xE0, 0x00, 0x00};
int dospunt[]= {0x00, 0x00, 0x66, 0x66, 0x00, 0x00};
int nul[]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

std::vector<int*> car = {zero, un, dos, tres, quatre, cinc, sis, nset, vuit, nou,
                         nul, dospunt, grau, tpc2, llc, llh, lla, llb, llc, lld, lle,
                         llf, llg, llh, lli, llj, llk, lll, llm, lln, llo, llp, llq,
                         llr, lls, llt, llu, llv, llw, llx, lly, llz};

void dibuixarDigit(int numero[], int pos, int red, int green, int blue) {
  int cnt = 8 * pos;
  for (int byte = 0; byte < 6; byte++) { // Iterem sobre els 6 bytes
    for (int bit = 0; bit < 8; bit++) { // Iterem sobre els 8 bits de cada byte
      pixelIndex = cnt + byte * 8 + bit; // Calculem l'índex del píxel (0-47)
      int color = (numero[byte] & (1 << bit)) ? pixels.Color(red, green, blue) : pixels.Color(0, 0, 0);
      pixels.setPixelColor(pixelIndex, color);
    }
  }
}

void dibuixarPunts(int pos, bool clr, int red, int green, int blue) {
  int dospun[] = {0x66, 0x66};
  if (clr == true) {dospun[0] = 0x66; dospun[1] = 0x66; } else {dospun[0] = 0x00; dospun[1] = 0x00; }
  int cnt = 8 * pos;
  for (int byte = 0; byte < 2; byte++) { // Iterem sobre els 2 bytes
    for (int bit = 0; bit < 8; bit++) { // Iterem sobre els 8 bits de cada byte
      pixelIndex = cnt + byte * 8 + bit; // Calculem l'índex del píxel (0-15)
      int color = (dospun[byte] & (1 << bit)) ? pixels.Color(red, green, blue) : pixels.Color(0, 0, 0);
      pixels.setPixelColor(pixelIndex, color);
    }
  }
}

void dibuixarPunt(int pos, int red, int green, int blue) {
  int dospun[] = {0x03, 0x03};
  int cnt = 8 * pos;
  for (int byte = 0; byte < 2; byte++) { // Iterem sobre els 2 bytes
    for (int bit = 0; bit < 8; bit++) { // Iterem sobre els 8 bits de cada byte
      pixelIndex = cnt + byte * 8 + bit; // Calculem l'índex del píxel (0-15)
      int color = (dospun[byte] & (1 << bit)) ? pixels.Color(red, green, blue) : pixels.Color(0, 0, 0);
      pixels.setPixelColor(pixelIndex, color);
    }
  }
}

void dibuixarGrau(int pos, int red, int green, int blue) {
  int dospun[] = {0xE0, 0xA0, 0xE0};
  int cnt = 8 * pos;
  for (int byte = 0; byte < 3; byte++) { // Iterem sobre els 2 bytes
    for (int bit = 0; bit < 8; bit++) { // Iterem sobre els 8 bits de cada byte
      pixelIndex = cnt + byte * 8 + bit; // Calculem l'índex del píxel (0-15)
      int color = (dospun[byte] & (1 << bit)) ? pixels.Color(red, green, blue) : pixels.Color(0, 0, 0);
      pixels.setPixelColor(pixelIndex, color);
    }
  }
}


void clrgfx(){
  pixelIndex = 0;
  dibuixarDigit(nul, 0, 0, 0, 0);
  dibuixarDigit(nul, 6, 0, 0, 0);
  dibuixarDigit(nul, 12, 0, 0, 0);
  dibuixarDigit(nul, 18, 0, 0, 0);
  dibuixarDigit(nul, 24, 0, 0, 0);
  dibuixarDigit(nul, 30, 0, 0, 0);
  pixels.show();
}

void horamingfx(int hh, int hl, int mh, int ml, bool punt){
  pixelIndex = 0;
  dibuixarDigit(car[hh], 1, 255, 255, 255);
  dibuixarDigit(car[hl], 8, 255, 255, 255);
  dibuixarPunts(15, punt, 255, 255, 255);
  dibuixarDigit(car[mh], 18, 255, 255, 255);
  dibuixarDigit(car[ml], 25, 255, 255, 255);
  pixels.show();
}


void tempgfx(int hh, int hl){
  pixelIndex = 0;
  int deci = int((temperature - int(temperature))*10);
  dibuixarDigit(car[hh],  0, 255, 127, 0);
  dibuixarDigit(car[hl],  7, 255, 127, 0);
  dibuixarPunt(14, 255, 127, 0);
  dibuixarDigit(car[deci], 17, 255, 127, 0);
  dibuixarGrau( 24, 255, 127, 0);
  dibuixarDigit(car[14], 28, 255, 127, 0);
  pixels.show();
}

void humitgfx(int hh, int hl){
  pixelIndex = 0;
  dibuixarDigit(car[hh],  1, 0, 127, 255);
  dibuixarDigit(car[hl],  8, 0, 127, 255);
  dibuixarDigit(car[13], 17, 0, 127, 255);
  dibuixarDigit(car[15], 25, 0, 127, 255);
  pixels.show();
}
  
