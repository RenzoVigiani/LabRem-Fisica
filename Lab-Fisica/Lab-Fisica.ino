/*
/////////////////////
Procedimiento:
Enviar GET (para saber estado actual)
Enviar POST(para asignar datos)
Arduino responde  "HTTP/1.1 200 OK"
luego enviar GET hasta que arduino responda(si no se conecta es porque está trabajando en el pedido del post)
*/
#include <ArduinoJson.h>
#include <EtherCard.h>
#include <Servo.h>

#define const_status 500
#define const_instruc 500

static byte mymac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static byte myip[] = {172,20,5,140};
byte Ethernet::buffer[const_status];
//--------- Declaración de variables ----------//
//---- VAriables de json ----// (GENERICO)
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

//---------- Funciones  ---------------//
/**
 * @brief 
 * 
 * @param diafragma 
 * @param cant_med 
 * @param distancia_fl 
 * @param distancia_lp 
 */
void Convergentes(int diafragma, int cant_med, int distancia_fl, int distancia_lp);
void Divergentes(int diafragma, int cant_med, int distancia_fl1, int distancia_l1l2, int distancia_l2p);
void Control_Motor(int motor, int distancia);
bool control_distancia(int distancia_act, int distancia);
int  Mover_Motor(int bobina_1, int bobina_2, int bobina_3, int bobina_4, int dist_mov, int aux_dist, bool sentido);
void valorSalidas(int selector,int bobina_1, int bobina_2, int bobina_3, int bobina_4);
void stopMotor(int bobina_1, int bobina_2, int bobina_3, int bobina_4);

//////////// declaración de salidas ///////////////////
//---------------- Motores --------------------------//
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

void setup() {
  // Initialize Arduino server parameters
  ether.hisport=22; // Definimos el puerto
  // Initialize serial 0
  Serial.begin(250000);
  // Initialize Ethernet libary
  Serial.println("Inicializando Conexion Ethernet"); 
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 53)) // se controla acceso al modulo y se guarda datos en buffer 
    Serial.println( "No se ha podido acceder a la controlador Ethernet");
  else
   Serial.println("Controlador Ethernet inicializado");
  if (!ether.staticSetup(myip))
    Serial.println("No se pudo establecer la dirección IP");
  Serial.println();
  // declaro salidas para motor 1
  pinMode(IN1_1, OUTPUT);
  pinMode(IN2_1, OUTPUT);
  pinMode(IN3_1, OUTPUT);
  pinMode(IN4_1, OUTPUT);
  // declaro salidas para motor 2
  pinMode(IN1_2, OUTPUT);
  pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT);
  pinMode(IN4_2, OUTPUT);
  // declaro salidas para motor 3
  pinMode(IN1_3, OUTPUT);
  pinMode(IN2_3, OUTPUT);
  pinMode(IN3_3, OUTPUT);
  pinMode(IN4_3, OUTPUT);
  // declaro Leds
  pinMode(Led_M1, OUTPUT); // Led Motor 1
  pinMode(Led_M2, OUTPUT); // Led Motor 2
  pinMode(Led_M3, OUTPUT); // Led Motor 3
  pinMode(Led_aux, OUTPUT);// Led Aux
  pinMode(Led_S1, OUTPUT); // Led Servo 1
  pinMode(Led_S2, OUTPUT); // Led Servo 2
  // Declaro salida de Foto (LED)
  pinMode(Foco_pin, OUTPUT);
  // Declaro salidas Servos
  servo_diafragma.attach(Diafragma_pin);
  servo_lente.attach(Lente_pin);
  //------ Definir estados iniciales ------//
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
  "{\"Estado\":[$D,$D,$D],\"Analogico\":[$D,$D,$D,$D,$D]}"),num_Lab,subLab,iniLab,
  Analogico_0,Analogico_1,Analogico_2,Analogico_3,Analogico_4);
  return bfill.position();
}

