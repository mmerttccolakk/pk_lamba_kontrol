

#include <ESP8266WiFi.h>   //  ESP8266 library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputD5State = "on";
String outputD2State = "on";

// Assign output variables to GPIO pins
const int outputD5 = D5;
const int outputD2 = D2;

void setup() {
  Serial.begin(115200);
  
  // Initialize the output variables as outputs
  pinMode(outputD5, OUTPUT);
  // Set outputs to LOW
  digitalWrite(outputD5, HIGH);


  WiFiManager wifiManager;
  wifiManager.autoConnect("Cyber_JET", "mert1994");

  Serial.println("Connected.");
  server.begin();   // initialize the Server  
}

void loop(){
  WiFiClient client = server.available();   

  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    // if the byte is a newline character

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /D5/on") >= 0) {
              Serial.println("GPIO D5 on");
              outputD5State = "on";
              digitalWrite(outputD5, HIGH);
            } else if (header.indexOf("GET /D5/off") >= 0) {
              Serial.println("GPIO D5 off");
              outputD5State = "off";
              digitalWrite(outputD5, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Plaka Gizleme v.01</h1>");
            
            client.println("<p>PIN DURUM " + outputD5State + "</p>");
            if (outputD5State=="off") {
              client.println("<p> PLAKA GIZLI </p>");
              client.println("<p><a href=\"/D5/on\"><button class=\"button\">PLAKAYI AC</button></a></p>");
            } else {
              client.println("<p> PLAKA GORUNEBILIR </p>");
              client.println("<p><a href=\"/D5/off\"><button class=\"button button2\">PLAKAYI GIZLE</button></a></p>");
            } 
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
