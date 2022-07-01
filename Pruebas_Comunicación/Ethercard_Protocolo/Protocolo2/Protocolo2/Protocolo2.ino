#include <EtherCard.h>

static byte mymac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static byte myip[] = {172,20,5,140};
byte Ethernet::buffer[700];

//---------//
int bandera=0;
const int ledPin = 3;
void encender_led();
int counter=0;
int habilitador=0;
//---------//
void setup () {
 
  Serial.begin(115200);
  Serial.println("Test del Modulo  ENC28J60");
  ether.hisport=22;
 
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 53))
    Serial.println( "No se ha podido acceder a la controlador Ethernet");
 else
   Serial.println("Controlador Ethernet inicializado");
 
  if (!ether.staticSetup(myip))
    Serial.println("No se pudo establecer la dirección IP");

  Serial.println();
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

static word homePage() {
  Serial.println("Comando 1");
     BufferFiller bfill = ether.tcpOffset();
     Serial.println("Comando 2");
     bfill.emit_p(PSTR("HTTP/1.1 200 OK\r\n"),millis()/1000,analogRead(0));
     Serial.println("Comando 3");
     return bfill.position();
     Serial.println("Comando 4");
}
 
void loop() {

  //-----------------------//
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len); 
  if(pos) {
    //------------------------------------//
      
      
        if(strstr((char *)Ethernet::buffer + pos, "GET / HTTP/1.1") != 0) {
          Serial.println("Comando ON recibido");
          ether.packetLoop(6);
        }
    
        if(strstr((char *)Ethernet::buffer + pos, "POST / HTTP/1.1") != 0) {
          Serial.println("Comando OFF recibido");
          bandera=1;
          ether.httpServerReply(homePage()); // se envia OK
          habilitador=1;
        }
    }
    //------------------------------------//        
//  }
//  if(habilitador==1){
//    Serial.println("CONEXION NO DISPONIBLE");
//    //ether.tcpOffset();
//  }

  
  if(bandera==1)
  {
    if(counter<4)
    {
      encender_led();
      counter++;
    }
    else
    {
      bandera=0;
      counter=0;
      habilitador=0;
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
      delay(200);
      digitalWrite(ledPin, LOW); 
      delay(2000);
}
