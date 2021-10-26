/*
 * AnimatroicEyesCalibration
 * Part of the animatronic exploration of Team Practical Projects
 * https://github.com/TeamPracticalProjects
 * 
 * This software is used to determine the correct extents of the six
 * servo mechanisms used to animate the eyes. The objective is to find the appropriate
 * settings for #defines that can be used in the AnimatronicEyesTest firmware.
 * 
 * This software requires 
 *    1. a 10k ohm potentiometer between ground and vcc with the wiper 
 *       connected to analog pin A0.
 *    2. a momentary contact switch between ground and digital pin D2
 *    3. The 16 channel servo controller board from AdaFruit hooked up as 
 *       described in the Instructable referenced below
 *    4. The animatronic eye mechanism described in the Intstructable referenced below.
 * 
 * 
 * This is a hack of Nilheim Mechatronics code to see how we like it. His eye mechanism
 * is very cool. https://www.instructables.com/Simplified-3D-Printed-Animatronic-Dual-Eye-Mechani/
 *
 * (cc) Share Alike - Non Commercial - Attibution
 * 2020 Bob Glicksman and Jim Schrempp
 * 
 * v1.0
 * v1.1 moved trim pot to A3 and button to A4
 *    
 */ 

// Original Source from:
//  Nilheim Mechatronics Simplified Eye Mechanism Code

const String version = "1.0";
 
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Input Pins
#define TRIM_INPUT A3

const int BUTTON_PIN = A4;    // the number of the pushbutton pin used for commands
const int LED_ONBOARD = D7;      // the number of the LED pin

// Servo Numbers for the Servo Driver board
#define X_SERVO 0
#define Y_SERVO 1
#define L_UPPERLID_SERVO 2
#define L_LOWERLID_SERVO 3
#define R_UPPERLID_SERVO 4
#define R_LOWERLID_SERVO 5

#define SERVO_MIN_POS 150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVO_MAX_POS 650 // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_MID_POS (SERVO_MAX_POS - SERVO_MIN_POS)/2 + SERVO_MIN_POS

// Servo Positions
// THE CALIBRATION TEST WILL PRINT A SET OF DEFINES THAT YOU CAN COPY
// AND PASTE OVER THE ONES BELOW. 
// When you are satisfied with the servo operation, copy and paste these
// into the other animatronic programs we have provided.

// -------------------------------------------------------------------
/*
// FOR INITIAL RUN COPY THESE INTO THE CODE:
  #define X_POS_MID 400
  #define X_POS_LEFT_OFFSET 0
  #define X_POS_RIGHT_OFFSET 0

  #define Y_POS_MID 400
  #define Y_POS_UP_OFFSET 0
  #define Y_POS_DOWN_OFFSET 0

  #define LEFT_UPPER_CLOSED 450
  #define LEFT_UPPER_OPEN 300

  #define LEFT_LOWER_CLOSED 300
  #define LEFT_LOWER_OPEN 450

  #define RIGHT_UPPER_CLOSED 250
  #define RIGHT_UPPER_OPEN 450

  #define RIGHT_LOWER_CLOSED 500
  #define RIGHT_LOWER_OPEN 350
*/
// -------------------------------------------------------------------
   
  #define X_POS_MID 400
  #define X_POS_LEFT_OFFSET 141
  #define X_POS_RIGHT_OFFSET -121

  #define Y_POS_MID 407
  #define Y_POS_UP_OFFSET 98
  #define Y_POS_DOWN_OFFSET -82

  #define LEFT_UPPER_CLOSED 486
  #define LEFT_UPPER_OPEN 287

  #define LEFT_LOWER_CLOSED 258
  #define LEFT_LOWER_OPEN 450

  #define RIGHT_UPPER_CLOSED 261
  #define RIGHT_UPPER_OPEN 469

  #define RIGHT_LOWER_CLOSED 509
  #define RIGHT_LOWER_OPEN 256
  
