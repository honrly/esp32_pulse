int RX_PIN = 18;
int TX_PIN = 19;
void setup() {
  // シリアル初期化
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.begin(9600);
  
}
void loop() {
  String command;
  if (Serial1.available() > 0) {
    // 改行までをコマンドとして受付
    command = Serial1.readStringUntil('\n');
    // コマンド文字列をトリム
    command.trim();
    Serial.println("command="+command);

  }
  delay(100);
}