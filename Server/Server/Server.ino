#include <WebSockets.h>
#include <WebSocketsServer.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>               // SPIFF library to access the ESP flash


const char* ssid = "ESP_D54736";  // SSID of esp8266
const char* password = "123";   //
bool toggle=0;                  //Variable to switch on and off the solenoid
ESP8266WebServer server(80);    //Specify port for TCP connection
WebSocketsServer webSocket = WebSocketsServer(81);
int zone = 0;

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

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

void handleDock () {
  
}

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

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

void setup() {
  delay(200);                           //Stable Wifi
  Serial.begin(115200);                 //Set Baud Rate
  pinMode(2, OUTPUT);                   //Led/Solenoid at pin 2
  WiFi.softAP(ssid, password);          //In Access Point Mode

  IPAddress myIP = WiFi.softAPIP();     //Check the IP assigned. Put this Ip in the client host.
  Serial.print("AP IP address: ");
  Serial.println(myIP);                 //Print the esp8266-01 IP(Client must also be on the save IP series)
  //  Alarm levels
  server.on("/Red", handleRed);
  server.on("/Yellow", handleYellow);
  server.on("/Green", handleGreen);
             
  server.on("/Dock", handleDock);         //  Send info to Dock Panel at Client Browser

  SPIFFS.begin();                           // Start the SPI Flash Files System
  
  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  
  server.begin();                       // Start the server
  Serial.println("Server started");

  startWebSocket();
}

void loop() {
  webSocket.loop();
  // Check if a client has connected. On first connection switch on the Solenoid on next switch off.
  server.handleClient();

}

