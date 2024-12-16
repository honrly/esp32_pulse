#include "MindWave.h"

MindWave::MindWave() {
  //変数の初期化
  _is_connected = false;
  _attention = 0;
  _meditation = 0;
  _raw_value = 0;
  _delta = 0;
  _theta = 0;
  _low_alpha = 0;
  _high_alpha = 0;
  _low_beta = 0;
  _high_beta = 0;
  _low_gamma = 0;
  _mid_gamma = 0;
  _poor_signal = 0;
  _blink_strength = 0;
  raw_detected = false;
  other_detected = false;
}

MindWave::~MindWave() {
  disconnect();
}

void MindWave::bluetooth_start() {
  //BluetoothSerialの初期化
  String myName = "ESP32-BT-Master";
  char *pin = "0000";
  _SerialBT.begin(myName, true);
  _SerialBT.setPin(pin);
}

void MindWave::connect(uint8_t *macadd) {
  if(!_is_connected) {
    bool connected = _SerialBT.connect(macadd);
    Serial.println("Connecting to MindWave with MAC Address");

    if(connected) {
      Serial.println("Connected Successfully!");
    }
    else {
      while(!_SerialBT.connected(10000)) {
        Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
      }
    }
    _is_connected = true;
  }
}

void MindWave::disconnect() {
  if(_is_connected) {
    _SerialBT.write(DISCONNECT);
    _SerialBT.disconnect();
    _is_connected = false;
  }
}

void MindWave::packet_parser() {
  int p1, p2, checksum, payload_length;
  int payload[100];

  checksum = 0;

  if(_is_connected) {
    while(_SerialBT.available() > 1) {
      p1 = _SerialBT.read();
      p2 = _SerialBT.read();

      while(p1 != SYNC || p2 != SYNC) {
        p1 = p2;
        p2 = _SerialBT.read();
      }
      payload_length = _SerialBT.read();
      while(payload_length == SYNC) payload_length = _SerialBT.read();
      for(int i=0;i<payload_length;i++) {
        int tmp_packet = _SerialBT.read();
        payload[i] = tmp_packet;
        checksum += tmp_packet;
      }
      checksum = ~checksum & 0x000000ff;
      if(checksum == _SerialBT.read()) {
        int i = 0;

        while(i < payload_length) {
          int code = payload[i];

/////////////////////////////////////////////////////////
///<<<<<<<<<<< must add _SerialBTother_detected to detect other values
          switch(code) {
            case POOR_SIGNAL:
              i++;
              _poor_signal = payload[i];
			  other_detected = true;
              break;

            case ATTENTION:
              i++;
              _attention = payload[i];
			  other_detected = true;
              break;

            case MEDITATION:
              i++;
              _meditation = payload[i];
			  other_detected = true;
              break;

            case BLINK:
              i++;
              _blink_strength = payload[i];
			  other_detected = true;
              break;

            case RAW_VALUE:
			  raw_detected = true;
              _raw_value = payload[i+2] * 256 + payload[i+3];
			  raw_detected = true;
              i += 3;

              break;

            case ASIC_EEG_POWER:
              //delta
              _delta = payload[i+2] * 65536 + payload[i+3] * 256 + payload[i+4];
              i += 5;
              //theta
              _theta = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 3;
              //low_alpha
              _low_alpha = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 3;
              //high_alpha
              _high_alpha = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 3;
              //low_beta
              _low_beta = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 3;
              //high_beta
              _high_beta = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 3;
              //low_gamma
              _low_gamma = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 3;
              //mid_gamma
              _mid_gamma = payload[i] * 65536 + payload[i+1] * 256 + payload[i+2];
              i += 2;
			  other_detected = true;

              break;

            default:
              break;
          }
          i++;
        }
      }
    }
  }
}

String MindWave::output_others() {
	String output;
	output = String(_attention);
	output += ",";
	output += String(_meditation);
	output += ",";
	output += String(_theta);
	output += ",";
	output += String(_delta);
	output += ",";
	output += String(_low_alpha);
	output += ",";
	output += String(_high_alpha);
	output += ",";
	output += String(_low_beta);
	output += ",";
	output += String(_high_beta);
	output += ",";
	output += String(_low_gamma);
	output += ",";
	output += String(_mid_gamma);
	output += ",";
	output += String(_poor_signal);
	output += ",";
	output += String(_blink_strength);
	output += ",";
	return output;
}

String MindWave::output_raw(){
	return String(_raw_value) + ",";
}
