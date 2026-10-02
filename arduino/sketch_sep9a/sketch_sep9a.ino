const int LED = 9;
int min, max;

// const int MIN = 660, MAX = 900;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  min = 1023;
  max = 0;
}

void loop() {
  int light = analogRead(A0);
  min = min(light, min);
  max = max(light, max);
  // light = constrain(light, MIN, MAX);

  int output = map(light, min, max, 255, 0);
  analogWrite(LED, output);

  Serial.print(light);
  Serial.print(", ");
  Serial.print(output);
  Serial.print(", ");
  Serial.print(min);
  Serial.print(", ");
  Serial.println(max);

  delay(20);
}
