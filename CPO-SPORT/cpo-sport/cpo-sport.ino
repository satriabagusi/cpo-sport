#include <WiFi.h>
#include <Arduino.h>

const char* ssid = "Rahasia";
const char* password = "maungapain";

#define RELAY1_PIN 19 
#define RELAY2_PIN 26 

WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String relay1State = "off";
String relay2State = "off";


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin as an output.
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

}

// the loop function runs over and over again forever
void loop() {

  WiFiClient client = server.available(); 

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /relay1/on") >= 0) {
              Serial.println("Relay 1 on");
              relay1State = "on";
              digitalWrite(RELAY1_PIN, HIGH);
            } else if (header.indexOf("GET /relay1/off") >= 0) {
              Serial.println("Relay 1 off");
              relay1State = "off";
              digitalWrite(RELAY1_PIN, LOW);
            } else if (header.indexOf("GET /relay2/on") >= 0) {
              Serial.println("Relay 2 on");
              relay2State = "on";
              digitalWrite(RELAY2_PIN, HIGH);
            } else if (header.indexOf("GET /relay2/off") >= 0) {
              Serial.println("Relay 2 on");
              relay2State = "off";
              digitalWrite(RELAY2_PIN, LOW);
            } else if (header.indexOf("GET /relay/"))
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Kontrol Relay</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Relay 1 - State " + relay1State + "</p>");
            // If the relay1 is off, it displays the ON button       
            if (relay1State=="on") {
              client.println("<p><a href=\"/relay1/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/relay1/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            client.println("<p>Relay 2 - State " + relay2State + "</p>");
            if (relay2State=="on") {
              client.println("<p><a href=\"/relay2/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/relay2/on\"><button class=\"button button2\">OFF</button></a></p>");
            }
               
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}