/*
 * TPPAnimateHead.h
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
 *      Head:      holds eyeballs and eyelids and provides convenience functions to control them
 *      Eyeballs:  controls the x and y axis of the eyeballs
 *      Eyelid:    controls one eyelid
 *
 * 
 * Key methods
 *      head
 *          .process()  called over and over to cause the servos to move from current
 *              position to the new target position. This function in turn calls process()
 *              on each of the other control objects
 *          .eyeOpen()  one of several other convenice functions
 *      eyeball
 *          .init()  sets all the parameters needed to control the eyeball mechanism
 *          .positionX/Y() used to set the position of the eyeballs
 *          .lookCenter()  one of several other convenience functions
 *      eyelid
 *          .init() sets parameters needed to control one eyelid
 *          .position()  moves eyelid with a % open parameter
 * 
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

// TPP_Head holds all the other objects that map to a servo(s)
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
        
    private:

};


#endif