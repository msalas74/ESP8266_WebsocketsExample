#include <WebSockets.h>
#include <WebSocketsServer.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>                     // SPIFF library to access the ESP flash


const char* ssid = "ESP_8266WSs";
const char* password = "123";

ESP8266WebServer server(80);    //Specify port for HTTP connection
WebSocketsServer webSocket = WebSocketsServer(81);

String getContentType(String filename);
bool handleFileRead(String path);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * response, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[WSc] Connected to url: %s\n", response);
        
        // send message to client
        webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", response);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");

            if (response[0] == '#') {
              Serial.printf("[WSc] Noise Level: %s\n", response);
              webSocket.broadcastTXT(response, length);
            }
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);

            // send message to client
            // webSocket.sendBIN(num, response, length);
            break;
    }

}


String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;
}

//  Example for GET HTTP requests for client
void handleRed() {
  String s = "\r\n\r\n<!DOCTYPE HTML>\r\n<html><h1>Alarm Level</h1> ";
  s += "<p>Red Level!!!</p></html>\r\n\r\n";
  server.send(200,"text/html",s);      //Reply to the client
  Serial.println("replied to client: RED Level");
}

void handleYellow() {
  String s = "\r\n\r\n<!DOCTYPE HTML>\r\n<html><h1>Alarm Level</h1> ";
  s += "<p>Yellow Level!!!</p></html>\r\n\r\n";
  server.send(200,"text/html",s);      //Reply to the client
  Serial.println("replied to client: YELLOW Level");
}

void handleGreen() {
  String s = "\r\n\r\n<!DOCTYPE HTML>\r\n<html><h1>Alarm Level</h1> ";
  s += "<p>Green Level</p></html>\r\n\r\n";
  server.send(200,"text/html",s);      //Reply to the client
  Serial.println("replied to client: GREEN Level");
}

void startWIFI() {
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void setUpSPIFFS() {
  SPIFFS.begin();
  Serial.println("SPIFFS server started.");
}

void startWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started.");
}

void startWebServer(){
   //  Alarm levels
  server.on("/Red", handleRed);
  server.on("/Yellow", handleYellow);
  server.on("/Green", handleGreen);
  
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "404: Not Found");
  });
  
  server.begin();
  Serial.println("HTTP Server started");
}

void setup() {
  delay(200);
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  startWIFI();

  setUpSPIFFS();

  startWebSocket();

  startWebServer();
}

void loop() {
  webSocket.loop();
  server.handleClient();

}

