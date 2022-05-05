#include <ArduinoJson.h>
#include <UIPEthernet.h>
#include <Servo.h>

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
  int distancia_act=0;

////////////// Funciones  ////////////////////
void Convergentes(bool diafragma, int distancia_fl, int distancia_lp, int cant_med);
void Divergentes(int distancia_fl1, int distancia_l1l2, int distancia_l2p, int cant_med);
void Control_Motor(int motor, int distancia);
void Mover_Motor(int bobina_1, int bobina_2, int bobina_3, int bobina_4, int dist_mov, bool sentido);
void valorSalidas(int selector,int bobina_1, int bobina_2, int bobina_3, int bobina_4);
void stopMotor(int bobina_1, int bobina_2, int bobina_3, int bobina_4);


//////////// declaración de salidas ///////////////////
//---------------- Motores --------------------------//

// Motor 1
#define IN1_1  3
#define IN2_1  4
#define IN3_1  5
#define IN4_1  6
// Motor 2
#define IN1_2  7
#define IN2_2  8
#define IN3_2  9
#define IN4_2  10
// Motor 3
#define IN1_3  11
#define IN2_3  12
#define IN3_3  13
#define IN4_3  14

// Declaramos la variable para controlar el servo
Servo servo_diafragma;
Servo servo_lente;

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
  pinMode(IN1_1, OUTPUT);
  pinMode(IN2_1, OUTPUT);
  pinMode(IN3_1, OUTPUT);
  pinMode(IN4_1, OUTPUT);
/*    // declaro salidas para motor 2
  pinMode(IN1_2, OUTPUT);
  pinMode(bobina_2_2, OUTPUT);
  pinMode(Ibobina_32, OUTPUT);
  pinMode(INbobina_4, OUTPUT);
    // declaro salidas para motor 3
  pinMode(IN1_3, OUTPUT);
  pinMode(bobina_2_3, OUTPUT);
  pinMode(Ibobina_33, OUTPUT);
  pinMode(INbobina_4, OUTPUT);
  }
*/
  // Iniciamos el servo para que empiece a trabajar con el pin 9
  servo_diafragma.attach(9);
}

void loop() 
{
  //////////// Strings de comunicación /////////////
  char status[120] = {0};
  char instrucciones[100] = {0};
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
// client.stop();
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
        if (subLab and iniLab)
        {
          Serial.println("Sub - Laboratorio: Lentes convergentes"); 
          Convergentes(SW_0, variable_0, variable_1, variable_2);
        }
        else if (!subLab and iniLab)
        {
          Serial.println("Sub - Laboratorio: Lentes Divergentes");  
          Divergentes(variable_0, variable_1, variable_2, variable_3);
        }
      }
      else
      {
        Serial.println("Laboratorio incorrecto");    
      }
      // Disconnect
//      client.stop();
    }
  }
}

void Convergentes(bool diafragma, int distancia_fl, int distancia_lp, int cant_med)
{
  Serial.println("Convergentes");
  if(diafragma)
  {
    servo_diafragma.write(0);  // Desplazamos a la posición 0º
    delay(1000);
  }
  else
  {
    servo_diafragma.write(90); // Desplazamos a la posición 90º
    delay(1000);
  }    
  Control_Motor(1, distancia_fl);
  variable_0 = distancia_act;
  delay(1000);  
  Control_Motor(1, distancia_lp);    
  variable_1 = distancia_act;
  variable_2 = cant_med + 3;
}

void Divergentes(int distancia_fl1, int distancia_l1l2, int distancia_l2p, int cant_med)
{
  Serial.println("Divergentes");
  variable_0 = distancia_fl1;
  variable_1 = distancia_l1l2 + 1;
  variable_2 = distancia_l2p + 2;
  variable_3 = cant_med + 3;
}

void Control_Motor(int motor, int distancia)
{
  bool sentido = true; // true = derecha, false = izquierda
  int dist_mov = 0; // distancia que se debe mover el motor en realidad
  if (distancia > 0 and distancia < 100) // maximo movimiento es 100 mm
  {
    if (distancia_act > distancia) // tiene que retroceder
    {
      sentido = false;
      dist_mov = (distancia_act - distancia); 

    } 
    if (distancia_act < distancia) // tiene que avanzar
    {
      sentido = true; 
      dist_mov = (distancia - distancia_act);
    }
    Serial.print("Distancia requerida: ");
    Serial.println(distancia);
//    distancia_act = distancia;  

  }
  else
  {
    Serial.println("Distancia no permitida");
  }
  switch (motor)  // se controla motor a mover
  {
    case 1:
      Mover_Motor(IN1_1, IN2_1, IN3_1, IN4_1, dist_mov, sentido);
      break;
    case 2:
      Mover_Motor(IN1_2, IN2_2, IN3_2, IN4_2, dist_mov, sentido);
      break;
    case 3:
      Mover_Motor(IN1_3, IN2_3, IN3_3, IN4_3, dist_mov, sentido);
      break;
    default:
      Serial.println("El motor no existe");
      break;
  }
}

void Mover_Motor(int bobina_1, int bobina_2, int bobina_3, int bobina_4, int dist_mov, bool sentido)
{
  const int factor_vueltas = 50; //512
  int vueltas= dist_mov * factor_vueltas;

  while((vueltas)>=0)
  {
    if(sentido and (vueltas > 0))
    {
      for(int i=0;i<8;i++)
      {
        valorSalidas(i,bobina_1,bobina_2,bobina_3,bobina_4);
        delay(5);
      }            
      if( (vueltas % factor_vueltas) == 0)
      {
        distancia_act = distancia_act + 1; 
        Serial.print("Distancia actual: ");
        Serial.println(distancia_act);
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
        delay(5);
      }
      if( (vueltas % factor_vueltas) == 0)
      {
        distancia_act = distancia_act - 1; 
        Serial.print("Distancia actual: ");
        Serial.println(distancia_act);
      }            
    }    
    if(!sentido and (vueltas <= 0))      
    {  
      stopMotor(bobina_1,bobina_2,bobina_3,bobina_4);
    }        
    vueltas--;    
    }
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