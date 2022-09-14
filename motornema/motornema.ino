const int dirPin = 23;
const int stepPin = 22;

const int steps = 200;
int stepDelay;

void setup() {
   // Marcar los pines como salida
   Serial.begin(9600);
   pinMode(dirPin, OUTPUT);
   pinMode(stepPin, OUTPUT);
}

void loop() {
   Serial.println("Activar una direccion y fijar la velocidad con stepDelay");
   digitalWrite(dirPin, HIGH);
   stepDelay = 500;
   Serial.println("Giramos 200 pulsos para hacer una vuelta completa");
   for (int x = 0; x < steps * 1; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }
   delay(1000);

   Serial.println("Cambiamos la direccion y aumentamos la velocidad");
   digitalWrite(dirPin, LOW);
   stepDelay = 500;
   Serial.println("Giramos 400 pulsos para hacer dos vueltas completas");

   for (int x = 0; x < steps * 2; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }
   delay(1000);
}