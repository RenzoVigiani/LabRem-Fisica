
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
#define const_valores 241
//----------------------------------------------//
//------- Defino mensajes de error predeterminado
// una variable error int
uint8_t Errores = 0;
// 0 - Sin errores
// 1 - Error de distancia.
// 2 - Error de tipo de diafragma.
// 3 - Laboratorio incorrecto.

//----------------------------------------------//
//------- Defino Nombres de pines
//----------------------------------------------//
// MOTORES
//---Leds 
  #define M1_Led 12 // Indicador Motor 1
  #define M2_Led 3// Indicador Motor 2
  #define M3_Led 2 // Indicador Motor 3
//---Finales de carrera
  #define M1_SW_inicio 16 // Motor Foco 1
  #define M1_SW_fin 17    
  #define M2_SW_inicio 18 // Motor Pantalla 2
  #define M2_SW_fin 19
  #define M3_SW_inicio 20 // Motor Lente Div 3
  #define M3_SW_fin 21
//---Control de movimiento
  #define M1_Step 29
  #define M1_Direction 30
  #define M2_Step 33
  #define M2_Direction 34
  #define M3_Step 36
  #define M3_Direction 37
//---Enables
  #define M1_Enable 28
  #define M2_Enable 32
  #define M3_Enable 35
//----------------------------------------------//
//SERVOS
//---Leds 
  #define S_Diafragma_Led 5  // Indicador Servo Diafragma 1
  #define S_Lente_Led 11 // Indicador Servo Lente 2
//---Control de movimiento
  #define S_Diafragma_Pin 4 // Servo diafragma
  #define S_Lente_Pin 10 // Servo lente divergente
// Declaramos la variable para controlar el servo
  Servo servo_diafragma;
  Servo servo_lente;
//----------------------------------------------//
//FOCO - LAMPARA LED 3W
  #define Foco_pin 48 // Pin para habilitar relay de lampara.
//----------------------------------------------//
//Sentidos
  #define AntiHorario LOW
  #define Horario HIGH
// VAriables de json 
//---Estado
  int num_Lab=3; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica] 
  bool subLab=0; // 1 [SubLab 1], 0 [SubLab 2]
  bool iniLab=0;// 1 [Inicia Experimento], 0 [Finaliza Experimento]
//---Analogico
  int Analogico_0=0; // Tipo de diafragma
  int Analogico_1=0; // Distancia 1
  int Analogico_2=0; // Distancia 2
  int Analogico_3=0; // Distancia 3
//----------------------------------------------//
//--- Variables auxiliares ---//
//----------------------------------------------//
// CALCULO DE DISTANCIAS ( Considerar diferencias de distancias generadas por las bases y la separación)
  int distancia_actual_foco=10; //Distancia actual de motor de fuente de luz
  int distancia_act_lente_div=10; // Distancia actual de motor de lente divergente
  int distancia_act_pantalla=10; // Distancia actual de motor de pantalla
  int distancia_fl2_act=0; // Distancia actual entre fuente y lente convergente (fija)
  int distancia_fl1_act=0; // Distancia actual entre fuente y lente div
  int distancia_l1l2_act=0; // Distancia actual entre lente div y lente convergente (fija)
  int distancia_l2p_act=0; // Distancia actual entre lente convergente (fija) y pantalla
  int dist_mov; // distancia que se debe mover el motor en realidad. dist_mov=distancia_actual - distancia_deseada
  bool sentido=0; // indica el sentido de giro.
// CONSTANTES
#define min_real_fl1 50
#define max_real_fl1 920
#define min_real_fl2 120
#define max_real_fl2 970
#define min_real_l2p 70
#define max_real_l2p 970
#define min_real_l1l2 70
#define max_real_l1l2 900 

