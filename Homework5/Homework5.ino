#include <EEPROM.h>

// Pin Definitions
const int trigPin = 9;
const int echoPin = 10;
const int redPin = 3;
const int greenPin = 5;
const int bluePin = 6;
const int buzzerPin = 11;
const int photocellPin = A0;

// EEPROM Addresses
const int ultrasonicThresholdAddr = 0;
const int ldrThresholdAddr = 2;
const int samplingIntervalAddr = 4;
const int readingsCounterAddr = 6;
const int automaticModeAddr = 8;
const int ultrasonicStartAddr = 10;
const int ldrStartAddr = 30;
const int redColorAddr = 50;
const int greenColorAddr = 52;
const int blueColorAddr = 54;

// Menu and Submenu State enums
enum MenuState {
  sensorSettings,
  resetData,
  systemStatus,
  rgbControls,
  executingCode,
  mainMenu
};

enum SensorSettingsState {
  sensorSamplingState,
  ultrasonicThresholdState,
  ldrThresholdState,
  sensorBack
};

enum ResetDataState {
  resetYes,
  resetNo
};

enum SystemStatusState {
  currentReadings,
  currentSettings,
  loggedData,
  systemBack
};

enum RGBControlsState {
  manualInput,
  automatic,
  rgbBack
};

enum ExecutingCodeState {
  executeSensorSampling,
  executeUltrasonicThreshold,
  executeLDRThreshold,
  executeCurrentReadings,
  executeManualColor
};


// Constants and helper variables
const int second = 1000;
const int maxReadings = 10;
const int initialSamplingInterval = 1000;
const int initialUltrasonicThreshold = 2048;
const int initialLDRThreshold = 2048;
const int maxLedValue = 255;
const int minLedValue = 0;
const int buzzerFrequency = 400;
int distance = 0;
int codeToExecute = 0;
int menuState = mainMenu;
int input;
int photocellValue;
int automaticMode;
int stopInput;
int sensorReadings[maxReadings];
int ultrasonicThreshold;
int ldrThreshold;
int samplingInterval;
int samplingInput;
int redValue;
int greenValue;
int blueValue;
int savedReadingsCounter;
bool needInput = true;
bool stringComplete = false;
bool inputComplete = false;
bool seeReadings = false;
bool executingDone = false;
bool invalid = false;
char incomingByte = 0;
String inputString = "";
String RGBString;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
long duration = 0;



void setupPins() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(photocellPin, INPUT);
}

void setupMenu() {
  Serial.begin(9600);
  printMainMenu();
}

void setupEEPROM() {
  EEPROM.get(ultrasonicThresholdAddr, ultrasonicThreshold);
  EEPROM.get(ldrThresholdAddr, ldrThreshold);
  EEPROM.get(samplingIntervalAddr, samplingInterval);
  EEPROM.get(readingsCounterAddr, savedReadingsCounter);
  EEPROM.get(automaticModeAddr, automaticMode);
}


void setup() {
  setupPins();
  setupMenu();
  setupEEPROM();
}

void loop() {
  menu();
  handleReadings();
}

