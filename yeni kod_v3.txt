#include <ESP8266WiFi.h>   //  ESP8266 library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

const char* host = "esp8266-webupdate";
ESP8266WebServer httpServer(8080);
ESP8266HTTPUpdateServer httpUpdater;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// role icin pin durumu
String role_durum = "on";
//role pini
const int role = D5;

//ilk calisma tek seferlik zaman kapaması icin
bool running = true;

//timer icin degiskenler
unsigned long first_time=0;
unsigned long last_time;

//otomatik kapanma saniye cinsinden
unsigned int kapanma_sn=300000;//5*60*1000

void setup() {
  Serial.begin(115200);
  
  // pin output setting
  pinMode(role, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // ilk baslangicta pin acik
  digitalWrite(role, HIGH);

  WiFiManager wifiManager;
  wifiManager.autoConnect("Cyber_JET", "12345678");
  wifiManager.setTimeout(120);
  
  Serial.println("Connected.");
  server.begin();   // initialize the Server  
  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 8080);
  Serial.println("HTTPUpdateServer ready!"); 
  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println(":8080/update in your browser\n");


}
void loop(){

  //timer kullanımı ile sayac
  last_time=millis();
  
  //debug calisma kontrolu
  if(last_time-first_time>=10000){
    Serial.println("sistem calisiyor");
    first_time=last_time;
    digitalWrite(LED_BUILTIN, LOW);
  }
  //otomatik plaka kapama
    if(last_time>=kapanma_sn and running==true){
    running =false;
    digitalWrite(role, LOW);
    role_durum = "off";
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  httpServer.handleClient();
  MDNS.update();


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
            if (header.indexOf("GET /pdurum/on") >= 0) {
              digitalWrite(role, HIGH);
              Serial.println("GPIO D5 on");
              role_durum = "on";
              digitalWrite(LED_BUILTIN, LOW);
            } else if (header.indexOf("GET /pdurum/off") >= 0) {
              digitalWrite(role, LOW);
              Serial.println("GPIO D5 off");
              role_durum = "off";
              digitalWrite(LED_BUILTIN, LOW);
            }

client.println("<!doctype html>");
client.println("<html lang=\"en\">");
client.println("  <head>");
client.println("    <!-- Required meta tags -->");
client.println("    <meta charset=\"utf-8\">");
client.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

client.println("    <!-- Bootstrap CSS -->");
client.println("<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-eOJMYsd53ii+scO/bJGFsiCZc+5NDVN2yr8+0RDqr0Ql0h+rP48ckxlpbzKgwra6\" crossorigin=\"anonymous\">");
client.println("    <title>Plaka Gizleme V2</title>");
client.println("  </head>");

client.println("  <body>");
client.println("    <main>");
client.println("  <div class=\"container py-4\">");
client.println("    <header class=\"pb-3 mb-4 border-bottom\">");
client.println("        <h2>Plaka Gizleme <span class=\"badge bg-secondary\">V2</span></h2>");
client.println("    </header>");

client.println("    <div class=\"row align-items-md-stretch\">");
client.println("      <div class=\"col-md-6 py-4\">");
client.println("        <div class=\"h-100 p-5 bg-light border rounded-3\">");
client.println("          <h2 class=\"py-1\">Durum</h2>");

if (role_durum=="off") {
client.println("      <div class=\"alert alert-danger\" role=\"alert\">");
client.println("        Plaka Gizli");
client.println("      </div>");
client.println("      <a href=\"/pdurum/on\">");
client.println("        <button type=\"button\" class=\"btn btn-outline-primary\">Plakayı Aç</button>");
client.println("      </a>");
} else {
client.println("      <div class=\"alert alert-primary\" role=\"alert\">");
client.println("        Plaka Görünebilir");
client.println("      </div>");
client.println("      <a href=\"/pdurum/off\">");
client.println("        <button type=\"button\" class=\"btn btn-outline-danger\">Plakayı Gizle</button>");
client.println("      </a>");
} 

client.println("        </div>");
client.println("      </div>");
client.println("      <div class=\"col-md-6 py-4\">");
client.println("        <div class=\"h-100 p-5 bg-light border rounded-3\">");
client.println("          <h2>Otomatik Gizleme</h2>");
if(last_time>=kapanma_sn and running==false){
    client.println("<div class=\"alert alert-success\" role=\"success\">Islem Tamamlandı. </div>");
    }else{
      client.print("<div class=\"alert alert-warning\" role=\"warning\">");
      if((kapanma_sn-last_time)>=60000){
         client.print((kapanma_sn-last_time)/60000);
         client.println(" Dakika sonra kapatılacak.");
        }else{
          client.print((kapanma_sn-last_time)/1000);
          client.println(" Saniye sonra kapatılacak.");
          }
      }
client.println("        </div>");
client.println("      </div>");
 client.println("   </div>");

client.println("    <footer class=\"pt-3 mt-4 text-muted border-top\" align=\"center\">");
client.print("      <a href=\"https://github.com/mmerttccolakk/\" target=\"_blank\" class=\"link-info\"> MertC</a>&copy; 2021&nbsp;<a href=\"http://");
client.print(WiFi.localIP());
client.println(":8080/update\" target=\"_blank\" class=\"link-secondary\" target=\"_blank\">Update</a>");
client.println("    </footer>");
client.println("  </div>");
client.println("</main>");
client.println("    <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-JEW9xMcG8R+pH31jmWH6WWP0WintQrMb4s7ZOdauHnUtxwoG2vI5DkLtS3qm9Ekf\" crossorigin=\"anonymous\"></script>");
client.println("  </body>");
client.println("</html>");
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
    digitalWrite(LED_BUILTIN, HIGH);
  
}