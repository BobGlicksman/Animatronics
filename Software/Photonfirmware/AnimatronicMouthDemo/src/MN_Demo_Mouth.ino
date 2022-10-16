/*
 * Project MN_Demo_Mouth
 * Description: This Photon software supports a demo at Maker Nexus.  It uses the 
 *  animatronics mouth PC board to play a "welcome" clip and to move the mouth
 *  servo in accordance with the envelope of the clip.  Other clips are supported
 *  as well.  An event from the TOF sensor of the eyes code triggers off which clip to play.
 * 
 *  The project supports an external Green LED and an external Red LED.  The Green LED indicates
 *  that the speaking sequence is in progress and the speech cannot be retriggered if a new
 *  event comes in within this time period.  This is to prevent the speaking sequence from
 *  being retriggered while someone remains in front of the animatronic demo but is moving
 *  around.  The Red LED lights when the
 *  button is pressed, indicating that the demo will pause at the conclusion of the speaking
 *  sequence.  The speaking sequence is paused until the button is pressed again, at which point
 *  the Red LED goes out and the speaking sequence can be retriggered.
 * 
 *  The following Photon pins are used:
 *    D0, D1: these are reserved for I2C and brought out to the I2C PCB connector.  These
 *      pins are not used for the mouth function at this time.   
 *    D2: connected on the PCB to the mini MP3 player BUSY line.
 *    D3: connected to the mouth driving servo
 *    D4: available as a 5 volt output pin, e.g. for a second servo.  Not used in this demo
 *    D5: connected to a red LED.  Indicated when the demo is paused.
 *    D6: connected to a green LED.  Indicates when the eyes have signalled.
 *    A0, A1, A2: connected internally on the PCB to the analog processing circuirty. Pin
 *      A2 has the envelope data that is samplled every 10 ms for analog signal processing.
 *    A3: connected to an external momentary pushbutton switch.  The switch is used to pause
 *      and unpause the demo.
 * 
 *  This software retains the ability to set analog signal processing parameters via
 *  the Particle console and to report max and min values found when playing a clip
 *  back to the console.  It is not necessary to use the console to read or set
 *  any values as defaults are provided that have been determined to be optimal via
 *  prior experimentation.  However, the ability to change these without having to recompile
 *  the software is retained.
 *      
 * Author: Bob Glicksman, Jim Schrempp, Team Practical Projects
 * (c) 2021, Team practical projects.  All rights reserved.
 * Released under open source, non-commercial license.
 * Date: 10/13/2022
 * 
 * version 1.7: moved audio clips to TPP_clipinfo and added TPP_Animatronic_Global.h
 * version 1.6: Added second personality based on A4 (open = personality 0, ground = personality 1)
 *                 see the structure "personalities"
 *              Added cloud function to set mouth position for calibration (had to redo a bit of
 *                 the speak() routine so mouth was not constantly being closed)
 * version 1.5: changed to .playMp3Folder. Audio files must now be in 
 *     /MP3/  and begin with 4 digits: 0000   0003 etc
 * version 1.4: added #define for Jim vs Bob
 * version 1.3: updated the event interface enum to the latest spec
 * version 1.2: changed from PIR signalling to event message from the eyes code
 * version 1.1: changed mouth open and closed limit constants for demo hardware
 * version 1.0: initial release; full capability
 * version 0.2: implemented and tested button state machine. Need to add pause state.
 * version 0.1: code is tested but needs pause button to be implemented.
 * 
 */


#include <DFRobotDFPlayerMini.h>
#include <math.h>
#include <TPP_clipinfo.h>
#include <TPP_Animatronic_Global.h>

#define BOB_MOUTH
//#define JIM_MOUTH


// create an instance of the mini MP3 player
DFRobotDFPlayerMini miniMP3Player;

// create an instance of the servo
Servo mouthServo;

// define Photon pins
const int BUSY_PIN = D2;
const int SERVO_PIN = D3;
const int RED_LED_PIN = D5;
const int GREEN_LED_PIN = D6;
const int BUTTON_PIN = A3;
const int LED_PIN = D7;
const int ANALOG_ENV_INPUT = A0;
const int PERSONALITY_PIN0 = A4;
const int PERSONALITY_PIN1 = A5;

// defined constants
const unsigned long SAMPLE_INTERVAL = 10; // 10 ms analog input sampling interval


