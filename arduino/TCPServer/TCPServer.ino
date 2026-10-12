#include <WiFi.h>
#include <ESP32Servo.h>

const char *ssid = "SeSAC GANGNAM";
const char *password = "sesac01234";

WiFiServer server(80); // 80: port 
Servo servo;

const int SERVO = 5;
const int PHOTORESISTER = 34;
const int LED21 = 21;
const int LED22 = 22;
const int LED23 = 23;

struct Protocol{
  int pin = 21;
  int status = LOW;
};

void setup() {
  servo.attach(SERVO);

  pinMode(LED21, OUTPUT);
  pinMode(LED22, OUTPUT);
  pinMode(LED23, OUTPUT);

  Serial.begin(115200);
  Serial.println("ESP32 TCP Server Start. Servo ver.");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());

  server.begin(); // listen
}

void loop() {
  WiFiClient client = server.available();
  if(client)
  {
    Serial.print("Client Connected : ");
    Serial.println(client.remoteIP());
    Protocol p;

    while (client.connected())
    {
      char recv[8];
      memset(recv, 0x00, sizeof(recv)); // 초기화

      while (client.available() > 0)
      {
        client.readBytes(recv, sizeof(p));
        memcpy(&p, recv, sizeof(p));

        if (p.pin == SERVO) {
          servo.write(p.status);
        } else if (p.pin == PHOTORESISTER) {
          int value = analogRead(p.pin);
          p.status = value;
          memcpy(recv, &p, sizeof(p));
        } else {
          digitalWrite(p.pin, p.status); // LED
        }

        Serial.println(p.pin);
        Serial.println(p.status);

        client.write(recv, sizeof(recv)); // 받은 메시지를 다시 보낸다
      }
      delay(10);
    }
  }
}