// -------------------------------------------------------------------
// -------------------------------------------------------------------

// These variables hold the values we determine by tuning with the trim potentiometer
int vLEFT_UPPER_CLOSED = LEFT_UPPER_CLOSED;
int vLEFT_UPPER_OPEN = LEFT_UPPER_OPEN;
int vLEFT_LOWER_CLOSED = LEFT_LOWER_CLOSED;
int vLEFT_LOWER_OPEN = LEFT_LOWER_OPEN;

int vRIGHT_UPPER_CLOSED = RIGHT_UPPER_CLOSED;
int vRIGHT_UPPER_OPEN = RIGHT_UPPER_OPEN;
int vRIGHT_LOWER_CLOSED = RIGHT_LOWER_CLOSED;
int vRIGHT_LOWER_OPEN = RIGHT_LOWER_OPEN;

//int vRIGHT_LOWER_OFFSET = RIGHT_LOWER_OFFSET; xxx
//int vRIGHT_UPPER_OFFSET = RIGHT_UPPER_OFFSET ;

int vX_POS_MID = X_POS_MID;
int vX_POS_LEFT_OFFSET = X_POS_LEFT_OFFSET;
int vX_POS_RIGHT_OFFSET = X_POS_RIGHT_OFFSET;

int vY_POS_MID = Y_POS_MID;
int vY_POS_UP_OFFSET = Y_POS_UP_OFFSET;
int vY_POS_DOWN_OFFSET = Y_POS_DOWN_OFFSET;
 


// Globals
struct { 
  float xpos; // last x position sent to the x servo
  int ledState = HIGH;   // the current state of the output pin
  char strBuf[100];  // used by all sprintf formatting calls
} g;

// Trim is used to slightly adjust the positions during calibration
int trimVal = 0;

//------ setup -----------
void setup() {

  Serial.begin(9600);
  delay(100);
  Serial.println("\n\n\nAnimatronic Eyeballs Calibration for servo positions");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_ONBOARD, OUTPUT);

  // set initial LED state
  digitalWrite(LED_ONBOARD, g.ledState);

  pinMode(TRIM_INPUT, INPUT);
 
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  delay(10);

}



