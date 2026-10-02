#include <Servo.h>

const int PIR = 2;
const int LED = 3;
Servo servo;

unsigned long start_time = millis();

const int open_degree = 180;
const int close_degree = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIR, INPUT);
  pinMode(LED, OUTPUT);
  servo.attach(9);
}

void loop() {
  int input = digitalRead(PIR);
  if (input == HIGH)
  {
    start_time = millis();
    servo.write(open_degree);
  }

  if (millis() - start_time < 5000)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  if (millis() - start_time > 10000)
  {
    servo.write(close_degree);
  }
    
  Serial.println(input);
  delay(100);
}