#ifdef BOB_MOUTH
const int MOUTH_CLOSED = 123; //  servo position for the mouth closed
const int MOUTH_OPENED = 112; //  servo position for the wide open mouth
#endif

#ifdef JIM_MOUTH
const int MOUTH_CLOSED = 154;  //  servo position for the mouth closed
const int MOUTH_OPENED = 143;  //  servo position for the wide open mouth
#endif

const unsigned long BUSY_WAIT = 0; //2000UL; // busy pin wait time = 2 second
const unsigned long EYES_START_TIME = 0; // 1000UL; // time to eye sequence to start up
const unsigned long EYES_COMPLETE_TIME = 1000UL;  // time to eye sequence to stop
const unsigned long DEBOUNCE_TIME = 10UL; // time for button debouncing

// define global variables for the audio envelope data
int maxValue = 4095; // the highest expected analog input value - for servo mapping
int minValue = 0; // the lowest expected analog input value - for servo mapping
int numSamples = 5; // the number of analog input samples to average for a servo command
int nlProcess = 0;  // 0 for skip non linear processing, 1 for sqrt processing, more later...

// cloud variables to report statistics
int maxFound = 0; // the maximum analog value found in the data set
int minFound = 4095; // the minimum analog value found in the data set


// global variables for eyes event processing
TOF_detect statusChange;    // the new status from the eyes code
bool newDetectionFlag;      // indication that a new event came in from the eyes code
int mg_personalityNumber;   // 0-4 set by jumpers XX and XX on the board read as binary number
int mg_mouthPosition;       // current mouth position, mostly for reporting to cloud function

#define MAX_CLIPS_PER_EVENT 10 
struct clipsForEvent {
    int numClips;
    ClipData *p_clipdata[MAX_CLIPS_PER_EVENT]; // no more than 10 clips per TOF event
} ;


struct Personality {
    clipsForEvent events[NUM_TOF_EVENTS]; // to match number of choises in events TOF_Detect enum
} ;

#define NUM_PERSONALITIES 2
Personality personalities[NUM_PERSONALITIES]; // = // one for each personality


// define enumerated state variable for loop() state machine
enum StateVariable {
    idle,
    motionDetected,
    clipWaiting,
    clipPlaying,
    clipComplete,
    clipEnd,
    paused
};

// define enumerated state variable for buttonPressed() function
enum ButtonStates {
  buttonOff,    // the button is not pressed
  pressedTentative,   // button seems to be pressed, need debounce verification
  buttonOn, // button remains pressed but don't indicate true anymore
  releasedTentative  // button seems to be released, need verificaton
};

// Initialize the personalities structure
void initPersonalities(){
    for (int i = 0; i < NUM_PERSONALITIES; i++) {
        for (int j = 0; j < NUM_TOF_EVENTS; j++ ){
            personalities[i].events[j].numClips = 0;
        }
    }
}

// load the personality structure from the audioClips array in TPP_clipinfo.cpp
void loadPersonalities(ClipData *audioClips){

    int i = 0;

    // -1 signals the end of the audioClips[] array
    while (audioClips[i].personalityNum != -1) {

        int thisPersonality = audioClips[i].personalityNum;
        TOF_detect thisTOFEvent = audioClips[i].TOFEvent; 

        clipsForEvent *p_toEvent = &personalities[thisPersonality].events[thisTOFEvent];
        
        if (p_toEvent->numClips < MAX_CLIPS_PER_EVENT ) {
            p_toEvent->p_clipdata[p_toEvent->numClips] = &audioClips[i];
            p_toEvent->numClips++;
        } 

        i++; 
    }
}

//function to set up the data and playback a clip
void clipPlay(ClipData *p_thisClip) {
    analogMin(p_thisClip->aMin);
    analogMax(p_thisClip->aMax);
    nlp(p_thisClip->nlproc);
    samples(p_thisClip->avSamples);
    clipVolume(p_thisClip->volume);
    clipNum(p_thisClip->clipNumber);
    Particle.publish("playing clip: " + p_thisClip->clipNumber);
}  // end of clipPlay()

// pick a random clip to play from the choices we have
//   returns true if there is a clip to play, otherwise false
bool eventResponse(clipsForEvent audioClips) {
    bool rtnCode = false;
    if (audioClips.numClips > 0) {
        rtnCode = true;
        int clipToPlay = random(audioClips.numClips);
        ClipData *chosenClip = audioClips.p_clipdata[clipToPlay];
        clipPlay(chosenClip);
    };
    return rtnCode;
}