//------- MAIN LOOP --------------
void loop() {

    /*
    * In the main loop we have a large switch statement. Each case is a calibration test. 
    * When the button is pressed the code advances to the next calibration test.
    * 
    * In each calibration test the pattern is the same:
    *   a. if (needTestSetup) then 
    *      - save the result of the previous test
    *      - print instructions to the user
    *      - position the servos as needed for this test
    *      - set the servoPosTestBase to the starting calibration value
    *      - set needTestSetup to false
    *   b. Position the servo under test
    * 
    * Outside of the switch statement we get the trim potentiometer input and add it to
    * the servoPosTestBase. This way the user can adjust the servo position via the trim pot.
    * When the button is pressed the next text will save the result of the previous test.
    * 
    * The final calibration test prints out the results in the form of #defines that can
    * be copied and pasted into the top of this program. The user can then run through these
    * calibration tests again. When no more changes to trim are needed, the process is done.
    */

    static bool firstLoop = true;
    static int trimOfPreviousTest = 0;  // used to rezero the trim input after each test
    static int lastTrim = 99999;  // holds the value of the last trim input
    static int servoPosTestBase = SERVO_MID_POS;
    const int TEST_MIN = 1;   // first test case in the main switch statement
    const int TEST_MAX = 16;  // last test case in the main switch statement
    static int testNumber = TEST_MIN - 1;  // set to one less than the first test number in the switch statement
    static bool needTestSetup = false; // tells a test case to print information and initialize any servos
    String finalOutput = "";
    int trimInput = 0; // value from the trim potentiometer
    int adjustedTrimInput = 0; 
    int currentServoPosition = 0; // the position of the servo under calibration (includes trim)

    if (firstLoop){

        firstLoop = false;        
        
        pwm.setPWM(L_UPPERLID_SERVO,0,SERVO_MID_POS);
        pwm.setPWM(L_LOWERLID_SERVO,0,SERVO_MID_POS);
        pwm.setPWM(R_UPPERLID_SERVO,0,SERVO_MID_POS);
        pwm.setPWM(R_LOWERLID_SERVO,0,SERVO_MID_POS);
        pwm.setPWM(Y_SERVO,0,SERVO_MID_POS);
        pwm.setPWM(X_SERVO,0,SERVO_MID_POS);

        Serial.println("\n\nSetup: All servos have been set to middle position.");
        Serial.println("a. Connect power to servo board.");
        Serial.println("b. Now attach a servo horn to each servo so that the corresponding");
        Serial.println("   eye part is in the MIDDLE of its expected travel.");
        Serial.println("c. Turn trim to a value of about 0");
        Serial.println("d. Then start calibration tests by pressing button once.");
        Serial.println("e. Follow the printed instructions, pressing the button to"); 
        Serial.println("   move on to the next calibration test.");
        Serial.println("f. At the end of all tests will print #defines to copy/paste");
        Serial.println("   into this software.");
        Serial.println("g. You may have to do this whole process several times to");
        Serial.println("   dial in the correct values.\n");

        //sprintf(g.strBuf, "\rTrim Value: %d       ", trimInput );
        //Serial.print(g.strBuf);

    }

    // If button push, advance test
    if (buttonWasPushedBUTTON_PIN()) {

        g.ledState = !g.ledState;
        digitalWrite(LED_ONBOARD, g.ledState);

        needTestSetup = true;

        testNumber++;
        testNumber = constrain(testNumber, TEST_MIN, TEST_MAX);

    }

    // Get the value of the trim potentiometer
    trimInput = analogRead(TRIM_INPUT);

    adjustedTrimInput =   map(trimInput,0,4095,-300,300);  // On Particle processor analog input is 0 to 4096
    
    // subtract the trim at the end of the previous test so that
    // we effectively re-zero after each test
    adjustedTrimInput = adjustedTrimInput - trimOfPreviousTest;

    // add trim to current servo base position
    currentServoPosition = servoPosTestBase + adjustedTrimInput; 

    // display current trim position. 
    if ((abs(adjustedTrimInput - lastTrim) > 1)) { // so we don't do this every time through the main loop
      lastTrim = adjustedTrimInput;
      sprintf(g.strBuf, "\rTrim Value: %4d, servo position: %4d, trim from previous test: %4d ", 
                adjustedTrimInput, currentServoPosition, trimOfPreviousTest );
      Serial.print(g.strBuf);
    }

    // Calibration test sets
    switch (testNumber) {

        case 0:
            // waiting for user to press button
            break;

        case 1: 
            // Test: trim x position for middle
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Position servos that are significant to this test
                // none
                
                testHeaderPrint( "Trim x val until EYES HORIZONTAL MIDDLE", testNumber, adjustedTrimInput, currentServoPosition);    
                servoPosTestBase = vX_POS_MID;

            }
            pwm.setPWM(X_SERVO, 0, currentServoPosition);
            break;

        case 2:
            // Test: trim x position for far left
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vX_POS_MID = currentServoPosition;

                //Position servos that are significant to this test
                // none

                testHeaderPrint( "Trim x val until EYES FAR LEFT", testNumber, adjustedTrimInput, currentServoPosition);    
            
                servoPosTestBase = vX_POS_LEFT_OFFSET + vX_POS_MID;

            }
            pwm.setPWM(X_SERVO, 0, currentServoPosition);
            break;

        case 3:
            // Test: trim x position for far right
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vX_POS_LEFT_OFFSET = currentServoPosition - vX_POS_MID;

                //Position servos that are significant to this test
                // none

                testHeaderPrint( "Trim x val until EYES FAR RIGHT", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase =  vX_POS_RIGHT_OFFSET + vX_POS_MID;

            }
            pwm.setPWM(X_SERVO, 0, currentServoPosition);
            break;

        case 4:
            // Test: trim y position for middle
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vX_POS_RIGHT_OFFSET = currentServoPosition - vX_POS_MID;

                //Position servos that are significant to this test
                pwm.setPWM(X_SERVO,0,vX_POS_MID);

                testHeaderPrint( "Trim y val until EYES VERTICAL MIDDLE", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vY_POS_MID;

            }
            pwm.setPWM(Y_SERVO, 0, currentServoPosition);
            break;

        case 5:
            // Test: trim y position for eyes up
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vY_POS_MID = currentServoPosition;

                //Position servos that are significant to this test
                // none

                testHeaderPrint( "Trim y val until EYES UP", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase =  vY_POS_UP_OFFSET + vY_POS_MID;

            }
            pwm.setPWM(Y_SERVO, 0, currentServoPosition);
            break;

        case 6:
            // Test: trim y position for eyes down
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vY_POS_UP_OFFSET = currentServoPosition - vY_POS_MID;

                //Position servos that are significant to this test
                //  none

                testHeaderPrint( "Trim y val until EYES DOWN", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase =  vY_POS_DOWN_OFFSET + vY_POS_MID;

            }
            pwm.setPWM(Y_SERVO, 0, currentServoPosition);
            break;

        case 7:
            // Test: trim upper left lid closed
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vY_POS_DOWN_OFFSET = currentServoPosition - vY_POS_MID;

                //Position servos that are significant to this test
                pwm.setPWM(Y_SERVO,0,vY_POS_MID);

                testHeaderPrint( "Trim until LEFT UPPER LID CLOSED", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vLEFT_UPPER_CLOSED;
            }
            pwm.setPWM(L_UPPERLID_SERVO, 0, currentServoPosition);
            break;

        case 8:
            // Test: trim upper left lid open
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vLEFT_UPPER_CLOSED = currentServoPosition;

                //Position servos that are significant to this test
                //  none

                testHeaderPrint( "Trim until LEFT UPPER LID OPEN", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vLEFT_UPPER_OPEN;
            }
            pwm.setPWM(L_UPPERLID_SERVO, 0, currentServoPosition);
            break;
        
        case 9:
            // Test: trim left lower lid closed
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vLEFT_UPPER_OPEN = currentServoPosition;

                //Position servos that are significant to this test
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_CLOSED);
                
                testHeaderPrint( "Trim until LEFT LOWER LID CLOSED", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vLEFT_LOWER_CLOSED;
            }
            pwm.setPWM(L_LOWERLID_SERVO, 0, currentServoPosition);
            break;

        case 10:
            // Test: trim left lower lid open
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vLEFT_LOWER_CLOSED = currentServoPosition;

                //Position servos that are significant to this test
                //  none

                testHeaderPrint( "Trim until LEFT LOWER LID OPEN", testNumber, adjustedTrimInput, currentServoPosition);    
                
                servoPosTestBase = vLEFT_LOWER_OPEN;
            }
            pwm.setPWM(L_LOWERLID_SERVO, 0, currentServoPosition);
            break;

        case 11:
            // Test: trim upper right lid closed
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vLEFT_LOWER_OPEN = currentServoPosition;

                //Position servos that are significant to this test
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_CLOSED);

                testHeaderPrint( "Trim until RIGHT UPPER LID CLOSED", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vRIGHT_UPPER_CLOSED;
            }
            pwm.setPWM(R_UPPERLID_SERVO, 0, currentServoPosition);
            break;

        case 12:
            // Test: trim upper right lid open
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vRIGHT_UPPER_CLOSED = currentServoPosition;

                //Position servos that are significant to this test
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);

                testHeaderPrint( "Trim until RIGHT UPPER LID OPEN", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vRIGHT_UPPER_OPEN;
            }
            pwm.setPWM(R_UPPERLID_SERVO, 0, currentServoPosition);
            break;
      
        case 13:
            // Test: trim right lower lid closed
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vRIGHT_UPPER_OPEN = currentServoPosition;

                //Position servos that are significant to this test
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_CLOSED);
                
                testHeaderPrint( "Trim until RIGHT LOWER LID CLOSED", testNumber, adjustedTrimInput, currentServoPosition);    

                servoPosTestBase = vRIGHT_LOWER_CLOSED;
            }
            pwm.setPWM(R_LOWERLID_SERVO, 0, currentServoPosition);
            break;

        case 14:
            // Test: trim right lower lid open
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vRIGHT_LOWER_CLOSED = currentServoPosition;

                //Position servos that are significant to this test
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_OPEN);

                testHeaderPrint( "Trim until RIGHT LOWER LID OPEN", testNumber, adjustedTrimInput, currentServoPosition);    
                
                servoPosTestBase = vRIGHT_LOWER_OPEN;
            }
            pwm.setPWM(R_LOWERLID_SERVO, 0, currentServoPosition);
            break;

        case 15:
            // Show off the results
            if (needTestSetup) {
                needTestSetup = false;
                trimOfPreviousTest = adjustedTrimInput;

                //Capture results of last test
                vRIGHT_LOWER_OPEN = currentServoPosition;

                Serial.println("\nNow we'll go through some of the set positions ...");

                //Eyes: open and straight ahead
                pwm.setPWM(X_SERVO,0,vX_POS_MID);
                pwm.setPWM(Y_SERVO,0,vY_POS_MID);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_OPEN);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_OPEN);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_OPEN);
                delay(2000);

                //Eyes: both closed
                pwm.setPWM(X_SERVO,0,vX_POS_MID + vX_POS_LEFT_OFFSET);
                pwm.setPWM(Y_SERVO,0,vY_POS_MID);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_CLOSED);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_CLOSED);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_CLOSED);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_CLOSED);
                delay(2000);

                //Eyes: both open
                pwm.setPWM(X_SERVO,0,vX_POS_MID + vX_POS_LEFT_OFFSET);
                pwm.setPWM(Y_SERVO,0,vY_POS_MID);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_OPEN);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_OPEN);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_OPEN);
                delay(2000);

                //Eyes: top lids open, eyes up right
                pwm.setPWM(X_SERVO,0,vX_POS_MID + vX_POS_RIGHT_OFFSET);
                pwm.setPWM(Y_SERVO,0,vY_POS_MID + vY_POS_UP_OFFSET);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_CLOSED);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_OPEN);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_CLOSED);
                delay(2000);

                //Eyes: down, top lids at half, bottom open
                pwm.setPWM(Y_SERVO,0,vY_POS_MID + vY_POS_DOWN_OFFSET);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_OPEN);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_OPEN);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_OPEN);
                delay(2000);

                //Eyes: back to normal
                pwm.setPWM(X_SERVO,0,vX_POS_MID);
                pwm.setPWM(Y_SERVO,0,vY_POS_MID);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_OPEN);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_OPEN);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_OPEN);
                delay(2000);

                //Eyes: back to wide open
                pwm.setPWM(X_SERVO,0,vX_POS_MID);
                pwm.setPWM(Y_SERVO,0,vY_POS_MID);
                pwm.setPWM(L_UPPERLID_SERVO,0,vLEFT_UPPER_OPEN);
                pwm.setPWM(L_LOWERLID_SERVO,0,vLEFT_LOWER_OPEN);
                pwm.setPWM(R_UPPERLID_SERVO,0,vRIGHT_UPPER_OPEN);
                pwm.setPWM(R_LOWERLID_SERVO,0,vRIGHT_LOWER_OPEN);
                delay(2000);

                Serial.println("\nPress the button to finish.");

            }
            break;

        case 16:
            // Report the results
            if (needTestSetup) {
            
            needTestSetup = false;
            
            finalOutput =  "\n\nRESULTS to paste into the code:\n ";
            finalOutput += "\n  #define X_POS_MID " + String(vX_POS_MID);
            finalOutput += "\n  #define X_POS_LEFT_OFFSET " + String(vX_POS_LEFT_OFFSET);
            finalOutput += "\n  #define X_POS_RIGHT_OFFSET " + String(vX_POS_RIGHT_OFFSET);
            finalOutput += "\n";
            finalOutput += "\n  #define Y_POS_MID " + String(vY_POS_MID);
            finalOutput += "\n  #define Y_POS_UP_OFFSET " + String(vY_POS_UP_OFFSET);
            finalOutput += "\n  #define Y_POS_DOWN_OFFSET " + String(vY_POS_DOWN_OFFSET);
            finalOutput += "\n";
            finalOutput += "\n  #define LEFT_UPPER_CLOSED " + String(vLEFT_UPPER_CLOSED);
            finalOutput += "\n  #define LEFT_UPPER_OPEN " + String(vLEFT_UPPER_OPEN);
            finalOutput += "\n";
            finalOutput += "\n  #define LEFT_LOWER_CLOSED " + String(vLEFT_LOWER_CLOSED);
            finalOutput += "\n  #define LEFT_LOWER_OPEN " + String(vLEFT_LOWER_OPEN);
            finalOutput += "\n";
            finalOutput += "\n  #define RIGHT_UPPER_CLOSED " + String(vRIGHT_UPPER_CLOSED);
            finalOutput += "\n  #define RIGHT_UPPER_OPEN " + String(vRIGHT_UPPER_OPEN);
            finalOutput += "\n";
            finalOutput += "\n  #define RIGHT_LOWER_CLOSED " + String(vRIGHT_LOWER_CLOSED);
            finalOutput += "\n  #define RIGHT_LOWER_OPEN " + String(vRIGHT_LOWER_OPEN);
            finalOutput += "\n";


            //finalOutput += "\n  #define RIGHT_UPPER_OFFSET " + String(vRIGHT_UPPER_OFFSET);
            // xxx finalOutput += "\n  #define RIGHT_LOWER_OFFSET " + String(vRIGHT_LOWER_OFFSET);
            Serial.println(finalOutput);

            }
            pwm.setPWM(R_UPPERLID_SERVO, 0, currentServoPosition);
            break;

            break;

        default:
            Serial.println("\nTest is over. Restart photon to do it again.");
            break;
    }
  }



