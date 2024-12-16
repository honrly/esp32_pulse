#ifndef MINDWAVE_H_INCLUDE
#define MINDWAVE_H_INCLUDE

#include "BluetoothSerial.h"

class MindWave
{
    static const uint8_t CONNECT = 192;
    static const uint8_t DISCONNECT = 193;
    static const uint8_t AUTOCONNECT = 194;
    static const uint8_t SYNC = 170;
    static const uint8_t EXCODE = 85;
    static const uint8_t POOR_SIGNAL = 2;
    static const uint8_t ATTENTION = 4;
    static const uint8_t MEDITATION = 5;
    static const uint8_t BLINK = 22;
    static const uint8_t HEADSET_CONNECTED = 208;
    static const uint8_t HEADSET_NOT_CONNECTED = 209;
    static const uint8_t HEADSET_DISCONNECTED = 210;
    static const uint8_t REQUEST_DENIED = 211;
    static const uint8_t STANDBY_SCAN = 212;
    static const uint8_t RAW_VALUE = 128;
    static const uint8_t ASIC_EEG_POWER = 131;
    
private:
  bool _is_connected;
  int _attention;
  int _meditation;
  int _raw_value;
  int _delta;
  int _theta;
  int _low_alpha;
  int _high_alpha;
  int _low_beta;
  int _high_beta;
  int _low_gamma;
  int _mid_gamma;
  int _poor_signal;
  int _blink_strength;
  uint8_t _macADD[6];
  BluetoothSerial _SerialBT;
public:
  MindWave();
  ~MindWave();
  void bluetooth_start();
  void connect(uint8_t *macadd);
  void disconnect();
  void packet_parser();
  String output_others();
  String output_raw();
  bool raw_detected;
  bool other_detected;
};

#endif
