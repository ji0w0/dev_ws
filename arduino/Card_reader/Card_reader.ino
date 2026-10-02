/*
[일반적인 개발 Process -> V Model]
* 개발
1. User Requirement
2. System Requirement
3. System Architecture
- 평가 항목/방법/지표 수립 같은 부분은 QA팀(중에서도 좀 높은 직급 인력)이 1,2,3번 과정에서 참여
4. Detailed Design 상세 설셰
5. 구현

* 검증
1. unit test
-  함수 및 기능을 개별 검증
- coding standard(중요) : 버그를 줄이는 코딩 법칙 모음을 일컫는 말 (회사마다 다르고 체크함)
2. Component test
3. Integration test
- Architecture 기반해서 데이터를 통신 및 처리 test
4. System test
- 이 단계부터 전문가 QA팀이 참여함
5. Usability(사용성 Test)
*/

#pragma region 결제,충전,초기화 기능 있는 카드 단말기 만들기
/*
시나리오

카드를 인식하면 → 잔액을 가져오고 → 잔액을 초기화 할수 있고 → 충전도 할수 있고 → 결제도 가능
카드가 인식되지 않으면 → 카드를 인식할때 까지 대기
*/

#include <SPI.h>                // RC522와 SPI 통신을 사용하기 위한 기본 라이브러리
#include <MFRC522.h>            // MFRC522 RFID 리더 제어 라이브러리

const int RST_PIN = 9;          // RC522 Reset 핀
const int SS_PIN = 10;          // RC522 SPI Slave Select(SDA/SS) 핀
MFRC522 rc522(SS_PIN, RST_PIN); // SS, RST 핀을 사용해 RC522 객체 생성

const int TOTAL_INDEX = 60;     // 카드 잔액 4 byte를 저장할 MIFARE Classic Block 번호
MFRC522::MIFARE_Key key;        // MIFARE Classic 인증에 사용할 6 byte Key A 저장

MFRC522::StatusCode checkAuth(int index) // 지정한 Block이 속한 Sector를 Key A로 인증
{
  MFRC522::StatusCode status = rc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, index, &key, &(rc522.uid));  // 현재 UID + Key A로 Block 인증
  return status;                                    // STATUS_OK 또는 오류 Status 반환
}

MFRC522::StatusCode readData(int index, byte* data) // RFID Block에서 4 byte 잔액 데이터를 읽는 함수
{
  MFRC522::StatusCode status = checkAuth(index);    // 읽기 전에 해당 Block 인증

  if (status != MFRC522::STATUS_OK)                 // 인증 실패 시
  {
    return status;                                  // 읽지 않고 오류 Status 반환
  }

  byte buffer[18];                                  // MIFARE_Read()가 요구하는 18 byte 수신 buffer
  byte length = 18;                                 // buffer 크기를 라이브러리에 전달하기 위한 변수
  status = rc522.MIFARE_Read(index, buffer, &length); // 지정 Block의 16 byte 데이터 + CRC를 읽음

  if (status == MFRC522::STATUS_OK)                 // 읽기에 성공하면
  {
    memcpy(data, buffer, 4);                        // Block 앞 4 byte만 잔액 데이터로 복사
  }

  return status;                                    // 최종 읽기 결과 반환
}

// RFID Block에 원하는 byte 데이터를 쓰는 함수
MFRC522::StatusCode writeData(int index, byte* data, int length) 
{
  MFRC522::StatusCode status = checkAuth(index);    // 쓰기 전에 해당 Block 인증

  if (status != MFRC522::STATUS_OK)                 // 인증 실패 시
  {
    return status;                                  // 쓰기 없이 오류 Status 반환
  }

  byte buffer[16];                                  // MIFARE Classic 한 Block 크기인 16 byte buffer 생성
  memset(buffer, 0x00, sizeof(buffer));              // 전체 buffer를 0으로 초기화
  memcpy(buffer, data, length);                      // 실제 기록할 data를 buffer 앞부분에 복사

  status = rc522.MIFARE_Write(index, buffer, 16);   // 16 byte 전체를 지정 Block에 기록
  return status;                                    // 최종 쓰기 결과 반환
}
void setup() {
  // 시리얼 통신 시작
  Serial.begin(9600);           // baudrate [bit/s]

  for (int i = 0; i < 6; i++)             // MIFARE Classic Key A는 6 byte
  {
    key.keyByte[i] = 0xFF;                 // 기본 Key A = FF FF FF FF FF FF
  }
}