#define min_mot_l1 50
#define min_mot_p 50
#define min_mot_f 100
#define max_mot_l1 920
#define max_mot_p 950
#define max_mot_f 950
/*
Lente 2 = convergente
Lente 1 = divergente

Distancias reales: 
min_l1l2 = 60 mm -- uso 70 mm Real entre lente 1 y 2
max_l1l2 = 900 mm Real entre lente 1 y 2
min_l2p = 60 mm -- uso 70 mm  Real entre lente 2 y pantalla
max_l2p = 980 mm Real entre lente 2 y pantalla
min_fl1 = 40 mm -- uso 50 mm Real entre fuente y lente 1
max_fl1 = 920 mm Real entre fuente y lente 1
min_fl2 = 100 mm -- uso 120 mm Real entre fuente y lente 2
max_fl2 = 980 mm Real entre fuente y lente 2

Posiciones Motores:
min_l1 = 40 mm -- uso 50 desde el inicio de la barra
max_l1 = 940 mm -- uso 920 desde el inicio de la barra
Desplazamiento max l1= 870 mm
min_p = 40 mm -- uso 50 desde el inicio de la barra
max_p = 960 mm -- uso 950 desde el inicio de la barra
Desplazamiento max p = 900 mm
min_f = 80 mm -- uso 100 desde el inicio de la barra
max_f = 960 mm -- uso 950 desde el inicio de la barra
Desplazamiento max f = 850 mm

Condiciones:
Distancia_fl1 = pos_f - pos_l1 > 50 mm
Distancia_l1l2 = pos_l1 > 50 mm
Distancia_fl2 = pos_f > 120 mm
Distancia_l2p = pos_p > 50 mm 
*/


// BANDERAS
  bool bandera_fin_m1=0; // bandera para determinar fin de mov de motor 1
  bool bandera_fin_m2=0; // bandera para determinar fin de mov de motor 2
  bool bandera_fin_m3=0; // bandera para determinar fin de mov de motor 3
  bool bandera_vueltas=0;// sirve para hacer el conteo de vueltas. Cuando termina se activa.
  bool bandera_rep=0; // bandera para limitar la cantidad de repeticiones de mensaje de lab incorrecto 
  bool bandera_cero=0;    // indica que ya se puso a cero el motor.
// CONTADORES
  int stepDelay = 5; //delay de cada paso del motor
  int conta_pasos=0; // contador de pasos para realizar una vuelta.
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
  pinMode(M1_Step, OUTPUT);
  pinMode(M1_Direction, OUTPUT);
  pinMode(M1_Enable, OUTPUT); // Enable Motor 1
  pinMode(M1_SW_inicio,INPUT_PULLUP);
  pinMode(M1_SW_fin,INPUT_PULLUP);
// Motor 2
  pinMode(M2_Step, OUTPUT);
  pinMode(M2_Direction, OUTPUT);
  pinMode(M2_Enable, OUTPUT); // Enable Motor 2
  pinMode(M2_SW_inicio,INPUT_PULLUP);
  pinMode(M2_SW_fin,INPUT_PULLUP);
// Motor 3
  pinMode(M3_Step, OUTPUT);
  pinMode(M3_Direction, OUTPUT);
  pinMode(M3_Enable, OUTPUT); // Enable Motor 3
  pinMode(M3_SW_inicio,INPUT_PULLUP);
  pinMode(M3_SW_fin,INPUT_PULLUP);
  // Leds
  pinMode(M1_Led, OUTPUT); // Led Motor 1
  pinMode(M2_Led, OUTPUT); // Led Motor 2
  pinMode(M3_Led, OUTPUT); // Led Motor 3
  pinMode(S_Diafragma_Led, OUTPUT); // Led Servo 1
  pinMode(S_Lente_Led, OUTPUT); // Led Servo 2
 // Foco (LED)
  pinMode(Foco_pin, OUTPUT);
// Servos
  servo_diafragma.attach(S_Diafragma_Pin);
  servo_lente.attach(S_Lente_Pin);
  //------ Definir estados iniciales ------//
  busco_cero();
}

