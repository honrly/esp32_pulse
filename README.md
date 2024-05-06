# ESP32_pnnx
ESP32とmicro-rosでpnnxをpublishするコード

## 起動
Ubuntu側
```
ros2 run micro_ros_agent micro_ros_agent udp4 --port {PORT} -v6
```
その後ESP32に書き込み、実行

## 各種設定
### Wifi
line 18
```
const int PORT = {PORT};
```
line 112
```
set_microros_wifi_transports("SSID", "PASS", "IP_ADDRESS_UBUNTU", PORT);
```

### Set ROS_DOMAIN_ID, Humble ver.
line 28
```
size_t domain_id = {ROS_DOMAIN_ID};
```
line 125
```
// Set ROS domain id
RCCHECK(rcl_init_options_set_domain_id(&init_options, domain_id));
```
