#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* ssid = "CPO Sport";
const char* password = "CPOsport";

WebServer server(80);

int relayPin = 19;
int countdownTime = 0;

void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body {";
  html += "font-family: Arial, sans-serif;";
  html += "text-align: center;";
  html += "}";
  html += "h1 {";
  html += "margin-top: 50px;";
  html += "}";
  html += ".container {";
  html += "width: 300px;";
  html += "margin: 0 auto;";
  html += "margin-top: 50px;";
  html += "}";
  html += "input[type='number'] {";
  html += "width: 100%;";
  html += "padding: 10px;";
  html += "margin-bottom: 10px;";
  html += "border: 1px solid #ccc;";
  html += "border-radius: 4px;";
  html += "}";
  html += "input[type='button'] {";
  html += "width: 100%;";
  html += "padding: 10px;";
  html += "background-color: #4CAF50;";
  html += "color: white;";
  html += "border: none;";
  html += "border-radius: 4px;";
  html += "cursor: pointer;";
  html += "margin-bottom: 10px;";
  html += "}";
  html += "input[type='button']:hover {";
  html += "background-color: #45a049;";
  html += "}";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>ESP32 Relay Control</h1>";
  html += "<p>Countdown Time (hours): ";
  html += "<input type='number' name='countdown' min='1' max='24' value='";
  html += String(countdownTime);
  html += "'><br><br>";
  html += "<input type='button' value='Start' onclick='startCountdown()'>";
  html += "<input type='button' value='Stop' onclick='stopCountdown()'>";
  html += "<script>";
  html += "function startCountdown() {";
  html += "var countdown = document.getElementsByName('countdown')[0].value;";
  html += "window.location.href = '/start?time=' + countdown;";
  html += "}";
  html += "function stopCountdown() {";
  html += "window.location.href = '/stop';";
  html += "}";
  html += "</script>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleStart() {
  String time = server.arg("time");
  // countdownTime = time.toInt() * 3600; // Mengonversi jam menjadi detik
  // countdownTime = time.toInt() * 1000; // Mengonversi jam menjadi detik
  countdownTime = time.toInt() * 1000 * 60; // Mengonversi jam menjadi detik
  digitalWrite(relayPin, HIGH);  // Mengaktifkan relay
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleStop() {
  countdownTime = 0;
  digitalWrite(relayPin, LOW);  // Mematikan relay
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Mematikan relay saat booting

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (countdownTime > 0) {
    countdownTime--;
    if (countdownTime == 0) {
      digitalWrite(relayPin, LOW);  // Mematikan relay setelah waktu penghitung mundur selesai
    }
  }
}
