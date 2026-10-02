const int LED[7] = {2, 3, 4, 5, 6, 7, 8};
const int BUTTON = 9;

int digits[10][7]
{
  {1, 1, 1, 1, 1, 1, 0}, // digit 0
  {1, 0, 1, 0, 0, 0, 0}, // digit 1
  {1, 1, 0, 1, 1, 0, 1}, // digit 2
  {1, 1, 1, 1, 0, 0, 1}, // digit 3
  {1, 0, 1, 0, 0, 1, 1}, // digit 4
  {0, 1, 1, 1, 0, 1, 1}, // digit 5
  {0, 1, 1, 1, 1, 1, 1}, // digit 6
  {1, 1, 1, 0, 0, 0, 0}, // digit 7
  {1, 1, 1, 1, 1, 1, 1}, // digit 8
  {1, 1, 1, 1, 0, 1, 1}  // digit 9
};

unsigned long pre_millis;

int pre_state;
int cur_number;

void displayDigit(int d)
{
  for (int i = 0; i < 7; i++)
  {
    if (digits[d][i] == 1)
    {
      digitalWrite(LED[i], HIGH);
    }
    else
    {
      digitalWrite(LED[i], LOW);
    }
  }
}

void setNextNumber()
{
  if (cur_number == 9)
  {
    cur_number = 0;
  }
  else
  {
    cur_number++;
  }

  displayDigit(cur_number);
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  for (int i = 0; i < 7; i++)
  {
    pinMode(LED[i], OUTPUT);
  }

  cur_number = 0;
  displayDigit(0);

  pre_state = HIGH;
  pre_millis = 0;
}

void loop() {
  // for (int i = 0; i < 10; i++)
  // {
  //   displayDigit(i);
  //   delay(1000);
  // }

  int input = digitalRead(BUTTON);
  if (input == HIGH && pre_state == LOW)
  {
    Serial.print("Click");

    unsigned long cur_millis = millis();
    if (cur_millis - pre_millis >= 200)
    {
      setNextNumber();
      pre_millis = millis();
    }
  }

  pre_state = input;
}
