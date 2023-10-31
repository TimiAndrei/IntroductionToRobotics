// RGB led pins and helper constants
const int rgbPinRed = 5;
const int rgbPinGreen = 6;
int redBrightness = 0;
int redFadeAmount = 5;
const int redFadeDelay = 10;
unsigned long previousMillis = 0;

// floor leds constants
const int floor0Led = 2;
const int floor1Led = 4;
const int floor2Led = 7;
const int ledPins[] = {floor0Led, floor1Led, floor2Led};
byte floor0LedState = HIGH;
byte floor1LedState = LOW;
byte floor2LedState = LOW;
byte ledStates[] = {floor0LedState, floor1LedState, floor2LedState};
const int ledComfortBrightness = 200;

// buzzer constants
const int buzzerPin = 11;

// elevator constants
const int floor0Button = 8;
const int floor1Button = 12;
const int floor2Button = 13;
byte elevatorLedState = HIGH;  //Low if the lift is moving, high if not
byte floor0ButtonState = LOW;
byte floor1ButtonState = LOW;
byte floor2ButtonState = LOW;
byte floor0LastRead = LOW;
byte floor1LastRead = LOW;
byte floor2LastRead = LOW;
byte floor0ButtonRead = LOW;
byte floor1ButtonRead = LOW;
byte floor2ButtonRead = LOW;
int currentFloor = 0;
int targetFloor = 0;

// debounce constants
unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 30;

// buzzer constants
unsigned long doorsDuration = 500;
unsigned long movingDuration = 1500; 
const int movingSound = 200;
const int doorsSound = 400;

// other constants
unsigned long waitTime = 1250;
unsigned long startTime = 0;

enum ElevatorMovementState {
  ELEVATOR_IDLE,
  ELEVATOR_CURRENT_DOORS,
  ELEVATOR_TARGET_DOORS,
  ELEVATOR_MOVING,
};

ElevatorMovementState elevatorMovementState = ELEVATOR_IDLE;
unsigned long elevatorMovementStartTime = 0;


void setup() {
  pinMode(rgbPinRed, OUTPUT);
  pinMode(rgbPinGreen, OUTPUT);

  pinMode(floor0Button, INPUT_PULLUP); 
  pinMode(floor1Button, INPUT_PULLUP);
  pinMode(floor2Button, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);
  pinMode(floor0Led, OUTPUT);
  pinMode(floor1Led, OUTPUT);
  pinMode(floor2Led, OUTPUT);

  digitalWrite(floor0Led, HIGH);

  Serial.begin(9600);
}

