#include <Servo.h>

Servo servo;

// int min, max;

unsigned long start_time;

bool isBlackMode = false;
int count;

void Jump();

void setup() {
  Serial.begin(9600);
  servo.attach(9);

  // min = 1023;
  // max = 0;

  start_time = millis();

  servo.write(90);
}

void loop() {
  int light = analogRead(A0);
  // min = min(light, min);
  // max = max(light, max);

  if (light < 578 && !isBlackMode){
    if (count > 0) {
      isBlackMode = true;
      count = 0;
      return;
    }
    count++;
    Jump();
  } else if (light > 550 && isBlackMode) {
    if (count > 0) {
      isBlackMode = false;
      count = 0;
      return;
    }
    count++;
    Jump();
  } else {
    count = 0;
  }

  Serial.println(light);

  // delay(10);
}

void Jump() {
  unsigned long elapsedTime = (millis() - start_time) / 1000;
  float speedMultiplier = 1.0 + (elapsedTime * 0.011);
  if (speedMultiplier > 2.16) speedMultiplier = 2.16;
  delay(250 / speedMultiplier);

  servo.write(65);

  delay(700);

  servo.write(90);
}
