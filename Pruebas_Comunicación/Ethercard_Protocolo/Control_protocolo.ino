/*
/////////////////////
Procedimiento:
Enviar GET (para saber estado actual)
Enviar POST(para asignar datos)
Arduino responde  "HTTP/1.1 200 OK"
luego enviar GET hasta que arduino responda(si no se conecta es porque está trabajando en el pedido del post)
*/
#include <EtherCard.h>
#include <ArduinoJson.h>

#define const_status 700
#define const_instruc 700
//------------------------------
static byte mymac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static byte myip[] = {172,20,5,140};
byte Ethernet::buffer[const_status];

//--------- Declaración de variables ----------//
//////// VAriables de json ////////////// (GENERICO)
// Estado
int num_Lab=0;
bool subLab=0;
bool iniLab=1;
// Pulsadores
bool Pulsadores_0=0;
bool Pulsadores_1=0;
bool Pulsadores_2=0;
bool Pulsadores_3=0;
// Indicadores
bool Indicadores_0=0;
bool Indicadores_1=0;
bool Indicadores_2=0;
bool Indicadores_3=0;
// Analogico
int Analogico_0=0;
int Analogico_1=0;
int Analogico_2=0;
int Analogico_3=0;
// UART
char* Serial_rx={"Holis"};

//---------- Declaración de pines -----------//
const int ledPin = 3;

//---------- Config -------------//
void setup () {
  ether.hisport=22; // Definimos el puerto
  Serial.begin(250000); // inicia serial
  Serial.println("Test del Modulo  ENC28J60"); 

  if (!ether.begin(sizeof Ethernet::buffer, mymac, 53)) // se controla acceso al modulo y se guarda datos en buffer 
    Serial.println( "No se ha podido acceder a la controlador Ethernet");
  else
   Serial.println("Controlador Ethernet inicializado");
  if (!ether.staticSetup(myip))
    Serial.println("No se pudo establecer la dirección IP");
  Serial.println();
  //--------Modo de pines---------//  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

//----- Respuestas de Pedidos ETHERNET -----//
static word postpage() {
  BufferFiller bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.1 200 OK\r\n"));
  return bfill.position();
}

static word getpage() {
  BufferFiller bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.1 200 OK\r\n"
  // guiarse de la siguiente pagina.
  // https://www.aelius.com/njh/ethercard/class_buffer_filler.html 
  "{\"Estado\":[$D,$D,$D],\"Pulsadores\":[$D,$D,$D,$D],\"Indicadores\":[$D,$D,$D,$D],\"Analogico\":[$D,$D,$D,$D],\"Serial\":$S}"
  ),num_Lab,subLab,iniLab,
    Pulsadores_0,Pulsadores_1,Pulsadores_2,Pulsadores_3,
    Indicadores_0,Indicadores_1,Indicadores_2,Indicadores_3,
    Analogico_0,Analogico_1,Analogico_2,Analogico_3,
    Serial_rx);
  return bfill.position();

}
  
void loop() {
    //////////// Strings de comunicación /////////////
  char status[const_status] = {0};
  char instrucciones[const_instruc] = {"Hola"};
  bool bandera=0;
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len); 
  if(pos) { 
    if(strstr((char *)Ethernet::buffer + pos, "GET / HTTP/1.1") != 0) {
      Serial.println("Comando GET recibido");
      ether.persistTcpConnection(true);
      ether.httpServerReply(getpage()); // se envia OK y datos necesarios
    }
    if(strstr((char *)Ethernet::buffer + pos, "POST / HTTP/1.1") != 0) 
    {
      Serial.println("Comando POST recibido");
      //obtengo las instrucciones del formato json
      strcpy(status,(char *)Ethernet::buffer + pos);
      strncpy(instrucciones,&status[15],sizeof(status)-15);
/*      Serial.print("status:");
      Serial.println(status);
      Serial.print("Instrucciones:");
      Serial.println(instrucciones);*/
      bandera=1;
//      ether.httpServerReply(12);
      ether.httpServerReply(postpage()); // se envia OK
      ether.persistTcpConnection(true);
    }        
  }    
  if(bandera==1)
  {
    post_json(instrucciones);
    bandera=0;
  } 
}

void post_json(char instrucciones[const_instruc])
{
  StaticJsonDocument<256> doc; // Creo un doc de json
  DeserializationError error = deserializeJson(doc, instrucciones); // Deserializo
  if (error) // Analizo posibles errores.
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonArray Estado = doc["Estado"];
  num_Lab = Estado[0]; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica]
  subLab = Estado[1]; // 1 [SubLab 1], 0 [SubLab 2]
  iniLab = Estado[2]; // 1 [Inicia Experimento], 0 [Finaliza Experimento]

  if(num_Lab==3) // Control de numero de lab.
  {
    JsonArray Pulsadores = doc["Pulsadores"];
    Pulsadores_0 = Pulsadores[0]; 
    Pulsadores_1 = Pulsadores[1]; 
    Pulsadores_2 = Pulsadores[2]; 
    Pulsadores_3 = Pulsadores[3]; 

    JsonArray Indicadores = doc["Indicadores"];
    Indicadores_0 = Indicadores[0]; 
    Indicadores_1 = Indicadores[1]; 
    Indicadores_2 = Indicadores[2]; 
    Indicadores_3 = Indicadores[3]; 

    JsonArray Analogico = doc["Analogico"];
    Analogico_0 = Analogico[0];
    Analogico_1 = Analogico[1];
    Analogico_2 = Analogico[2];
    Analogico_3 = Analogico[3];

    Serial_rx = doc["Serial"]; 

    if (subLab and iniLab)
    {
      Serial.println("Sub - Laboratorio: 1"); 
    }
    else if (!subLab and iniLab)
    {
      Serial.println("Sub - Laboratorio: 2");
      if(!ether.packetLoop(ether.packetReceive()))
      {
        for (int i = 0; i < 10; i++)
        {
          delay(1000);
          Serial.print("Pasaron ");
          Serial.print(i+1);
          Serial.println(" segundos");
        }
      }else  ether.httpServerReply(getpage());              
    }
    else Serial.println("Laboratorio Parado"); 
  }
  else Serial.println("Laboratorio incorrecto");    
}