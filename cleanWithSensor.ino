const int frontEchoPin = 7;
const int frontTriggerPin = 6;
const int leftEchoPin = 11;
const int leftTriggerPin = 10;
const int rightEchoPin = 9;
const int rightTriggerPin = 8;
const int en_A = 12;
const int en_B = 13;
const int motorL1 = 2;
const int motorL2 = 3;
const int motorR1 = 4;
const int motorR2 = 5;
volatile float maxFrontDistance = 25.00;
volatile float frontDuration, frontDistanceCm, leftDuration, leftDistanceCm, rightDuration, rightDistanceCm;
volatile float maxLeftDistance, maxRightDistance = 20.00;
void setup() {
  // serial
  Serial.begin(9600);
  // ultrasonic
  pinMode(frontTriggerPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(leftTriggerPin, OUTPUT);
  pinMode(leftEchoPin, INPUT);
  pinMode(rightTriggerPin, OUTPUT);
  pinMode(rightEchoPin, INPUT);
  // motors
  pinMode(en_A, OUTPUT);
  pinMode(en_B, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
}
void loop() {
  // front distance check
  checkFrontDistance();
  if (frontDistanceCm < maxFrontDistance) {
    Serial.println("Too close");
    checkLeftDistance();
    delay(20);
    checkRightDistance();
    delay(20);
    if (leftDistanceCm < rightDistanceCm)
      moveRight();
    else if (leftDistanceCm > rightDistanceCm) {
      moveLeft();
    }
  }
  else {
    Serial.println("OK");
    moveForward();
  }
  // left distance check
  checkLeftDistance();
  if (leftDistanceCm < maxLeftDistance) {
    Serial.println("Left too close");
    delay(20);
    checkLeftDistance();
    delay(20);
    checkRightDistance();
    delay(20);
    if (leftDistanceCm > rightDistanceCm)
      moveForward();
    else if (leftDistanceCm < rightDistanceCm) {
      moveRight();
    }
  }
  // right distance check
  checkRightDistance();
  if (rightDistanceCm < maxRightDistance) {
    Serial.println("Right too close");
    delay(20);
    checkRightDistance();
    delay(20);
    checkLeftDistance();
    delay(20);
    if (rightDistanceCm > leftDistanceCm)
      moveForward();
    else if (rightDistanceCm < leftDistanceCm) {
      moveLeft();
    }
  }
}
void checkFrontDistance() {
  digitalWrite(frontTriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(frontTriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(frontTriggerPin, LOW);
  frontDuration = pulseIn(frontEchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  frontDistanceCm = frontDuration * 10 / 292 / 2;  //convertimos a distancia, en cm
  Serial.print("Distance: ");
  Serial.print(frontDistanceCm);
  Serial.println(" cm");
}
void checkLeftDistance() {
  digitalWrite(leftTriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(leftTriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(leftTriggerPin, LOW);
  leftDuration = pulseIn(leftEchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  leftDistanceCm = leftDuration * 10 / 292 / 2;  //convertimos a distancia, en cm
  Serial.print("Left distance: ");
  Serial.print(leftDistanceCm);
  Serial.println(" cm");
}
void checkRightDistance() {
  digitalWrite(rightTriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(rightTriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(rightTriggerPin, LOW);
  rightDuration = pulseIn(rightEchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  rightDistanceCm = rightDuration * 10 / 292 / 2;  //convertimos a distancia, en cm
  Serial.print("Right distance: ");
  Serial.print(rightDistanceCm);
  Serial.println(" cm");
}
void moveBackward() {
  Serial.println("Backward.");
  analogWrite(en_A, 130);
  analogWrite(en_B, 130);
  analogWrite(motorL1, 130);
  analogWrite(motorL2, LOW);
  analogWrite(motorR1, 130);
  analogWrite(motorR2, LOW);
}
void moveForward() {
  Serial.println("Forward.");
  analogWrite(en_A, 130);
  analogWrite(en_B, 130);
  analogWrite(motorL1, 0);
  analogWrite(motorL2, 130);
  analogWrite(motorR1, 0);
  analogWrite(motorR2, 130);
}
void moveLeft() {
  Serial.println("Left.");
  analogWrite(en_A, 130);
  analogWrite(en_B, 130);
  analogWrite(motorL1, 0);
  analogWrite(motorL2, 130);
  analogWrite(motorR1, 130);
  analogWrite(motorR2, 0);
}
void moveRight() {
  Serial.println("Right.");
  analogWrite(en_A, 130);
  analogWrite(en_B, 130);
  analogWrite(motorL1, 13);
  analogWrite(motorL2, 0);
  analogWrite(motorR1, 0);
  analogWrite(motorR2, 130);
}