void menu() {
  if (Serial.available() > 0) {

    switch (menuState) {
      case mainMenu:
        {
          if (needInput) {
            getInput(input);
          }

          if (inputComplete == true) {
            inputComplete = false;
            switch (input) {
              case sensorSettings:
                {
                  menuState = sensorSettings;
                  printSensorSettingsMenu();
                  break;
                }
              case resetData:
                {
                  menuState = resetData;
                  printResetDataMenu();
                  break;
                }
              case systemStatus:
                {
                  printSystemStatusMenu();
                  menuState = systemStatus;
                  break;
                }
              case rgbControls:
                {
                  printRGBControlMenu();
                  menuState = rgbControls;
                  break;
                }
              default:
                {
                  invalid = true;
                  break;
                }

                break;
            }
          }
          case sensorSettings:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case sensorSamplingState:
                    {
                      Serial.println("Enter sensor sampling value (between 1 and 10):");
                      needInput = false;
                      menuState = executingCode;
                      codeToExecute = executeSensorSampling;
                      break;
                    }
                  case ultrasonicThresholdState:
                    {
                      Serial.print("Current Ultrasonic alert threshold is ");
                      Serial.println(ultrasonicThreshold);
                      Serial.println("Enter new Ultrasonic alert threshold minimum value:");
                      needInput = false;
                      menuState = executingCode;
                      codeToExecute = executeUltrasonicThreshold;

                      break;
                    }
                  case ldrThresholdState:
                    {
                      Serial.print("Current LDR alert threshold is ");
                      Serial.println(ldrThreshold);
                      Serial.println("Enter new LDR alert threshold minimum value:");
                      needInput = false;
                      menuState = executingCode;
                      codeToExecute = executeLDRThreshold;
                      break;
                    }
                  case sensorBack:
                    {
                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }

              break;
            }
          case resetData:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case resetYes:
                    {
                      Serial.println("Resetting Data...");
                      EEPROM.put(ultrasonicThresholdAddr, initialUltrasonicThreshold);
                      EEPROM.put(ldrThresholdAddr, initialLDRThreshold);
                      EEPROM.put(samplingIntervalAddr, initialSamplingInterval);
                      EEPROM.get(ultrasonicThresholdAddr, ultrasonicThreshold);
                      EEPROM.get(ldrThresholdAddr, ldrThreshold);
                      EEPROM.get(samplingIntervalAddr, samplingInterval);
                      resetReadings();
                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  case resetNo:
                    {
                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }
              break;
            }
          case systemStatus:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case currentReadings:
                    {
                      seeReadings = true;
                      menuState = executingCode;
                      codeToExecute = executeCurrentReadings;
                      printMainMenu();
                      break;
                    }
                  case currentSettings:
                    {
                      Serial.println("Current Settings:");
                      Serial.print("Current Sampling interval: ");
                      Serial.println(samplingInterval);
                      Serial.print("Current Ultrasonic alert threshold: ");
                      Serial.println(ultrasonicThreshold);
                      Serial.print("Current LDR alert threshold: ");
                      Serial.println(ldrThreshold);
                      Serial.println("");
                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  case loggedData:
                    {
                      Serial.println("Ultrasonic data:");
                      for (int i = ultrasonicStartAddr; i < savedReadingsCounter + ultrasonicStartAddr - 2; i += 2) {
                        int x;
                        EEPROM.get(i, x);
                        Serial.print(x);
                        Serial.print(" ");
                      }
                      Serial.println("\n\nLDR data");
                      for (int i = ldrStartAddr; i < savedReadingsCounter + ldrStartAddr - 2; i += 2) {
                        int x;
                        EEPROM.get(i, x);
                        Serial.print(x);
                        Serial.print(" ");
                      }
                      Serial.println("");


                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  case systemBack:
                    {
                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }
              break;
            }
          case rgbControls:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case manualInput:
                    {
                      Serial.println("Color code format:'255 255 255'");
                      needInput = false;
                      menuState = executingCode;
                      codeToExecute = executeManualColor;
                      break;
                    }
                  case automatic:
                    {
                      Serial.println("Automatic mode toggled");
                      menuState = mainMenu;
                      automaticMode = !automaticMode;
                      EEPROM.put(automaticModeAddr, automaticMode);
                      printMainMenu();
                      break;
                    }
                  case rgbBack:
                    {
                      menuState = mainMenu;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }
              break;
            }
          case executingCode:
            {
              switch (codeToExecute) {
                case executeSensorSampling:
                  {
                    sensorSampling();
                    break;
                  }
                case executeUltrasonicThreshold:
                  {
                    alertThreshold(ultrasonicThreshold, ultrasonicThresholdAddr);
                    break;
                  }
                case executeLDRThreshold:
                  {
                    alertThreshold(ldrThreshold, ldrThresholdAddr);
                    break;
                  }
                case executeCurrentReadings:
                  {
                    currentSensorReadings();
                    break;
                  }
                case executeManualColor:
                  {
                    changeColor();
                    break;
                  }
              }

              if (executingDone) {
                executingDone = false;
                printMainMenu();
                menuState = mainMenu;
                needInput = true;
              }
              break;
            }
          default:
            {
              invalid = true;
              break;
            }
        }
    }
    if (invalid) {
      printInvalidMessage();
      invalid = false;
    }
  }
}

void handleReadings() {
  currentMillis = millis();
  if (currentMillis - previousMillis > samplingInterval) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    previousMillis = currentMillis;
    photocellValue = analogRead(photocellPin);
    addSensorReading(photocellValue, ldrStartAddr);
    addSensorReading(distance, ultrasonicStartAddr);
    if (savedReadingsCounter < maxReadings * 2) {
      savedReadingsCounter += 2;

      EEPROM.put(readingsCounterAddr, savedReadingsCounter);
    }
    if (seeReadings) {
      Serial.print("Ultrasonic: ");
      Serial.println(distance);
      Serial.print("LDR: ");
      Serial.println(photocellValue);
      Serial.println("Press any key to exit\n");
    }
    if (automaticMode) {
      if (distance <= ultrasonicThreshold || photocellValue >= ldrThreshold) {
        redValue = maxLedValue;
        greenValue = minLedValue;
        blueValue = minLedValue;
        tone(buzzerPin, buzzerFrequency);
      } else {
        redValue = minLedValue;
        greenValue = maxLedValue;
        blueValue = minLedValue;
        noTone(buzzerPin);
      }
    } else {
      EEPROM.get(redColorAddr, redValue);
      EEPROM.get(greenColorAddr, greenValue);
      EEPROM.get(blueColorAddr, blueValue);
      noTone(buzzerPin);
    }

    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
  }
}

void getInput(int &input) {
  if (Serial.available() > 0) {
    incomingByte = (char)Serial.read();
    inputString += incomingByte;
    if (incomingByte == '\n') {
      input = atoi(inputString.c_str());
      inputString = "";
      inputComplete = true;
    }
  }
}

