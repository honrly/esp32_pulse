#include <micro_ros_arduino.h>

#include <stdio.h>
#include <math.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <my_custom_message/msg/brain_data.h>
#include <rosidl_runtime_c/string_functions.h>

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

#define PORT 55000

#define CHECKER_LED_PIN 13
#define IBI_SIZE 31

/***** ROS *****/
rcl_publisher_t publisher;
my_custom_message__msg__BrainData msg;
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
    msg.poorsignal++;
  }
}

/***** Brain *****/
int RECEIVE_LED = 19;
int count = 0;
int RX_PIN = 22;
int TX_PIN = 23;

String read_rx;

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
  /***** Brain *****/
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(10);
  pinMode(RECEIVE_LED, OUTPUT);

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
  rclc_node_init_default(&node, "micro_ros_arduino_brainwave_node", "", &support);

  /* Create publisher
  RCCHECK(rclc_publisher_init_best_effort(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "topic_name"));
  */
  RCCHECK(rclc_publisher_init(
    &publisher, &node, 
		ROSIDL_GET_MSG_TYPE_SUPPORT(my_custom_message, msg, BrainData),
		"brain_wave", &rmw_qos_profile_default
	));

  // msg.data = 0.0;
  //msg.timestamp = ""
  rosidl_runtime_c__String__assign(&msg.timestamp, "");
  msg.poorsignal = -1;
  msg.delta = 0;
  msg.theta = 0;
  msg.alpha_l = 0;
  msg.alpha_h = 0;
  msg.beta_l = 0;
  msg.beta_h = 0;
  msg.gamma_l = 0;
  msg.gamma_m = 0;

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
  if (Serial1.available() > 0) {
    read_rx = Serial1.readStringUntil('\n');
    read_rx.trim();

    /*if (count < 1) {
      digitalWrite(RECEIVE_LED, HIGH);
      count++;
    }
    else {
      digitalWrite(RECEIVE_LED, LOW);
      count = 0;
    }*/

    int values[12]; // Array to hold the parsed values
    int index = 0;
    int start = 0;
    int end = read_rx.indexOf(',');

    while (end != -1 && index < 12) {
      values[index] = read_rx.substring(start, end).toInt();
      start = end + 1;
      end = read_rx.indexOf(',', start);
      index++;
    }

    // Assigning the parsed values to the respective variables
    if (index == 12) { 
      msg.delta = values[2];
      msg.theta = values[3];
      msg.alpha_l = values[4];
      msg.alpha_h = values[5];
      msg.beta_l = values[6];
      msg.beta_h = values[7];
      msg.gamma_l = values[8];
      msg.gamma_m = values[9];
      msg.poorsignal = values[10];
    }

    // msg.timestamp = Timestamp
    //rosidl_runtime_c__String__assign(&msg.timestamp, tStamp().c_str());
    // Serial.println(read_rx);
    Serial.println(msg.beta_l);
    // publish TIMESTAMP, BrainData msg
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    
  }/*
  else {
    digitalWrite(RECEIVE_LED, LOW);
  }*/
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
