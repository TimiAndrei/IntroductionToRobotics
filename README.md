# Introduction to Robotics (2023 - 2024)

---

This repository pertains to the Robotics Laboratory assignments undertaken during the third year at the University of Bucharest's Faculty of Mathematics and Computer Science. Each assignment comprises stipulated requirements, detailed implementation instructions, as well as the requisite code and image files.

# Homework 1

---

This assignment focuses on controlling each channel (Red, Green, and Blue) of  an  RGB  LED  using  individual  potentiometers. We will use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. Specifically, we will read  the  potentiometer’s  value  with  Arduino  and  then  write  amapped value to the LED pins.  

To solve this assignment we will have a configuration like this: 

![Homework 1 setup in simulator](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Images/Homework1.png)
(Link to simulator: https://wokwi.com/projects/379468552305848321)

![Homework 1 setup in real life](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Images/Homework1_irl.jpeg)

This is the code we will have for this setup:

```
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

```

Here is a video to see the working project: https://youtu.be/cOPk6X2yHXM