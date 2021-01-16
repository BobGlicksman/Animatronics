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

#include <TPPAnimateHead.h>

Logger logHead("app.head");



// ---------------------------------------------------------
//-------------------   HEAD  ---------------------------
//   Holds all other objects that are in the head


/*----- process -----
 * called often to give the animation a chance to step forward
 * This will call process() on all objects it owns.
*/
void TPP_Head::process() {
    
    eyeballs.process();
    eyelidLeftUpper.process();
    eyelidLeftLower.process();
    eyelidRightUpper.process();
    eyelidRightLower.process();
}

/*----- eyesOpen -----
 * position 0:closed, 100:wide open; speed 1-10
*/
int TPP_Head::eyesOpen(int position, int speed){
    logHead.info("eyesOpen %d%%",  position);
    int time1 = eyelidLeftLower.position(position,speed);
    int time2 = eyelidLeftUpper.position(position,speed);
    int time3 = eyelidRightLower.position(position,speed);
    int time4 = eyelidRightUpper.position(position,speed);
    int estMS = max(time1,time2);
    estMS = max(estMS,time3);
    estMS = max(estMS,time4);
    return estMS;
}

/* ----- blink -----
 * both eyes close for an instant, then open to 50%
*/
int TPP_Head::blink(){

    logHead.info("Blink");

    eyesOpen(0, MOVE_SPEED_FAST);
    delay(200);
    eyesOpen(50, MOVE_SPEED_FAST);
    return 200;

}

/* ----- wink -----
 * one eye closes for an instant, then opens to 50%
*/
int TPP_Head::wink(bool leftorright){

    logHead.info("Wink");

    if (leftorright) {
        eyelidLeftUpper.position(0,MOVE_SPEED_FAST);
        eyelidLeftLower.position(0,MOVE_SPEED_FAST);
        delay(200);
        eyelidLeftUpper.position(50,MOVE_SPEED_FAST);
        eyelidLeftLower.position(50,MOVE_SPEED_FAST);
    } else {
        eyelidRightUpper.position(0,100);
        eyelidRightLower.position(0,100);
        delay(200);
        eyelidRightUpper.position(50,MOVE_SPEED_FAST);
        eyelidRightLower.position(50,MOVE_SPEED_FAST);

    }

    return 600;

}

// ---------------------------------------------------------
//-------------------   EYE BALLS ---------------------------


/* ----- init -----
 * initializes the x and y control servos for the eyeballs
 */
void TPP_Eyeball::init(int xservoNumIn, int xmidPosIn, int leftOffsetIn, int rightOffsetIn, 
    int yservoNumIn, int ymidPosIn, int upOffsetIn, int downOffsetIn) {

    xmidPos = xmidPosIn;
    leftOffset = leftOffsetIn;
    rightOffset = rightOffsetIn;

    ymidPos = ymidPosIn;
    upOffset = upOffsetIn;
    downOffset = downOffsetIn;

    logHead.trace("eyeball init %d %d %d ",xservoNumIn,  xmidPosIn,  xmidPos);
    xServo.begin(xservoNumIn, xmidPos);
    yServo.begin(yservoNumIn, ymidPos);

}

/* ----- process -----
 * Called to allow the eyeball servos to move
 */
void TPP_Eyeball::process() {

    xServo.process();
    yServo.process();

};

/* ----- lookCenter -----
 * Moves the eyeballs center
 * param: speed 1:slow  10:fast
 */
int TPP_Eyeball::lookCenter(int speed){

    logHead.info("eyeballs lookCenter");

    int xMS = positionX(50,speed);
    int yMS = positionY(50, speed);
    
    return max(xMS,yMS);
    
}

/* ----- positionX -----
 * Moves the eyeballs right/left
 * params: 
 * position 0:left, 100:right;
 * speed 1:slow  10:fast
 */
int TPP_Eyeball::positionX(int position, int speed) {

    logHead.info("eyeballs positionX");

    position = map(position, 0, 100, xmidPos+leftOffset, xmidPos+rightOffset );
    return xServo.moveTo(position, speed);

}

/* ----- positionY -----
 * Moves the eyeballs up/down
 * params: 
 * position 0:down, 100:up;
 * speed 1:slow  10:fast
 */
int TPP_Eyeball::positionY(int position, int speed) {

    logHead.info("eyeballs positionY");

    position = map(position, 0, 100, ymidPos+downOffset, ymidPos+upOffset);
    return yServo.moveTo(position, speed);

}

// ---------------------------------------------------------
//-------------------   EYE LIDS ---------------------------

/* ----- init -----
 * Servo number, fully open position, fully closed position
*/
void TPP_Eyelid::init(int servoNumIn, int openPosIn, int closedPosIn){

    servoNum = servoNumIn;
    openPos = openPosIn;
    closedPos = closedPosIn;

    myServo.begin(servoNum, closedPos);

}

/* ----- process -----
 * Called to allow the eyeball servos to move
 */
void TPP_Eyelid::process(){

    myServo.process();    

}

/*----- Position Eyelid -----
 * 0: closed, 100: full open; speed 1-10
*/
int TPP_Eyelid::position(int position, int speed){

    logHead.info("Eyelid to position %d%%", position);
    position = map(position, 0, 100, closedPos, openPos);
    return myServo.moveTo(position, speed);

}


