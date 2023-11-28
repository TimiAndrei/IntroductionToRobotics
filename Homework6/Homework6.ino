#include "LedControl.h"

// Matrix constants
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = 5;

// Joystick Constants
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
int xValue = 0;
int yValue = 0;
int prevXValue = 0;
int prevYValue = 0;
const int maxValue = 900;
const int minValue = 100;

// Buzzer constants
const int pinBuzzer = 3;
const int fireFreq = 400;
const int collisionFreq = 100;
const int soundDuration = 50;
const int collisionSoundDuration = 200;

// Movement constants
const int UP = 0;
const int DOWN = 1;
const int RIGHT = 2;
const int LEFT = 3;

// Button debounce constants
volatile unsigned long interruptTime = 0;
volatile unsigned long lastInterruptTime = 0;
volatile byte lastButtonRead = LOW;
volatile byte buttonRead = LOW;
const unsigned long debounceDelay = 200;
bool buttonState = false;

// Bullet constants
int bulletPos = 0;
int lastBulletPos = 0;
unsigned long lastBulletBlinkMillis = 0;
const int bulletBlinkTime = 200;
bool bulletFired = false;

// Player constants
int currentPos = 0;
unsigned long lastBlinkMillis = 0;
bool pixelState = false;
const int playerBlinkTime = 500;
enum PlayerState {
  moving,
  staying
};
PlayerState playerState = moving;
volatile int lastMovement = LEFT;


// wall constants
const int wallPercentage = 55;
const int maxWallPercentage = 100;
bool walls[matrixSize * matrixSize];

const bool GG[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 0, 0, 0, 1, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0 },
  { 1, 1, 1, 0, 0, 0, 1, 0 },
  { 1, 0, 1, 0, 1, 0, 1, 0 },
  { 1, 1, 1, 0, 1, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, CHANGE);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  currentPos = random(matrixSize * matrixSize);
  randomSeed(analogRead(A3));
  generateRandomMap();
  lc.setLed(0, currentPos % matrixSize, currentPos / matrixSize, pixelState);
}

void generateRandomMap() { //function that generates walls skipping player and its neighbours
  lc.clearDisplay(0);
  walls[currentPos] = false;
  lc.setLed(0, currentPos % matrixSize, currentPos / matrixSize, LOW);
  int neighbors[4] = {
    currentPos + matrixSize,  
    currentPos - matrixSize,  
    currentPos - 1,           
    currentPos + 1            
  };

  for (int i = 0; i < matrixSize * matrixSize; ++i) {
    if (i == currentPos || i == neighbors[0] || i == neighbors[1] || i == neighbors[2] || i == neighbors[3]) {
      continue;
    }
    int randomValue = random(maxWallPercentage);
    if (randomValue < wallPercentage) {
      walls[i] = true;
      lc.setLed(0, i % matrixSize, i / matrixSize, HIGH);
    } else {
      walls[i] = false;
    }
  }
}

void loop() {
  if (areAllWallsRemoved()) {
    displayGGMessage();
    return;
  }
  handleMovement();
  handleBlinking();
  handleBulletMovement();
}

byte reverseByte(byte b) {
  byte reversed = 0;
  for (int i = 0; i < 8; ++i) {
    reversed = (reversed << 1) | (b & 1);
    b >>= 1;
  }
  return reversed;
}  // for reversing row data to display correctly on matrix


void displayGGMessage() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; ++row) {
    byte rowData = 0;
    for (int col = 0; col < matrixSize; ++col) {
      if (GG[row][col]) {
        rowData |= 1 << col;
      }
    }
    lc.setRow(0, row, reverseByte(rowData));
  }
}

bool areAllWallsRemoved() {
  for (int i = 0; i < matrixSize * matrixSize; ++i) {
    if (walls[i]) {
      return false;
    }
  }
  return true;
}

bool isWall(int position) {
  if (position != -1) {
    return walls[position];
  } else return false;
}

void removeWall(int position) {
  walls[position] = false;
  lc.setLed(0, position % matrixSize, position / matrixSize, LOW);
}

void handleMovement() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (xValue > maxValue && prevXValue <= maxValue) {
    currentPos = movePlayer(currentPos, RIGHT);
  }
  if (xValue < minValue && prevXValue >= minValue) {
    currentPos = movePlayer(currentPos, LEFT);
  }

  if (yValue > maxValue && prevYValue <= maxValue) {
    currentPos = movePlayer(currentPos, DOWN);
  }

  if (yValue < minValue && prevYValue >= minValue) {
    currentPos = movePlayer(currentPos, UP);
  }

  prevXValue = xValue;
  prevYValue = yValue;
}

