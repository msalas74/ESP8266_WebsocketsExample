# ESP8266 Websocket server and client example

A ESP8266 Websocket server that passes data from a ESP8266 client.  The client monitors AO for voltage level.  Three LED's are controlled via D0, D1, and D2.  The states of the LED's are passed to the server through sockets.  The server than broadcasts the states to all connected clients.

## Features
* WiFi Server
* Webserver
* Websockets server
* SPIFFS
* Bootstrap Framework

## Built With

* Arduino Wifi Library
* Arduino ESP8266 Webserver Library
* [ESP8266 SPIFFS Library](http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html)
* [Links2004's Websockets Library](https://github.com/Links2004/arduinoWebSockets)

## TODO List

* Add threshold parameter UI
* Add threshold levels for client ESP8266 controller

## Authors

* **Matias Salas** - *Initial work* - [Website Portfolio](http://www.matiasis.me)

## Acknowledgments

* [tttapa](https://github.com/tttapa)  ** Great tutorial for SPIFFS and most of the features in this example.
* Markus [Links2004](https://github.com/Links2004)  ** Websockets Library
