/*
 * TPPAnimateServo.h
 * 
 * Team Practical Project animatronic library
 * 
 * Servos move on command. For animatronics we need to command a servo to move to 
 * some position with some amount of speed. This library wraps the AdaFruit_PWMServoDriver
 * to provide this functionality.
 * 
 * Instantiate this class, and it will create an instance of the AdaFruit PWM Servo Driver.
 * Key methods
 *      begin:  pass in the servo number on the AdaFruit servo driver board
 *      moveTo: pass in a target PWM duration and increment 
 *      process: called over and over to cause the servo to move from its current
 *              position to the new target position
 * 
 * For full documentation see https://github/TeamPracticalProjects/XXXX
 * 
 * (cc) Non-Commercial Share-Alike Attribution 2021 Bob Glicksman, Jim Schrempp
 * 
 */

#ifndef _TPP_Servo_H
#define _TPP_Servo_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define MOVE_SPEED_SLOW 1
#define MOVE_SPEED_FAST 10
#define MOVE_SPEED_IMMEDIATE 100

/*!
 *  @brief  Class that stores state and functions for interacting with the animatronic eyeball mechanism
 */
class TPP_AnimateServo{
    public:
        // Servo Numbers for the Servo Driver board
        struct {
            int x;
            int y;
            int lUpper;
            int lLower;
            int rUpper;
            int rLower;
        } servo;
       
        void begin(int servoNum, int postion);
        void setServoOffsets (int xLeft, int xMid, int xRight, int yUp, int yMid, int yDown, int lUpperOpen, int lUpperClosed,
            int rUpperOpen, int rUpperClosed, int lLowerOpen, int lLowerClosed, int rLowerOpen, int rLowerClosed);
        void process(); // called every time in the loop to keep the eyes moving
        int eyesOpenCenter();
        int lookLeft(int speed);
        int lookRight(int speed);
        int moveTo (int newX, float speed);

    private:
        Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

        int servoNum = -1;   // Number of this servo on the driver board 
        float position = 0;    // the current position of the servo
        int destination = 0; // the position we are heading towards
        float increment = 0; // increment we are using to get from position to destination
        int lastMoveMade = 0; // time the last time we moved the servo position
        
        int timeStart = 0;   // time we started moving. Used for debug
        int startPosition = 0; // position at the start of the move. Used for debug
        bool lastDebugNeedsPrinting = false; // in debugging used to print a message when destination is reached
                                        // set to true when a new destination is set 


};









#endif 