int movePlayer(int currentPos, int direction) {
  int newPos = currentPos;
  lc.setLed(0, currentPos % matrixSize, currentPos / matrixSize, LOW);
  playerState = moving;

  if (direction == UP && currentPos < matrixSize * (matrixSize - 1) && !isWall(currentPos + matrixSize)) {
    newPos = currentPos + matrixSize;
    lastMovement = UP;
  } else if (direction == DOWN && currentPos >= matrixSize && !isWall(currentPos - matrixSize)) {
    newPos = currentPos - matrixSize;
    lastMovement = DOWN;
  } else if (direction == LEFT && currentPos % matrixSize != 0 && !isWall(currentPos - 1)) {
    newPos = currentPos - 1;
    lastMovement = LEFT;
  } else if (direction == RIGHT && (currentPos + 1) % matrixSize != 0 && !isWall(currentPos + 1)) {
    newPos = currentPos + 1;
    lastMovement = RIGHT;
  }

  lc.setLed(0, newPos % matrixSize, newPos / matrixSize, pixelState);
  return newPos;
}

void handleBlinking() {
  unsigned long currentMillis = millis();

  if (areAllWallsRemoved()) {
    return;
  }

  if (currentMillis - lastBlinkMillis >= playerBlinkTime) {
    pixelState = !pixelState;
    lc.setLed(0, currentPos % matrixSize, currentPos / matrixSize, pixelState);
    lastBlinkMillis = currentMillis;
  }
  if (playerState == moving) {
    lc.setLed(0, currentPos % matrixSize, currentPos / matrixSize, HIGH);
    playerState = staying;
  }
}

void playSound() {
  tone(pinBuzzer, fireFreq, soundDuration);
}

void playCollisionSound() {
  tone(pinBuzzer, collisionFreq, collisionSoundDuration);
}

void handleInterrupt() {
  buttonRead = digitalRead(pinSW);
  if (buttonRead != lastButtonRead) {
    interruptTime = micros();
  }
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
    if (buttonState != buttonRead) {
      buttonState = buttonRead;
    }
    if (buttonState == LOW) {
      fireBullet();
    }
  }
  lastInterruptTime = interruptTime;
  lastButtonRead = buttonRead;
}

void fireBullet() {
  bulletFired = true;
  bulletPos = currentPos;
  lastBulletPos = currentPos;
  playSound();
}

void handleBulletMovement() {
  static unsigned long lastBulletMoveMillis = 0;
  const unsigned long bulletMoveDelay = 40;

  if (bulletFired) {
    unsigned long currentMillis = millis();
    switchBulletLed(bulletPos, true);
    if (currentMillis - lastBulletMoveMillis >= bulletMoveDelay) {
      switchBulletLed(lastBulletPos, false);
      lastBulletPos = bulletPos;
      if (lastMovement == UP && bulletPos < matrixSize * (matrixSize - 1) && !isWall(bulletPos + matrixSize)) {
        bulletPos = bulletPos + matrixSize;
      } else if (lastMovement == DOWN && bulletPos >= matrixSize && !isWall(bulletPos - matrixSize)) {
        bulletPos = bulletPos - matrixSize;
      } else if (lastMovement == LEFT && bulletPos % matrixSize != 0 && !isWall(bulletPos - 1)) {
        bulletPos = bulletPos - 1;
      } else if (lastMovement == RIGHT && (bulletPos + 1) % matrixSize != 0 && !isWall(bulletPos + 1)) {
        bulletPos = bulletPos + 1;
      } else {
        if (isWall(computeBulletNextPos())) {
          playCollisionSound();
          removeWall(computeBulletNextPos());
        }
        bulletFired = false;
      }
      lastBulletMoveMillis = currentMillis;
    }
    switchBulletLed(bulletPos, false);
  }
}

void switchBulletLed(int position, bool state) {
  if (position != -1 && position != currentPos) {
    lc.setLed(0, position % matrixSize, position / matrixSize, state);
  }
}

int computeBulletNextPos() {
  if (lastMovement == UP && bulletPos < matrixSize * (matrixSize - 1)) {
    return bulletPos + matrixSize;
  } else if (lastMovement == DOWN && bulletPos >= matrixSize) {
    return bulletPos - matrixSize;
  } else if (lastMovement == LEFT && bulletPos % matrixSize != 0) {
    return bulletPos - 1;
  } else if (lastMovement == RIGHT && (bulletPos + 1) % matrixSize != 0) {
    return bulletPos + 1;
  } else return -1;
}
