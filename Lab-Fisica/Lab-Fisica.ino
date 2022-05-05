#include <ArduinoJson.h>
#include <UIPEthernet.h>


EthernetServer server = EthernetServer(22);

////////////// Funciones  ////////////////////
void ControlPost(bool iniLab,bool subLab,bool SW_0,int variable_0,int variable_1,int variable_2,int variable_3);
void valorSalidas(int);
void prueva_lab(int vueltas, bool Sentido);
void stopMotor(void);

//////////// declaración de salidas ///////////////////
/////////// salidas para el motor 1////////////
#define IN1  3
#define IN2  4
#define IN3  5
#define IN4  6

void setup() {
  // Initialize Arduino server parameters
  uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress myIP(172,20,5,140);
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;
  // Initialize Ethernet libary
  Ethernet.begin(mac,myIP);  
  // Start to listen
  server.begin();
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
 
  // declaro salidas para motor 1
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  }

void loop() 
{
  //////// VAriables de json //////////////
  // Estado
  int num_Lab=0;
  bool subLab=true;
  bool iniLab=true;
  // Llaves
  bool SW_0=false;
  // Analogico
  int variable_0=0;
  int variable_1=0;
  int variable_2=0;
  int variable_3=0;
  ////////////////////

  //////////// Strings de comunicación /////////////
  char status[170] = {0};
  char instrucciones[150] = {0};
  char operacion[20] = {0};

  // Wait for an incomming connection
  EthernetClient client = server.available(); 
  // Do we have a client?
  if (!client) return;
  Serial.println();
  Serial.println(F("New client"));
  // Read the request (we ignore the content in this example)
  while (client.available()) 
  {
    client.readBytesUntil('\r', status, sizeof(status));
    Serial.println("status:");
    Serial.println(status);
  // Comparo la cadena recibida con las opcciones
    strncpy(operacion,status,15);
//    Serial.println("operacion: ");
//    Serial.println(operacion);
//obtengo las instrucciones del formato json
    strncpy(instrucciones,&status[15],(sizeof(status)-15));
//    Serial.println("instrucciones:");
//    Serial.println(instrucciones);   
    if (strstr(status, "GET / HTTP/1.1") != NULL) 
    {     
      StaticJsonDocument<256> doc;     
      JsonArray Estado = doc.createNestedArray("Estado");
      Estado.add(num_Lab);
      Estado.add(subLab);
      Estado.add(iniLab);

      JsonArray Llaves = doc.createNestedArray("Llaves");
      Llaves.add(SW_0);

      JsonArray Analogico = doc.createNestedArray("Analogico");
      Analogico.add(variable_0);
      Analogico.add(variable_1);
      Analogico.add(variable_2);
      Analogico.add(variable_3);

      Serial.print(F("Sending: "));
      serializeJson(doc, Serial);
      Serial.println();
// Write response headers
      client.println(F("HTTP/1.0 200 OK"));
      client.println(F("Content-Type: application/json"));
// client.println(F("Connection: close"));
      client.print(F("Content-Length: "));
      client.println(measureJsonPretty(doc));
      client.println(); 
// Write JSON document
      serializeJsonPretty(doc, client);
// Disconnect
 client.stop();

      }
///////////////////////////// POST ///////////////////////////////////
    if (strstr(status, "POST / HTTP/1.1") !=NULL) 
    {
        Serial.println("Solicitud de escritura recibida");
        client.println(F("HTTP/1.1 200 OK"));
        client.println();
        StaticJsonDocument<256> doc;
        // Deserializo
        DeserializationError error = deserializeJson(doc, instrucciones);
        
        if (error) 
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        
        JsonArray Estado = doc["Estado"];
        num_Lab = Estado[0]; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica]
        subLab = Estado[1]; // true[SubLab 1], false [SubLab 2]
        iniLab = Estado[2]; // true[Inicia Experimento], false[Finaliza Experimento]

        JsonArray Llaves = doc["Llaves"];
        SW_0 = Llaves[0]; // Para habilitar Diafragma

        JsonArray Analogico = doc["Analogico"];
        variable_0 = Analogico[0]; // 
        variable_1 = Analogico[1]; // 
        variable_2 = Analogico[2]; // 
        variable_3 = Analogico[3]; // 

        if(num_Lab==2)
        {
          ControlPost(iniLab,subLab,SW_0,variable_0,variable_1,variable_2,variable_3);
          Serial.println("Salidas asignadas");
        }
        else
        {
          Serial.println("Laboratorio incorrecto");    
        }
    }
  }
}