// move the mouth servo
void moveMouth(int position) {
    mouthServo.write(position);
    mg_mouthPosition = position;
}

// Cloud function to set mouth position
int PFmouthPosition(String data) {
    int position = data.toInt();
    if (position != 0) {
        moveMouth(position);
    }
    return mg_mouthPosition;

}

// subscription handler for events from eyes code
void tofHandler(String event, String eventData) {
    newMouthEvent(eventData);
}

// function to process a new event from the eyes code
int newMouthEvent(String detection) {
    int ordinalDetection = detection.toInt();
    statusChange = (TOF_detect)ordinalDetection;

    // make sure we got a valid enumerated value or else don't set the newDetectionFlag
    if ((statusChange > -1) && (statusChange < NUM_TOF_EVENTS)) {
        newDetectionFlag = true;
    } else {
        newDetectionFlag = false;
        Particle.publish("received event was out of range: " + detection );
    }
    return ordinalDetection;
}

void setup() {
    // set up Photon pins
    pinMode(BUSY_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(PERSONALITY_PIN0, INPUT_PULLUP);
    pinMode(PERSONALITY_PIN1, INPUT_PULLUP);

    // register Particle Cloud functions and variables
    Particle.function("clip number", clipNum);
    Particle.function("volume", clipVolume);
    Particle.function("number of samples", samples);
    Particle.function("non-linear processing type", nlp);
    Particle.function("analog input max", analogMax);
    Particle.function("analog input min", analogMin);
    Particle.variable("max envelope value", maxFound);
    Particle.variable("min envelope value", minFound);
    Particle.function("mouth_event", newMouthEvent);    // this is for testing putposes
    Particle.subscribe("TOF_event", tofHandler);   // this will respond to published event from the eyes code
    Particle.function("MouthPosition", PFmouthPosition); // for testing mouth position 

    // clear out the new event detection flag
    newDetectionFlag = false;

    // set up the mini MP3 player
    Serial1.begin(9600);
    miniMP3Player.begin(Serial1);

    // set up the mouth servo
    mouthServo.attach(SERVO_PIN);

    // blink and turn on the D7 LED on to indicate that the device is ready
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, HIGH);
    moveMouth(MOUTH_CLOSED);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    moveMouth(MOUTH_OPENED);
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    moveMouth(MOUTH_CLOSED);

    // initialize the personalities array with clipData
    initPersonalities();
    loadPersonalities(audioClips);
    mg_personalityNumber = 0;  // default 


} // end of setup()

