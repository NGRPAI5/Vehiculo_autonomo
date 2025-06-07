#include <Servo.h>

const int Rmotor_forward_pin = 3;
const int Rmotor_backward_pin = 2;
const int Rmotor_speed_pin = 10;

const int servoPin = 9;
Servo steeringServo;

const int triggerPin = 11;
const int echoPin = 7;

const int leftIRPin = A0;
const int rightIRPin = A1;
const int leftIRthreshold = 200;
const int rightIRthreshold = 200;

const int brakingDistance = 10;

const int lightSensorPin = A2;
const int lightPin = 12;
const int lightPin2 = 13; 
const int lightThreshold = 500;

int speedR = 255;
bool stoppedRecently = false;

void setup() {
  pinMode(Rmotor_forward_pin, OUTPUT);
  pinMode(Rmotor_backward_pin, OUTPUT);
  pinMode(Rmotor_speed_pin, OUTPUT);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(lightSensorPin, INPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(lightPin2, OUTPUT);

  steeringServo.attach(servoPin);
  steeringServo.write(80);

  Serial.begin(9600);
}

void loop() {
  int leftIR = analogRead(leftIRPin);
  int rightIR = analogRead(rightIRPin);

  int lightLevel = analogRead(lightSensorPin);

  if (lightLevel > lightThreshold) {
    digitalWrite(lightPin, HIGH);  
    digitalWrite(lightPin2, HIGH); 
  } else {
    digitalWrite(lightPin, LOW);   
    digitalWrite(lightPin2, LOW);
  }

  long distance = readUltrasonic();

  if (distance > 0 && distance <= brakingDistance) {
    stopMotors();
    Serial.println("Obstáculo detectado. Deteniendo...");
    return;
  }

  if (leftIR < leftIRthreshold && rightIR < rightIRthreshold) {
    if (!stoppedRecently) {
      stopMotors();
      steeringServo.write(80);  
      Serial.println("Línea detectada en ambos sensores. Deteniendo por 5 segundos...");
      delay(5000);
      stoppedRecently = true;
    } else {
      moveForward();
      steeringServo.write(80);  
      Serial.println("Línea aún detectada, pero continuando el avance...");
    }
  } else {
    stoppedRecently = false;

    if (leftIR < leftIRthreshold) {
      moveForward();
      steeringServo.write(30);  
      Serial.println("Girando a la izquierda...");
    } else if (rightIR < rightIRthreshold) {
      moveForward();
      steeringServo.write(110);  
      Serial.println("Girando a la derecha...");
    } else {
      moveForward();
      steeringServo.write(80);  
      Serial.println("Avanzando recto...");
    }
  }

  Serial.print("IR Izquierdo: "); Serial.print(leftIR);
  Serial.print(" | IR Derecho: "); Serial.print(rightIR);
  Serial.print(" | Distancia: "); Serial.print(distance);
  Serial.print(" | Nivel de luz: "); Serial.println(lightLevel);
}

void moveForward() {
  digitalWrite(Rmotor_forward_pin, HIGH);
  digitalWrite(Rmotor_backward_pin, LOW);
  analogWrite(Rmotor_speed_pin, speedR);
}

void stopMotors() {
  digitalWrite(Rmotor_forward_pin, LOW);
  digitalWrite(Rmotor_backward_pin, LOW);
  analogWrite(Rmotor_speed_pin, 0);
}

long readUltrasonic() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration / 29 / 2;
}
