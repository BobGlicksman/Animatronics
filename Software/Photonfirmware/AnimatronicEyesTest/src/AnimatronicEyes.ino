/*
 * AnimatroicEyes 
 * Part of the animatronic exploration of Team Practical Projects
 * https://github.com/TeamPracticalProjects
 * 
 * This is a hack of Nilheim Mechatronics code to see how we like it. His eye mechanism
 * is very cool. https://www.instructables.com/Simplified-3D-Printed-Animatronic-Dual-Eye-Mechani/
 *
 * (cc) Share Alike - Non Commercial - Attibution
 * 2020 Bob Glicksman and Jim Schrempp
 * 
 * v1.0 First checkin with everything working to do a small 8 step animation
 *    
 */ 


const String version = "1.0";
 
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);  // added this in an attempt to get the software timer to work. didn't help

#include <Wire.h>
#include <TPPAnimationList.h>
#include <TPPAnimatePuppet.h>

#define CALLIBRATION_TEST 
#define DEBUGON

SerialLogHandler logHandler1(LOG_LEVEL_WARN, {  // Logging level for non-application messages
    { "app.main", LOG_LEVEL_INFO }               // Logging for main loop
    ,{ "app.puppet", LOG_LEVEL_INFO }               // Logging for Animate puppet methods
    ,{ "app.anilist", LOG_LEVEL_INFO }               // Logging for Animation List methods
    ,{ "app.aniservo", LOG_LEVEL_INFO }          // Logging for Animate Servo details
});

Logger mainLog("app.main");
  
// This is the master class that holds all the objects to be controlled
animationList animation1;  // When doing a programmed animation, this is the list of
                           // scenes and when they are to be played


// Servo Numbers for the Servo Driver board
#define X_SERVO 0
#define Y_SERVO 1
#define L_UPPERLID_SERVO 2
#define L_LOWERLID_SERVO 3
#define R_UPPERLID_SERVO 4
#define R_LOWERLID_SERVO 5

// Servo Positions
// Use the calibration test code to figure the correct values for each.
// THE CALIBRATION TEST WILL PRINT A SET OF DEFINES THAT YOU CAN COPY
// AND PASTE OVER THE ONES BELOW. 
// -------------------------------------------------------------------
// -------------------------------------------------------------------

    #define X_POS_MID 413
    #define X_POS_LEFT_OFFSET 119
    #define X_POS_RIGHT_OFFSET -137

    #define Y_POS_MID 390
    #define Y_POS_UP_OFFSET 53
    #define Y_POS_DOWN_OFFSET -63

    #define LEFT_UPPER_CLOSED 508
    #define LEFT_UPPER_OPEN 301

    #define LEFT_LOWER_CLOSED 269
    #define LEFT_LOWER_OPEN 473

    #define RIGHT_UPPER_OFFSET 781
    #define RIGHT_LOWER_OFFSET 892
// -------------------------------------------------------------------
// -------------------------------------------------------------------


//------- midValue --------
// Pass in two ints and this returns the value in the middle of them.
int midValue(int value1, int value2) {
  
    if (value1 == value2){
        return value1;
    }

    int halfway = abs(value1 - value2)/2;
    
    if (value1 > value2) {
        return value2 + halfway;
    } else {
        return value1 + halfway;
    }

}

// This timer is used to pulse the top level object process() method
// which then gets passed down all the way to the AnimateServo library.
// This timer allows the servos to continue to move even when the main
// code is in a delay() function.
//Timer animationTimer(500, animationTimerCallback);  
//Timer animationTimer(5, &animationList::process, animation1); 

// when called from a timer it crashes
void animationTimerCallback() {

    // now have animation pass this on to all the servos it manages
    static bool inCall = false;
    if (!inCall) {
        inCall = true;
        animation1.process();
        inCall = false;
    }
}

//------ setup -----------
void setup() {

    delay(1000);
    mainLog.info("===========================================");
    mainLog.info("===========================================");
    mainLog.info("Animate Eye Mechanism");
    
    animation1.puppet.eyeballs.init(X_SERVO,X_POS_MID,X_POS_LEFT_OFFSET,X_POS_RIGHT_OFFSET,
            Y_SERVO, Y_POS_MID, Y_POS_UP_OFFSET, Y_POS_DOWN_OFFSET);

    animation1.puppet.eyelidLeftUpper.init(L_UPPERLID_SERVO, LEFT_UPPER_OPEN, LEFT_UPPER_CLOSED);
    animation1.puppet.eyelidLeftLower.init(L_LOWERLID_SERVO, LEFT_LOWER_OPEN, LEFT_LOWER_CLOSED);
    animation1.puppet.eyelidRightUpper.init(R_UPPERLID_SERVO, RIGHT_UPPER_OFFSET - LEFT_UPPER_OPEN, RIGHT_UPPER_OFFSET - LEFT_UPPER_CLOSED);
    animation1.puppet.eyelidRightLower.init(R_LOWERLID_SERVO, RIGHT_LOWER_OFFSET - LEFT_LOWER_OPEN, RIGHT_LOWER_OFFSET - LEFT_LOWER_CLOSED);


    // Establish Animation List

    animation1.addScene(sceneEyesAhead, -1, MOVE_SPEED_IMMEDIATE, -1);
    animation1.addScene(sceneEyesOpen, 0, MOVE_SPEED_IMMEDIATE, 0);
   
    // xxx using the timer causes a crash
    // Start the animation timer 
    //animationTimer.start(); 

    animation1.startRunning();
    animation1.process();

    sequenceGeneralTests();
    
}

