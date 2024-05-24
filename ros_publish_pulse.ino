#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/Float32.h>

#define USE_ARDUINO_INTERRUPTS false
#include<PulseSensorPlayground.h>
#include<Ticker.h>

#if !defined(ESP32) && !defined(TARGET_PORTENTA_H7_M7) && !defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_WIO_TERMINAL)
#error This example is only avaible for Arduino Portenta, Arduino Nano RP2040 Connect, ESP32 Dev module and Wio Terminal
#endif

#define SSID "doly-wifi"
#define PASSWORD "doly2021"
#define IP "192.168.65.29"
#define PORT 50000
#define LED_PIN 13
#define MAX_LEN_DATA 30

/***** ROS *****/
rcl_publisher_t publisher;
std_msgs__msg__Float32 msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

rcl_init_options_t init_options; // Humble
size_t domain_id = 30;
/***************/

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
  }
}

/***** Pulse *****/
char ibi[16];
int Threshold = 550;
int inpin = 35;
int led = 4;
double Emotion = 0;
int Signal;
const int fade = 5;
const int OUTPUT_TYPE = SERIAL_PLOTTER;
//const int OUTPUT_TYPE = PROCESSING_VISUALIZER;
int rri;
int bpm;
float pnn20 = 0.0;
float pnn50 = 0.0;

PulseSensorPlayground pulseSensor;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

volatile SemaphoreHandle_t timerSemaphore;
hw_timer_t *timer1 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer1() {
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}
/******************/

void setup() {
  /***** Pulse *****/
  Serial.begin(115200);
  delay(10);

  pulseSensor.analogInput(inpin);
  pulseSensor.blinkOnPulse(led);
  pulseSensor.fadeOnPulse(fade);
  pulseSensor.setSerial(Serial);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  if (!pulseSensor.begin()) {
    for (;;) {
      digitalWrite(led, LOW);
      delay(50);
      digitalWrite(led, HIGH);
      delay(50);
    }

  }

  int intersamp = 1000000 / 4096;
  timerSemaphore = xSemaphoreCreateBinary();
  timer1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, intersamp, true);
  timerAlarmEnable(timer1);

  /***** ROS *****/
  set_microros_wifi_transports(SSID, PASSWORD, IP, PORT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
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
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
		"pulse", &rmw_qos_profile_default
	));

  msg.data = 0.0;
}

void loop() {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    if (pulseSensor.sawNewSample()) {
      rri = pulseSensor.getInterBeatIntervalMs(0);
      if (pulseSensor.sawStartOfBeat()) {
        pnn50 = calc_pnn50(float(rri));
        msg.data = pnn50;
        // publish BPM, IBI, PNN20, PNN50
        RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      }
    }
  }
}

float calc_pnn50(float ibi) {
  int X_PNN = 50;
  static float ibi_arr[MAX_LEN_DATA + 2] = {0};
  static int xx_count = 0;
  xx_count++;

  if (xx_count >= MAX_LEN_DATA + 2) {
    for (int i = 0; i < MAX_LEN_DATA; i++) {
      ibi_arr[i] = ibi_arr[i + 1];
    }
    ibi_arr[MAX_LEN_DATA - 1] = ibi;

    int count = 0;
    for (int i = 0; i < MAX_LEN_DATA; i++) {
      if (fabs(ibi_arr[i] - ibi_arr[i + 1]) > X_PNN) {
        count++;
      }
    }
    // Calc and update pnnx
    float pnn50 = (float)count / MAX_LEN_DATA;
    return pnn50;
  }
  // No enough data for pnnx
  else if (xx_count < MAX_LEN_DATA + 2) {
    ibi_arr[xx_count - 1] = ibi;
    return 0.0;
  }
}

float calc_pnn20(float ibi) {
  int X_PNN = 20;
  static float ibi_arr[MAX_LEN_DATA + 2] = {0};
  static int xx_count = 0;
  xx_count++;

  if (xx_count >= MAX_LEN_DATA + 2) {
    for (int i = 0; i < MAX_LEN_DATA; i++) {
      ibi_arr[i] = ibi_arr[i + 1];
    }
    ibi_arr[MAX_LEN_DATA - 1] = ibi;

    int count = 0;
    for (int i = 0; i < MAX_LEN_DATA; i++) {
      if (fabs(ibi_arr[i] - ibi_arr[i + 1]) > X_PNN) {
        count++;
      }
    }
    // Calc and update pnnx
    float pnn20 = (float)count / MAX_LEN_DATA;
    return pnn20;
  }
  // No enough data for pnnx
  else if (xx_count < MAX_LEN_DATA + 2) {
    ibi_arr[xx_count - 1] = ibi;
    return 0.0;
  }
}