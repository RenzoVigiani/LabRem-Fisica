// Defino las librerias a utilizar
#include <ArduinoJson.h> // Para el manejo y conversión de JSON a vartiables
#include <UIPEthernet.h> // Para el manejo del Shield Ethernet
#include <Servo.h> // Para el manejo de los Servos.
//----------------------------------------------//
// Defino el servidor y el Puerto
#define server_port 22 // 80
EthernetServer server = EthernetServer(server_port);
//----------------------------------------------//
// Defino variables para Json
#define const_mje 256
#define const_valores 236

//char Mensaje_recibido[const_mje] = {0}; // Mensaje recibido por ethernet. (Comando + JSON) 
//char valores_recibidos[const_valores] = {0}; // JSON recibido.
//----------------------------------------------//
//------- Defino mensajes de error predeterminado
// una variable error int
uint8_t Errores = 0;
// 0 - Sin errores
// 1 - Error de distancia.
// 2 - Error de cantidad de medicion.
// 3 - Error de tipo de diafragma.
// 4 - Laboratorio incorrecto.

//----------------------------------------------//
//------- Defino variables globales
// Nombres para los pines GPIO
// Motor 1
  #define IN1_1  6 //bobina 1
  #define IN2_1  7 //bobina 2
  #define IN3_1  8 //bobina 3
  #define IN4_1  9 //bobina 4
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
  #define Led_M1 10 // Indicador Motor 1
  #define Led_M2 11 // Indicador Motor 2
  #define Led_M3 12 // Indicador Motor 3
  #define Led_aux 13 // Indicador aux
  #define Led_S1 2  // Indicador Servo 1
  #define Led_S2 3 // Indicador Servo 2
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
int num_Lab=0; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica] 
bool subLab=0; // 1 [SubLab 1], 0 [SubLab 2]
bool iniLab=1;// 1 [Inicia Experimento], 0 [Finaliza Experimento]
// Analogico
int Analogico_0=0; // Tipo de diafragma
int Analogico_1=0; // Cantidad de mediciones
int Analogico_2=0; // Distancia 1
int Analogico_3=0; // Distancia 2
int Analogico_4=0; // Distancia 3
//--- Variables auxiliares ---//
int distancia_act_1; //Distancias actuales de cada motor
int distancia_act_2;
int distancia_act_3;
int dist_mov; // distancia que se debe mover el motor en realidad
bool bandera_vueltas=0;// sirve para hacer el conteo de vueltas. Cuando termina se activa.
bool bandera_rep=0; // bandera para limitar la cantidad de repeticiones de mensaje de lab incorrecto 
bool bandera_fin_m1=0; // bandera para determinar fin de mov de motor 1
bool bandera_fin_m2=0; // bandera para determinar fin de mov de motor 2
bool bandera_fin_m3=0; // bandera para determinar fin de mov de motor 3
bool sentido=0;
// Nombres de variables auxiliares
int conta_bobinas=0; // sirve para cambiar las bobinas
int conta_vueltas; 
int v_time = 0;
const int ledPin = 3;
const int limite_inferior_riel=0; // Indica el minimo valor que puede tomar distancia
const int limite_superior_riel=900; // indica el minimo valor que puede tomar distancia.
//----------------------------------------------//
// Funciones
void get_json(EthernetClient client);
void post_json(char instrucciones[const_valores], EthernetClient client);

