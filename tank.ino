#define CH_NUM 0x60
#define SIG_POWER RF24_PA_MIN
#define SIG_SPEED RF24_1MBPS

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10);

byte pipeNo;
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

int8_t recieved_data[2];   // массив принятых данных
int8_t telemetry[2]; // ответ

uint8_t motorA_1 = 4;
uint8_t motorA_2 = 5;

uint8_t motorB_1 = 6;
uint8_t motorB_2 = 7;

uint32_t cnt = 0;

void setup() {
  Serial.begin(9600);
  for (uint8_t i=4; i <= 7; i++ ) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  radioSetup();
}


void left_track_forward() {
  digitalWrite(motorB_1, LOW);
  digitalWrite(motorB_2, HIGH);
}

void right_track_forward() {
  digitalWrite(motorA_1, HIGH);
  digitalWrite(motorA_2, LOW);
}

void left_track_back() {
  digitalWrite(motorB_1, HIGH);
  digitalWrite(motorB_2, LOW);
}

void right_track_back () {
  digitalWrite(motorA_1, LOW);
  digitalWrite(motorA_2, HIGH);
}

void left_track_stop() {
  digitalWrite(motorB_1, LOW);
  digitalWrite(motorB_2, LOW);
}

void right_track_stop () {
  digitalWrite(motorA_1, LOW);
  digitalWrite(motorA_2, LOW);
}

void forward() {
  right_track_forward();
  left_track_forward();
}

void back() {
  right_track_back();
  left_track_back();
}

void stop_() {
  for (uint8_t i=4; i <= 7; i++ ) {
    digitalWrite(i, LOW);
  }
}

void right() {
  right_track_stop();
  left_track_forward();
}

void left() {
  left_track_stop();
  right_track_forward;
}

void cleft() {
  left_track_back();
  right_track_forward();
}

void cright() {
  right_track_back();
  left_track_forward();
}

void loop() {
  while (radio.available(&pipeNo)) {                    // слушаем эфир
    radio.read(&recieved_data, sizeof(recieved_data));  // чиатем входящий сигнал
    Serial.print("Recieve: ");
    Serial.print(recieved_data[0]);
    Serial.print(" ");
    Serial.print(recieved_data[1]);
    Serial.println();
    if (recieved_data[0] > 0) {
      left_track_forward();
    }
    else if (recieved_data[0] < 0) {
      left_track_back();
    }
    else {
      left_track_stop();
    }
    if (recieved_data[1] > 0) {
      right_track_forward();
    }
    else if (recieved_data[1] < 0) {
      right_track_back();
    }
    else {
      right_track_stop();
    }
  }
}

void radioSetup() {             // настройка радио
  radio.begin();                // активировать модуль
  radio.setAutoAck(1);          // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);      // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();     // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     // размер пакета, байт
  radio.openReadingPipe(1, address[0]); // хотим слушать трубу 0
  radio.setChannel(CH_NUM);     // выбираем канал (в котором нет шумов!)
  radio.setPALevel(SIG_POWER);  // уровень мощности передатчика
  radio.setDataRate(SIG_SPEED); // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();         // начать работу
  radio.startListening();  // начинаем слушать эфир, мы приёмный модуль
}
