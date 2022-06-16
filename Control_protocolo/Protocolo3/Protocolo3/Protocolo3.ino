#include <UIPEthernet.h>
EthernetServer server = EthernetServer(22);
byte dato=0x00;
char status[100] = {0};
char mistatus[100] = {0};
char operacion[100] = {0};
char valores_recibidos[100] = {0};
//-----------------//
const int ledPin = 3;
int habilitador=0;
int bandera=0;
void encender_led();
int counter=0;
//-----------------//

void setup() {
  uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress myIP(172,20,5,140);
  
  // Initialize serial port
  Serial.begin(115200);
  while (!Serial) continue;
 
  // Initialize Ethernet libary
  Ethernet.begin(mac,myIP);  

  // Start to listen
  server.begin();
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
  }

void loop() {
  // Wait for an incomming connection
  EthernetClient client = server.available();
  
  // Do we have a client?
  if (!client) return;
//  else
//  Serial.println("New client aparecido");
  
  // Read the request (we ignore the content in this example)
  while (client.available())
    client.readBytesUntil('\r', status, sizeof(status));
    Serial.println("New client");
    strncpy(mistatus,status,sizeof(status));
    strncpy(operacion,status,15);
    Serial.print("operacion: ");
    Serial.println(operacion);   
 
    if (strstr(status, "GET / HTTP/1.1") != NULL) {
          client.println(F("HTTP/1.0 200 OK"));
          client.println(); 
          }

    if (strstr(status, "POST / HTTP/1.1") !=NULL) {
        Serial.println("Solicitud de escritura recibida");
        client.println(F("HTTP/1.1 200 OK"));
        client.println();
        bandera=1;
        Serial.println(bandera);
        //encender_led();
    }  
    if(bandera==1){
      if(counter<4){
        encender_led();
        counter++;  
      }else{
        bandera=0;
        counter=0;
      }      
    }
        
}

void encender_led()
{
  delay(200);
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);
      Serial.println("LEDDD");
      delay(200);
      digitalWrite(ledPin, LOW); 
      delay(2000);
}
