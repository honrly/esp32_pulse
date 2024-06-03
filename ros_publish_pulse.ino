#include <micro_ros_arduino.h>

#include <stdio.h>
#include <math.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <my_custom_message/msg/bio_data.h>

#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>

#if !defined(ESP32) && !defined(TARGET_PORTENTA_H7_M7) && !defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_WIO_TERMINAL)
#error This example is only avaible for Arduino Portenta, Arduino Nano RP2040 Connect, ESP32 Dev module and Wio Terminal
#endif

#define SSID "doly-wifi"
#define PASSWORD "doly2021"
#define IP "192.168.64.169"
#define PORT 50000
#define LED_PIN 13
#define MAX_LEN_DATA 30
#define PNNX_SIZE 4
#define IBI_SIZE 2

/***** ROS *****/
rcl_publisher_t publisher;
my_custom_message__msg__BioData msg;
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
    msg.ibi++;
  }
}

/***** Pulse *****/
// char ibi[16];
int Threshold = 2200;
int inpin = 35;
int led = 2;
const int fade = 5;
const int OUTPUT_TYPE = SERIAL_PLOTTER;

int bpm = 0;
int ibi[2] = {0};
int ibi_count = 0;
int ibi_index = 0;
float sdnn = 0.0;
float rmssd = 0.0;
float pnn10 = 0.0;
float pnn20 = 0.0;
float pnn30 = 0.0;
float pnn40 = 0.0;
float pnn50 = 0.0;

PulseSensorPlayground pulseSensor;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

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
		ROSIDL_GET_MSG_TYPE_SUPPORT(my_custom_message, msg, BioData),
		"pulse", &rmw_qos_profile_default
	));

  // msg.data = 0.0;
  msg.bpm = 0;
  msg.ibi = 0;
  msg.sdnn = 0.0;
  msg.rmssd = 0.0;
  msg.pnn10 = 0.0;
  msg.pnn20 = 0.0;
  msg.pnn30 = 0.0;
  msg.pnn40 = 0.0;
  msg.pnn50 = 0.0;
}

void loop() {
  if (pulseSensor.sawNewSample()) {

    bpm = pulseSensor.getBeatsPerMinute(0);
    msg.bpm = bpm;

    if (ibi_count == 0) {
      ibi[0] = pulseSensor.getInterBeatIntervalMs(0);
      ibi_count++;
      ibi_index = 0;
    } else if (ibi_count == 1) {
      ibi[1] = pulseSensor.getInterBeatIntervalMs(0);
      ibi_count++;
      ibi_index = 1;
    } else if (ibi_count >= 2) {
      ibi[0] = ibi[1];
      ibi[1] = pulseSensor.getInterBeatIntervalMs(0);
    } else {
      error_loop();
    }
    msg.ibi = ibi[ibi_index];

    msg.sdnn = sdnn;
    //rmssd = calc_rmssd(ibi);
    msg.rmssd = rmssd;

    if (pulseSensor.sawStartOfBeat()) {
      pnn10 = calc_pnn10(float(ibi[ibi_index]));
      pnn20 = calc_pnn20(float(ibi[ibi_index]));
      pnn30 = calc_pnn30(float(ibi[ibi_index]));
      pnn40 = calc_pnn40(float(ibi[ibi_index]));
      pnn50 = calc_pnn50(float(ibi[ibi_index]));
      msg.pnn10 = pnn10;
      msg.pnn20 = pnn20;
      msg.pnn30 = pnn30;
      msg.pnn40 = pnn40;
      msg.pnn50 = pnn50;

      // publish IBI, RMSSD, BPM, PNN10-50
      RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    }
  }
}

float calc_rmssd(int ibi[]) {
  // mijissou
    float sumOfSquares = 0.0;
    // static float diffArray[];
    for (int i = 1; i < 2; i++) {
      int diff = ibi[i] - ibi[i - 1];
      sumOfSquares += diff * diff;
    }

    float meanOfSquares = sumOfSquares / (2 - 1);
    float rmssd = sqrt(meanOfSquares);

    return rmssd;
}

float calc_pnn10(float ibi) {
  int X_PNN = 10;
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
    float pnn10 = (float)count / MAX_LEN_DATA;
    return pnn10;
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

float calc_pnn30(float ibi) {
  int X_PNN = 30;
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
    float pnn30 = (float)count / MAX_LEN_DATA;
    return pnn30;
  }
  // No enough data for pnnx
  else if (xx_count < MAX_LEN_DATA + 2) {
    ibi_arr[xx_count - 1] = ibi;
    return 0.0;
  }
}

float calc_pnn40(float ibi) {
  int X_PNN = 40;
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
    float pnn40 = (float)count / MAX_LEN_DATA;
    return pnn40;
  }
  // No enough data for pnnx
  else if (xx_count < MAX_LEN_DATA + 2) {
    ibi_arr[xx_count - 1] = ibi;
    return 0.0;
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