void loop() {
    static unsigned long busyTime = millis();
    static StateVariable state = idle;
    static bool buttonToggle = false;   // if set true, put demo in pause mode

    // detect personality
    int personality0 = digitalRead(PERSONALITY_PIN0);
    // int personality1 = digitalRead(PERSONALITY_PIN1); // when we want to go to more than 2 personalities
    if (personality0 == HIGH) {
        mg_personalityNumber = 0;
    } else {
        mg_personalityNumber = 1;
    }

    // refresh the analog sampling and processing the mouth movement continuously
    speak();

    // check button status and toggle pause mode if button has been pressed
    if(buttonPressed() == true) {
        buttonToggle = !buttonToggle;
    }
    // the RED LED is the pause state indicator
    if(buttonToggle == true) {
        digitalWrite(RED_LED_PIN, HIGH);
    }
    else {
        digitalWrite(RED_LED_PIN, LOW);    
    }

    // state machine to signal the eyes, play the clip and move the mouth
    switch (state) {
        case idle:  // wait for new event from the eyes
            if(digitalRead(BUSY_PIN) == HIGH) { // make sure mini MP3 is ready
                // check the new event flag for an event from the eyes code
                if(newDetectionFlag == true) {
                    digitalWrite(GREEN_LED_PIN, HIGH);    // visual indication of motion    
                    busyTime = millis();    // update timer time
                    state = motionDetected; // transition to the next state
                }
                else {
                    state = idle; // stay in the idle state
                }
            }
            break;
        
        case motionDetected:  // motion is detected, signal the eyes and wait
            if( (millis() - busyTime) >= EYES_START_TIME) {

                bool hasAudio = eventResponse(personalities[mg_personalityNumber].events[statusChange]);
                newDetectionFlag = false;
                if (hasAudio) {
                    busyTime = millis();    // reset the timer for the next state
                    state = clipWaiting;  // transition to next state
                } else {
                    // no audio for the event
                    Particle.publish ("no audio clips for event " + String(statusChange));
                    state = idle;
                }
            }
            else {
                state = motionDetected; // stay in present state
            }
            break;

        case clipWaiting:   // wait for busy to assert (low)
            if(digitalRead(BUSY_PIN) == LOW) {   // now busy
                busyTime = millis();    // reset the timer for the next state
                state = clipPlaying;  // transition to next state
            }
            else {  // clip hasn't started yet
                state = clipWaiting; // stay in present state
            }
            break;

        case clipPlaying: // clip; playing, wait for busy to unassert (complete)
            if(digitalRead(BUSY_PIN) == HIGH) {   // not busy anymore, clip is done
                busyTime = millis();    // reset the timer for the next state
                state = clipComplete;  // transition to next state
            }
            else {  // clip still in process of playing
                state = clipPlaying; // stay in present state
            }
            break;

        case clipComplete:  // clip has finished, keep eyes going a little longer
            if( (millis() - busyTime) >= EYES_COMPLETE_TIME) {
                digitalWrite(GREEN_LED_PIN, LOW); // reset indicator
                state = clipEnd;  // transition to next state, don't update the timer!
            }
            else {
                state = clipComplete; // stay in present state
            }
            break;

        case clipEnd:   // just make sure busy pin has been unasserted long enough
            // test that busy pin is unasserted long enough and PIR is unasserted so don't retrigger
            if( (millis() - busyTime) >= BUSY_WAIT ) {  
                if(buttonToggle == false) { // no pause state indicated
                    state = idle;
                }
                else {    // we must go to pause state
                    state = paused;
                }
            }
            else {
                state = clipEnd; // stay in present state
            }
            break;    

        case paused: //   we stay in pause state until buttonToggle is toggled false
            if(buttonToggle == true) {  // stay in paused state
                state = paused;
            }
            else {    // exit the pause state - return to idle
                state = idle;
            }
            break;

        default:
            // the next state is idle
            state = idle;
    }

} // end of loop()

// called every time through the main loop to sample the current audio
// and move the mouth servo as needed
void speak() {
    static unsigned long lastSampleTime = millis();
    static int averagedData = 0;
    static int numberAveragedPoints = 0;
    static bool toggle = false;
    static bool wasPlaying = false;  // player state last time through this code
    int servoCommand;
    int isPlaying;

    isPlaying = digitalRead(BUSY_PIN);
    if( isPlaying == LOW ) {
        wasPlaying = true;
        // read a sample every 10 ms (non-blocking)
        if( (millis() - lastSampleTime) >= SAMPLE_INTERVAL) {
            // average the samples
            averagedData += analogRead(ANALOG_ENV_INPUT); // read in analog data and add
            numberAveragedPoints++; // keep track of how many points are added
            if(numberAveragedPoints >= numSamples) {  // number samples to average reached
                averagedData = averagedData / numSamples; // average the sum
                // non-linearly scale the averaged data
                if(nlProcess == 1) {
                    averagedData = nlScale(averagedData);
                }
                if (nlProcess == 2) {
                    averagedData = sqrt(averagedData) / sqrt(maxValue) * maxValue;
                }
                // command the servo
                servoCommand = map(averagedData, minValue, maxValue, MOUTH_CLOSED, MOUTH_OPENED);
                // constrain the servo so it doesn't peg at 0 or 180 degrees.
                servoCommand = constrain(servoCommand, 5, 175);

                // send data to servo 
                moveMouth(servoCommand);
                    
                // set max and min values found
                if(averagedData > maxFound) {
                    maxFound = averagedData;
                } else if (averagedData < minFound) {
                    minFound = averagedData;
                }

                averagedData = 0; // reset for the next average
                numberAveragedPoints = 0; // reset the average count
            }

            // toggle the D7 LED so that it will pulse at 1/2 averaged sample time
            //    this will normally be too fast to see on the LED, but good pin to scope
            if(toggle == false) {
                digitalWrite(LED_PIN, LOW);
                toggle = true;
            } else {
                digitalWrite(LED_PIN, HIGH);
                toggle = false;
            }
            lastSampleTime = millis();  // reset the sample timer  
        }

    } else {
        // we are not playing
        if (wasPlaying == true) {
            // we have just transitioned from playing to stopped
            moveMouth(MOUTH_CLOSED);
            wasPlaying = false;
        }
    }

} // end of speak()