//------- MAIN LOOP --------------
void loop() {

    static bool firstLoop = true;

    if (firstLoop){

        firstLoop = false;
        mainLog.info("start up");

    }

    animationTimerCallback();

}


void sequenceGeneralTests () {

    sequenceAsleep(2000);

    sequenceWakeUpSlowly(5000);
      
    animation1.addScene(sceneEyesAheadOpen, -1, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesRight, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesLeft, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesRight, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesLeft, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesRight, -1, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesLeft, -1, MOVE_SPEED_FAST, 0);
    animation1.addScene(sceneEyesRight, -1, MOVE_SPEED_FAST, 0);
    animation1.addScene(sceneEyesLeft, -1, MOVE_SPEED_FAST, 0);
    animation1.addScene(sceneEyesRight, -1, MOVE_SPEED_FAST, 0);

    animation1.addScene(sceneEyesAhead, -1, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesUp, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesUp, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesUp, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesUp, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, -1, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesAhead, -1, MOVE_SPEED_SLOW, 0);

    sequenceBlinkEyes(1000);
    sequenceBlinkEyes(1000);
    sequenceBlinkEyes(1000);
    sequenceBlinkEyes(1000);
    sequenceBlinkEyes(1000);


    
    

/*

    animation1.addScene(sceneEyesApuppet, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesOpenWide,MOVE_SPEED_FAST,500);

    //animation1.addScene(sceneBlink, MOVE_SPEED_SLOW, 500);
    //animation1.addScene(sceneBlink, MOVE_SPEED_SLOW, 500);

    animation1.addScene(sceneEyesClosed, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesApuppet, MOVE_SPEED_FAST, 1000);

    animation1.addScene(sceneEyesRight, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesOpenWide, MOVE_SPEED_FAST, 2000);

    animation1.addScene(sceneEyesOpen, 5, -1);
    animation1.addScene(sceneEyesApuppet, 5, 1000);


    animation1.addScene(sceneEyesRight, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesUp, MOVE_SPEED_SLOW, -1);
    animation1.addScene(sceneEyesOpenWide, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesOpen, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesDown,5,-1);
    animation1.addScene(sceneEyesLeft, 5, 0);

 */

    sequenceEndStandard();

}


void sequenceWakeUpSlowly(int delayAfterMS) {

    sequenceAsleep(1000);

    sequenceEyesWake(delayAfterMS); //left eye




}


void sequenceAsleep(int delayAfterMS) {

    animation1.addScene(sceneEyesAhead, -1, MOVE_SPEED_IMMEDIATE, -1);
    animation1.addScene(sceneEyesOpen, 0, MOVE_SPEED_IMMEDIATE, delayAfterMS);

}

void sequenceEyesWake(int delayAfterMS){

    animation1.addScene(sceneEyesOpen, eyelidSlit, MOVE_SPEED_SLOW, 500);
    animation1.addScene(sceneEyesRight, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesLeft, -1, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesAhead, -1, MOVE_SPEED_SLOW, -1);
    animation1.addScene(sceneEyesOpen, eyelidClosed, MOVE_SPEED_SLOW,1000);
    animation1.addScene(sceneEyesOpen, eyelidSlit, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesOpen, eyelidClosed, MOVE_SPEED_SLOW,1000);
    animation1.addScene(sceneEyesOpen, eyelidNormal, MOVE_SPEED_SLOW, 0);
    sequenceBlinkEyes(delayAfterMS);



}

void sequenceEndStandard() {

    animation1.addScene(sceneEyesAhead, -1, 3, -1);
    animation1.addScene(sceneEyesOpen, 50, 1, 100);
}

void sequenceBlinkEyes(int delayAfterMS) {

    // another way to do it
    //animation1.addScene(sceneEyesOpen, eyelidClosed, MOVE_SPEED_IMMEDIATE, 0);
    //animation1.addScene(sceneEyesOpen, eyelidNormal, MOVE_SPEED_IMMEDIATE, delayAfterMS);

    animation1.addScene(sceneEyelidsRight, eyelidClosed, MOVE_SPEED_IMMEDIATE, -1);
    animation1.addScene(sceneEyelidsLeft, eyelidClosed, MOVE_SPEED_IMMEDIATE, 0);
    animation1.addScene(sceneEyelidsRight, eyelidNormal, MOVE_SPEED_IMMEDIATE, -1);
    animation1.addScene(sceneEyelidsLeft, eyelidNormal, MOVE_SPEED_IMMEDIATE, delayAfterMS);
    
}