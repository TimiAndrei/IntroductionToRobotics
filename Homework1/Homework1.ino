const int potPinRed = A0;
const int potPinGreen = A1;
const int potPinBlue = A2;

const int rgbPinRed = 3;
const int rgbPinGreen = 5;
const int rgbPinBlue = 6;

const int analogMaxValue = 1023;
const int analogWriteMaxValue = 255;

void setup() {
  pinMode(potPinRed, INPUT);    
  pinMode(potPinGreen, INPUT);  
  pinMode(potPinBlue, INPUT);   

  pinMode(rgbPinRed, OUTPUT);   
  pinMode(rgbPinGreen, OUTPUT);  
  pinMode(rgbPinBlue, OUTPUT);   
}

void loop() {
  int redValue = map(analogRead(potPinRed), 0, analogMaxValue, 0, analogWriteMaxValue);
  int greenValue = map(analogRead(potPinGreen), 0, analogMaxValue, 0, analogWriteMaxValue);
  int blueValue = map(analogRead(potPinBlue), 0, analogMaxValue, 0, analogWriteMaxValue);

  analogWrite(rgbPinRed, redValue);
  analogWrite(rgbPinGreen, greenValue);
  analogWrite(rgbPinBlue, blueValue);
}
