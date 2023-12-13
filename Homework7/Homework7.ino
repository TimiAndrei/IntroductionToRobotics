#include "LedControl.h"
#include <EEPROM.h>

//////////////////////////////////////////////////////////////////////////
// Custom chars
byte arrowDown[] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B01110,
  B01110,
  B00100,
  B00000
};

byte arrowUp[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B00000
};

byte doubleArrows[] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B00000,
  B11111,
  B01110,
  B00100
};

// Matrix constants
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 13;  // de schimbat si la tema efectiv, era 10 inainte
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = 5;
int tempMatrixBrightness;
int tempLcdBrightness;

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
int score = 0;
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
const int wallPercentage = 10;
const int maxWallPercentage = 100;
bool walls[matrixSize * matrixSize];

// Display constants
const int maxContrast = 255;
const int maxLCDDisplayLength = 16;
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
const bool HI[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 1, 1 },
  { 1, 1, 0, 1, 1, 0, 0, 0 },
  { 1, 1, 0, 1, 1, 0, 1, 1 },
  { 1, 1, 1, 1, 1, 0, 1, 1 },
  { 1, 1, 1, 1, 1, 0, 1, 1 },
  { 1, 1, 0, 1, 1, 0, 1, 1 },
  { 1, 1, 0, 1, 1, 0, 1, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

volatile int displayContrast = 50;
int tempDisplayContrast = 0;
////////////////////////////////////
// EEPROM addresses for the saved settings
const int addrLcdBrightness = 0;
const int addrMatrixBrightness = sizeof(displayContrast);

// button handling
unsigned long lastDebounceTime = 0;  // the last time the button state was toggled
bool lastButtonState = HIGH;         // Assuming the button is in the HIGH state when not pressed

////// LCD
#include <LiquidCrystal.h>
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const int contrastPin = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setDisplayContrast() {
  analogWrite(contrastPin, displayContrast);
}
// Menu states
enum GameState {
  MENU,
  GAME
};

GameState currentState = MENU;

enum MenuStates {
  MAIN_MENU,
  SETTINGS_SUBMENU,
  ABOUT_SUBMENU,
  HTP_SUBMENU,
  MATRIX_BRIGHTNESS_ADJUST,
  GAME_ONGOING,
  GAME_OVER,
  LCD_BRIGHTNESS_ADJUST
};

enum MenuItems {
  START_GAME,
  HIGHSCORE,
  SETTINGS,
  ABOUT,
  HOW_TO_PLAY,
  NUM_MENU_ITEMS
};

enum SettingsItems {
  LCD_BRIGHTNESS,
  MATRIX_BRIGHTNESS,
  SETTINGS_BACK,
  NUM_SETTINGS_ITEMS
};

enum AboutItems {
  GAME_NAME,
  AUTHOR,
  GITHUB,
  ABOUT_BACK,
  NUM_ABOUT_ITEMS
};

enum HowToPlayItems {
  HTP_1,
  HTP_2,
  HTP_3,
  HTP_4,
  HTP_BACK,
  NUM_HTP_ITEMS
};

int currentMenuItem = 0;
MenuStates currentMenuState = MAIN_MENU;
bool menuNeedsUpdate = true;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings functions

void loadSettings() {
  displayContrast = EEPROM.read(addrLcdBrightness);
  matrixBrightness = EEPROM.read(addrMatrixBrightness);
  tempDisplayContrast = displayContrast;
  tempMatrixBrightness = matrixBrightness;
  tempLcdBrightness = displayContrast;
  setDisplayContrast();
  lc.setIntensity(0, matrixBrightness);
}
void start_game() {
  currentPos = random(matrixSize * matrixSize);
  randomSeed(analogRead(A3));
  generateRandomMap();
  lc.setLed(0, currentPos % matrixSize, currentPos / matrixSize, pixelState);
}

void displayHI() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, HI[row][col]);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, CHANGE);
  lc.shutdown(0, false);
  // load settings

  loadSettings();
  lcd.begin(16, 2);
  lcd.print("Welcome!");
  delay(3000);  // Display the welcome message for 3 seconds
  lcd.clear();
  displayHI();

  // Create and upload custom characters
  lcd.createChar(0, arrowDown);
  lcd.createChar(1, arrowUp);
  lcd.createChar(2, doubleArrows);
}

