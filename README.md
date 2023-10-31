# Introduction to Robotics (2023 - 2024)

---

This repository pertains to the Robotics Laboratory assignments undertaken during the third year at the University of Bucharest's Faculty of Mathematics and Computer Science. Each assignment comprises stipulated requirements, detailed implementation instructions, as well as the requisite code and image files.

# Homework 1

---

This assignment focuses on controlling each channel (Red, Green, and Blue) of  an  RGB  LED  using  individual  potentiometers. We will use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. Specifically, we will read  the  potentiometer’s  value  with  Arduino  and  then  write  amapped value to the LED pins.  

To solve this assignment we will have a configuration like this: 

![Homework 1 setup in simulator](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Homework1/Homework1.png)
(Link to simulator: https://wokwi.com/projects/379468552305848321)

![Homework 1 setup in real life](https://github.com/TimiAndrei/IntroductionToRobotics/blob/main/Homework1/Homework1_irl.jpeg)

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



Here is a video to see the working project: https://youtu.be/P__fKOr1wOE