void loop() {
  //////////// Strings de comunicación /////////////
  char Mensaje_recibido[const_mje] = {0}; // Mensaje recibido por ethernet. (Comando + JSON) 
  char valores_recibidos[const_valores] = {0}; // JSON recibido.  // Wait for an incomming connection
  EthernetClient client = server.available(); 
  if(client){ // Si tengo un cliente conectado
    while (client.available()){ 
      if(bandera_rep==1) {bandera_rep=0;} //reinicio bandera de repetición cuando tengo un mje nuevo.
//      Serial.println("New Command");
      client.readBytesUntil('\r', Mensaje_recibido, sizeof(Mensaje_recibido)); // Tomo el mensaje recibido.
      strncpy(valores_recibidos,&Mensaje_recibido[15],(sizeof(Mensaje_recibido)-15)); 
      Serial.print("Mensaje Recibido"); //      Serial.println(Mensaje_recibido);   //      Serial.print("Json_Recibido: "); //      Serial.println(valores_recibidos);   
      //------ GET ----- //
      if (strstr(Mensaje_recibido, "GET /HTTP/1.1") != NULL) { // Compruebo si llega un GET, respondo valores actuales
        StaticJsonDocument<256> doc;     
        JsonArray Estado = doc.createNestedArray("Estado");
        Estado.add(num_Lab);
        Estado.add(subLab);
        Estado.add(iniLab);
        JsonArray Analogicos = doc.createNestedArray("Analogicos");
        Analogicos.add(Analogico_0);
        if(subLab){
          Analogicos.add(distancia_fl2_act); // distancia del luz a lente fijo
          Analogicos.add(distancia_l2p_act); // distancia lente fijo a pantalla
        }else{
          Analogicos.add(distancia_fl1_act); 
          Analogicos.add(distancia_l1l2_act);
          Analogicos.add(distancia_l2p_act);
        }
        JsonArray Error = doc.createNestedArray("Error");  
        Error.add(Errores);
        Serial.print(F("Sending: "));
        serializeJson(doc, Serial);
        Serial.println();
        client.println(F("HTTP/1.0 200 OK"));   // Write response headers
        client.println(F("Content-Type: application/json"));
        client.print(F("Content-Length: "));  // client.println(F("Connection: close"));
        client.println(measureJsonPretty(doc));
        client.println();
        serializeJsonPretty(doc, client);  // Write JSON document
        client.println(F("Get terminado"));  // Disconnect  //      client.stop();
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
         if(subLab){
            Analogico_1 = Analogico[1];
            Analogico_2 = Analogico[2];
            Analogico_3 = min_mot_l1;            
          }else{
            Analogico_1 = Analogico[1];
            Analogico_2 = Analogico[2];
            Analogico_3 = Analogico[3];
          } 
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
      Convergentes(Analogico_0, Analogico_1, Analogico_2);
    }
    else if (!subLab and iniLab){
      Serial.println("Sub - Laboratorio: Lentes Divergentes");  
      Divergentes(Analogico_0, Analogico_1, Analogico_2, Analogico_3);
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
      Errores=3;
    }
  }
}

/**
 * @brief Funcion utilizada para la ejecución del laboratorio de lentes convergentes
 * 
 * @param diafragma indica el tipo de diafragma a utilizar
 * @param distancia_fl2 indica la distancia entre foco y lente
 * @param distancia_l2p indica la distancia entre lente y pantalla
 */
void Convergentes(int diafragma, int distancia_fl2, int distancia_l2p){
  digitalWrite(Foco_pin, HIGH); // Enciendo FOCO
  mover_servo(servo_lente,0); // Lente desactivado
  switch (diafragma){ // Posicion del diafragma
    case 0:
      mover_servo(servo_diafragma,0);// Desplazamos a la posición 0º
      break; 
    case 1:
      mover_servo(servo_diafragma,45);// Desplazamos a la posición 45º
      break;
    case 2:
      mover_servo(servo_diafragma,90);// Desplazamos a la posición 90º
      break;
    case 3:
      mover_servo(servo_diafragma,135);// Desplazamos a la posición 135º
      break;
    case 4:
      mover_servo(servo_diafragma,180);// Desplazamos a la posición 180º
      break;
    default:
      mover_servo(servo_diafragma,0);// Desplazamos a la posición 0º
      break;
  }
  // Controlo motores
  if(!error_distancia){
    if(!bandera_fin_m1) Control_Motor(1, distancia_fl2);
    if(!bandera_fin_m2) Control_Motor(2, distancia_l2p);
    if(bandera_fin_m1 and bandera_fin_m2){bandera_vueltas=1;
    digitalWrite(M1_Led,LOW);digitalWrite(M2_Led,LOW);digitalWrite(M3_Led,LOW);}
  }
}

/**
 * @brief Funcion utilizada para la ejecución del laboratorio de lentes divergente
 * 
 * @param diafragma indica el tipo de diafragma a utilizar
 * @param distancia_fl1 indica la distancia entre foco y lente
 * @param distancia_l1l2 indica la distancia entre lente 1 y lente 2
 * @param distancia_l2p indica la distancia entre lente 2 y la pantalla
 */
void Divergentes(int diafragma, int distancia_fl1, int distancia_l1l2, int distancia_l2p){
  mover_servo(servo_lente,90); // Lente activado
  switch (diafragma){ // Posicion del diafragma
    case 0:
      mover_servo(servo_diafragma,0);// Desplazamos a la posición 0º
      break; 
    case 1:
      mover_servo(servo_diafragma,45);// Desplazamos a la posición 45º
      break;
    case 2:
      mover_servo(servo_diafragma,90);// Desplazamos a la posición 90º
      break;
    case 3:
      mover_servo(servo_diafragma,135);// Desplazamos a la posición 135º
      break;
    case 4:
      mover_servo(servo_diafragma,180);// Desplazamos a la posición 180º
      break;
    default:
      mover_servo(servo_diafragma,0);// Desplazamos a la posición 0º
      break;
  }
  // Controlo motores
  if(!error_distancia){
    if(!bandera_fin_m1) {Control_Motor(1, distancia_fl1);}
    if(!bandera_fin_m2) {Control_Motor(2, distancia_l1l2);}
    if(!bandera_fin_m3) {Control_Motor(3, distancia_l2p); }
    if(bandera_fin_m1 and bandera_fin_m2 and bandera_fin_m3){bandera_vueltas=1;
    digitalWrite(M1_Led,LOW);digitalWrite(M2_Led,LOW);digitalWrite(M3_Led,LOW);}
  }
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
      sentido = control_giro(min_mot_f,max_mot_f, distancia_actual_foco, distancia);
      if(dist_mov==0) {bandera_fin_m1 = 1; digitalWrite(M1_Led,LOW);digitalWrite(M1_Enable,HIGH);}//Serial.println("bandera Fin M1");}
      distancia_actual_foco = controlDriver(dist_mov,distancia_actual_foco,sentido,M1_Step,M1_Direction);
      digitalWrite(M1_Led,HIGH);      
      break;
    case 2:
      sentido = control_giro(min_mot_p,max_mot_p, distancia_act_pantalla, distancia);
      if(dist_mov==0) {bandera_fin_m2 = 1; digitalWrite(M2_Led,LOW);digitalWrite(M2_Enable,HIGH);}//Serial.println("bandera Fin M2");}
      distancia_act_lente_div = controlDriver(dist_mov,distancia_act_lente_div,sentido,M2_Step,M2_Direction);
      digitalWrite(M2_Led,HIGH);
      break;
    case 3:
      sentido = control_giro(min_mot_l1,max_mot_l1, distancia_act_lente_div, distancia);
      if(dist_mov==0) {bandera_fin_m3 = 1;digitalWrite(M3_Led,LOW);digitalWrite(M2_Enable,HIGH);}//Serial.println("bandera Fin M3");}
      distancia_act_pantalla = controlDriver(dist_mov,distancia_act_pantalla,sentido,M3_Step,M3_Direction);
      digitalWrite(M3_Led,HIGH);
      break;
    default:
      Serial.println("El motor no existe");
      break;
  }
}

