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
  #define step1 6
  #define dir1 7
// Motor 2
  #define step2 6
  #define dir2 7
// Motor 3
  #define step3 6
  #define dir3 7
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
  int v_time = 0;
  const int ledPin = 3;
  const int limite_inferior_riel=0; // Indica el minimo valor que puede tomar distancia
  const int limite_superior_riel=900; // indica el minimo valor que puede tomar distancia.
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
  pinMode(step1, OUTPUT);
  pinMode(dir1, OUTPUT);
// Motor 2
  pinMode(step2, OUTPUT);
  pinMode(dir2, OUTPUT);
// Motor 3
  pinMode(step3, OUTPUT);
  pinMode(dir3, OUTPUT);
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
    while (client.available()){ 
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
        if (error){ // Analizo posibles errores.
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        JsonArray Estado = doc["Estado"];
        num_Lab = Estado[0]; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica]
        subLab = Estado[1]; // 1 [SubLab 1], 0 [SubLab 2]
        iniLab = Estado[2]; // 1 [Inicia Experimento], 0 [Finaliza Experimento]

        if(num_Lab==3){ // Control de numero de lab.
          JsonArray Analogico = doc["Analogico"];
          Analogico_0 = Analogico[0];
          Analogico_1 = Analogico[1];
          Analogico_2 = Analogico[2];
          Analogico_3 = Analogico[3];
          Analogico_4 = Analogico[4];
        }
      }
    }
  }
  else{ // Si no está el cliente enviando algo, sigo haciendo lo que corresponde.
      Control();
  }
}
/**
 * @brief Funcion utilizada para el control de laboratorios. "Se analiza las variables de entrada y se ejecuta lo necesario"
 * 
 */
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
      if(bandera_rep==0){
        Serial.println("Laboratorio Parado");
        bandera_rep=1;
      }
    }
  }
  else{
    if(bandera_rep==0){
      Serial.println("Laboratorio incorrecto");  
      bandera_rep = 1; 
      Errores=4;
    }
  }
//  hacer();
//  delay(500);
}
/*
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
*/

/**
 * @brief Funcion utilizada para la ejecución del laboratorio de lentes convergentes
 * 
 * @param diafragma indica el tipo de diafragma a utilizar
 * @param cant_med indica la cantidad de mediciones
 * @param distancia_fl indica la distancia entre foco y lente
 * @param distancia_lp indica la distancia entre lente y pantalla
 */
void Convergentes(int diafragma, int cant_med, int distancia_fl, int distancia_lp){
  digitalWrite(Foco_pin, HIGH); // Enciendo FOCO
  switch (diafragma){ // Posicion del diafragma
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

/**
 * @brief Funcion utilizada para la ejecución del laboratorio de lentes divergente
 * 
 * @param diafragma indica el tipo de diafragma a utilizar
 * @param cant_med indica la cantidad de mediciones
 * @param distancia_fl1 indica la distancia entre foco y lente
 * @param distancia_l1l2 indica la distancia entre lente 1 y lente 2
 * @param distancia_l2p indica la distancia entre lente 2 y la pantalla
 */
void Divergentes(int diafragma, int cant_med, int distancia_fl1, int distancia_l1l2, int distancia_l2p){
  Serial.println("Divergentes");
  switch (diafragma){ // Posicion del diafragma
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
  if(!bandera_fin_m1) Control_Motor(1, distancia_fl1);
  if(!bandera_fin_m2) Control_Motor(2, distancia_l1l2);
  if(!bandera_fin_m3) Control_Motor(3, distancia_l2p); 
  if(bandera_fin_m1 and bandera_fin_m2 and bandera_fin_m3) bandera_vueltas=1;
}

/**
 * @brief 
 * Función utilizada para el sistema completo de manejo de motor en base a la distancia requerida.
 * @param motor  Indica el numero del motor a utilizar
 * @param distancia indica la distancia requerida para el motor seleccionado
 */
void Control_Motor(int motor, int distancia){
  bool sentido=true;
  switch (motor){ // se controla motor a mover
    case 1:
      sentido = control_distancia(distancia_act_1, distancia);
      if(dist_mov==0) {bandera_fin_m1 = 1; }//Serial.println("bandera Fin M1");}
      Serial.println("Distancia requerida Motor 1: " + (String)distancia);
      Serial.println("Distancia actual Motor 1: " + (String)distancia_act_1);
      distancia_act_1 = controlDriver(dist_mov,distancia_act_1,sentido,step1,dir1);
      
      break;
    case 2:
      sentido = control_distancia(distancia_act_2, distancia);
      if(dist_mov==0) {bandera_fin_m2 = 1;}//Serial.println("bandera Fin M2");}
      Serial.println("Distancia requerida Motor 2: " + (String)distancia);
      Serial.println("Distancia actual Motor 2: " + (String)distancia_act_2);
      distancia_act_2 = controlDriver(dist_mov,distancia_act_2,sentido,step2,dir2);
      break;
    case 3:
      sentido = control_distancia(distancia_act_3, distancia);
      if(dist_mov==0) {bandera_fin_m3 = 1;}//Serial.println("bandera Fin M3");}
      Serial.println("Distancia requerida Motor 3: " + (String)distancia);
      Serial.println("Distancia actual Motor 3: " + (String)distancia_act_3);
      distancia_act_3 = controlDriver(dist_mov,distancia_act_3,sentido,step3,dir3);
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
bool control_distancia(int distancia_act, int distancia){
//  bool sentido = true; // true = derecha, false = izquierda
  if (distancia >= limite_inferior_riel and distancia <= limite_superior_riel){ // maximo movimiento es 100 mm
    if (distancia_act > distancia){sentido = false;dist_mov = (distancia_act - distancia);}    
    if (distancia_act < distancia){sentido = true;dist_mov = (distancia - distancia_act);}
    if (distancia_act == distancia) dist_mov = 0;    
//    Serial.println("Sentido = "+String(sentido));
  }
  else{ Serial.println("Distancia no permitida"); Errores=1;}
  return sentido;
}

/**
 * @brief Adaptación de valor salidas para driver Drv8825
 * 
 * @param dist_mov Distancia que debe moverse
 * @param aux_dist_actual distancia actual que tiene el motor
 * @param sentido indica el sentido de giro
 * @param step pin de salida de paso del motor
 * @param dir pin de salida de dirección del motor
 * @return int - devuelve el valor de la distancia actual
 */
int controlDriver(int dist_mov, int aux_dist_actual, bool sentido,int step, int dir){ 
  const int factor_vueltas = 200; //200 - una vuelta entera. (0.7 mm de paso)
  int vueltas= dist_mov * factor_vueltas;
  if((vueltas == 0)){
    digitalWrite(step,0);
    Serial.println("Vueltas=0");
  }
  if(vueltas>0){
    digitalWrite(dir,sentido);
    if(sentido){// giro positivo
      if(digitalRead(step)==0) digitalWrite(step,1);
      if( (vueltas % factor_vueltas) == 0) aux_dist_actual++; // sumo cuenta distancia
    }else{      //giro negativo
      if(digitalRead(step)==1) digitalWrite(step,0);
      if( (vueltas % factor_vueltas) == 0) aux_dist_actual--;// resto cuenta distancia
    }
    delay(2);
  }
  return aux_dist_actual;
}