void loop() {

  floor0ButtonRead = digitalRead(floor0Button);
  floor1ButtonRead = digitalRead(floor1Button);
  floor2ButtonRead = digitalRead(floor2Button);  
  

  if (floor0ButtonRead != floor0LastRead ){
    lastDebounceTime = millis();
  }
  if (floor1ButtonRead != floor1LastRead ){
    lastDebounceTime = millis();
  }
  if (floor2ButtonRead != floor2LastRead ){
    lastDebounceTime = millis();
  }
  if(( millis() - lastDebounceTime) > debounceDelay)
  {
    if(floor0ButtonRead != floor0ButtonState ){
      floor0ButtonState = floor0ButtonRead;

      if(floor0ButtonState == LOW){
        targetFloor = 0;
      }
    }

    if(floor1ButtonRead != floor1ButtonState ){
      floor1ButtonState = floor1ButtonRead;

      if(floor1ButtonState == LOW){
        targetFloor = 1;
      }
    }

    if(floor2ButtonRead != floor2ButtonState ){
      floor2ButtonState = floor2ButtonRead;

      if(floor2ButtonState == LOW){
        targetFloor = 2;
      }
    }
  } // debounce for all 3 buttons

  unsigned long currentTime = millis();
  static unsigned long waitStartTime = 0;
  static int Step = 0;

  switch (elevatorMovementState) {
    case ELEVATOR_IDLE:
    {
      if (targetFloor != currentFloor) {
        elevatorMovementState = ELEVATOR_CURRENT_DOORS;
        waitStartTime = currentTime;
        elevatorLedState = LOW;
        makeDoorsSound(); // start the door sound
      }
      break;
    }
    case ELEVATOR_CURRENT_DOORS:
    {
      if (currentTime - waitStartTime >= doorsDuration) { // wait 500 ms
        elevatorMovementState = ELEVATOR_MOVING; // change the elevator state
        waitStartTime = currentTime; // reset the tiemr for the next clock
      }
      break; 
    }
    case ELEVATOR_MOVING:
    {
      if (abs(targetFloor - currentFloor) == 1){
        switch (Step) {
          case 0:
            makeMovingSound(); // start the moving 1500ms sound 
            ledStates[currentFloor] = LOW; // turn off the current floor led
            waitStartTime = currentTime; // start timer for the next step's clock
            Step = 1; // go to next step
            break; 
          case 1:
            if (currentTime - waitStartTime >= movingDuration ) { // wait 1500ms
              makeDoorsSound(); // make the door sound 500ms
              Step = 2; //go to next step
              waitStartTime = currentTime; //update the start time for the next clock timer
            }
            break;
          case 2:
            if (currentTime - waitStartTime >= doorsDuration) { // wait 500ms for the door sound -- DON'T NEED TO WAIT FOR DOOR, WE ALREADY WAIT IN TARGET_DOORS
              waitStartTime = currentTime; //update the starting time for the next clock timer
              Step = 0; //reset Step variable 
              elevatorMovementState = ELEVATOR_TARGET_DOORS; // change the elevator state to final state
            }
            break;
        }
      }
      else if (abs(targetFloor - currentFloor) == 2) {
        switch (Step) {
          case 0:
            makeMovingSound(); // start the moving 1500ms sound
            ledStates[currentFloor] = LOW; // turn off the current floor led
            waitStartTime = currentTime; //start timer for the next clock 
            Step = 1;
            break;
          case 1:
            if (currentTime - waitStartTime >= waitTime) { // wait 1250ms
              ledStates[1] = HIGH;  // turn on the middle floor led
              makeMovingSound(); //start another moving sound 1500ms
              waitStartTime = currentTime; // reset timer for the next step's clock
              Step = 2; // go to next step 
            }
            break;
          case 2:
            if (currentTime - waitStartTime >= doorsDuration) { // wait 500ms
              ledStates[1] = LOW; // turn off the middle floor led 
              waitStartTime = currentTime; // reset timer for the next step's clock
              Step = 3; // go to next step
            }
            break;
          case 3:
            if (currentTime - waitStartTime >= waitTime) { // wait 1250ms
              makeDoorsSound(); // make the floor sound 500ms 
              waitStartTime = currentTime; // reset timer for the final state
              Step = 0; // reset step counter
              elevatorMovementState = ELEVATOR_TARGET_DOORS; // change elevator to final state
            }
            break;
        }
      }
      break;
    }
    case ELEVATOR_TARGET_DOORS: //final state of the elevator ( it has arrived at target floor)
    {
        ledStates[targetFloor] = HIGH; //change target floor Led state to HIGH 
        currentFloor = targetFloor;
        elevatorLedState = HIGH;
        elevatorMovementState = ELEVATOR_IDLE; // change elevator state to idle
      
      break;
    } 
  }


  digitalWrite(floor0Led, ledStates[0]);
  digitalWrite(floor1Led, ledStates[1]);
  digitalWrite(floor2Led, ledStates[2]);

  elevatorLed(elevatorLedState);
  
  floor0LastRead = floor0ButtonRead;
  floor1LastRead = floor1ButtonRead;
  floor2LastRead = floor2ButtonRead;

}

void elevatorLed(byte elevatorLedState)
{
    if (elevatorLedState == LOW) {
      analogWrite(rgbPinGreen, 0); //turn off the green led
    if ((millis() - previousMillis) > redFadeDelay) {
      previousMillis = millis();
      redBrightness = redBrightness + redFadeAmount;
      if (redBrightness <= 0 || redBrightness >= ledComfortBrightness) {
        redFadeAmount = -redFadeAmount; 
      }

      analogWrite(rgbPinRed, redBrightness);
    } // when elevatorState is LOW, the red led smoothly blinks
  } else if (elevatorLedState == HIGH) {
    analogWrite(rgbPinRed, 0);  
    analogWrite(rgbPinGreen, ledComfortBrightness); 
  } // when elevatorState is not LOW, the green led lights up 
}

void makeDoorsSound() {
    tone(buzzerPin, doorsSound, doorsDuration);
}

void makeMovingSound() {
    tone(buzzerPin, movingSound, movingDuration);
}
