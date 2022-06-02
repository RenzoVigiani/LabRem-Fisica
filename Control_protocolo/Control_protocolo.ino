#include <ArduinoJson.h>
#include <UIPEthernet.h>
#include <Servo.h>

#define const_status 150
#define const_instruc 130

EthernetServer server = EthernetServer(22);
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
  int distancia_act_1;
  int distancia_act_2;
  int distancia_act_3;
  int dist_mov; // distancia que se debe mover el motor en realidad

//-------------------//
void get_json(EthernetClient client);
void post_json(char instrucciones[const_instruc], EthernetClient client);

//////////// declaración de salidas ///////////////////
//---------------- Motores --------------------------//

void setup() {
  // Initialize Arduino server parameters
  uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress myIP(172,20,5,140);
  // Initialize serial 0 port
  Serial.begin(9600);
  while (!Serial) continue;
  // Initialize Ethernet libary
  Ethernet.begin(mac,myIP);  
  // Start to listen
  server.begin();
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
}

void loop() 
{
  //////////// Strings de comunicación /////////////
  char status[const_status] = {0};
  char instrucciones[const_instruc] = {0};
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
//obtengo las instrucciones del formato json
    strncpy(instrucciones,&status[15],(sizeof(status)-15));

    //------ GET ----- //
    if (strstr(status, "GET / HTTP/1.1") != NULL) 
    {
      get_json(client);
    }
  //------- POST -----//      
    if (strstr(status, "POST / HTTP/1.1") !=NULL) 
    {
      post_json(instrucciones, client);
    }
  }
}

void get_json(EthernetClient client)
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

void post_json(char instrucciones[const_instruc], EthernetClient client)
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

  if(num_Lab==3)
  {
    if (subLab and iniLab)
    {
      Serial.println("Sub - Laboratorio: Lentes convergentes"); 
    }
    else if (!subLab and iniLab)
    {
      Serial.println("Sub - Laboratorio: Lentes Divergentes");  
    }
    else
    {
      Serial.println("Laboratorio Parado");    
    }     
  }
  else
  {
    Serial.println("Laboratorio incorrecto");    
  }
  // Disconnect
//      client.stop();
}

