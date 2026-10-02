#include <Servo.h>

const int BUTTON = 2;
Servo servo;

int pre_state;
unsigned long pre_millis;

bool direction;
int cur_degree = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT); 
  servo.attach(9);

  pre_state = LOW;
  pre_millis = 0;
  direction = true;

  cur_degree = 0;
  servo.write(cur_degree);
}

void loop() {
  // while(Serial.available() > 0)
  // {
  //   String input = Serial.readStringUntil('\n');
  //   int degree = input.toInt();

  //   servo.write(degree);
  //   Serial.println(degree * 2);
  // }

  int input = digitalRead(BUTTON);
  if (input == HIGH && pre_state == LOW)
  {
    unsigned long cur_millis = millis();
    if (cur_millis - pre_millis < 500)
      return;

    direction = (cur_degree >= 180) ? false : direction;
    direction = (cur_degree <= 0) ? true : direction;

    if (direction)
    {
      cur_degree += 30;
    }
    else
    {
      cur_degree -= 30;
    }

    servo.write(cur_degree);
    
    Serial.print(direction);
    Serial.print(", ");
    Serial.println(cur_degree);

    pre_millis = millis();
  }

  pre_state = input;
}
