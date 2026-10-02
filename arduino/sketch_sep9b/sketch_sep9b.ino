const int LED = 7;
const int TRIG = 9;
const int ECHO = 8;
const int LIMIT = 30;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  long duration, distance;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 17 / 1000; // 소리의 속도

  digitalWrite(LED, distance < LIMIT ? HIGH : LOW);

  Serial.print(duration);
  Serial.print(", ");
  Serial.println(distance);

  delay(1000);
}
