const char * message;
bool showtime=true;
void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  message = "hello";
  int sensorValue = analogRead(A0);
  uint32_t mill = millis();
  String s = "";
  if (showtime) {
    Serial.print(mill);
    Serial.print(",");
  }
  Serial.print(sensorValue);
  Serial.println(",");
}

size_t print64(Print* pr, uint64_t n) {
  char buf[21];
  char *str = &buf[sizeof(buf) - 1];
  *str = '\0';
  do {
    uint64_t m = n;
    n /= 10;
    *--str = m - 10 * n + '0';
  } while (n);
  Serial.print(str);
}