//----------------------------------------------//
// Realizo las configuraciones iniciales
void setup() {
  uint8_t myMAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // defino mac del dispositivo.
  IPAddress myIP(192,168,1,108); // 172.20.5.140 Defino IP Address del dispositivo. 
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
      if(bandera_rep==1)bandera_rep=0; //reinicio bandera de repetición cuando tengo un mje nuevo.
      Serial.println("New Command");
      client.readBytesUntil('\r', Mensaje_recibido, sizeof(Mensaje_recibido)); // Tomo el mensaje recibido.
      strncpy(valores_recibidos,&Mensaje_recibido[15],(sizeof(Mensaje_recibido)-15)); 
      Serial.print("Mensaje Recibido: ");
      Serial.println(Mensaje_recibido);   
//      Serial.print("Json_Recibido: ");
//      Serial.println(valores_recibidos);   
      //------ GET ----- //
      if (strstr(Mensaje_recibido, "GET /HTTP/1.1") != NULL) { // Compruebo si llega un GET, respondo valores actuales
        StaticJsonDocument<256> doc;     
        JsonArray Estado = doc.createNestedArray("Estado");
        Estado.add(num_Lab);
        Estado.add(subLab);
        Estado.add(iniLab);

        JsonArray Analogicos = doc.createNestedArray("Analogicos");
        Analogicos.add(Analogico_0);
        Analogicos.add(Analogico_1);
        Analogicos.add(distancia_act_1);
        Analogicos.add(distancia_act_2);
        Analogicos.add(distancia_act_3);

        JsonArray Error = doc.createNestedArray("Error");  
        Error.add(Errores);

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
        if (bandera_vueltas) {bandera_fin_m1=0; bandera_fin_m2=0; bandera_fin_m3=0; bandera_vueltas=0;}
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
      Control();
  }
}

void Control(){
  if(num_Lab==3 and bandera_vueltas==0){ // Control de numero de lab.
    if (subLab and iniLab){
      Serial.println("Sub - Laboratorio: Lentes convergentes"); 
      Convergentes(Analogico_0, Analogico_1, Analogico_2,Analogico_3);
    }
    else if (!subLab and iniLab){
      Serial.println("Sub - Laboratorio: Lentes Divergentes");  
      Divergentes(Analogico_0, Analogico_1, Analogico_2, Analogico_3,Analogico_4);
    }
    else{
      if(bandera_rep==0) Serial.println("Laboratorio Parado"); bandera_rep=1;
    }
  }
  else {
    if(bandera_rep==0) Serial.println("Laboratorio incorrecto");  bandera_rep = 1; Errores=4;
  }
  hacer();
  delay(500);
}

