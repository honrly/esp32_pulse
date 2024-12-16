#include <micro_ros_arduino.h>

#include <stdio.h>
#include <math.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <my_custom_message/msg/pulse_data.h>
#include <rosidl_runtime_c/string_functions.h>

#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>

#if !defined(ESP32) && !defined(TARGET_PORTENTA_H7_M7) && !defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_WIO_TERMINAL)
#error This example is only avaible for Arduino Portenta, Arduino Nano RP2040 Connect, ESP32 Dev module and Wio Terminal
#endif

//#define SSID "doly-wifi"
//#define PASSWORD "doly2021"
//#define IP "192.168.64.49" // raspi

#define SSID "G-LAB-TB"
#define PASSWORD "ponta78909tb3"
#define IP "192.168.32.49" // raspi

//#define SSID "dlab13D30"
//#define PASSWORD "doly2024"
//#define IP "192.168.64.121"

#define PORT 50000

#define CHECKER_LED_PIN 13
#define IBI_SIZE 31

/***** ROS *****/
rcl_publisher_t publisher;
my_custom_message__msg__PulseData msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

rcl_init_options_t init_options; // Humble
size_t domain_id = 30;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    digitalWrite(CHECKER_LED_PIN, !digitalRead(CHECKER_LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.ibi++;
  }
}

/***** Pulse *****/
//int Threshold = 550;
int Threshold = 2200;
int sensor_pin = 34;
int blink_led = 2;
const int fade = 5;
const int OUTPUT_TYPE = SERIAL_PLOTTER;

int bpm = 0;
int ibi_list[IBI_SIZE] = {0};
int ibi_count = 0;
int ibi = 0;

PulseSensorPlayground pulseSensor;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

/*** Timestamp ***/
String buf[4];
int hour;
int minute;
int second;
int milsec;
int start;

unsigned long tm ;
char buff[12] ;

void setup() {
  /***** Pulse *****/
  Serial.begin(115200);
  delay(10);

  pulseSensor.analogInput(sensor_pin);
  pulseSensor.blinkOnPulse(blink_led);
  pulseSensor.fadeOnPulse(fade);
  pulseSensor.setSerial(Serial);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  if (!pulseSensor.begin()) {
    for (;;) {
      digitalWrite(blink_led, LOW);
      delay(50);
      digitalWrite(blink_led, HIGH);
      delay(50);
    }
  }

  /***** ROS *****/
  set_microros_wifi_transports(SSID, PASSWORD, IP, PORT);
  pinMode(CHECKER_LED_PIN, OUTPUT);
  digitalWrite(CHECKER_LED_PIN, HIGH);
  // Wait init complete
  delay(2000);

  // Manage memory for micro-ROS
  allocator = rcl_get_default_allocator();

  /* Humble */
  // Create init_options
  init_options = rcl_get_zero_initialized_init_options();
  RCCHECK(rcl_init_options_init(&init_options, allocator)); // <--- This was missing on ur side
  // Set ROS domain id
  RCCHECK(rcl_init_options_set_domain_id(&init_options, domain_id));
  // Setup support structure with ROS domain_id
  RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));
  // RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  // Create node
  rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support);

  /* Create publisher
  RCCHECK(rclc_publisher_init_best_effort(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "topic_name"));
  */
  RCCHECK(rclc_publisher_init(
    &publisher, &node, 
		ROSIDL_GET_MSG_TYPE_SUPPORT(my_custom_message, msg, PulseData),
		"pulse", &rmw_qos_profile_default
	));

  // msg.data = 0.0;
  //msg.timestamp = ""
  rosidl_runtime_c__String__assign(&msg.timestamp, "");
  msg.bpm = 0;
  msg.ibi = 0;
  msg.sdnn = 0.0;
  msg.cvnn = 0.0;
  msg.rmssd = 0.0;
  msg.pnn10 = 0.0;
  msg.pnn20 = 0.0;
  msg.pnn30 = 0.0;
  msg.pnn40 = 0.0;
  msg.pnn50 = 0.0;

  // Set Time
  hour = buf[0].toInt();
  minute = buf[1].toInt();
  second = buf[2].toInt();
  switch (buf[3].length()) {
    case 0: buf[3] += "000"; break;
    case 1: buf[3] += "00"; break;
    case 2: buf[3] += "0"; break;
  }
  milsec = buf[3].toInt();
  start = millis();
  Serial.println(String(hour) + ":" + String(minute) + ":" + String(second) + "." + String(milsec));

}

