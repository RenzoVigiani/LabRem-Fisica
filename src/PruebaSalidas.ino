#include <ArduinoJson.h>
#include <UIPEthernet.h>

EthernetServer server = EthernetServer(22);

//////// VAriables de json //////////////
// Estado
int num_Lab=0;
bool subLab=true;
bool iniLab=true;
// Pulsadores
bool pulsador_0=false;
bool pulsador_1=false;
bool pulsador_2=false;
bool pulsador_3=false;
// Llaves
bool SW_0=false;
bool SW_1=false;
bool SW_2=false;
bool SW_3=false;
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


////////////// Funciones  ////////////////////
void ControlPost(void);
void valorSalidas(int);
void enciendoled(bool p0,bool p1,bool p2,bool p3);
void prueva_lab(int vueltas, bool Sentido);
void stopMotor(void);

//////////// declaración de salidas ///////////////////
const int Led_0=7;
const int Led_1=8;
const int Led_2=9;
const int Led_3=10;

/////////// salidas para el motor ////////////
#define IN1  3
#define IN2  4
#define IN3  5
#define IN4  6

//secuencia de vueltas
/*  
int paso [8][4] =
  {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
  };
  int vueltas=100;
  */

//  int dir=0;
/////////////////////////////////////
void setup() {
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
  
  // declaro salidas para leds
  pinMode(Led_0, OUTPUT);
  pinMode(Led_1, OUTPUT);
  pinMode(Led_2, OUTPUT);
  pinMode(Led_3, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(Led_0,LOW);
  digitalWrite(Led_1,LOW);
  digitalWrite(Led_2,LOW);
  digitalWrite(Led_3,LOW);
  // declaro salidas para motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  }

void loop() {
  // Wait for an incomming connection
  EthernetClient client = server.available(); 
  // Do we have a client?
  if (!client) return;
  Serial.println();
  Serial.println(F("New client"));
  // Read the request (we ignore the content in this example)
  while (client.available()) {
    client.readBytesUntil('\r', status, sizeof(status));
    Serial.println("status:");
    Serial.println(status);
  // Comparo la cadena recibida con las opcciones
    strncpy(operacion,status,15);
    Serial.println("operacion: ");
    Serial.println(operacion);
//obtengo las instrucciones del formato json
    strncpy(instrucciones,&status[15],(sizeof(status)-15));
    Serial.println("instrucciones:");
    Serial.println(instrucciones);   
    if (strstr(status, "GET / HTTP/1.1") != NULL) {
      
      StaticJsonDocument<256> doc;     
      JsonArray Estado = doc.createNestedArray("Estado");
      Estado.add(num_Lab);
      Estado.add(subLab);
      Estado.add(iniLab);

      JsonArray Pulsadores = doc.createNestedArray("Pulsadores");
      Pulsadores.add(pulsador_0);
      Pulsadores.add(pulsador_1);
      Pulsadores.add(pulsador_2);
      Pulsadores.add(pulsador_3);

      JsonArray Llaves = doc.createNestedArray("Llaves");
      Llaves.add(SW_0);
      Llaves.add(SW_1);
      Llaves.add(SW_2);
      Llaves.add(SW_3);

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
    if (strstr(status, "POST / HTTP/1.1") !=NULL) {
        Serial.println("Solicitud de escritura recibida");
        client.println(F("HTTP/1.1 200 OK"));
        client.println();
        StaticJsonDocument<256> doc;
        // Deserializo
        DeserializationError error = deserializeJson(doc, instrucciones);
        
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        
        JsonArray Estado = doc["Estado"];
        num_Lab = Estado[0]; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica]
        subLab = Estado[1]; // true[SubLab 1], false [SubLab 2]
        iniLab = Estado[2]; // true[Inicia Experimento], false[Finaliza Experimento]

        JsonArray Pulsadores = doc["Pulsadores"];
        pulsador_0 = Pulsadores[0]; // false
        pulsador_1 = Pulsadores[1]; // false
        pulsador_2 = Pulsadores[2]; // false
        pulsador_3 = Pulsadores[3]; // false

        JsonArray Llaves = doc["Llaves"];
        SW_0 = Llaves[0]; // 
        SW_1 = Llaves[1]; // 
        SW_2 = Llaves[2]; // 
        SW_3 = Llaves[3]; //  

        JsonArray Analogico = doc["Analogico"];
        variable_0 = Analogico[0]; // 
        variable_1 = Analogico[1]; // 
        variable_2 = Analogico[2]; // 
        variable_3 = Analogico[3]; // 
      ControlPost();
    }

    
  }

}

void ControlPost(){
  switch (num_Lab) {
    case 0:
      Serial.println("Laboratorio: Sistemas Digitales");
      int vueltas = variable_0 * 512;
      prueva_lab(vueltas, SW_0);
      break;
    case 1:
      Serial.println("Laboratorio: Sistemas de Control");
      vueltas = variable_0 * 512;
      prueva_lab(vueltas, SW_0);
      break;
    case 2:
      Serial.println("Laboratorio: Telecomunicaciones");
      break;
    case 3:
      Serial.println("Laboratorio: Fisica Basica");
      break;
    default:
      Serial.println("Laboratorio: Default");
      break;
  }
}

void prueva_lab(int vueltas, bool Sentido){ // Función de prueba para los lab
  while((vueltas)>=0)
  {
    Serial.println(vueltas);
    if(Sentido==true)
    {
      if(vueltas>0){        
          for(int i=0;i<8;i++)
          {
          valorSalidas(i);
          delay(5);
          }            
    }else if(vueltas<=0)
      {  
        stopMotor();
        enciendoled(pulsador_0,pulsador_1,pulsador_2,pulsador_3);
      }
    }
    else if(Sentido==false) 
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
        enciendoled(pulsador_0,pulsador_1,pulsador_2,pulsador_3);
      }
    }
    vueltas--;    
  }
}

void valorSalidas(int i){ // Salidas Motor
  switch (i) {
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

void enciendoled(bool p0,bool p1,bool p2,bool p3){
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
