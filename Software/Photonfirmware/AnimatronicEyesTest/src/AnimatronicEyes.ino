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
 
#include <Wire.h>
#include <TPPAnimateHead.h>

#define CALLIBRATION_TEST 
#define DEBUGON

SerialLogHandler logHandler1(LOG_LEVEL_WARN, {  // Logging level for non-application messages
    { "app.head", LOG_LEVEL_ALL }               // Logging for Animate Head methods
   // ,{ "app.AnimateServo", LOG_LEVEL_ALL }       // Logging for Animate Servo details
});

TPP_Head head;  // This is the master class that holds all the objects to be controlled

#define SERVOMIN  140 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  520 // this is the 'maximum' pulse length count (out of 4096)



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
Timer animationTimer(3, &TPP_Head::process, head);  

//------ setup -----------
void setup() {

    Serial.begin(9600);
    delay(100);
    Serial.println("===========================================");
    Serial.println("===========================================");
    Serial.println("8 channel Servo test!");
    
    animationTimer.start(); // Start the animation timer


    head.eyeballs.init(X_SERVO,X_POS_MID,X_POS_LEFT_OFFSET,X_POS_RIGHT_OFFSET,
            Y_SERVO, Y_POS_MID, Y_POS_UP_OFFSET, Y_POS_DOWN_OFFSET);

    head.eyelidLeftUpper.init(L_UPPERLID_SERVO, LEFT_UPPER_OPEN, LEFT_UPPER_CLOSED);
    head.eyelidLeftLower.init(L_LOWERLID_SERVO, LEFT_LOWER_OPEN, LEFT_LOWER_CLOSED);
    head.eyelidRightUpper.init(R_UPPERLID_SERVO, RIGHT_UPPER_OFFSET - LEFT_UPPER_OPEN, RIGHT_UPPER_OFFSET - LEFT_UPPER_CLOSED);
    head.eyelidRightLower.init(R_LOWERLID_SERVO, RIGHT_LOWER_OFFSET - LEFT_LOWER_OPEN, RIGHT_LOWER_OFFSET - LEFT_LOWER_CLOSED);
    
}

//------- MAIN LOOP --------------
void loop() {

    int timeForPositionChange = 0;

    static bool firstLoop = true;
    static int startTime = 0;
    static int nextSceneTime = 0;
    static int sceneNumber = -1;
    bool sceneChange = false;
    const int SCENE_MAX = 10;
    static int sceneList[SCENE_MAX] = {1,2,3,4,5,1,6,7,6,8};
    int sceneToSet = 0;

    if (firstLoop){

        firstLoop = false;
        Serial.println("start up");

    }

    int runTime = millis() - startTime;

    // Change to the next scene
    if (runTime > nextSceneTime) {
        sceneNumber++;
        if (sceneNumber < SCENE_MAX) {
            sceneChange = true;
            sceneToSet = sceneList[sceneNumber];
        }
    }


    if (sceneChange) {
        sceneChange = false;
        int time1;
        int time2;
        int time3;
        switch (sceneToSet) {
            case 1: 
                Serial.println("\nScene: eyes center, open a   ===========================================");
                head.eyesOpen(50, MOVE_SPEED_FAST);
                timeForPositionChange =   head.eyeballs.lookCenter(MOVE_SPEED_SLOW) ;
                nextSceneTime = runTime + 2000;
                break;
            case 2:
                Serial.println("\nScene: close eyes b   ==========================================="); 
                time1 = head.eyesOpen(0,MOVE_SPEED_SLOW);    
                time3 = head.eyeballs.lookCenter(MOVE_SPEED_SLOW); 
                timeForPositionChange = max(time1,time3);
                nextSceneTime = runTime + timeForPositionChange;
                break;
            case 3:
                Serial.println("\nScene: eyes right, eyes open c   ===========================================");
                time1 = head.eyesOpen(50, MOVE_SPEED_SLOW);
                time3 = head.eyeballs.positionX(100,MOVE_SPEED_SLOW); 
                timeForPositionChange = max(time1,time3);
                nextSceneTime = runTime + timeForPositionChange;
                break;
            case 4:
                Serial.println("\nScene: eyes up d   ===========================================");
                timeForPositionChange = head.eyeballs.positionY(100,MOVE_SPEED_SLOW);
                nextSceneTime = runTime + timeForPositionChange;
                break;
            case 5:
                Serial.println("\nScene: eyes down e   ===========================================");
                timeForPositionChange = head.eyeballs.positionY(0,MOVE_SPEED_SLOW);
                nextSceneTime = runTime + timeForPositionChange;
                break;
            case 6:
                Serial.println("\nScene: delay a second f   ===========================================");
                nextSceneTime = runTime + 1000;
                break;
            case 7:
                Serial.println("\nScene: blink   ===========================================");
                timeForPositionChange = head.blink();
                nextSceneTime = runTime + timeForPositionChange;
                break;
            case 8:
                Serial.println("\nScene: wink right   ===========================================");
                timeForPositionChange = head.wink(false);
                nextSceneTime = runTime + timeForPositionChange;
                break;
            default:
                break;
        }
    }

}
