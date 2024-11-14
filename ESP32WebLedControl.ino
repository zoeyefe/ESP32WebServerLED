#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SSID";         // Wi-Fi ağ adı
const char* password = "PASSWORD";   // Wi-Fi şifresi

WebServer server(80);

const int ledPin = 2;  // D2 pinindeki LED

bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // Wi-Fi bağlantısı
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = R"====(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 LED Control</title>
      <style>
        body { font-family: Arial, sans-serif; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; background-color: #f0f0f5; }
        .container { text-align: center; }
        h1 { color: #333; }
        .button { padding: 20px; font-size: 24px; color: white; border: none; border-radius: 10px; cursor: pointer; }
        .on { background-color: #4CAF50; }
        .off { background-color: #f44336; }
      </style>
      <script>
        function toggleLED() {
          fetch("/toggle")
            .then(response => response.json())
            .then(data => {
              let btn = document.getElementById("ledButton");
              btn.innerHTML = data.state ? "Turn LED Off" : "Turn LED On";
              btn.className = data.state ? "button on" : "button off";
            });
        }
      </script>
    </head>
    <body>
      <div class="container">
        <h1>ESP32 LED Control</h1>
        <button id="ledButton" class="button )====" + String(ledState ? "on" : "off") + R"====(" onclick="toggleLED()">
          )====" + String(ledState ? "Turn LED Off" : "Turn LED On") + R"====(
        </button>
      </div>
    </body>
    </html>
  )====";
  server.send(200, "text/html", html);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  String response = "{\"state\": " + String(ledState) + "}";
  server.send(200, "application/json", response);
}
