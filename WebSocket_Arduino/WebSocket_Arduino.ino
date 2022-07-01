/*
 * Arduino Mega 2560 and enc28j60
 * Simple WebSocket client
 * https://www.mischainti.org
 *
 * I use the ws://echo.websocket.org/ echo server
 * When you send a message to this server you receive
 * a response with the same message
 *
 */
 
 
#include <Arduino.h>
 
#include <SPI.h>
#include <UIPEthernet.h>
 
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
 
#define DEBUG_SERIAL Serial
 
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(172, 20, 5, 140);
 
WebSocketsClient webSocket;
 
unsigned long messageInterval = 5000;
bool connected = false;
 
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
 
 
    switch(type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.println("[WSc] Disconnected!\n");
            connected = false;
            break;
        case WStype_CONNECTED:
            {
                // send message to server when Connected
                DEBUG_SERIAL.println("[WSc] SENT: Connected");
                DEBUG_SERIAL.println((char *)payload);
                webSocket.sendTXT("Connected");
 
                connected = true;
            }
            break;
        case WStype_TEXT:
            DEBUG_SERIAL.print("[WSc] RESPONSE: ");
            DEBUG_SERIAL.println((char *)payload);
            break;
        case WStype_BIN:
            DEBUG_SERIAL.print("[WSc] get binary length: ");
            DEBUG_SERIAL.println(length);
           // hexdump(payload, length);
 
            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
        case WStype_ERROR:
            break;
    }
 
}
 
void setup()
{
    // Open serial communications and wait for port to open:
    DEBUG_SERIAL.begin(115200);
    while (!Serial) {}
 
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      DEBUG_SERIAL.println("Failed to configure Ethernet using DHCP");
      Ethernet.begin(mac, ip);
    }
    DEBUG_SERIAL.print("IP address ");
    DEBUG_SERIAL.println(Ethernet.localIP());
 
    webSocket.begin("echo.websocket.org", 80, "/");
    webSocket.onEvent(webSocketEvent);
 
}
 
unsigned long lastUpdate = millis();
void loop() {
    webSocket.loop();
    if (connected && lastUpdate+messageInterval<millis()){
        DEBUG_SERIAL.println("[WSc] SENT: Simple js client message!!");
        webSocket.sendTXT("Simple js client message!!");
        lastUpdate = millis();
    }
}