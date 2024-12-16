
int LED = 2;
int count = 0;
int RX_PIN = 22;
int TX_PIN = 23;

void setup() {
  pinMode(LED, OUTPUT);
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

void twinkle(){
  if ((count % 2) == 1) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}

void loop() {
  Serial1.print("count = ");
  Serial1.println(count);
  twinkle();
  delay(1000);

  count++;
}