void loop() {

  SPI.begin();
  rc522.PCD_Init();
  
  int recv_size = 0;    // 이번 loop에서 Python으로부터 받은 byte 수
  char recv_buffer[16]; // Python 요청 Packet을 담을 임시 수신 buffer

  if (Serial.available() > 0) // PC에서 들어온 Serial 데이터가 있으면
  {
    // '\n' 직전까지 최대 16 byte를 읽어 recv_buffer에 저장
    recv_size = Serial.readBytesUntil('\n', recv_buffer, 16); 
  }

  bool newCard = rc522.PICC_IsNewCardPresent();     // 새 RFID 카드가 리더에 감지되었는지 확인
  bool readCard = rc522.PICC_ReadCardSerial();      // 카드 UID/SAK 정보를 rc522.uid에 읽어옴

  if (recv_size > 0)                                // Python 요청 Packet을 실제로 받았을 때만 명령 처리
  {
    char cmd[2];                                    // Packet의 앞 2 byte인 GS/GT/ST 명령 저장
    memset(cmd, 0x00, sizeof(cmd));                 // cmd buffer 초기화
    memcpy(cmd, recv_buffer, 2);                    // 수신 Packet 앞 2 byte를 cmd로 복사

    char send_buffer[16];                           // Python으로 보낼 응답 Packet용 buffer
    memset(send_buffer, 0x00, sizeof(send_buffer)); // 응답 buffer 초기화
    memcpy(send_buffer, cmd, 2);                    // 응답 Packet의 앞 2 byte에 원래 명령을 그대로 복사

    // GS가 아닌 GT/ST는 요청 Packet에 UID가 포함되어 있으므로 UID 검증
    if (strncmp(cmd, "GS", 2) != 0)                 
    {
      // Python이 보낸 UID와 현재 카드 UID가 다르면
      if (memcmp(recv_buffer + 2, rc522.uid.uidByte, 4) != 0) 
      {
        memset(send_buffer + 2, 0xFB, 1);           // Status byte에 INVALID_TAG(0xFB) 기록
        Serial.write(send_buffer, 3);               // Command 2 byte + Status 1 byte 전송
        Serial.println();                           // Packet 끝을 알리는 CRLF 전송
        return;                                     // 현재 loop 명령 처리 종료
      }
    }

    MFRC522::StatusCode status = MFRC522::STATUS_ERROR; // 기본 작업 상태를 오류로 초기화

    if (newCard == true && readCard == true)        // 카드가 실제로 감지되고 UID까지 읽혔을 때
    {
      if (strncmp(cmd, "GS", 2) == 0)               // GS = Get Status
      {
        memset(send_buffer + 2, MFRC522::STATUS_OK, 1); // Status byte에 성공(0x00) 기록
        memcpy(send_buffer + 3, rc522.uid.uidByte, 4);  // 뒤 4 byte에 현재 카드 UID 기록
        Serial.write(send_buffer, 7);                   // GS 2B + Status 1B + UID 4B 전송
      }
      else if (strncmp(cmd, "GT", 2) == 0)          // GT = Get Total
      {
        byte total[4];                              // 카드에서 읽은 잔액 4 byte 저장
        memset(total, 0x00, 4);                     // 잔액 buffer 초기화
        status = readData(TOTAL_INDEX, total);      // Block 60의 앞 4 byte를 읽음

        memset(send_buffer + 2, status, 1);         // 응답 Status byte에 readData() 결과 저장
        memcpy(send_buffer + 3, total, 4);          // 응답 뒤 4 byte에 잔액 저장
        Serial.write(send_buffer, 7);               // GT 2B + Status 1B + Total 4B 전송
      }
      else if (strncmp(cmd, "ST", 2) == 0)          // ST = Set Total
      {
        char total[4];                              // Python이 보낸 새 잔액 4 byte 저장
        memset(total, 0x00, sizeof(total));         // 잔액 buffer 초기화
        memcpy(total, recv_buffer + 6, 4);          // 요청 Packet의 6~9번째 위치에서 int 4 byte 복사

        status = writeData(TOTAL_INDEX, (byte*)total, 4); // 새 잔액 4 byte를 Block 60에 기록
        memset(send_buffer + 2, status, 1);               // 응답 Status byte에 쓰기 결과 저장
        Serial.write(send_buffer, 3);                     // ST 2B + Status 1B 전송
      }
      else                                          // GS/GT/ST가 아닌 알 수 없는 명령이면
      {
        memset(send_buffer + 2, 0xFE, 1);           // UNKNOWN_COMMAND(0xFE) 기록
        Serial.write(send_buffer, 3);               // Command 2B + Status 1B 전송
      }
    }

    else                                            // 카드가 감지되지 않았거나 UID 읽기에 실패했을 때
    {
      memset(send_buffer + 2, 0xFA, 1);             // NO_TAG(0xFA) 기록
      Serial.write(send_buffer, 3);                 // Command 2B + Status 1B 전송
    }

    Serial.println();                               // Python read_until(b'\n')이 Packet 끝을 찾도록 CRLF 전송
  }
}

#pragma endregion

#pragma region 



#pragma endregion

#pragma region 



#pragma endregion

#pragma region 



#pragma endregion

#pragma region 



#pragma endregion