void getRGBInput(String &RGBString) {
  if (Serial.available() > 0) {
    incomingByte = (char)Serial.read();
    inputString += incomingByte;
    if (incomingByte == '\n') {
      RGBString = inputString;
      inputString = "";
      inputComplete = true;
    }
  }
}


void printMainMenu() {
  Serial.println("Main Menu");
  Serial.println("0.Sensor Settings");
  Serial.println("1.Reset Logger Data");
  Serial.println("2.System Status");
  Serial.println("3.RGB LED Control\n");
}

void printSensorSettingsMenu() {
  Serial.println("Sensor Settings Menu");
  Serial.println("--> 0.Sensor Sampling Interval");
  Serial.println("--> 1.Ultrasonic Alert Threshold");
  Serial.println("--> 2.LDR Alert Threshold");
  Serial.println("--> 3.Back\n");
}

void printResetDataMenu() {
  Serial.println("Reset Logger Data Menu");
  Serial.println("Are you sure you want to reset?");
  Serial.println("--> 0.Yes");
  Serial.println("--> 1.No\n");
}

void printSystemStatusMenu() {
  Serial.println("System Status Menu");
  Serial.println("--> 0.Current Sensor Readings");
  Serial.println("--> 1.Current Sensor Settings");
  Serial.println("--> 2.Display Logged Data");
  Serial.println("--> 3.Back\n");
}

void printRGBControlMenu() {
  Serial.println("RGB LED Control Menu");
  Serial.println("--> 0.Manual Color Control");
  Serial.println("--> 1.LED: Toggle Automatic ON/ OFF");
  Serial.println("--> 2.Back\n");
}

void printInvalidMessage() {
  if (input != -49) {
    Serial.println("Invalid input, try something else\n");
  }
}

void sensorSampling() {
  getInput(samplingInput);
  if (inputComplete) {
    inputComplete = false;
    if (samplingInput > 10 || samplingInput < 1) {
      printInvalidMessage();
    } else {
      samplingInterval = samplingInput * second;
      EEPROM.put(samplingIntervalAddr, samplingInterval);
      executingDone = true;
    }
  }
}

void alertThreshold(int &threshold, const int byte) {
  getInput(threshold);
  if (inputComplete) {
    inputComplete = false;
    if (threshold < 0 || threshold > 2047) {
      printInvalidMessage();
    } else {
      EEPROM.put(byte, threshold);
      Serial.println(threshold);
      executingDone = true;
    }
  }
}

void addSensorReading(int reading, int startingByte) {

  for (int i = savedReadingsCounter - 2 + startingByte; i > startingByte; i -= 2) {
    int currentValue;
    currentValue = sensorReadings[i - 2];
    EEPROM.get(i - 2, currentValue);
    EEPROM.put(i, currentValue);
  }
  EEPROM.put(startingByte, reading);
}

void resetReadings() {
  for (int i = 0; i < 20; i += 2) {
    EEPROM.put(ultrasonicStartAddr + i, 0);
    EEPROM.put(ldrStartAddr + i, 0);
  }
  EEPROM.put(readingsCounterAddr, 0);
  EEPROM.put(redColorAddr, 0);
  EEPROM.put(greenColorAddr, 0);
  EEPROM.put(blueColorAddr, 0);
  EEPROM.put(automaticModeAddr, 0);

  EEPROM.get(readingsCounterAddr, savedReadingsCounter);
  EEPROM.get(redColorAddr, redValue);
  EEPROM.get(greenColorAddr, greenValue);
  EEPROM.get(blueColorAddr, blueValue);
  EEPROM.get(automaticModeAddr, automaticMode);
}

void currentSensorReadings() {
  getInput(stopInput);
  if (inputComplete) {
    inputComplete = false;
    seeReadings = false;
    executingDone = true;
  }
}

void changeColor() {
  getRGBInput(RGBString);
  if (inputComplete) {
    inputComplete = false;
    int spaceIndex1 = RGBString.indexOf(' ');
    int spaceIndex2 = RGBString.lastIndexOf(' ');

    if (spaceIndex1 != -1 && spaceIndex2 != -1) {
      String redStr = RGBString.substring(0, spaceIndex1);
      String greenStr = RGBString.substring(spaceIndex1 + 1, spaceIndex2);
      String blueStr = RGBString.substring(spaceIndex2 + 1);
      redValue = redStr.toInt();
      greenValue = greenStr.toInt();
      blueValue = blueStr.toInt();
      if (redValue >= minLedValue && redValue <= maxLedValue && greenValue >= minLedValue && greenValue <= maxLedValue && blueValue >= minLedValue && blueValue <= maxLedValue) {
        EEPROM.put(redColorAddr, redValue);
        EEPROM.put(greenColorAddr, greenValue);
        EEPROM.put(blueColorAddr, blueValue);
        analogWrite(redPin, redValue);
        analogWrite(greenPin, greenValue);
        analogWrite(bluePin, blueValue);
        executingDone = true;
      } else {
        Serial.println("Invalid RGB values");
      }
    } else {
      Serial.println("Invalid color format");
    }
  }
}
