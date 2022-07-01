/*
 * Arduino Mega 2560 and enc28j60
 * Simple WebSocket Server
 *
 * https://www.mischainti.org
 *
 * I use the ws://echo.websocket.org/ echo server
 * When you send a message to this server you receive
 * a response with the same message
 *
 */
 
 
#include <Arduino.h>
//#include <Hexdump.h>
#include <SPI.h>
#include <UIPEthernet.h>
#include <WebSocketsServer.h>
//#include <WebSocketsClient.h>
#define DEBUG_SERIAL Serial
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 108);
const uint8_t wsPort = 81;
unsigned long messageInterval = 10000;
bool connected = false; 
//WebSocketsClient webSocket;
WebSocketsServer webSocket = WebSocketsServer(wsPort);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
 
    switch(type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.println( "[" + String(num) + "] Disconnected!");
            connected = false;
            break;
        case WStype_CONNECTED:
            {
            IPAddress ip = Ethernet.dnsServerIP();// remoteIP(num);
            Serial.println( "[" + String(num) + "] Disconnected!");
            DEBUG_SERIAL.println("[" + String(num) + "] Connected from " +
                              String(ip[0]) + "."+ String(ip[1]) + "." + 
                              String(ip[2]) + "."+ String(ip[3]) + " url:" + 
                              String((char *)payload));
            connected = true;
                // send message to client
        webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            DEBUG_SERIAL.println("[" + String(num) + "] RECEIVE TXT: " + String((char *)payload));
            // send message to client
             webSocket.sendTXT(num, "(ECHO MESSAGE)"+String((char *)payload));
 
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            DEBUG_SERIAL.print("[" + String(num)+"] get binary length: "+String(length));
//            hexdump((Stream&)payload,(uint8_t)length); 
            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }
}
void setup()
{
    // Open serial communications and wait for port to open:
    DEBUG_SERIAL.begin(115200);

    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();

//    while (!Serial) {}

    
    for(uint8_t t = 4; t > 0; t--) {
        DEBUG_SERIAL.print("[SETUP] BOOT WAIT ...\n");
        DEBUG_SERIAL.flush();
        delay(1000);
    }

    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      DEBUG_SERIAL.println("Failed to configure Ethernet using DHCP");
      Ethernet.begin(mac, ip);
    }

    DEBUG_SERIAL.print("IP address: ");
    DEBUG_SERIAL.println(Ethernet.localIP());
    DEBUG_SERIAL.print("Port: ");
    DEBUG_SERIAL.println(wsPort);
    
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}
unsigned long lastUpdate = millis();

void loop() {
    webSocket.loop();
    if (connected && lastUpdate+messageInterval<millis()){
        DEBUG_SERIAL.println("[WSc] SENT: Simple js client message!!");
        webSocket.broadcastTXT("Simple js client message!!");
        lastUpdate = millis();
    }
} 