/**
 * @brief Se realiza el control de distancias permitidas en la medición.
 * Devuelve una variable buleana indicando si hay error en los valores de distancia. 
 * @return bool indica si hay error en la distancia.  variable Errores = 1;
 */
bool error_distancia(){
//Condiciones:
  if(Analogico_1 < 120){Errores=1;} //Distancia_fl2 = pos_f > 120 mm
  if(Analogico_2 < 50){Errores=1;} //Distancia_l2p = pos_p > 50 mm 
  if(Analogico_3 < 50){Errores=1;} //Distancia_l1l2 = pos_l1 > 50 mm
  if((Analogico_3 - Analogico_1)<50){Errores=1;} // Distancia_fl1 = pos_f - pos_l1 > 50 mm
  if(Errores==1){return false;}
  return true;
}

/**
 * @brief 
 * Funcion utilizada para determinar el sentido de giro del motor.
 * Depende de la posición actual del motor.
   true = Indica que el sentido es horario 
 * false = Indica que el sentido es antihorario
 * @param limite_inferior_riel Limite inferior de posición de cada motor.
 * @param limite_superior_riel Limite superior de posición de cada motor.
 * @param distancia_act Distancia actual del motor indicado
 * @param distancia distancia requerida.
 * @return bool Indica el sentido de giro 
 */
