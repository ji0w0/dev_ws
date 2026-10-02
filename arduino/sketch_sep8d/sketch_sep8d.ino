const int BUTTON = 2;
const int LED = 7;
int pre_state;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

  digitalWrite(LED, HIGH);
  pre_state = LOW;
}

void loop() {
  // put your main code here, to run repeatedly:
  int input = digitalRead(BUTTON);

  if (input == HIGH)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED, LOW);
    if (pre_state == LOW)
      Serial.println("버튼 누름");
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED, HIGH);
    if (pre_state == HIGH)
      Serial.println("버튼 뗌");
  }

  pre_state = input;
}
