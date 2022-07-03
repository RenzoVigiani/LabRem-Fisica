// Defino las librerias a utilizar
#include <ArduinoJson.h> // Para el manejo y conversión de JSON a vartiables
#include <UIPEthernet.h> // Para el manejo del Shield Ethernet
#include <Servo.h> // Para el manejo de los Servos.
//----------------------------------------------//
// Defino el servidor y el Puerto
#define server_port 80
EthernetServer server = EthernetServer(server_port);
//----------------------------------------------//
// Defino variables para Json
#define const_mje 256
#define const_valores 236

//char Mensaje_recibido[const_mje] = {0}; // Mensaje recibido por ethernet. (Comando + JSON) 
//char valores_recibidos[const_valores] = {0}; // JSON recibido.

//----------------------------------------------//
//------- Defino variables globales
// Nombres para los pines GPIO
// Motor 1
  #define IN1_1  6
  #define IN2_1  7
  #define IN3_1  8
  #define IN4_1  9
// Motor 2
  #define IN1_2  20
  #define IN2_2  21
  #define IN3_2  22
  #define IN4_2  23
// Motor 3
  #define IN1_3  24
  #define IN2_3  25
  #define IN3_3  26
  #define IN4_3  27
// Leds
  #define Led_M1 10
  #define Led_M2 11
  #define Led_M3 12
  #define Led_aux 13
  #define Led_S1 2
  #define Led_S2 3
// Foco
  #define Foco_pin 28
// Servo Diafragma
  #define Diafragma_pin 4
// Servo Lente
  #define Lente_pin 5
// Declaramos la variable para controlar el servo
  Servo servo_diafragma;
  Servo servo_lente;

// VAriables de json 
// Estado
int num_Lab=0;
bool subLab=0;
bool iniLab=1;
// Analogico
int Analogico_0=0;
int Analogico_1=0;
int Analogico_2=0;
int Analogico_3=0;
int Analogico_4=0;
//--- Variables auxiliares ---//
int distancia_act_1; //Distancias actuales de cada motor
int distancia_act_2;
int distancia_act_3;
int dist_mov; // distancia que se debe mover el motor en realidad
bool bandera=0;
// Nombres de variables auxiliares
int v_time = 0;
const int ledPin = 3;
//----------------------------------------------//
// Funciones
void get_json(EthernetClient client);
void post_json(char instrucciones[const_valores], EthernetClient client);

//----------------------------------------------//
// Realizo las configuraciones iniciales
void setup() {
  uint8_t myMAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // defino mac del dispositivo.
  IPAddress myIP(192,168,1,140); // 172.20.5.140 Defino IP Address del dispositivo. 
  Serial.begin(115200); // Inicializo Puerto serial 0 
  while (!Serial) continue; 
  Ethernet.begin(myMAC,myIP);  // Inicializo libreria Ethernet
  server.begin(); // Start to listen
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
  Serial.println("Port:" + (String)server_port);

// ------- Defino GPIO MODE (INPUT/OUTPUT)--------  //  
// Motor 1
  pinMode(IN1_1, OUTPUT);
  pinMode(IN2_1, OUTPUT);
  pinMode(IN3_1, OUTPUT);
  pinMode(IN4_1, OUTPUT);
// Motor 2
  pinMode(IN1_2, OUTPUT);
  pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT);
  pinMode(IN4_2, OUTPUT);
// Motor 3
  pinMode(IN1_3, OUTPUT);
  pinMode(IN2_3, OUTPUT);
  pinMode(IN3_3, OUTPUT);
  pinMode(IN4_3, OUTPUT);
// Leds
  pinMode(Led_M1, OUTPUT); // Led Motor 1
  pinMode(Led_M2, OUTPUT); // Led Motor 2
  pinMode(Led_M3, OUTPUT); // Led Motor 3
  pinMode(Led_aux, OUTPUT);// Led Aux
  pinMode(Led_S1, OUTPUT); // Led Servo 1
  pinMode(Led_S2, OUTPUT); // Led Servo 2
  pinMode(ledPin,OUTPUT);
// Foco (LED)
  pinMode(Foco_pin, OUTPUT);
// Servos
  servo_diafragma.attach(Diafragma_pin);
  servo_lente.attach(Lente_pin);
  //------ Definir estados iniciales ------//
}

void loop() {
  //////////// Strings de comunicación /////////////
char Mensaje_recibido[const_mje] = {0}; // Mensaje recibido por ethernet. (Comando + JSON) 
char valores_recibidos[const_valores] = {0}; // JSON recibido.  // Wait for an incomming connection
  EthernetClient client = server.available(); 
  if(client){ // Si tengo un cliente conectado
    while (client.available()) 
    {
      Serial.println("New Command");
      client.readBytesUntil('\r', Mensaje_recibido, sizeof(Mensaje_recibido)); // Tomo el mensaje recibido.
      strncpy(valores_recibidos,&Mensaje_recibido[15],(sizeof(Mensaje_recibido)-15)); 
      Serial.print("Mensaje Recibido: ");
      Serial.println(Mensaje_recibido);   
      Serial.print("Json_Recibido: ");
      Serial.println(valores_recibidos);   
      //------ GET ----- //
      if (strstr(Mensaje_recibido, "GET /HTTP/1.1") != NULL) { // Compruebo si llega un GET, respondo valores actuales
        StaticJsonDocument<256> doc;     
        JsonArray Estado = doc.createNestedArray("Estado");
        Estado.add(num_Lab);
        Estado.add(subLab);
        Estado.add(iniLab);

        JsonArray Indicadores = doc.createNestedArray("Analogicos");
        Indicadores.add(Analogico_0);
        Indicadores.add(Analogico_1);
        Indicadores.add(Analogico_2);
        Indicadores.add(Analogico_3);
        Indicadores.add(Analogico_4);

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
        client.println(F("Get terminado"));
  //      client.stop();
      }
      //------- POST -----//      
      if (strstr(Mensaje_recibido, "POST /HTTP/1.1") !=NULL) { // Compruebo si llega un POST y respondo, habilito banderas.
        Serial.println("Solicitud de escritura recibida");        
        client.println();
        client.println(F("HTTP/1.1 200 OK"));
        client.println();
        StaticJsonDocument<256> doc; // Creo un doc de json
        DeserializationError error = deserializeJson(doc, valores_recibidos); // Deserializo
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
          JsonArray Analogico = doc["Analogico"];
          Analogico_0 = Analogico[0];
          Analogico_1 = Analogico[1];
          Analogico_2 = Analogico[2];
          Analogico_3 = Analogico[3];
          Analogico_4 = Analogico[4];
        //client.stop();
        }
      }
    }
  }
  else{ // Si no está el cliente enviando algo, sigo haciendo lo que corresponde.
    hacer();
  }
}

void hacer(){
  if(v_time == 10){
    if(digitalRead(ledPin)==0){
        digitalWrite(ledPin, 1);
    }
    else if(digitalRead(ledPin)==1){
        digitalWrite(ledPin, 0);
    }
    v_time = 0;
  }
  else{
    v_time++;
  }
}
