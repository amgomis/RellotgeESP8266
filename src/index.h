const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Rellotge ESP8266</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 0;
            background-color: #f4f4f9;
        }
        .time {
            font-size: 100px;
            margin-top: 20px;
            color: #333;
        }
        .data {
            font-size: 48px;
            margin-top: 10px;
            color: #555;
        }
        .timezone {
            font-size: 32px;
            margin-top: 20px;
            color: #777;
        }
        button {
            padding: 10px 20px;
            margin: 10px;
            font-size: 18px;
            border: none;
            background-color: #008cba;
            color: white;
            border-radius: 5px;
            cursor: pointer;
        }
        button:hover {
            background-color: #005f7a;
        }
    </style>
</head>
<body>
    <div class="time" id="time">00:00</div>
    <div class="data" id="data">Temp: 0°C, Hum: 0%</div>
    <div class="timezone">Fus horari: <span id="timezone">+0</span></div>
    <script>
        async function fetchData() {
            const response = await fetch('/data');
            const data = await response.json();
            document.getElementById('time').textContent = data.time;
            document.getElementById('data').textContent = `Temp: ${data.temperature}°C, Hum: ${data.humidity}%`;
            document.getElementById('timezone').textContent = (data.timezone >= 0 ? "+" : "") + data.timezone;
        }
        async function changeTimezone(offset) {
            await fetch(`/timezone?offset=${offset}`);
            fetchData();
        }
        setInterval(fetchData, 1000);
        fetchData();
    </script>
</body>
</html>
)rawliteral";
