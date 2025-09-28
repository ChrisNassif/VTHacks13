const int NUM_BUTTONS = 24;
uint8_t buttonStates[4];  // 32 bits (last byte unused, stays 0)

void setup() {
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = 0;
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(i, INPUT_PULLUP);  // Use pullups (LOW = pressed)
  }

  Serial.begin(115200);
}

void readButtons() {
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = 0;
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    int byteIndex = i / 8;
    int bitIndex = i % 8;

    if (digitalRead(i) == LOW) {
      buttonStates[byteIndex] |= (1 << bitIndex);
    }
  }
}

void loop() {
  readButtons();

  // Send all 4 bytes over UART as raw binary
  Serial.write(buttonStates, 4);

  delayMicroseconds(100);
}