// Generare mapa
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generateRandomMap() {  // function that generates walls skipping player and its neighbours
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LCD and Menu functions

void handleMenuNavigation() {
  int previousMenuItem = currentMenuItem;
  xValue = analogRead(pinX);

  if (xValue > maxValue && prevXValue <= maxValue) {
    if (currentMenuState == MAIN_MENU) {
      currentMenuItem++;
      if (currentMenuItem >= NUM_MENU_ITEMS) {
        currentMenuItem = NUM_MENU_ITEMS - 1;  // Limit scrolling to the last menu item
      }
    } else if (currentMenuState == ABOUT_SUBMENU) {
      currentMenuItem++;
      if (currentMenuItem >= NUM_ABOUT_ITEMS) {
        currentMenuItem = NUM_ABOUT_ITEMS - 1;  // Limit scrolling to the last menu item
      }
    } else if (currentMenuState == SETTINGS_SUBMENU) {
      currentMenuItem++;
      if (currentMenuItem >= NUM_SETTINGS_ITEMS) {
        currentMenuItem = NUM_SETTINGS_ITEMS - 1;  // Limit scrolling to the last menu item
      }
    } else if (currentMenuState == HTP_SUBMENU) {
      currentMenuItem++;
      if (currentMenuItem >= NUM_HTP_ITEMS) {
        currentMenuItem = NUM_HTP_ITEMS - 1;  // Limit scrolling to the last menu item
      }
    }
  }

  if (xValue < minValue && prevXValue >= minValue) {
    currentMenuItem--;
    if (currentMenuItem <= 0) {
      currentMenuItem = 0;  // Limit scrolling to the first menu item
    }
  }

  prevXValue = xValue;

  if (currentMenuItem != previousMenuItem) {
    menuNeedsUpdate = true;
  }
}

void adjustMatrixBrightness() {
  yValue = analogRead(pinY);

  // Adjust brightness based on joystick movement
  if (yValue > maxValue && prevYValue <= maxValue && tempMatrixBrightness > 0) {
    tempMatrixBrightness--;
    lc.setIntensity(0, tempMatrixBrightness);  // Update matrix brightness immediately
    menuNeedsUpdate = true;
  } else if (yValue < minValue && prevYValue >= minValue && tempMatrixBrightness < 15) {
    tempMatrixBrightness++;
    lc.setIntensity(0, tempMatrixBrightness);  // Update matrix brightness immediately
    menuNeedsUpdate = true;
  }
  prevYValue = yValue;

  bool currentButtonState = digitalRead(pinSW);
  // Check for state change from HIGH to LOW (button press)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    // Software debounce: Check if enough time has passed since last state change
    if (millis() - lastDebounceTime > debounceDelay) {
      // Actual button action
      matrixBrightness = tempMatrixBrightness;
      EEPROM.update(addrMatrixBrightness, matrixBrightness);
      currentMenuItem = 0;
      menuNeedsUpdate = true;

      lastDebounceTime = millis();  // Reset the debounce timer
    }
  }
  lastButtonState = currentButtonState;  // Update the last button state
}

void adjustLcdBrightness() {
  yValue = analogRead(pinY);
  const int stepSize = maxContrast / maxLCDDisplayLength;  // Dividing the PWM range into 16 steps

  // Increase brightness
  if (yValue > maxValue && prevYValue <= maxValue && tempLcdBrightness > 0) {
    tempLcdBrightness -= stepSize;
    analogWrite(contrastPin, tempLcdBrightness);
    menuNeedsUpdate = true;
  }
  // Decrease brightness
  else if (yValue < minValue && prevYValue >= minValue && tempLcdBrightness < maxContrast - stepSize) {
    tempLcdBrightness += stepSize;
    analogWrite(contrastPin, tempLcdBrightness);
    menuNeedsUpdate = true;
  }
  prevYValue = yValue;

  bool currentButtonState = digitalRead(pinSW);
  // Check for state change from HIGH to LOW (button press)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      displayContrast = tempLcdBrightness;
      // Optionally update EEPROM if you want to save the contrast setting
      EEPROM.update(addrLcdBrightness, displayContrast);
      currentMenuItem = 0;
      menuNeedsUpdate = true;

      lastDebounceTime = millis();  // Reset the debounce timer
    }
  }
  lastButtonState = currentButtonState;  // Update the last button state
}

