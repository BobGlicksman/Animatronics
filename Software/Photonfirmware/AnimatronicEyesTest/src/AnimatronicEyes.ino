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

// Original Source from:
//  Nilheim Mechatronics Simplified Eye Mechanism Code
//  Make sure you have the Adafruit servo driver library installed >>>>> https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
//  X-axis joystick pin: A1
//  Y-axis joystick pin: A0
//  Trim potentiometer pin: A2
//  Button pin: D2

const String version = "1.0";
 
SYSTEM_MODE(MANUAL);

#include <Wire.h>
#include <TPPAnimateHead.h>
#include <TPPAnimationList.h>

#define CALLIBRATION_TEST 
#define DEBUGON

SerialLogHandler logHandler1(LOG_LEVEL_WARN, {  // Logging level for non-application messages
    { "app.main", LOG_LEVEL_INFO }               // Logging for main loop
    ,{ "app.head", LOG_LEVEL_INFO }               // Logging for Animate Head methods
    ,{ "app.anilist", LOG_LEVEL_TRACE }               // Logging for Animation List methods
    ,{ "app.aniservo", LOG_LEVEL_TRACE }          // Logging for Animate Servo details
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
    
    animation1.head.eyeballs.init(X_SERVO,X_POS_MID,X_POS_LEFT_OFFSET,X_POS_RIGHT_OFFSET,
            Y_SERVO, Y_POS_MID, Y_POS_UP_OFFSET, Y_POS_DOWN_OFFSET);

    animation1.head.eyelidLeftUpper.init(L_UPPERLID_SERVO, LEFT_UPPER_OPEN, LEFT_UPPER_CLOSED);
    animation1.head.eyelidLeftLower.init(L_LOWERLID_SERVO, LEFT_LOWER_OPEN, LEFT_LOWER_CLOSED);
    animation1.head.eyelidRightUpper.init(R_UPPERLID_SERVO, RIGHT_UPPER_OFFSET - LEFT_UPPER_OPEN, RIGHT_UPPER_OFFSET - LEFT_UPPER_CLOSED);
    animation1.head.eyelidRightLower.init(R_LOWERLID_SERVO, RIGHT_LOWER_OFFSET - LEFT_LOWER_OPEN, RIGHT_LOWER_OFFSET - LEFT_LOWER_CLOSED);


    // Establish Animation List

    animation1.addScene(sceneEyesAhead, MOVE_SPEED_SLOW, -1);
    animation1.addScene(sceneEyesClosed, MOVE_SPEED_SLOW, 0);
    
    animation1.addScene(sceneEyesAheadOpen, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesRight, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesLeft, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesRight, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesLeft, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesRight, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesLeft, MOVE_SPEED_FAST, 0);
    animation1.addScene(sceneEyesRight, MOVE_SPEED_FAST, 0);
    animation1.addScene(sceneEyesLeft, MOVE_SPEED_FAST, 0);
    animation1.addScene(sceneEyesRight, MOVE_SPEED_FAST, 0);

    animation1.addScene(sceneEyesAhead, MOVE_SPEED_SLOW, 0);

    animation1.addScene(sceneEyesUp, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesUp, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesUp, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesUp, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesDown, MOVE_SPEED_SLOW, 0);


    animation1.addScene(sceneEyesClosed, 400, 0);
    animation1.addScene(sceneEyesOpen, 400, 1000);
    animation1.addScene(sceneEyesClosed, 400, 0);
    animation1.addScene(sceneEyesOpen, 400, 1000);
    animation1.addScene(sceneEyesClosed, 400, 0);
    animation1.addScene(sceneEyesOpen, 400, 1000);
    animation1.addScene(sceneEyesClosed, 400, 0);
    animation1.addScene(sceneEyesOpen, 400, 1000);

/*

    animation1.addScene(sceneEyesAhead, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesOpenWide,MOVE_SPEED_FAST,500);

    //animation1.addScene(sceneBlink, MOVE_SPEED_SLOW, 500);
    //animation1.addScene(sceneBlink, MOVE_SPEED_SLOW, 500);

    animation1.addScene(sceneEyesClosed, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesAhead, MOVE_SPEED_FAST, 1000);

    animation1.addScene(sceneEyesRight, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesOpenWide, MOVE_SPEED_FAST, 2000);

    animation1.addScene(sceneEyesOpen, 5, -1);
    animation1.addScene(sceneEyesAhead, 5, 1000);


    animation1.addScene(sceneEyesRight, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesUp, MOVE_SPEED_SLOW, -1);
    animation1.addScene(sceneEyesOpenWide, MOVE_SPEED_SLOW, 0);
    animation1.addScene(sceneEyesOpen, MOVE_SPEED_FAST, -1);
    animation1.addScene(sceneEyesDown,5,-1);
    animation1.addScene(sceneEyesLeft, 5, 0);


 */
    //end position
    animation1.addScene(sceneEyesAhead, 3, -1);
    animation1.addScene(sceneEyesClosed, 1, 100);

   




    // xxx using the timer causes a crash
    // Start the animation timer 
    //animationTimer.start(); 

    animation1.startRunning();
    
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




