#include <SPI.h>
#include <MFRC522.h>

const int RST_PIN = 9;
const int SS_PIN = 10;
MFRC522 rc522(SS_PIN, RST_PIN);

const int TOTAL_INDEX = 60;
MFRC522::MIFARE_Key key;

MFRC522::StatusCode checkAuth(int index) {
  MFRC522::StatusCode status = rc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, index, &key, &(rc522.uid));
  return status;
}

MFRC522::StatusCode readData(int index, byte* data) {
  MFRC522::StatusCode status = checkAuth(index);
  if (status != MFRC522::STATUS_OK)
    return status;

  byte buffer[18];
  byte length = 18;

  status = rc522.MIFARE_Read(index, buffer, &length);
  if (status == MFRC522::STATUS_OK) {
    memcpy(data, buffer, 4);
  }

  return status;
}

MFRC522::StatusCode writeData(int index, byte* data, int length) {
  MFRC522::StatusCode status = checkAuth(index);
  if (status != MFRC522::STATUS_OK)
    return status;

  byte buffer[16];
  memset(buffer, 0x00, sizeof(buffer));
  memcpy(buffer, data, length);

  status = rc522.MIFARE_Write(index, buffer, 16);
  return status;

  // ######################### page 28 ###################################
}

void setup() {
  Serial.begin(9600);
  
  SPI.begin();
  rc522.PCD_Init();

  for (int i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  int recv_size = 0;
  char recv_buffer[16];

  if (Serial.available() > 0)
    // '\n'가 들어올 때까지 최대 16바이트의 데이터를 읽어 recv_buffer에 저장
    recv_size = Serial.readBytesUntil('\n', recv_buffer, 16);

  bool newCard = rc522.PICC_IsNewCardPresent(); // 새로운 카드가 접근했는가
  bool readCard = rc522.PICC_ReadCardSerial(); // 카드의 UID를 정상적으로 읽었는가

  if (recv_size > 0) // 명령을 전달받았다면
  {
    char cmd[2];
    memset(cmd, 0x00, sizeof(cmd));
    memcpy(cmd, recv_buffer, 2);

    char send_buffer[16];
    memset(send_buffer, 0x00, sizeof(send_buffer));
    memcpy(send_buffer, cmd, 2);

    if (strncmp(cmd, "GS", 2) != 0) // 명령어가 GS가 아닌 경우
    {
      if (memcmp(recv_buffer + 2, rc522.uid.uidByte, 4) != 0) // 저장된 카드 UID 4bytes 복사
      {
        memset(send_buffer + 2, 0xFB, 1);
        Serial.write(send_buffer, 3);
        Serial.println();
        return;
      }
    }

    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    if (newCard == true && readCard == true) // 카드를 인식한 경우
    {
      if (strncmp(cmd, "GS", 2) == 0) // 명령어가 Get Status인 경우
      {
        memset(send_buffer + 2, MFRC522::STATUS_OK, 1); // 상태값: 0x00 (성공)
        memcpy(send_buffer + 3, rc522.uid.uidByte, 4); // 읽어온 카드 UID 4bytes 복사
        Serial.write(send_buffer, 7); // 총 7bytes 전송
      }
      else if (strncmp(cmd, "GT", 2) == 0) // 명령어가 Get Total인 경우
      {
        byte total[4];
        memset(total, 0x00, 4); // 초기화
        status = readData(TOTAL_INDEX, total); // 60번 블록에서 데이터 4바이트 읽기

        memset(send_buffer + 2, status, 1); // 상태값: 읽기 성공 여부
        memcpy(send_buffer + 3, total, 4); // 읽어온 잔액 데이터 4bytes 복사
        Serial.write(send_buffer, 7); // 총 7bytes 전송
      }
      else // 아무 명령어도 아닌 경우
      {
        memset(send_buffer + 2, 0xFE, 1); // 상태값: 0xFE (알 수 없는 명령어)
        Serial.write(send_buffer, 3); // 총 3bytes 전송
      }
    }
    else // 카드가 없는 경우
    {
      memset(send_buffer + 2, 0xFA, 1); // 상태값: 0xFA (카드 없음/인식 실패)
      Serial.write(send_buffer, 3); // 총 3bytes 전송
    }

    Serial.println();
  }
}
