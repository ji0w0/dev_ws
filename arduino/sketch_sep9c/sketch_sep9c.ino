const int BUTTON = 2;

enum ECOLOR {
  RED = 9,
  GREEN = 10,
  BLUE = 11,
  OFF = 12,
};

int pre_state;
ECOLOR current_color;

void getNextColor();
void updateLED(ECOLOR color);

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT); 
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  pre_state = LOW;
  current_color = OFF;
}

void loop() {
  while (Serial.available() > 0)
  {
    char recv = Serial.read();

    if (recv == '\n' || recv == '\r') continue;

    Serial.println(recv);

    if (recv == 'r')
      current_color = RED;
    else if (recv == 'g')
      current_color = GREEN;
    else if (recv == 'b')
      current_color = BLUE;
    else
      current_color = OFF;

    updateLED(current_color);
  }

  int input = digitalRead(BUTTON);
  if (input == HIGH && pre_state == LOW)
  {
    getNextColor();
    updateLED(current_color);
    delay(100);
  }

  pre_state = input;
}

void getNextColor() {
  if (current_color == OFF)
  {
    current_color = RED;
  }
  else
  {
    current_color = (ECOLOR)(current_color + 1);
  }
}

void updateLED(ECOLOR color) {
  digitalWrite(RED, color == RED ? HIGH : LOW);
  digitalWrite(GREEN, color == GREEN ? HIGH : LOW);
  digitalWrite(BLUE, color == BLUE ? HIGH : LOW);
}