void displayMenu() {
  // Clear the LCD only if necessary to avoid flickering
  if (menuNeedsUpdate) {
    lcd.clear();
    if (currentMenuState == MAIN_MENU) {
      // Display "Main Menu" with scroll indicators
      lcd.setCursor(0, 0);
      lcd.print("Main Menu ");
      if (currentMenuItem == 0)
        lcd.write((byte)0);  // Display arrowDown
      else if (currentMenuItem == NUM_MENU_ITEMS - 1)
        lcd.write((byte)1);  // Display arrowUP
      else
        lcd.write((byte)2);  // Display doubleArrows

      // Display the current menu item
      lcd.setCursor(0, 1);
      switch (currentMenuItem) {
        case START_GAME:
          lcd.print(">Start Game");
          break;
        case HIGHSCORE:
          lcd.print(">Highscore");
          break;
        case SETTINGS:
          lcd.print(">Settings");
          break;
        case ABOUT:
          lcd.print(">About");
          break;
        case HOW_TO_PLAY:
          lcd.print(">How to play");
          break;
      }
    } else if (currentMenuState == SETTINGS_SUBMENU) {
      // Display "Settings" with scroll indicators
      lcd.setCursor(0, 0);
      lcd.print("Settings ");
      if (currentMenuItem == 0)
        lcd.write((byte)0);  // Display arrowDown
      else if (currentMenuItem == NUM_SETTINGS_ITEMS - 1)
        lcd.write((byte)1);  // Display arrowUP
      else
        lcd.write((byte)2);  // Display doubleArrows

      // Display the current settings item
      lcd.setCursor(0, 1);
      switch (currentMenuItem) {
        case LCD_BRIGHTNESS:
          lcd.print(">LCD Brightness");
          break;
        case MATRIX_BRIGHTNESS:
          lcd.print(">Mat Brightness");
          break;
        case SETTINGS_BACK:
          lcd.print(">Back");
          break;
      }
    } else if (currentMenuState == ABOUT_SUBMENU) {
      lcd.setCursor(0, 0);
      lcd.print("About ");
      if (currentMenuItem == 0)
        lcd.write((byte)0);  // Display arrowDown
      else if (currentMenuItem == NUM_ABOUT_ITEMS - 1)
        lcd.write((byte)1);  // Display arrowUP
      else
        lcd.write((byte)2);  // Display doubleArrows

      // Display the current menu item
      lcd.setCursor(0, 1);
      switch (currentMenuItem) {
        case GAME_NAME:
          lcd.print(">Timi's Shooter");
          break;
        case AUTHOR:
          lcd.print(">Timi ofc");
          break;
        case GITHUB:
          lcd.print(">TimiAndrei");
          break;
        case ABOUT_BACK:
          lcd.print(">Back");
          break;
      }
    } else if (currentMenuState == HTP_SUBMENU) {
      lcd.setCursor(0, 0);
      lcd.print("How to play ");
      if (currentMenuItem == 0)
        lcd.write((byte)0);  // Display arrowDown
      else if (currentMenuItem == NUM_HTP_ITEMS - 1)
        lcd.write((byte)1);  // Display arrowUP
      else
        lcd.write((byte)2);  // Display doubleArrows

      // Display the current menu item
      lcd.setCursor(0, 1);
      switch (currentMenuItem) {
        case HTP_1:
          lcd.print(">Move with js");
          break;
        case HTP_2:
          lcd.print(">Shoot with btn");
          break;
        case HTP_3:
          lcd.print(">Destroy walls");
          break;
        case HTP_4:
          lcd.print(">Have fun");
          break;
        case HTP_BACK:
          lcd.print(">Back");
          break;
      }
    } else if (currentMenuState == MATRIX_BRIGHTNESS_ADJUST) {
      // Display the current brightness level
      lcd.setCursor(0, 0);
      lcd.print("Mat Brightness:");
      lcd.setCursor(0, 1);
      for (int i = 0; i < tempMatrixBrightness; i++) {
        lcd.print("#");  // Display a progress bar
      }
    } else if (currentMenuState == LCD_BRIGHTNESS_ADJUST) {
      int displayLevel = map(tempLcdBrightness, 0, maxContrast, 0, maxLCDDisplayLength);

      lcd.setCursor(0, 0);
      lcd.print("LCD Brightness:");
      lcd.setCursor(0, 1);
      for (int i = 0; i < displayLevel; i++) {
        lcd.print("#");
      }
    } else if (currentMenuState == GAME_ONGOING) {
      displayScore();
      displayWallsLeft();
    } else if (currentMenuState == GAME_OVER) {
      displayGameOver();
    }
    menuNeedsUpdate = false;
  }
}