void ControlPost(bool iniLab,bool subLab,bool SW_0,int variable_0,int variable_1,int variable_2,int variable_3)
{
  if (subLab and iniLab)
  {
      Serial.println("Sub - Laboratorio: Lentes convergentes"); 
      int vueltas = variable_0 * 512;
      prueva_lab(vueltas, SW_0);
   
  }
  else if (!subLab and iniLab)
  {
      Serial.println("Sub - Laboratorio: Lentes Divergentes");  
  }
}

void prueva_lab(int vueltas, bool Sentido)
{ // Función de prueba para los lab
  while((vueltas)>=0)
  {
    Serial.println(vueltas);
    if(Sentido)
    {
      if(vueltas>0)
      {        
          for(int i=0;i<8;i++)
          {
          valorSalidas(i);
          delay(5);
          }            
      }
      else if(vueltas<=0)
      {  
        stopMotor();
        enciendoled(pulsador_0,pulsador_1,pulsador_2,pulsador_3);
      }
    }
    else if(!Sentido) 
    {
      if(vueltas>0){
          for(int i=7;i>=0;i--)
          {
          valorSalidas(i);
          delay(5);
          }            
      }else if(vueltas<=0)
      {  
        stopMotor();
      }
    }
    vueltas--;    
  }
}

void valorSalidas(int i)
{ // Salidas Motor
  switch (i) 
  {
    case 0:
      digitalWrite(IN1,1);
      digitalWrite(IN2,0);
      digitalWrite(IN3,0);
      digitalWrite(IN4,0);
      break;
    case 1:
      digitalWrite(IN1,1);
      digitalWrite(IN2,1);
      digitalWrite(IN3,0);
      digitalWrite(IN4,0);
      break;
    case 2:
      digitalWrite(IN1,0);
      digitalWrite(IN2,1);
      digitalWrite(IN3,0);
      digitalWrite(IN4,0);
      break;
    case 3:
      digitalWrite(IN1,0);
      digitalWrite(IN2,1);
      digitalWrite(IN3,1);
      digitalWrite(IN4,0);
      break;
    case 4:
      digitalWrite(IN1,0);
      digitalWrite(IN2,0);
      digitalWrite(IN3,1);
      digitalWrite(IN4,0);
      break;
    case 5:
      digitalWrite(IN1,0);
      digitalWrite(IN2,0);
      digitalWrite(IN3,1);
      digitalWrite(IN4,1);
      break;
    case 6:
      digitalWrite(IN1,0);
      digitalWrite(IN2,0);
      digitalWrite(IN3,0);
      digitalWrite(IN4,1);
      break;
    case 7:
      digitalWrite(IN1,1);
      digitalWrite(IN2,0);
      digitalWrite(IN3,0);
      digitalWrite(IN4,1);
      break;
    default:
      digitalWrite(IN1,0);
      digitalWrite(IN2,0);
      digitalWrite(IN3,0);
      digitalWrite(IN4,0);
  }
}

void enciendoled(bool p0,bool p1,bool p2,bool p3)
{
  if(p0)
    digitalWrite(Led_0,HIGH);
  else
    digitalWrite(Led_0,LOW);
   if(p1)
    digitalWrite(Led_1,HIGH);
   else
    digitalWrite(Led_1,LOW);
   if(p2)
    digitalWrite(Led_2,HIGH);
   else
    digitalWrite(Led_2,LOW);
   if(p3)
    digitalWrite(Led_3,HIGH);
   else
    digitalWrite(Led_3,LOW);
}

void stopMotor(){
 digitalWrite(IN1, 0);
 digitalWrite(IN2, 0);
 digitalWrite(IN3, 0);
 digitalWrite(IN4, 0); 
}