// cloud function to set the clip number and play the clip
int clipNum(String playClip) {
    int clip;
    clip = playClip.toInt();
    if (clip < 0) {
        clip = 0;
    }
    // reset the max and min values found for a new clip
    maxFound = 0;
    minFound = 4095;
    // play the clip
    miniMP3Player.playMp3Folder(clip);
    return clip;
} // end of clipNum()

// cloud function to set the playback volume
int clipVolume(String volume) {
    int vol;
    vol = volume.toInt();
    if(vol > 30) {
        vol = 30;
    } else if(vol < 0) {
        vol = 0;
    }
    miniMP3Player.volume(vol);
    return vol;
} // end of clipVolume

// cloud function to set the number of samples to average
int samples(String numberSamples) {
    numSamples = numberSamples.toInt();
    // make sure that the number is positive and non-zero
    if(numSamples < 1) {
        numSamples = 1;
    }
    return numSamples;
} // end of samples()

// cloud function to set the global maxValue
int analogMax(String theMax) {
    maxValue = theMax.toInt();
    if (maxValue > 4095) {
        maxValue = 4095;
    }
    return maxValue;
} // end of analogMax()

// cloud function to set the global minValue
int analogMin(String theMin) {
    minValue = theMin.toInt();
    if (minValue < 0) {
        minValue = 0;
    }
    return minValue;
} // end of analogMax()

// function to non-linearly scale the averaged data values
//  to better represent mouth movements
unsigned int nlScale(unsigned int dataToScale) {
    float data = (float)dataToScale;
    float max = (float)maxValue;
    double scaled = sqrt(data/max);
    return (unsigned int)(scaled * dataToScale);
} // end of nlScale

//  cloud function to select the type of non-linear processing of envelope
//    data. 0 = no non-linear processing; 1 = sqrt processing, more
//    types to be added later
int nlp(String processType) {
    nlProcess = processType.toInt();
    return nlProcess;
} // end of nlp()

// function to detect when the button is pressed, including debounce verification
bool buttonPressed() {
    static ButtonStates _buttonState = buttonOff;
    static unsigned long lastTime = millis();

    switch(_buttonState) {

    case buttonOff:
        if(digitalRead(BUTTON_PIN) == HIGH) { // button not pressed
            _buttonState = buttonOff;
            return false;
        }
        else {  // button is pressed, need to debounce and verify
            lastTime = millis();  // set up the timer'
            _buttonState = pressedTentative;
            return false;
        }

    case pressedTentative:   // button seems to be pressed, need debounce verification
        if(digitalRead(BUTTON_PIN) == HIGH) { // button not pressed
            _buttonState = buttonOff;
            return false;
        }
        else {  // button is pressed
            if( (millis() - lastTime < DEBOUNCE_TIME)) { // button not yet debounced
                _buttonState = pressedTentative; 
                _buttonState = pressedTentative; 
                _buttonState = pressedTentative; 
                return false;
            }
            else {  // button is debounced
                _buttonState = buttonOn;
                return true;  // tell caller that the button has been presed
            }
        }

    case buttonOn: // button remains pressed but don't indicate true anymore
        if(digitalRead(BUTTON_PIN) == LOW)  { // button remains pressed, stay here
            _buttonState = buttonOn;
            return false;
        }
        else {  // button tentatively released, need to verify
            lastTime = millis();  // set timer for debounce
            _buttonState = releasedTentative;
            return false;
        }

    case releasedTentative:  // button seems to be released, need verificaton
        if(digitalRead(BUTTON_PIN) == HIGH) { // button still released
            if( (millis() - lastTime < DEBOUNCE_TIME)) {  // not yet debounced
                _buttonState = releasedTentative;
                return false;
            }
            else {  // debounced and verified released
                _buttonState = buttonOff;
                return false;
            }
        }
        else {  // false reading, button still pressed
            _buttonState = buttonOn;
            return false;
        }

    default:
        if(digitalRead(BUTTON_PIN) == HIGH) { // button not pressed
            _buttonState = buttonOff;
            return false;
        }
        else {  // button is pressed
            _buttonState = pressedTentative;
            return false;
        }
  }

} // end of buttonPressed()

