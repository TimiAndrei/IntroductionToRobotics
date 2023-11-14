# Introduction to Robotics (2023 - 2024)

---

This repository pertains to the Robotics Laboratory assignments undertaken during the third year at the University of Bucharest's Faculty of Mathematics and Computer Science. Each assignment comprises stipulated requirements, detailed implementation instructions, as well as the requisite code and image files.

# Homework 1

---

This assignment focuses on controlling each channel (Red, Green, and Blue) of an RGB LED using individual potentiometers. We will use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. Specifically, we will read the potentiometer’s value with Arduino and then write amapped value to the LED pins.

To solve this assignment we will have a configuration like this:

![Homework 1 schema](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Homework1/Homework1.png)
(Link to simulator: https://wokwi.com/projects/379468552305848321)

![Homework 1 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Homework1/Homework1_irl.jpeg)

This is the code we will have for this setup:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/372f497a38467b5c5c651583c9a5b094905ad342/Homework1/Homework1.ino

Here is a video to see the working project: https://youtu.be/cOPk6X2yHXM

# Homework 2

---

In this assignment we need to simulate a 3-floor elevator control system using LEDs, buttons and a buzzer. Components consist of 4 LEDs, 3 buttons, 1 buzzer and rezistors.

-> Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up. Additionally, another LED should represent the elevator’s operational state. It should blink when the elevator is moving and remain static when stationary.
(For the state LED I used an RGB led, having it blinking red when operating and remain static green when stationary)

-> Implement 3 buttons that represent the call buttons from the 3 floors. When pressed, the elevator should simulate movement towards the floor after a short interval (2-3 seconds).

-> The buzzer should sound briefly during the following scenarios:
– Elevator arriving at the desired floor (something resembling a ”cling”).
– Elevator doors closing and movement.

-> If the elevator is already at the desired floor, pressing the button for that floor should have no effect. Otherwise, after a button press, the elevator should ”wait for the doors to close” and then ”move” to the corresponding floor. If the elevator is in movement, it should either do nothing or it should stack its decision (get to the first programmed floor, open the doors, wait, close them and then go to the next desired floor).

To solve this assignment we will have a configuration like this:

![Homework 2 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/372f497a38467b5c5c651583c9a5b094905ad342/Homework2/Homework2_irl.jpeg)

This is the code I used for this homework:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/372f497a38467b5c5c651583c9a5b094905ad342/Homework2/Homework2.ino

Here is a video to see the working project: https://youtu.be/P__fKOr1wOE

# Homework 3

---

For this homework we will use the joystick to control the position of the segment and ”draw” on the display. The movement between segments should be natural, meaning they should jump from the current position only to neighbors, but without passing through ”walls”.

-> The initial position should be on the DP.

-> The current position always blinks (irrespective of the fact that the segment is on or off).

-> Use the joystick to move from one position to neighbors (see table for corresponding movement).

-> Short pressing the button toggles the segment state from ON to OFF or from OFF to ON.

-> Long pressing the button resets the entire display by turning all the segments OFF and moving the current position to the decimal point.

-> Must implement interrupts

-> Bonus: added sound when toggling

To solve this assignment we will have a configuration like this:
![Homework 3 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/e6d21f2cb4b4ee777bc9788aa665f2287ef58528/Homework3/Homework3_irl.jpeg)

This is the code I used for this homework:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/e6d21f2cb4b4ee777bc9788aa665f2287ef58528/Homework3/Homework3.ino

Here is a video to see the working project: https://youtu.be/dbHrMogH7ek

# Homework 4

---

For this assignment we will be using the 4 digit 7 segment display and 3 buttons, we will implement a stopwatch timer that counts in 10ths of a second and has a save lap functionality.

Requirements: 1 7-segment display, 3 buttons, resistors and wires

The starting value of the 4 digit 7 segment display should be ”000.0”. The buttons should have the following functionalities:

->Button 1: Start / pause.

->Button 2: Reset (if in pause mode). Reset saved laps (if in lap viewing mode).

->Button 3: Save lap (if in counting mode), cycle through last saved laps (up to 4 laps).

The workflow must look something like this:

-> Display shows ”000.0”. When pressing the Start button, the timer should start.

-> During timer counter, each time you press the lap button, you should save that timer’s value in memory (not persistent, it is OK to be deleted upon reset), up to 4 laps (or more if you want); pressing the 5th time should override the 1st saved one. If you press the reset button while timer works, nothing happens. If you press the pause button, the timer stops.

-> In Pause Mode, the lap flag button doesn’t work anymore. Pressing the reset button resets you to 000.0.

-> After reset, you can now press the flag buttons to cycle through the lap times. Each time you press the flag button, it takes you to the next saved lap. Pressing it continuously should cycle you through it continuously. Pressing the reset button while in this state resets all your flags and takes the timer back to ”000.0”.

To solve this assignment we will have a configuration like this:
![Homework 4 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/809757adfea6264a6e7100d412fd9aabb7539e47/Homework4/Homework4_irl.jpeg)

This is the code I used for this homework:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/809757adfea6264a6e7100d412fd9aabb7539e47/Homework4/Homework4.ino

Here is a video to see the working project: https://youtu.be/ajQxxiOyL9E
