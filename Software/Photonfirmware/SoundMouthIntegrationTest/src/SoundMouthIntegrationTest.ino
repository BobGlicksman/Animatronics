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
 * Version: 1.0
 * Date:  1/16/21
 * (c) 2021, Bob Glicksman, Jim Schrempp, Team Practical Projects
 *  all rights reservd.
 * License: open source, non-commercial
 * 
 ***************************************************************************************/

#include <DFRobotDFPlayerMini.h>

// create an instance of the mini MP3 player
DFRobotDFPlayerMini miniMP3Player;

// create an instance of the servo
Servo mouthServo;

// define Photon pins
const int BUSY_PIN = D2;
const int SERVO_PIN = D3;
const int LED_PIN = D7;
const int ANALOG_ENV_INPUT = A2;

// defined constants
const int SAMPLE_INTERVAL = 10; // 10 ms analog input sampling interval
const int MOUTH_CLOSED = 90;  // servo position for the mouth closed
const int MOUTH_OPENED = 180;  // servo position for the wide open mouth

// define global variables for the mini MP3 Player
int maxValue = 4095; // the highest expected analog input value - for servo mapping
int minValue = 0; // the lowest expected analog input value - for servo mapping
int numSamples = 5; // the number of analog input samples to average for a servo command

// cloud variables to report statistics
int maxFound = 0; // the maximum analog value found in the data set
int minFound = 0; // the minimum analog value found in the data set



void setup() {
  // set up Photon pins
  pinMode(BUSY_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // register Particle Cloud functions and variables
  Particle.function("clip number", clipNum);
  Particle.function("volume", clipVolume);
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
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);

}


void loop() {
  // read a sample every 10 ms (non-blocking)
  // average the samples
  // record min and max values
  // map averaged values and control the servo

}

// cloud function to set the clip number and play the clip
int clipNum(String playClip) {
  int clip;
  clip = playClip.toInt();
  if (clip < 0) {
    clip = 0;
  }
  miniMP3Player.play(clip);
  return 0;
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
  return 0;
} // end of clipVolume

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