void hacer(){
  if(v_time == 2){
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

void Convergentes(int diafragma, int cant_med, int distancia_fl, int distancia_lp)
{
  digitalWrite(Foco_pin, HIGH); // Enciendo FOCO
  switch (diafragma) // Posicion del diafragma
  {
  case 0:
    servo_diafragma.write(0);// Desplazamos a la posición 0º
    break; 
  case 1:
    servo_diafragma.write(45);// Desplazamos a la posición 0º
    break;
  case 2:
    servo_diafragma.write(90);// Desplazamos a la posición 0º
    break;
  case 3:
    servo_diafragma.write(135);// Desplazamos a la posición 0º
    break;
  case 4:
    servo_diafragma.write(180);// Desplazamos a la posición 0º
    break;
  default:
    servo_diafragma.write(0);// Desplazamos a la posición 0º
    break;
  }
  // Controlo motores
  Control_Motor(1, distancia_fl);
  Control_Motor(2, distancia_lp);    
  if(bandera_fin_m1){
    if(bandera_fin_m2) bandera_vueltas=1;
  }
}

void Divergentes(int diafragma, int cant_med, int distancia_fl1, int distancia_l1l2, int distancia_l2p)
{
  Serial.println("Divergentes");
  switch (diafragma) // Posicion del diafragma
  {
  case 0:
    servo_diafragma.write(0);// Desplazamos a la posición 0º
    break; 
  case 1:
    servo_diafragma.write(30);// Desplazamos a la posición 0º
    break;
  case 2:
    servo_diafragma.write(60);// Desplazamos a la posición 0º
    break;
  case 3:
    servo_diafragma.write(90);// Desplazamos a la posición 0º
    break;
  case 4:
    servo_diafragma.write(120);// Desplazamos a la posición 0º
    break;
  default:
    servo_diafragma.write(0);// Desplazamos a la posición 0º
    break;
  }
  // Controlo motores
  Control_Motor(1, distancia_fl1);

  Control_Motor(2, distancia_l1l2);
  Control_Motor(3, distancia_l2p); 
  if(bandera_fin_m1){
    if(bandera_fin_m2){
      if(bandera_fin_m3) bandera_vueltas=1;
    }
  }
}

/**
 * @brief 
 * Función utilizada para el sistema completo de manejo de motor en base a la distancia requerida.
 * @param motor 
 * @param distancia 
 */
void Control_Motor(int motor, int distancia)
{
  bool sentido=true;
  switch (motor)  // se controla motor a mover
  {
    case 1:
      sentido = control_distancia(distancia_act_1, distancia);
      if(dist_mov==0) {bandera_fin_m1 = 1; Serial.println("bandera Fin M1");}
      Serial.println("Distancia requerida Motor 1: " + (String)distancia);
      Serial.println("Distancia actual Motor 1: " + (String)distancia_act_1);
      distancia_act_1 = Mover_Motor(IN1_1, IN2_1, IN3_1, IN4_1, dist_mov, distancia_act_1, sentido);
      
      break;
    case 2:
      sentido = control_distancia(distancia_act_2, distancia);
      if(dist_mov==0) {bandera_fin_m2 = 1;Serial.println("bandera Fin M2");}
      Serial.println("Distancia requerida Motor 2: " + (String)distancia);
      Serial.println("Distancia actual Motor 2: " + (String)distancia_act_2);
      distancia_act_2 = Mover_Motor(IN1_2, IN2_2, IN3_2, IN4_2, dist_mov, distancia_act_2, sentido);
      break;
    case 3:
      sentido = control_distancia(distancia_act_3, distancia);
      if(dist_mov==0) {bandera_fin_m3 = 1;Serial.println("bandera Fin M3");}
      Serial.println("Distancia requerida Motor 3: " + (String)distancia);
      Serial.println("Distancia actual Motor 3: " + (String)distancia_act_3);
      distancia_act_3 = Mover_Motor(IN1_3, IN2_3, IN3_3, IN4_3, dist_mov, distancia_act_3, sentido);
      break;
    default:
      Serial.println("El motor no existe");
      break;
  }
}

/**
 * @brief 
 * Funcion utilizada para determinar el sentido de giro del motor.
 * Depende de la posición actual del motor.
   true = Indica que el sentido es horario 
 * false = Indica que el sentido es antihorario
 * @param distancia_act Distancia actual del motor indicado
 * @param distancia distancia requerida.
 * @return bool Indica el sentido de giro 
 */
bool control_distancia(int distancia_act, int distancia)
{
//  bool sentido = true; // true = derecha, false = izquierda
  if (distancia >= limite_inferior_riel and distancia <= limite_superior_riel) // maximo movimiento es 100 mm
  {
    if (distancia_act > distancia){sentido = false;dist_mov = (distancia_act - distancia);}    
    if (distancia_act < distancia){sentido = true;dist_mov = (distancia - distancia_act);}
    if (distancia_act == distancia) dist_mov = 0;    
    Serial.println("Sentido = "+String(sentido));
  }
  else Serial.println("Distancia no permitida"); Errores=1; 
  return sentido;
}

/**
 * @brief 
 * Funcion utilizada para controlar el movimiento del motor PaP
 * @param bobina_1 
 * @param bobina_2 
 * @param bobina_3 
 * @param bobina_4 
 * @param dist_mov Distancia a moverse
 * @param aux_dist Distancia que devuelve. se debe ingresar la distancia actual
 * @param sentido Sentido de giro del motor
 * @return int - Valor de la distancia actual despues de un movimiento.
 */
int Mover_Motor(int bobina_1, int bobina_2, int bobina_3, int bobina_4, int dist_mov, int aux_dist, bool sentido)
{
  const int factor_vueltas = 128; //512 - una vuelta entera.
  int vueltas= dist_mov * factor_vueltas;
  
//  if(vueltas>=0){
    if(conta_bobinas<8){
      if((vueltas == 0)){
        stopMotor(bobina_1,bobina_2,bobina_3,bobina_4);    
      }
      if(vueltas>0){
        if(sentido){// giro positivo
          valorSalidas(conta_bobinas,bobina_1,bobina_2,bobina_3,bobina_4);
          if( (vueltas % factor_vueltas) == 0) aux_dist++; // sumo cuenta distancia
        }else{      //giro negativo
          valorSalidas((8-conta_bobinas),bobina_1,bobina_2,bobina_3,bobina_4);
          if( (vueltas % factor_vueltas) == 0) aux_dist--;// resto cuenta distancia
        }
        delay(2);
        conta_bobinas++;
      }
    }
    if(conta_bobinas==8) {conta_bobinas=0; vueltas--; }     
//  }
  return aux_dist;
}

/**
 * @brief 
 * Funcion definida para selección la activación de las bobinas. 
 * Esto permite el movimiento del motot PaP.
 * @param selector Indica el caso de activación
 * @param bobina_1 
 * @param bobina_2 
 * @param bobina_3 
 * @param bobina_4 
 */
void valorSalidas(int selector,int bobina_1, int bobina_2, int bobina_3, int bobina_4){ // Salidas Motor
  switch (selector){
    case 0:
      digitalWrite(bobina_1,1);
      digitalWrite(bobina_2,0);
      digitalWrite(bobina_3,0);
      digitalWrite(bobina_4,0);
      break;
    case 1:
      digitalWrite(bobina_1,1);
      digitalWrite(bobina_2,1);
      digitalWrite(bobina_3,0);
      digitalWrite(bobina_4,0);
      break;
    case 2:
      digitalWrite(bobina_1,0);
      digitalWrite(bobina_2,1);
      digitalWrite(bobina_3,0);
      digitalWrite(bobina_4,0);
      break;
    case 3:
      digitalWrite(bobina_1,0);
      digitalWrite(bobina_2,1);
      digitalWrite(bobina_3,1);
      digitalWrite(bobina_4,0);
      break;
    case 4:
      digitalWrite(bobina_1,0);
      digitalWrite(bobina_2,0);
      digitalWrite(bobina_3,1);
      digitalWrite(bobina_4,0);
      break;
    case 5:
      digitalWrite(bobina_1,0);
      digitalWrite(bobina_2,0);
      digitalWrite(bobina_3,1);
      digitalWrite(bobina_4,1);
      break;
    case 6:
      digitalWrite(bobina_1,0);
      digitalWrite(bobina_2,0);
      digitalWrite(bobina_3,0);
      digitalWrite(bobina_4,1);
      break;
    case 7:
      digitalWrite(bobina_1,1);
      digitalWrite(bobina_2,0);
      digitalWrite(bobina_3,0);
      digitalWrite(bobina_4,1);
      break;
    default:
      digitalWrite(bobina_1,0);
      digitalWrite(bobina_2,0);
      digitalWrite(bobina_3,0);
      digitalWrite(bobina_4,0);
  }
}

/**
 * @brief 
 * Función utilizada para apagar todas las bobinas del motor PaP. 
 * Se debe colocar las bobinas correspondientes al motor.
 * @param bobina_1  Bobina Nº 1
 * @param bobina_2  Bobina Nº 2
 * @param bobina_3  Bobina Nº 3
 * @param bobina_4  Bobina Nº 4
 */
void stopMotor(int bobina_1, int bobina_2, int bobina_3, int bobina_4){
 digitalWrite(bobina_1, 0);
 digitalWrite(bobina_2, 0);
 digitalWrite(bobina_3, 0);
 digitalWrite(bobina_4, 0); 
}
