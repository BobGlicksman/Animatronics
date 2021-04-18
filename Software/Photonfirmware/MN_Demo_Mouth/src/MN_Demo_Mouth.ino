/*
 * Project MN_Demo_Mouth
 * Description:
 * Author:
 * Date:
 */

#include <DFRobotDFPlayerMini.h>
#include <math.h>

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
const int PIR_PIN = A4;

// defined constants
const unsigned long SAMPLE_INTERVAL = 10; // 10 ms analog input sampling interval
const int MOUTH_CLOSED = 90;  // servo position for the mouth closed
const int MOUTH_OPENED = 105; // servo position for the wide open mouth
const unsigned long BUSY_WAIT = 2000UL; // busy pin wait time = 2 second

// define global variables for the audio envelope data
int maxValue = 4095; // the highest expected analog input value - for servo mapping
int minValue = 0; // the lowest expected analog input value - for servo mapping
int numSamples = 5; // the number of analog input samples to average for a servo command
int nlProcess = 0;  // 0 for skip non linear processing, 1 for sqrt processing, more later...

// cloud variables to report statistics
int maxFound = 0; // the maximum analog value found in the data set
int minFound = 4095; // the minimum analog value found in the data set

// structure definition for clip data
struct ClipData {
  String clipNumber; // the track number on the SD card
  String volume;     // the playback volume setting on the mini MP3 player
  String nlproc;        // the non-linear processing type for clip data
  String avSamples;  // the number of samples to average
  String aMax;       // the largest analog value to map to servo upper limit
  String aMin;       // the smallest analog value to map to the servo lower limit
};

// define some clips
ClipData welcome {"11", "23", "1", "1", "2500", "0"};
ClipData pirate {"12", "23", "1", "1", "3000", "0"};
ClipData walkAway {"13", "23", "1", "1", "3000", "0"};

//function to set up the data and playback a clip
void clipPlay(ClipData thisClip) {
  analogMin(thisClip.aMin);
  analogMax(thisClip.aMax);
  nlp(thisClip.nlproc);
  samples(thisClip.avSamples);
  clipVolume(thisClip.volume);
  clipNum(thisClip.clipNumber);
}  // end of clipPlay()

void setup() {
  // set up Photon pins
  pinMode(BUSY_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

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
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);
  mouthServo.write(MOUTH_CLOSED);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  mouthServo.write(MOUTH_OPENED);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  mouthServo.write(MOUTH_CLOSED);

} // end of setup()

void loop() {
  static bool pirFlag = false;
  static unsigned long busyTime = millis();
  
  speak();

  // wait for the busy line to go high for BUSY_WAIT before triggering playing a clip

  if( (millis() - busyTime) >= BUSY_WAIT) {
    if(digitalRead(BUSY_PIN) == HIGH) {
      // check the PIR; if triggered, play welcome clip
      if(digitalRead(PIR_PIN) == HIGH) {
        if(pirFlag == false) {  // rising edge of PIR flag
         pirFlag = true;
         clipPlay(welcome);
        }
      }
      else {
        pirFlag = false;
      }
      busyTime = millis();
    }
  }
  // button check - no debounding yet
  if(digitalRead(BUTTON_PIN) == LOW) {  // button is activated
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
  else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  }

} // end of loop()




void speak() {
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

