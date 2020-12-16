unsigned long long previousMillis = 0;
int ledState = LOW;        // will store last time LED was updated
int currentByte = -1;
int index = 0;
int flip = 0;
int lastAvailable = 0;
int written = 0;
char buf[32];
char nextBuf[32];
bool readSerial = true;
int nextByte = -1;

// constants won't change:
long interval = 15;
void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.write("test\n");
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  //Serial.write(0x01);
  // put your main code here, to run repeatedly:
  unsigned long long currentMillis = millis();
  /*if (lastAvailable != 0 && currentMillis - lastAvailable >= 5000) {

    digitalWrite(LED_BUILTIN, LOW);
    exit;
  }*/

    lastAvailable = currentMillis;
    if(currentByte == -1) {
      currentByte = recvOneChar();
    } 
    if (currentByte != -1 && nextByte == -1) {
      nextByte = recvOneChar();
    }
  
  if (currentByte != -1 && currentMillis - previousMillis >= interval) {

    if (currentByte & (int)(pow(2, index) + 0.5)) {
      ledState = flip ? LOW : HIGH;
      //Serial.print(flip ? 0 : 1);
    }
    else {
      ledState = flip ? HIGH : LOW;
      //Serial.print(flip ? 1 : 0);
    }
    previousMillis = currentMillis;

    if (flip) {
      index++;
      flip = 0;
    } else {
      flip = 1;
    }
    if (index >= 8) {
      index = 0;
      flip = 0;
      currentByte = -1;
      written++;
    }


    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }
}

char recvOneChar() {
  if (Serial.available()) {
    char c[1];
    Serial.readBytes(c,1);
    return c[0];
  }
  return -1;
}
