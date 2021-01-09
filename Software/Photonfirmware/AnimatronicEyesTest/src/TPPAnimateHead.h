/*
 * TPPAnimateHead.cpp
 * 
 * Team Practical Project eye mechanism control
 * 
 * This library uses the TPPAnimateServo library to control the AdaFruit_PWMServoDriver.
 * 
 * The library contains objects to control the eyeball mechanism and the four eyelids.
 * 
 * Instantiate this class, and it will create an instance of the TPPAnimateServo library.
 * 
 * Classes: 
 *      Eyeballs:  controls the x and y axis of the eyeballs
 *      Eyelid:    controls one eyelid
 *      Head:      holds eyeballs and eyelids and provides convenience functions to control them
 * 
 * Key methods
 *      begin:  pass in the servo number(s) on the AdaFruit servo driver board to control
 *      process: called over and over to cause the servo to move from its current
 *              position to the new target position
 * 
 * For full documentation see https://github/TeamPracticalProjects/XXXX
 * 
 * (cc) Non-Commercial Share-Alike Attribution 2021 Bob Glicksman, Jim Schrempp
 * 
 */

#ifndef _TPP_TPPAnimateHead_H
#define _TPP_TPPAnimateHead_H

#include <TPPAnimateServo.h>

class TPP_Eyeball {
    public:
        void init(int xservoNum, int xmidPos, int leftOffset, int rightOffset, 
             int yservoNum, int ymidPos, int upOffset, int downOffset);
        void process();
        int positionX(int position, int speed);
        int positionY(int position, int speed); 
        int lookCenter(int speed);
    private:
        int xservoNum;
        int yservoNum;
        int xmidPos;
        int leftOffset;
        int rightOffset;
        int ymidPos;
        int upOffset;
        int downOffset;
        TPP_AnimateServo xServo;
        TPP_AnimateServo yServo;

};

class TPP_Eyelid {
    public:
        void init(int servoNum, int openPos, int closedPos);
        void process();
        int position(int position, int speed);
    private:
        int servoNum;
        int openPos;
        int closedPos;
        TPP_AnimateServo myServo;

};

class TPP_Head {
    public:
        void process();
        int eyesOpen(int position, int speed);
        int blink();
        int wink(bool leftorright);

        TPP_Eyelid eyelidLeftUpper;
        TPP_Eyelid eyelidLeftLower;
        TPP_Eyelid eyelidRightUpper;
        TPP_Eyelid eyelidRightLower;
        TPP_Eyeball eyeballs;

};


#endif