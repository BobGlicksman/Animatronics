/*
 * TPPAnimateServo.cpp
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

#include <TPPAnimateServo.h>
#define TPPServo_DEBUG   // note that the debug prints in process() are not recommended because they
                            // slow the timer work

#define MS_BETWEEN_MOVES 10  // we will not move any particular servo more ofen than this
#define MAX_SERVOS 6

Logger logAnimate("app.AnimateServo");

// begin
// Servo Number, initial position
void TPP_AnimateServo::begin(int servoNumIn, int positionIn) {

    pwm.begin();
    pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

    servoNum = servoNumIn;
    destination = positionIn; 
    position = positionIn;
    int setPos = trunc(position);
    pwm.setPWM(servoNum, 0, setPos); 

    logAnimate.trace("Begin Servo: %d at Pos: %f", servoNum, position);

}


/*------- moveTo -------
 *  newPos: new position for the servo
 *  speed: the increment to use in each call to get to the new position 
 *     1 is slow, 10 is fast
 *  Returns the estimated milliseconds needed to get from the current position 
 *     to the new position.
 */
int TPP_AnimateServo::moveTo (int newPos, float speed) {

    int estimatedMSToFinish = 0;

    // Set new destination and start time
    destination = newPos;
    timeStart = millis();
    lastDebugNeedsPrinting = true;

    // Will we count up or down?
    if (destination < position) {
        increment = -1 * speed; // count down
    } else {
        increment = speed; // count up
    }

    // How long do we anticipate the move will take?
    estimatedMSToFinish = (abs((destination-position)/speed) * MS_BETWEEN_MOVES * 1.4);
    if (estimatedMSToFinish < 200) {
      estimatedMSToFinish = 200;
    }

    timeStart = millis();
    logAnimate.trace("MoveTo, ServoNum: %i, pos: %.1f, dest: %i, inc: %.1f, estDur: %d", 
             servoNum, position, destination, increment, estimatedMSToFinish);

    return estimatedMSToFinish;

};

// process
// called often to give the animation a chance to step forward
void TPP_AnimateServo::process() {

    bool atDestination = false;

    //are we at the destination now?
    float distanceToGo = abs(position - destination);
    if ( distanceToGo < 2)  {
        atDestination = true;
        position = destination; 
    }

    // Not at the destination yet, find new position for this cycle
    if (!atDestination) {

        //have we waited long enough to make a new position change?
        if (millis() - lastMoveMade > MS_BETWEEN_MOVES) {

            position += increment;

            // don't overshoot the destination
            if (increment < 0) {
                // we are counting down
                if (position < destination) {
                    position = destination;  
                }
            } else {
                // we are counting up
                if (position > destination) {
                    position = destination; 
                }
            }
            
            // Command the servo
            int newPosition = floor(position);
            pwm.setPWM (servoNum, 0, newPosition);
            lastMoveMade = millis();
        }

    }

    // we have arrived
    if (atDestination) {
        // we've arrived at the destination, so print some final info but only once
        if (lastDebugNeedsPrinting) {
            int timeEnd = millis();
            lastDebugNeedsPrinting = false;
            
            float MSPerMoveUnit;
            if (startPosition == position) {
                MSPerMoveUnit = 0;
            } else {
                MSPerMoveUnit = ((timeEnd - timeStart)/abs(startPosition - position));
            }
            int actualDuration = timeEnd - timeStart;

        // XXX This is called from a Timer object. If I uncomment these then
        //    the process crashes. The documentation explicitly says to use
        //    the log object instead of Serial.print in a Timer call back.
        //    why doesn't this work? (It worked with Serial.print)

            //logAnimate.trace("Arrived, ServoNum: %i, dest: %.1f, inc: %.1f, actDur: %d", 
            //  servoNum, position, increment, actualDuration );
            //logAnimate.trace("Summary, inc: %.1f , MS Per Move Unit: %.2f", 
            //    increment, MSPerMoveUnit);
        }
    }

}