bool control_giro(int limite_inferior_riel, int limite_superior_riel,int distancia_act, int distancia){
  if (distancia >= limite_inferior_riel and distancia <= limite_superior_riel){ // maximo movimiento es 100 mm
    if (distancia_act > distancia){sentido = false;dist_mov = (distancia_act - distancia);}    
    if (distancia_act < distancia){sentido = true;dist_mov = (distancia - distancia_act);}
    if (distancia_act == distancia) dist_mov = 0;    
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
  int pasos= dist_mov * factor_vueltas;
  if((pasos == 0)){digitalWrite(step,LOW);}
  else if(pasos>0){
    mover_motor(dir, step, sentido, 100);  
    conta_pasos++;
    if(sentido){// giro positivo
      if(conta_pasos == 200){ aux_dist_actual++; conta_pasos=0;}// sumo cuenta distancia
    }else{      //giro negativo
      if(conta_pasos == 200){ aux_dist_actual--; conta_pasos=0;}// resto cuenta distancia
    }
  }
  return aux_dist_actual;
}

/**
 * @brief Funcion utilizada para llevar los motores y los servos a la posición inicial 
 * 
 **/
void busco_cero(){
  while (!bandera_cero)
  {
//    if(servo_diafragma.read()!=0){ mover_servo(servo_diafragma,0); }
//    if(servo_lente.read()!=0){ mover_servo(servo_lente,0); }
    mover_servo(servo_diafragma,0);
    mover_servo(servo_lente,0);
    if(!digitalRead(M1_SW_inicio)){ distancia_actual_foco=0; Serial.println("Motor 1 puesto a cero");
    }else{
      if(distancia_actual_foco!=0){
        digitalWrite(M1_Enable,LOW);
        mover_motor(M1_Direction,M1_Step,AntiHorario,75);
        digitalWrite(M1_Led,HIGH);
      }else{digitalWrite(M1_Led,LOW);digitalWrite(M1_Enable,HIGH);}
    }
    if(!digitalRead(M2_SW_inicio)){ distancia_act_lente_div=0; Serial.println("Motor 2 puesto a cero");
    }else{
      if(distancia_act_lente_div!=0){
        digitalWrite(M2_Enable,LOW);
        mover_motor(M2_Direction,M2_Step,AntiHorario,75);
        digitalWrite(M2_Led,HIGH);
      }else{digitalWrite(M2_Led,LOW);digitalWrite(M2_Enable,HIGH);}
    }
    if(!digitalRead(M3_SW_inicio)){ distancia_act_pantalla=0; Serial.println("Motor 3 puesto a cero");
    }else{ 
      if(distancia_act_pantalla!=0){
        digitalWrite(M3_Enable,LOW);
        mover_motor(M3_Direction,M3_Step,AntiHorario,75);
        digitalWrite(M3_Led,HIGH);
      }else{digitalWrite(M3_Led,LOW);digitalWrite(M3_Enable,HIGH);}
    }
    if(distancia_actual_foco==0 and distancia_act_lente_div==0 and distancia_act_pantalla==0 and servo_diafragma.read()==0 and servo_lente.read()==0) {
      digitalWrite(M1_Led,LOW);digitalWrite(M2_Led,LOW);digitalWrite(M3_Led,LOW);
      digitalWrite(M1_Enable,HIGH);digitalWrite(M2_Enable,HIGH);digitalWrite(M3_Enable,HIGH);
      bandera_cero=true;
    }
  }
}

/**
 * @brief Funcion utilizada para mover cada motor
 * 
 * @param dir Indica el pin de dirección del motor seleccionado.
 * @param step Indica el pin de paso del motor seleccionado.
 * @param sentido Indica el sentido de giro del motor seleccionado.
 * @param velocidad Indica la velocidad de giro. Es porcentual. 0 - 100%. Si es cero el motor se para.
*/
void mover_motor(int dir, int step, int sentido, int velocidad){
  stepDelay = 1000 + (10000 -(velocidad*100)) ;
  if(velocidad !=0){
    digitalWrite(dir,sentido);  
    digitalWrite(step, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(step, LOW);
    delayMicroseconds(stepDelay);
  }
}

/**
 * @brief Funcion utilizada para mover el servo de forma lenta
 * 
 * @param servo_sel Es el servo seleccionado.
 * @param ang_sel Es el angulo requerido a mover.
*/
void mover_servo(Servo servo_sel,int ang_sel){
  int ang_actual = servo_sel.read(); // angulo actual 
  if(ang_actual < ang_sel){
    servo_sel.write(ang_actual+1);
  }
  if(ang_actual > ang_sel){
    servo_sel.write(ang_actual-1);
  }
  delay(1);
}
