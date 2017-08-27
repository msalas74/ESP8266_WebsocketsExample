#include <WebSockets.h>
#include <WebSocketsClient.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

const char* host = "192.168.4.1"; //Ip of the Host(Our Case esp8266-01 as server. Its the ip of the esp8266-01 as Access point)
const int httpPort = 80;
const char* password = "123";   //
bool isNormal = true;
WiFiClient client;
WebSocketsClient webSocket;

//  The analog input for sound device
const int POTPIN = A0;
int analogInputValue = 0;

void sendToServerNoiseLevel(int level) {
  //Request to server to activate the led
  if (client.connect("192.168.4.1", httpPort)) {
    Serial.println("connected to web server");
    if (level == 3){
      webSocket.sendTXT("#3Alarm");
      //client.print(String("GET ") +"/Red"+" HTTP/1.1\r\n" + 
      //         "Host: " + host + "\r\n" +
      //         "Connection: close\r\n\r\n");
    } else if(level == 2){
      webSocket.sendTXT("#2Alarm"); 
      //client.print(String("GET ") +"/Yellow"+" HTTP/1.1\r\n" + 
      //         "Host: " + host + "\r\n" +
      //         "Connection: close\r\n\r\n");
    } else if (level == 1){
      webSocket.sendTXT("#1Normal");
      //client.print(String("GET ") +"/Green"+" HTTP/1.1\r\n" + 
      //         "Host: " + host + "\r\n" +
      //         "Connection: close\r\n\r\n");
    }
            
    delay(30);
    Serial.println("Got response from server.");
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    delay(30);
    
    //Serial.println();
    //Serial.println("closing connection");
  } else {
    delay(100);
    Serial.print(".");
  }
}

void connectToServer () {
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  if (client.connect("192.168.4.1", httpPort)) {
    Serial.println("connected");
  }      
}

void webSocketEvent(WStype_t type, uint8_t * response, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", response);

      // send message to server when Connected
      webSocket.sendTXT("Connected");
    }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", response);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      // send data to server
      // webSocket.sendBIN(response, length);
      break;
  }

}

void startWebSocket(){
  // server address, port and URL
  webSocket.begin("192.168.4.1", 81, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  //webSocket.setAuthorization("user", "Password");

  // try reconnection every 5 secs again if connection has failed
  webSocket.setReconnectInterval(2000);
}


void setup() {
  // put your setup code here, to run once:
  pinMode(POTPIN,INPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  Serial.begin(115200);

//  setup network connection to server
  WiFi.mode(WIFI_STA);           //NodeMcu esp12E in station mode
  WiFi.begin("ESP_D54736", password);      //Connect to this SSID. In our case esp-01 SSID.  

  while (WiFi.status() != WL_CONNECTED) {      //Wait for getting IP assigned by Access Point/ DHCP. 
                                               //Our case  esp-01 as Access point will assign IP to nodemcu esp12E.
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());             //Check out the Ip assigned by the esp12E

  connectToServer();

  startWebSocket();
}

void turnONLEDatThisLevel (int level){
  if (level < 250){
    digitalWrite(D0, HIGH);
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    if (!isNormal) {
      sendToServerNoiseLevel(1);
    }
    isNormal = true;
    
  }
    
  if (level > 250  && level < 600) {
    digitalWrite(D0, LOW);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
    isNormal = false;
    sendToServerNoiseLevel(2);
  }
    
  if( level > 600){
    digitalWrite(D0, LOW);
    digitalWrite(D1, LOW);
    digitalWrite(D2, HIGH);
    isNormal = false;
    sendToServerNoiseLevel(3);
  }
}

void loop() {
  analogInputValue = analogRead(POTPIN);
  turnONLEDatThisLevel(analogInputValue);
  webSocket.loop();
  
}