void loop() 
{
  //////////// Strings de comunicación /////////////
  char status[const_status] = {0};
  char instrucciones[const_instruc] = {"Hola"};
//  bool bandera=0;
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len); 
  if(pos) { 
    if(strstr((char *)Ethernet::buffer + pos, "GET / HTTP/1.1") != 0) {
      Serial.println("Comando GET recibido");
      ether.httpServerReply(getpage()); // se envia OK y datos necesarios
    }
    if(strstr((char *)Ethernet::buffer + pos, "POST / HTTP/1.1") != 0) {
      Serial.println("Comando POST recibido");
      //obtengo las instrucciones del formato json
      strcpy(status,(char *)Ethernet::buffer + pos);
      strncpy(instrucciones,&status[15],sizeof(status)-15);
      bandera=1;
      ether.httpServerReply(postpage()); // se envia OK
    }        
    if(bandera)
    {
      post_json(instrucciones);
      bandera=0;
    }
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
    JsonArray Analogico = doc["Analogico"];
    Analogico_0 = Analogico[0];
    Analogico_1 = Analogico[1];
    Analogico_2 = Analogico[2];
    Analogico_3 = Analogico[3];
    Analogico_4 = Analogico[4];

    if (subLab and iniLab)
    {
      Serial.println("Sub - Laboratorio: Lentes convergentes"); 
      Convergentes(Analogico_0, Analogico_1, Analogico_2,Analogico_3);
    }
    else if (!subLab and iniLab)
    {
      Serial.println("Sub - Laboratorio: Lentes Divergentes");  
      Divergentes(Analogico_0, Analogico_1, Analogico_2, Analogico_3,Analogico_4);
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

void Convergentes(int diafragma, int cant_med, int distancia_fl, int distancia_lp)
{
  Serial.println("Convergentes");
  digitalWrite(Foco_pin, HIGH); // Enciendo FOCO
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
  Control_Motor(1, distancia_fl);
  Control_Motor(2, distancia_lp);    
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
  Control_Motor(1, distancia_fl1);
  Control_Motor(2, distancia_l1l2);
  Control_Motor(3, distancia_l2p);
}

void Control_Motor(int motor, int distancia)
{
  bool sentido=true;
  switch (motor)  // se controla motor a mover
  {
    case 1:
      sentido = control_distancia(distancia_act_1, distancia);
      Serial.print("Distancia requerida Motor 1: ");
      Serial.println(distancia);
      Serial.print("Distancia actual: ");
      Serial.println(distancia_act_1);
      distancia_act_1 = Mover_Motor(IN1_1, IN2_1, IN3_1, IN4_1, dist_mov, distancia_act_1, sentido);
      break;
    case 2:
      sentido = control_distancia(distancia_act_2, distancia);
      Serial.print("Distancia requerida Motor 2: ");
      Serial.println(distancia);
      Serial.print("Distancia actual: ");
      Serial.println(distancia_act_2);
      distancia_act_2 = Mover_Motor(IN1_2, IN2_2, IN3_2, IN4_2, dist_mov, distancia_act_2, sentido);
      break;
    case 3:
      sentido = control_distancia(distancia_act_3, distancia);
      Serial.print("Distancia requerida Motor 3: ");
      Serial.println(distancia);
      Serial.print("Distancia actual: ");
      Serial.println(distancia_act_3);
      distancia_act_3 = Mover_Motor(IN1_3, IN2_3, IN3_3, IN4_3, dist_mov, distancia_act_3, sentido);
      break;
    default:
      Serial.println("El motor no existe");
      break;
  }
}

bool control_distancia(int distancia_act, int distancia)
{
  bool sentido = true; // true = derecha, false = izquierda
  if (distancia > 0 and distancia < 100) // maximo movimiento es 100 mm
  {
    if (distancia_act > distancia) sentido = false; dist_mov = (distancia_act - distancia);     
    if (distancia_act < distancia) sentido = true;  dist_mov = (distancia - distancia_act);
    if (distancia_act == distancia) dist_mov = 0;    
  }
  else Serial.println("Distancia no permitida");
  return sentido;
}

int Mover_Motor(int bobina_1, int bobina_2, int bobina_3, int bobina_4, int dist_mov, int aux_dist, bool sentido)
{
  const int factor_vueltas = 128; //512 - una vuelta entera.
  int vueltas= dist_mov * factor_vueltas;

  while((vueltas)>=0)
  {
    if(sentido and (vueltas > 0))
    {
      for(int i=0;i<8;i++)
      {
        valorSalidas(i,bobina_1,bobina_2,bobina_3,bobina_4);
        delay(2);
      }            
      if( (vueltas % factor_vueltas) == 0)
      {
        aux_dist = aux_dist + 1; 
        Serial.print("Distancia actual: ");
        Serial.println(aux_dist);
      } 
    }
    if(sentido and (vueltas <= 0))      
    {  
      stopMotor(bobina_1,bobina_2,bobina_3,bobina_4);
    }    
    if(!sentido and (vueltas > 0))
    {
      for(int i=7;i>=0;i--)
      {
        valorSalidas(i,bobina_1,bobina_2,bobina_3,bobina_4);
        delay(2);
      }
      if( (vueltas % factor_vueltas) == 0)
      {
        aux_dist = aux_dist - 1; 
        Serial.print("Distancia actual: ");
        Serial.println(aux_dist);
      }            
    }    
    if(!sentido and (vueltas <= 0))      
    {  
      stopMotor(bobina_1,bobina_2,bobina_3,bobina_4);
    }        
    vueltas--;    
    }
    return aux_dist;
  }

void valorSalidas(int selector,int bobina_1, int bobina_2, int bobina_3, int bobina_4)
{ // Salidas Motor
  switch (selector) 
  {
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

void stopMotor(int bobina_1, int bobina_2, int bobina_3, int bobina_4)
{
 digitalWrite(bobina_1, 0);
 digitalWrite(bobina_2, 0);
 digitalWrite(bobina_3, 0);
 digitalWrite(bobina_4, 0); 
}
