# Introduction to Robotics (2023 - 2024)

---

This repository pertains to the Robotics Laboratory assignments undertaken during the third year at the University of Bucharest's Faculty of Mathematics and Computer Science. Each assignment comprises stipulated requirements, detailed implementation instructions, as well as the requisite code and image files.

# Homework 1

---

This assignment focuses on controlling each channel (Red, Green, and Blue) of an RGB LED using individual potentiometers. We will use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. Specifically, we will read the potentiometer’s value with Arduino and then write amapped value to the LED pins.

<details>
<summary>Homework 1 details</summary>

To solve this assignment we will have a configuration like this:

![Homework 1 schema](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Homework1/Homework1.png)
(Link to simulator: https://wokwi.com/projects/379468552305848321)

![Homework 1 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Homework1/Homework1_irl.jpeg)

This is the code we will have for this setup:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/372f497a38467b5c5c651583c9a5b094905ad342/Homework1/Homework1.ino

Here is a video to see the working project: https://youtu.be/cOPk6X2yHXM

</details>

# Homework 2

---

In this assignment we need to simulate a 3-floor elevator control system using LEDs, buttons and a buzzer. Components consist of 4 LEDs, 3 buttons, 1 buzzer and rezistors.

<details>
<summary>Homework 2 details</summary>

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

</details>

# Homework 3

---

For this homework we will use the joystick to control the position of the segment and ”draw” on the display. The movement between segments should be natural, meaning they should jump from the current position only to neighbors, but without passing through ”walls”.

<details>
<summary>Homework 3 details</summary>
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

</details>

# Homework 4

---

For this assignment we will be using the 4 digit 7 segment display and 3 buttons, we will implement a stopwatch timer that counts in 10ths of a second and has a save lap functionality.

<details>
<summary>Homework 4 details</summary>
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

</details>

# Homework 5

---

For this assignment we have to develop a ”Smart Environment Monitor and Logger” using Arduino. This system will utilize various sensors to gather environmental data, log this data into EEPROM, and provide both visual feedback via an RGB LED and user interaction through a Serial Menu. The project focuses on integrating sensor readings, memory management, Serial Communication and the general objective of building a menu.

<details>
<summary>Homework 5 details</summary>
Requirements:Ultrasonic Sensor (HC-SR04), LDR (Light-Dependent Resistor), RGB LED, resistors and wires

Menu Structure:

1. Sensor Settings // Go to submenu
   1.1 Sensors Sampling Interval. Here you should be prompted for a value between 1 and 10 seconds. Use this value as a sampling rate for the sensors. You can read a separate value for each or have the same for both.

   1.2 Ultrasonic Alert Threshold. Here you should be prompted for a threshold value for the ultrasonic sensor. You can decide if that is the min or max value (you can signal that something is too close). When sensor value exceeds the threshold value, an alert should be given. This can be in the form of a message. If the LED is set to Automatic Mode (see section 4.2), it should also turn red if any of the sensors are outside the value.

   1.3 LDR Alert Threshold. Here you should be prompted for a threshold value for the LDR sensor. You can decide if that is the min or max value (for example, it could signal that night is coming). When sensor value exceeds the threshold value, an alert should be given. This can be in the form of a message. If the LED is set to Automatic Mode (see section 4.2), it should also turn red if any of the sensors are outside the value.

   1.4 Back // Return to main menu

2. Reset Logger Data. Should print a message, promting if you to confirm to delete all data. Something like ”are you sure?”, followed by the submenu with YES or NO. You can reset both sensor data at the same time, or you can do it individually. Your choice. Individually makes more sense, but I’m trying to simplify the homework.
   2.1 Yes.

   2.2 No.

3. System Status // Check current status and health
   3.1 Current Sensor Readings. Continuously print sensor readings at the set sampling rate, from all sensors. Make sure you have a way to exit this (such as pressing a specific key) and inform the user of this method through a message.

   3.2 Current Sensor Settings. Displays the sampling rate and threshold value for all sensors.

   3.3 Display Logged Data. Displays last 10 sensor readings for all sensors. (or be creative and do it another way).

   3.2 Back. Return to Main menu.

4. RGB LED Control // Go to submenu
   4.1 Manual Color Control. Set the RGB colors manually. You decide how to input them, either by making an option for each channel, or by putting a string etc. If you expect a specific format, make sure to inform the user.

   4.2 LED: Toggle Automatic ON/OFF. If automatic mode is ON, then the led color should be GREEN when all sensors value do not exceed threshold values (aka no alert) and RED when there is an alert (aka ANY sensor value exceeds the threshold). When automatic mode is OFF, then the LED should use the last saved RGB values.

   4.3 Back // Return to main menu

To solve this assignment we will have a configuration like this:
![Homework 5 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/b84167a92b1c72e83667ad7f2c28414c1e6615f7/Homework5/homework5_irl.jpeg)

This is the code I used for this homework:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/b84167a92b1c72e83667ad7f2c28414c1e6615f7/Homework5/Homework5.ino

Here is a video to see the working project: https://youtu.be/OSp2aD8tyY4

</details>

# Homework 6

---

In this assignment we will develop a small game on the 8x8 matrix. The game must have at least 3 types of elements: player (blinks slowly), bombs/bullets (blinks fast), wall (doesn’t blink). The purpose of this to get you started with your matrix project. The basic idea is that you generate walls on the map (50% - 75% of the map) and then you move around with the player and destroy them.

<details>
<summary>Homework 6 details</summary>
Requirements:Joystick, 8x8 LED Matrix, MAX7219, buzzer, resistors and wires

The map is generated after the player position is determined randomly, the walls are also generated randomly based on the seed gathered from the floating value of A3 pin.

The player will cast a bullet in the direction it last moved if the joystick button is pushed.

At the end of the game, if all walls are destroyed, a GJ! message is displayed.

To solve this assignment we will have a configuration like this:
![Homework 6 setup](https://github.com/TimiAndrei/IntroductionToRobotics/blob/f3a6cc037e525e7da3d60fd97c77c267d958d6e9/Homework6/Homework6_irl.jpeg)

This is the code I used for this homework:

https://github.com/TimiAndrei/IntroductionToRobotics/blob/f3a6cc037e525e7da3d60fd97c77c267d958d6e9/Homework6/Homework6.ino

Here is a video to see the working project: https://youtu.be/76gSHXKCoBI

</details>