void loop() {
  if (pulseSensor.sawNewSample()) {
    //Serial.println(pulseSensor.getLatestSample());
    if (pulseSensor.sawStartOfBeat()) {
      // Get BPM
      bpm = pulseSensor.getBeatsPerMinute(0);
      msg.bpm = bpm;

      // Get IBI
      ibi = pulseSensor.getInterBeatIntervalMs(0);
      msg.ibi = ibi;

      // Update ibi_list
      if (ibi_count >= IBI_SIZE) {
        for (int i = 0; i < IBI_SIZE - 1; i++) {
          ibi_list[i] = ibi_list[i+1];
        }
        ibi_list[IBI_SIZE - 1] = ibi;
      } else if (ibi_count >= 0) {
        ibi_list[ibi_count] = ibi;
        ibi_count++;
      } else {
        error_loop();
      }

      /*for (int i = 0; i < IBI_SIZE; i++) {
        Serial.println(ibi_list[i]);
      }
      Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");*/
      
      // Calc SDNN, CVNN, RMSSD, BPM, PNN10-50
      calcHRV();

      // msg.timestamp = Timestamp
      //rosidl_runtime_c__String__assign(&msg.timestamp, tStamp().c_str());
      
      Serial.println(msg.ibi);
      
      // publish TIMESTAMP, IBI, SDNN, CVNN, RMSSD, BPM, PNN10-50
      RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    }
  }
}

String tStamp() {
  int now = millis();

  int milli = milsec + now - start;
  int sec = second + milli / 1000;
  milli %= 1000;
  int min = minute + sec / 60;
  sec %= 60;
  int h = hour + min / 60;
  min %= 60;

  return String(h) + ":" + String(min) + ":" + String(sec) + ":" + String(milli);
}

void calcHRV() {
	float sum = 0;
	float mean = 0;

	for (int i = 0; i < IBI_SIZE; i++) {
		sum += ibi_list[i];
	}
	mean = sum / IBI_SIZE;

	calcSDNN(mean);
}

void calcSDNN(float mean) {
	float sum = 0;
	float sdnn = 0;

	for (int i = 0; i < IBI_SIZE; i++) {
		sum += ((ibi_list[i] - mean) * (ibi_list[i] - mean));
	}

	sdnn = sqrt(sum / IBI_SIZE);
  msg.sdnn = sdnn;

	calcCVNN(mean, sdnn);
	calcRMSSD(sdnn);
	calcPNN();
}

void calcCVNN(float mean, float sdnn) {
	float cvnn = sdnn / mean;
	msg.cvnn = cvnn;
}

void calcRMSSD(float sdnn) {
	float sum = 0;
	float rmssd = 0;

	for (int i = 1; i < IBI_SIZE; i++) {
		sum += ((ibi_list[i] - ibi_list[i - 1]) * (ibi_list[i] - ibi_list[i - 1]));
	}

	rmssd = sqrt(sum / IBI_SIZE);
  msg.rmssd = rmssd;
}

void calcPNN() {
	float nn10 = 0, nn20 = 0, nn30 = 0, nn40 = 0, nn50 = 0, nnX = 0;
	float pnn10 = 0, pnn20 = 0, pnn30 = 0, pnn40 = 0, pnn50 = 0;

	for (int i = 1; i < IBI_SIZE; i++) {
		nnX = abs(ibi_list[i] - ibi_list[i - 1]);

		if (nnX > 50) {
			nn50++;
			nn40++;
			nn30++;
			nn20++;
			nn10++;
		}
		else if (nnX > 40) {
			nn40++;
			nn30++;
			nn20++;
			nn10++;
		}
		else if (nnX > 30) {
			nn30++;
			nn20++;
			nn10++;
		}
		else if (nnX > 20) {
			nn20++;
			nn10++;
		}
		else if (nnX > 10) {
			nn10++;
		}
	}

	pnn50 = nn50 / (IBI_SIZE - 1);
	pnn40 = nn40 / (IBI_SIZE - 1);
	pnn30 = nn30 / (IBI_SIZE - 1);
	pnn20 = nn20 / (IBI_SIZE - 1);
	pnn10 = nn10 / (IBI_SIZE - 1);

  msg.pnn50 = pnn50;
  msg.pnn40 = pnn40;
  msg.pnn30 = pnn30;
  msg.pnn20 = pnn20;
  msg.pnn10 = pnn10;
}
