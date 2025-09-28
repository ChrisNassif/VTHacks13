const int NUM_BUTTONS = 24;
const int DEBOUNCE_DELAY = 20;

// Button states for FSM
enum ButtonState {
  NOT_PRESSED,
  DEBOUNCE_PRESS,
  PRESSED,
  DEBOUNCE_RELEASE
};

uint8_t buttonStates[4];      
ButtonState buttonFSM[NUM_BUTTONS];    
unsigned long stateChangeTime[NUM_BUTTONS];  

void setup() {
  // Initialize arrays
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = 0;
  }
  
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(i, INPUT_PULLUP);
    buttonFSM[i] = NOT_PRESSED;
    stateChangeTime[i] = 0;
  }

  Serial.begin(115200);
}

void updateButtonFSM(int buttonIndex, bool rawReading) {
  unsigned long currentTime = millis();
  ButtonState currentState = buttonFSM[buttonIndex];
  
  switch (currentState) {
    case NOT_PRESSED:
      if (rawReading) {  
        buttonFSM[buttonIndex] = DEBOUNCE_PRESS;
        stateChangeTime[buttonIndex] = currentTime;
      }
      break;
      
    case DEBOUNCE_PRESS:
      if (rawReading) { 
        if ((currentTime - stateChangeTime[buttonIndex]) >= DEBOUNCE_DELAY) {
          buttonFSM[buttonIndex] = PRESSED;
          int byteIndex = buttonIndex / 8;
          int bitIndex = buttonIndex % 8;
          buttonStates[byteIndex] |= (1 << bitIndex);
        }
      } else {  
        buttonFSM[buttonIndex] = NOT_PRESSED;
      }
      break;
      
    case PRESSED:
      if (!rawReading) {
        buttonFSM[buttonIndex] = DEBOUNCE_RELEASE;
        stateChangeTime[buttonIndex] = currentTime;
      }
      break;
      
    case DEBOUNCE_RELEASE:
      if (!rawReading) {  
        if ((currentTime - stateChangeTime[buttonIndex]) >= DEBOUNCE_DELAY) {
          buttonFSM[buttonIndex] = NOT_PRESSED;
          int byteIndex = buttonIndex / 8;
          int bitIndex = buttonIndex % 8;
          buttonStates[byteIndex] &= dccccccccc~(1 << bitIndex);
        }
      } else {  
        buttonFSM[buttonIndex] = PRESSED;
      }
      break;
  }
}

void readButtons() {

  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool rawReading = (digitalRead(i) == HIGH);  
    updateButtonFSM(i, rawReading);
  }
}

void loop() {
  readButtons();

  Serial.write(buttonStates, 4);

  delay(1);
}