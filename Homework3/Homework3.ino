// Pin variables
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;
const int pinBuzzer = 11;

// Buzzer variables
const int freq = 500;
const int buzzDuration = 100;
const int resetFreq = 700;
const int resetBuzzDuration = 300;

// Joystick variables
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
int xValue = 0;
int yValue = 0;
int prevXValue = 0;
int prevYValue = 0;
const int maxValue = 800;  //values for defining movement interval regarding the joystick
const int minValue = 200;

// Button related variables
unsigned long buttonPressStartTime = 0;
bool buttonState = false;
const unsigned long blinkInterval = 500;

// Debounce variables
volatile unsigned long interruptTime = 0;
volatile unsigned long lastInterruptTime = 0;
volatile byte lastButtonRead = LOW;
volatile byte buttonRead = LOW;
const unsigned long debounceDelay = 200;
const unsigned long longDelay = 1000;

// Segment variables
const int segSize = 8;
int currentSegment = 7;
int segments[segSize] = { pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP };
bool segmentsState[segSize] = { false };

// Movement variables
int RIGHT = 0;
int LEFT = 1;
int DOWN = 2;
int UP = 3;
const int movements = 4;
int movementMapping[segSize][movements] = {
  { 0, 6, 5, 1 },  // a = 0
  { 0, 6, 5, 1 },  // b = 1
  { 6, 3, 4, 7 },  // c = 2
  { 6, 3, 4, 2 },  // d = 3
  { 6, 3, 4, 2 },  // e = 4
  { 0, 6, 5, 1 },  // f = 5
  { 0, 3, 6, 6 },  // g = 6
  { 7, 7, 2, 7 }   // dp= 7
};


void setup() {
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, CHANGE);
}

void loop() {
  handleMovement();
  updateDisplay();
  blink();
}

void handleMovement() {  // this function checks for joystick movement and fits the reading into one movement (right, left, down, up)
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (xValue > maxValue && prevXValue <= maxValue) {
    currentSegment = moveSegment(currentSegment, RIGHT);
  }
  if (xValue < minValue && prevXValue >= minValue) {
    currentSegment = moveSegment(currentSegment, LEFT);
  }

  if (yValue > maxValue && prevYValue <= maxValue) {
    currentSegment = moveSegment(currentSegment, DOWN);
  }

  if (yValue < minValue && prevYValue >= minValue) {
    currentSegment = moveSegment(currentSegment, UP);
  }

  prevXValue = xValue;
  prevYValue = yValue;
}

int moveSegment(int currentSegment, int direction) {  // this function moves the current segment based on the possible movements available
  int newSegment = currentSegment;

  if (direction == UP) {
    newSegment = movementMapping[currentSegment][0];
  } else if (direction == DOWN) {
    newSegment = movementMapping[currentSegment][1];
  } else if (direction == LEFT) {
    newSegment = movementMapping[currentSegment][2];
  } else if (direction == RIGHT) {
    newSegment = movementMapping[currentSegment][3];
  }

  return newSegment;
}


void handleInterrupt() {  //interrupt function with debounce implementation
  buttonRead = digitalRead(pinSW);
  if (buttonRead != lastButtonRead) {
    interruptTime = micros();
  }
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
    if (buttonState != buttonRead) {
      buttonState = buttonRead;
    }
    if (buttonState == LOW) {
      toggleSegment(currentSegment);
    } else if (buttonState == HIGH && (interruptTime - lastInterruptTime > longDelay * 1000)) {
      resetDisplay();
    }
  }
  lastInterruptTime = interruptTime;
  lastButtonRead = buttonRead;
}


void blink() {
  if (millis() % longDelay < blinkInterval) {
    digitalWrite(segments[currentSegment], HIGH);
  } else {
    digitalWrite(segments[currentSegment], LOW);
  }
}

void toggleSegment(int segment) {
  tone(pinBuzzer,freq, buzzDuration);
  segmentsState[segment] = !segmentsState[segment];
}

void resetDisplay() {
  tone(pinBuzzer, resetFreq, resetBuzzDuration);
  for (int i = 0; i < segSize; i++) {
    segmentsState[i] = false;
  }
  currentSegment = 7;
}

void updateDisplay() {
  for (int i = 0; i < segSize; i++) {
    if (i != currentSegment) {
      digitalWrite(segments[i], segmentsState[i]);
    }
  }
}
