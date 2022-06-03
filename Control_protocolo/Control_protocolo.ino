#include <EtherCard.h>

static byte mymac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static byte myip[] = {172,20,5,140};
byte Ethernet::buffer[700];
int bandera=0;

const int ledPin = 3;

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
  
 BufferFiller bfill = ether.tcpOffset();
 bfill.emit_p(PSTR("HTTP/1.1 200 OK\r\n"   
//      "Content-Type: text/htmlrnPragma: no-cachernRefresh: 5\r\n\r\n"
//      "<html><head><title>Naylamp Mechatronics</title></head>"
//      "<body>"
//      "<div style='text-align:center;'>"
//      "<h1>Test del Módulo  ENC28J60</h1>"      
//      "Tiempo transcurrido : $L segundos"
//      "<br /><br />Estado del LED: $S<br />"      
//      "<a href=\"/?status=ON\"><input type=\"button\" value=\"ON\"></a>"
//      "<a href=\"/?status=OFF\"><input type=\"button\" value=\"OFF\"></a>"
//      "<br /><br />Potenciómetro: $D (resolución de 1024)"
//      "<br /><br />"
//      "<a href='https://naylampmechatronics.com/'>naylampmechatronics.com</a>"
//      "</body></html>" 
      ),millis()/1000,analogRead(0));
     
  return bfill.position();
}
  
void loop() {
 
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if(pos) {
    
    if(strstr((char *)Ethernet::buffer + pos, "GET / HTTP/1.1") != 0) {
      Serial.println("Comando ON recivido");
    }

    if(strstr((char *)Ethernet::buffer + pos, "POST / HTTP/1.1") != 0) {
      Serial.println("Comando OFF recivido");
      bandera=1;
      ether.httpServerReply(homePage()); // se envia OK
    }        

    if(bandera==1)
    {
      delay(2000);
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW); 
      delay(10000);
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      bandera=0;
    }
  }
    
}
