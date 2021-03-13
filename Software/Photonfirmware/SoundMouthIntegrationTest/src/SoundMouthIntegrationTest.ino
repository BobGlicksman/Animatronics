/**************************************************************************************
 * Project SoundMouthIntegrationTest
 * Description:  This test program allows a clip to be selected on the DR Robot
 *  mini MP3 Player.  The selected clip is played through the analog processing
 *  circuitry and the resulting envelope is sampled by the Photon's A/D converter
 *  (pin A2).  The sampled envelope data is averaged and the averaged samples
 *  are then mapped to servo controls for a "mouth" movement servo.
 * 
 *  The selected analog sampling rate is 100 Hz (one sample every 10 ms).  A 
 *  global variable is used to determine the number of samples to average and
 *  therefore the rate at which servo commands are issued.  Based upon earier testing,
 *  the nominal number of averaged samples is 5; however the intent is to 
 *  experiment in order to find an optimal value.
 * 
 *  Particle cloud functions are defined to:
 *  - select and play a clip on the mini MP3 player
 *  - define the playback volume for the mini MP3 player
 *  - select the max A/D value to map to the max servo mouth open value
 *  - select the min A/D value to map to the min servo mouth closed value
 * 
 *  The averaged A/D samples are examined and the max and min values are
 *  stored in a cloud variable for an entire clip.  The experimenter can
 *  use these reported max and min values to determine the best max and 
 *  min servo map values to try.
 * 
 *  Hardware connections are as follows:
 *  - Photon Tx to mini MP3 Rx
 *  - Photon Rx to mini MP3 Tx
 *  - Photon D2 is a digital input and connected to mini MP3 BUSY pin
 *  - Photon D3 is an output and is the servo control (through a 3.3 - 5v converter)
 *  - Photon A2 is an analog input fromt he envelope output of the analog
 *      processign circuitry.
 * 
 * Author: Bob Glicksman (Jim Schrempp, Team Practical Projects)
 * Version: 1.4
 * Date:  2/09/21
 * (c) 2021, Bob Glicksman, Jim Schrempp, Team Practical Projects
 *  all rights reservd.
 * License: open source, non-commercial
 * History:
 * version 1.4: changed MOUTH_OPENED and MOUTH_CLOSED defined constants for Jim's
 *  3D printed "mock mouth".
 * version 1.3: added cloud function to select the non-linear processing to
 *  use for scaling enbvelope data for servo command purposes. Also changed
 *  mouth wide open position to 105 deg from 90 deg for more realism.
 * version 1.2: added non-linear scaling of the A/D data (using sqrt function
 *  to enhance low level sounds, more or less like human hearing).
 * version 1.1: added constrain() to prevent the servo from pegging; Toggle
 *  the D7 LED every time that the servo is written to so that the actual
 *  servo update rate can be tested using a scope.
 * version 1.0: initial release
 * 
 ***************************************************************************************/

#include <DFRobotDFPlayerMini.h>
#include <math.h>

// create an instance of the mini MP3 player
DFRobotDFPlayerMini miniMP3Player;

// create an instance of the servo
Servo mouthServo;

// define Photon pins
const int BUSY_PIN = D2;
const int SERVO_PIN = D3;
const int LED_PIN = D7;
const int ANALOG_ENV_INPUT = A0;

// defined constants
const unsigned long SAMPLE_INTERVAL = 10; // 10 ms analog input sampling interval
const int MOUTH_CLOSED = 90;  // servo position for the mouth closed
const int MOUTH_OPENED = 105;  // servo position for the wide open mouth

// define global variables for the audio envelope data
int maxValue = 4095; // the highest expected analog input value - for servo mapping
int minValue = 0; // the lowest expected analog input value - for servo mapping
int numSamples = 5; // the number of analog input samples to average for a servo command
int nlProcess = 0;  // 0 for skip non linear processing, 1 for sqrt processing, more later...

// cloud variables to report statistics
int maxFound = 0; // the maximum analog value found in the data set
int minFound = 4095; // the minimum analog value found in the data set



void setup() {
  // set up Photon pins
  pinMode(BUSY_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // register Particle Cloud functions and variables
  Particle.function("clip number", clipNum);
  Particle.function("volume", clipVolume);
  Particle.function("number of samples", samples);
  Particle.function("non-linear processing type", nlp);
  Particle.function("analog input max", analogMax);
  Particle.function("analog input min", analogMin);
  Particle.variable("max envelope value", maxFound);
  Particle.variable("min envelope value", minFound);

  // set up the mini MP3 player
  Serial1.begin(9600);
  miniMP3Player.begin(Serial1);

  // set up the mouth servo
  mouthServo.attach(SERVO_PIN);

  // blink and turn on the D7 LED on to indicate that the device is ready
  digitalWrite(LED_PIN, HIGH);
  mouthServo.write(MOUTH_CLOSED);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  mouthServo.write(MOUTH_OPENED);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  mouthServo.write(MOUTH_CLOSED);

}


void loop() {
  static unsigned long lastSampleTime = millis();
  static unsigned int averagedData = 0;
  static unsigned int numberAveragedPoints = 0;
  static bool toggle = false;
  int servoCommand;

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
      // command the servo
      servoCommand = map(averagedData, minValue, maxValue, MOUTH_CLOSED, MOUTH_OPENED);
      // constrain the servo so it doesn't peg at 0 or 180 degrees.
      servoCommand = constrain(servoCommand, 5, 175);
      // send data to servo only if clip is playing, else close the mouth
      if(digitalRead(BUSY_PIN) == LOW) {
        mouthServo.write(servoCommand);
      } else {
        mouthServo.write(MOUTH_CLOSED);
      }

      // set max and min values found
      if(averagedData > maxFound) {
        maxFound = averagedData;
      } else if (averagedData < minFound) {
        minFound = averagedData;
      }

      averagedData = 0; // reset for the next average
      numberAveragedPoints = 0; // reset the average count

      // toggle the D7 LED so that it will pulse at 1/2 averaged sample time
      //    this will normally be too fast to see on the LED, but good pin to scope
      if(toggle == false) {
        digitalWrite(LED_PIN, LOW);
        toggle = true;
      } else {
        digitalWrite(LED_PIN, HIGH);
        toggle = false;
      }
    }

    lastSampleTime = millis();  // reset the sample timer
  }


  

} // end of loop()

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
  miniMP3Player.play(clip);
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

