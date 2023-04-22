#define CH_NUM 0x60

#define SIG_POWER RF24_PA_LOW

#define SIG_SPEED RF24_1MBPS

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; // возможные номера труб

byte transmit_data[2]; // массив пересылаемых данных

int8_t left_engine, right_engine;

int telemetry[2];         // массив принятых от приёмника данных телеметрии
byte rssi;
int trnsmtd_pack = 1, failed_pack;
unsigned long RSSI_timer;

uint8_t btn_one = 5;
uint8_t btn_two = 6;
uint8_t btn_three = 7;
uint8_t btn_four = 8;

int8_t btn_one_state = 0;
int8_t btn_two_state = 0;
int8_t btn_three_state = 0;
int8_t btn_four_state = 0;


void setup() {
  Serial.begin(9600);
  radioSetup();
  pinMode(btn_one, INPUT_PULLUP);
  pinMode(btn_two, INPUT_PULLUP);
  pinMode(btn_three, INPUT_PULLUP);
  pinMode(btn_four, INPUT_PULLUP);
}

void loop() {
  left_engine = left_engine = 0;
  btn_one_state = !digitalRead(btn_one);
  btn_two_state = !digitalRead(btn_two);
  btn_three_state = !digitalRead(btn_three);
  btn_four_state = !digitalRead(btn_four);
  Serial.print("Values: ");
  Serial.print(btn_one_state);
  Serial.print(" ");
  Serial.print(btn_two_state);
  Serial.print(" ");
  Serial.print(btn_three_state);
  Serial.print(" ");
  Serial.print(btn_four_state);
  Serial.println();

  if (btn_one_state >= 1) {
    left_engine = -1;
  }
  else if (btn_two_state >= 1) {
    left_engine = 1;
  }
  else {
    left_engine = 0;
  }
  
  if (btn_three_state >= 1) {
    right_engine = 1;
  }
  else if (btn_four_state >= 1) {
    right_engine = -1;
  }
  else {
    right_engine = 0;
  }
  transmit_data[0] = left_engine;
  transmit_data[1] = right_engine;
  
  // отправка пакета 
  if (radio.write(&transmit_data, sizeof(transmit_data))) {
    Serial.print("Transmit: ");
    for (uint8_t i : transmit_data) {
      Serial.print(i);
      Serial.print(" ");
    }
    Serial.println();
    trnsmtd_pack++;
  } else {
    failed_pack++;
  }
}

void radioSetup() {
  radio.begin();              // активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах
  radio.openWritingPipe(address[0]);   // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(CH_NUM);            // выбираем канал (в котором нет шумов!)
  radio.setPALevel(SIG_POWER);         // уровень мощности передатчика
  radio.setDataRate(SIG_SPEED);        // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();         // начать работу
  radio.stopListening();   // не слушаем радиоэфир, мы передатчик
}