//---------- testHeaderPrint
// Does the common work for each test 
void testHeaderPrint (const char* description, int testNumber, int adjustedTrimInput, int currentServoPosition) {

    sprintf(g.strBuf,"\n\nTest %d: %s", testNumber, description);
    Serial.println(g.strBuf);
    sprintf(g.strBuf, "\rTrim Value: %4d, servo position: %4d       ", adjustedTrimInput, currentServoPosition );
    Serial.print(g.strBuf);

}

//---------- buttonWasPushedBUTTON_PIN 
// Returns true if BUTTON_PIN goes HIGH to LOW
bool buttonWasPushedBUTTON_PIN() {
    
    static int lastSwitchState = HIGH;
    int retCode = false;

    // only return true if the button state goes HIGH to LOW
    int switchState = switchReadStateBUTTON_PIN();
    if ( switchState == LOW) {
    if (lastSwitchState == HIGH) {
        // Switch was HIGH, now LOW
        retCode = true;
    }
    }
    lastSwitchState = switchState; 
    return retCode;
}

//-------- switchReadStateBUTTON_PIN
// returns the debounced value of BUTTON_PIN
int switchReadStateBUTTON_PIN() {

    static int switchState = HIGH;   // the value of the switch that is returned
    static int lastButtonState = HIGH;   // the previous reading from the input pin
    static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
    unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

    // read the state of the switch into a local variable:
    int reading = digitalRead(BUTTON_PIN);

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    lastButtonState = reading; // remember what value we just saw
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
        switchState = reading;
    }

    return switchState;
}



