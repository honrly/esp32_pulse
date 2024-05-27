# ESP32_pnnx
ESP32とmicro-rosで心拍情報をpublishするコード

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

## 参考
### 環境構築
https://qiita.com/ousagi_sama/items/b4eb3d9c6b337cbe1b05

https://qiita.com/hirekatsu0523/items/434bc03ab40ae4b05a05

https://smtengkapi.com/engineer-esp32-wroom32-dev

### Humble, ROS_DOMAIN_ID
https://zenn.dev/tasada038/articles/83d78c8a8a3916

### 他メモ
https://zenn.dev/array/books/5efdb438cf8be3/viewer/4415c4

https://gist.github.com/KobayashiRui/094ac01d9d3cd2445faa2a1ef103646f

https://qiita.com/isseikz/items/fd0f55cb3d4b1ba2fa71

https://qiita.com/ousagi_sama/items/a814b3db925b7ce2aeea

https://klab.hateblo.jp/entry/2023/05/22/203251

https://github.com/micro-ROS/docker/issues/105
