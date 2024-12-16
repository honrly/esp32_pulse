#include "MindWave.h"

MindWave mindwave;

uint8_t macADD[6] = {0xc4, 0x64, 0xe3, 0xe7, 0xc6, 0x71};

String read_mw = "";
String write_tx = "";
int LED = 13;
int count = 0;
int RX_PIN = 22;
int TX_PIN = 23;

void setup() {
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  mindwave.bluetooth_start();
  mindwave.connect(macADD);
  pinMode(LED, OUTPUT);
}

void loop() {
  mindwave.packet_parser();

  if (mindwave.raw_detected) {
    mindwave.raw_detected = false;
    read_mw = mindwave.output_others();

    if (read_mw != write_tx) {
      write_tx = read_mw;
      Serial1.println(write_tx);
      
      if (count < 1) {
        digitalWrite(LED, HIGH);
        count++;
      }
      else {
        digitalWrite(LED, LOW);
        count = 0;
      }
    }
  }
}