void selectMenuItem() {
  switch (currentMenuState) {
    case MAIN_MENU:
      // Handle main menu item selection
      switch (currentMenuItem) {
        case START_GAME:
          currentState = GAME;
          currentMenuState = GAME_ONGOING;
          start_game();
          break;
        case HIGHSCORE:
          // Highscore logic
          break;
        case SETTINGS:
          currentMenuItem = 0;
          currentMenuState = SETTINGS_SUBMENU;
          break;
        case ABOUT:
          currentMenuItem = 0;
          currentMenuState = ABOUT_SUBMENU;
          break;
        case HOW_TO_PLAY:
          currentMenuItem = 0;
          currentMenuState = HTP_SUBMENU;
          break;
      }
      break;

    case ABOUT_SUBMENU:
      if (currentMenuItem == ABOUT_BACK) {
        currentMenuState = MAIN_MENU;
      }
      break;

    case SETTINGS_SUBMENU:
      if (currentMenuItem == MATRIX_BRIGHTNESS) {
        currentMenuState = MATRIX_BRIGHTNESS_ADJUST;
        menuNeedsUpdate = true;  // Ensure the menu is updated
      }
      if (currentMenuItem == LCD_BRIGHTNESS) {
        currentMenuState = LCD_BRIGHTNESS_ADJUST;
        menuNeedsUpdate = true;  // Ensure the menu is updated
      }
      if (currentMenuItem == SETTINGS_BACK) {
        currentMenuState = MAIN_MENU;
      }
      break;

    case HTP_SUBMENU:
      if (currentMenuItem == HTP_BACK) {
        currentMenuState = MAIN_MENU;
      }
      break;

    case MATRIX_BRIGHTNESS_ADJUST:
      adjustMatrixBrightness();
      currentMenuState = SETTINGS_SUBMENU;
      break;

    case LCD_BRIGHTNESS_ADJUST:
      adjustLcdBrightness();
      currentMenuState = SETTINGS_SUBMENU;
      break;

    case GAME_OVER:
      currentMenuState = MAIN_MENU;
      currentState = MENU;
      break;
  }
  menuNeedsUpdate = true;
}

// new loop:
void loop() {
  if (currentState == MENU) {
    if (currentMenuState == MATRIX_BRIGHTNESS_ADJUST) {
      adjustMatrixBrightness();
    }
    if (currentMenuState == LCD_BRIGHTNESS_ADJUST) {
      adjustLcdBrightness();
    }
    handleMenuNavigation();
    if (menuNeedsUpdate) {
      displayMenu();
      menuNeedsUpdate = false;
    }
  } else if (currentState == GAME) {
    if (currentMenuState == GAME_ONGOING) {
      if (menuNeedsUpdate) {
      displayMenu();
      menuNeedsUpdate = false;
    }
    }
    handleGameLogic();
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Endgame logic
int wallsLeft() {
  int wallsLeft = 0;
  for (int i = 0; i < matrixSize * matrixSize; ++i) {
    if (walls[i]) {
      wallsLeft++;
    }
  }
  return wallsLeft;
}

void displayWallsLeft() {
  lcd.setCursor(0, 1);
  lcd.print("Walls left: ");
  lcd.print(wallsLeft());
}

void displayScore() {
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
}

void displayGameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void handleGameLogic() {
  if (areAllWallsRemoved()) {
    displayGGMessage();
    currentState = MENU;
    currentMenuState = GAME_OVER;
    menuNeedsUpdate = true;
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
  } else
    return false;
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
      if (currentState == GAME) {
        fireBullet();
      } else if (currentState == MENU) {
        selectMenuItem();
      }
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
          score++;
          menuNeedsUpdate=true;
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
  } else
    return -1;
}