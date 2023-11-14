// Define connections to the shift register
const int latchPin = 11;  
const int clockPin = 10;  
const int dataPin = 12;   

// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

// Store the digits in an array for easy access
int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;  // Number of digits in the display

// Button pins
const int startPauseButtonPin = 2;
const int saveLapButtonPin = 3;
const int resetButtonPin = 8;

// interrupt and debounce variables
volatile unsigned long pauseInterruptTime = 0;
volatile unsigned long lapInterruptTime = 0;
unsigned long currentTime = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;
volatile byte lastStartPauseButtonRead = LOW;
volatile byte startPauseButtonRead = LOW;
volatile byte lastLapButtonRead = LOW;
volatile byte lapButtonRead = LOW;
volatile byte startPauseButtonState = LOW;
volatile byte lapButtonState = LOW;
byte resetButtonRead = LOW;

// laps variables
const int nLaps = 4;
unsigned long laps[nLaps] = { 0, 0, 0, 0 };
int lapCounter = 0;
bool firstIterationFlag = true;

// Define the number of unique encodings (0-9)
const int encodingsNumber = 10;
byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
};


// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  // Delay between updates (milliseconds)
unsigned long number = 0;        // The number being displayed
bool lapState = false;           //maybe will keep
const int maxDisplayValue = 10000;


enum clockState {
  stopped,
  counting,
  paused
};

clockState clockState = stopped;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(startPauseButtonPin, INPUT_PULLUP);
  pinMode(saveLapButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), handleStartPauseInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(saveLapButtonPin), handleLapInterrupt, FALLING);

  Serial.begin(9600);
}

void loop() {
  checkClockState();
}

void checkReset() {  
  resetButtonRead = digitalRead(resetButtonPin);
  if (resetButtonRead == LOW && clockState == paused) { //if resetButton is pressed and the clock is paused then reset the clock
    clockState = stopped;
    number = 0;
    lapState = false;
  }
  if (resetButtonRead == LOW && lapState) { // if clock is in lapmode and the button is pressed, reset laps
    for (int i = 0; i < nLaps; i++)
      laps[i] = 0;
    lapState = false;
    lapCounter = 0;
    clockState = stopped;
  }
}

void checkClockState() {
  if (clockState == counting) { // if the clock is in counting state, then the counter is running and update the digits
    if (millis() - lastIncrement > delayCount) {
      number++;
      number %= maxDisplayValue;
      lastIncrement = millis();
    }
  }
  if (lapState) { // if the clock is in lap mode, then display the lap number
    writeNumber(laps[lapCounter]);
  } else
    writeNumber(number); //display the number
  checkReset(); // check for reset button presses
}

void activateDisplay(int displayNumber) { 
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(int number) {
  int currentNumber = number;
  int displayDigit = 3;
  int lastDigit = 0;

  do {
    lastDigit = currentNumber % 10;
    activateDisplay(displayDigit);
    if (displayDigit == 2) {
      writeReg(byteEncodings[lastDigit] + 1);
    } else {
      writeReg(byteEncodings[lastDigit]);
    }
    displayDigit--;
    currentNumber /= 10;
    writeReg(B00000000);
  } while (displayDigit >= 0 || currentNumber > 0);
}


void writeReg(int digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  digitalWrite(latchPin, HIGH);
}

void handleStartPauseInterrupt() { // ISR function for Start/Pause button debounce
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = micros();

  startPauseButtonRead = digitalRead(startPauseButtonPin);

  if (startPauseButtonRead != lastStartPauseButtonRead) {
    lastInterruptTime = micros();
  }
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
    if (startPauseButtonRead == LOW) {
      if (clockState == stopped || clockState == paused)
        clockState = counting;
      else if (clockState == counting)
        clockState = paused;
    }
  }
  lastStartPauseButtonRead = startPauseButtonRead;
  lastInterruptTime = interruptTime;
}

void handleLapInterrupt() { // ISR function for Lap button debounce and memorizing the laps when pressed
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = micros();

  lapButtonRead = digitalRead(saveLapButtonPin);

  if (lapButtonRead != lastLapButtonRead) {
    lastInterruptTime = micros();
  }
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
    if (lapButtonRead == LOW) {
      if (clockState == counting) {
        getLap(number);
      } else {
        lapState = true;
        handleLaps();
      }
    }
  }
  lastLapButtonRead = lapButtonRead;
  lastInterruptTime = interruptTime;
}

void handleLaps() { // function to toggle between laps
  if (lapState) {
    if (firstIterationFlag) {
      firstIterationFlag = false;
    } else {
      if (lapCounter < nLaps - 1) {
        lapCounter++;
      }
      else {
        firstIterationFlag = true;
        lapCounter = 0;
      }
    }
  }
}


void getLap(int lapTime) { // function for storing the laps 
  if (lapCounter < nLaps) {
    laps[lapCounter] = lapTime % 10000;
    lapCounter++;
  }
  if (lapCounter >= nLaps)
    lapCounter = 0;
}
