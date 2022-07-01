/*
 * esp8266 simple WebSocket server
 * https://www.mischainti.org
 *
 * The server response with the
 * echo of the message you send
 * and send a broadcast every 5secs
 *
 */
 
#include <Arduino.h>
 
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
 
const char *ssid     = "Electronica";
const char *password = "alfaro1110";
const uint8_t wsPort = 81;
unsigned long messageInterval = 10000;
bool connected = false;
 
#define DEBUG_SERIAL Serial
 
WebSocketsServer webSocket = WebSocketsServer(wsPort);
 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
 
    switch(type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                DEBUG_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                 
                // send message to client
        webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            DEBUG_SERIAL.printf("[%u] RECEIVE TXT: %s\n", num, payload);
            // send message to client
//             webSocket.sendTXT(num, "(ECHO MESSAGE) ");//+String((char *)payload));
             webSocket.sendTXT(num, payload );
             enviar = mensaje + valor;
             webSocket.sendTXT(num, enviar );
 
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            DEBUG_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);
            hexdump()
            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }
}
 
void setup() {
    DEBUG_SERIAL.begin(115200);
 
//  DEBUG_SERIAL.setDebugOutput(true);
 
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();
 
    for(uint8_t t = 4; t > 0; t--) {
        DEBUG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        DEBUG_SERIAL.flush();
        delay(1000);
    }
 
    WiFi.begin(ssid, password);
 
    while ( WiFi.status() = WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "." );
    }
 
    DEBUG_SERIAL.println("WebSocket complete uri is: ");
    DEBUG_SERIAL.print("ws://");
    DEBUG_SERIAL.print(WiFi.localIP());
    DEBUG_SERIAL.print(":");
    DEBUG_SERIAL.print(wsPort);
    DEBUG_SERIAL.println("/");
 
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}
 
unsigned long lastUpdate = millis();
 
void loop() {
    webSocket.loop();
      if (lastUpdate+messageInterval<millis()){
        DEBUG_SERIAL.println("[WSc] SENT: Simple broadcast client message!!");
        webSocket.broadcastTXT("Simple broadcast client message!!");
        lastUpdate = millis();
      }
    
}
