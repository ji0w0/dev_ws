const int LEDS[] = {2, 3, 4, 5, 6, 7, 8, 9};
int min, max;

int pinCount;
int cur_grade;
int total_grade; // 0~8단계, 0 : 0개, 8: 8개

void setup() {
  Serial.begin(9600);

  pinCount = sizeof(LEDS) / sizeof(LEDS[0]);
  for(int i = 0; i < pinCount; i++)
  {
    pinMode(LEDS[i], OUTPUT);
  }
  
  min = 1023;
  max = 0;

  cur_grade = 0;
  total_grade = pinCount;
}

void loop() {
  int volume = analogRead(A0);
  min = (volume < min) ? volume : min;
  max = (volume > max) ? volume : max;

  // int light = map(volume, min, max, 0, 255);
  // analogWrite(LED1, light);

  int range = max - min;
  int new_grade = 0;

  if (range > 0) {
    new_grade = (long)(volume - min) * total_grade / range;
    if (new_grade > total_grade)
      new_grade = total_grade;
    if (new_grade < 0)
      new_grade = 0;
  }

  if (cur_grade < new_grade)
  {
    for (int i = cur_grade; i < new_grade; i++)
    {
      digitalWrite(LEDS[i], HIGH);
      delay(100);
    }

    cur_grade = new_grade;
  }
  else if (cur_grade > new_grade)
  {
    for (int i = cur_grade - 1; i >= new_grade; i--)
    {
      digitalWrite(LEDS[i], LOW);
      delay(100);
    }

    cur_grade = new_grade;
  }

  Serial.print(min);
  Serial.print(", ");
  Serial.print(max);
  Serial.print(", ");
  Serial.print(volume);
  Serial.print(", ");
  Serial.println(cur_grade);

  delay(50);
}
