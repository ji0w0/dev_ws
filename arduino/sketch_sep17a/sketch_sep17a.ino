#include <Servo.h>

const int X_AXIS = A0;
const int Y_AXIS = A1;
const int ZERO_POS = 512;
const int SERVO = 9;

Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO);
}

void loop() {
  int x = analogRead(X_AXIS);
  int y = analogRead(Y_AXIS);

  x = x - ZERO_POS;
  y = (y - ZERO_POS) * -1;

  double degree = atan2(y, x) * 180 / PI;
  if (degree < 0)
    degree *= -1;

  degree = 180 - degree;
  servo.write((int)degree);

  // inst x_pos = map(x, ZERO_POS * -1, ZERO_POS, 180, 0);
  // servo.write(x_pos);

  // Serial.print(x);
  // Serial.print(", ");
  // Serial.println(y);
  // Serial.println(x_pos);
  Serial.println(degree);

  delay(100);
}
