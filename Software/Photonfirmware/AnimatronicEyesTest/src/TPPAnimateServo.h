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

#include <Adafruit_PWMServoDriver.h>

#define MOVE_SPEED_SLOW 1
#define MOVE_SPEED_MEDIUM 8
#define MOVE_SPEED_FAST 16
#define MOVE_SPEED_IMMEDIATE 20

#define SERVOMIN  140 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  520 // this is the 'maximum' pulse length count (out of 4096)

/*!
 *  @brief  Class that stores state and functions for interacting with the animatronic eyeball mechanism
 */
class TPP_AnimateServo{

    public:
        TPP_AnimateServo();
        void begin(int servoNum, int postion) volatile;
        void process() volatile; // called every time in the loop to keep the eyes moving
        int moveTo (int newX, float speed) volatile;

    private:
        
        void initPWM();      // called once in the class inititator to init pwm library
        volatile int servoNum_ = 0;          // Number of this servo on the driver board 
        volatile float position_ = -1;       // the current position of the servo
        volatile int destination_ = 0;       // the position we are heading towards
        volatile float speed_ = 1;           // 1: immediate; 0.1: slow
        volatile int lastMoveMade_ = 0;      // time the last time we moved the servo position
        
        // used for debugging
        volatile int timeStart_ = 0;         // time we started moving. Used for debug
        volatile float startPosition_ = 0;     // position at the start of the move. Used for debug
        volatile bool lastDebugNeedsPrinting_ = true;// in debugging used to print a message when destination is reached
                                            // set to true when a new destination is set 


};

#endif 