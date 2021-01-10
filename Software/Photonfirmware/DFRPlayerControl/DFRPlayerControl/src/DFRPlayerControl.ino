/***************************************************
DFPlayerControl.ino:  Program that plays a clip from
the DFRobot miniMP3 player continuously (over and over).
The clip is selected via Particle.function() so that it
can be selected from the Particle Console.  This program
is intended for testing the analog processing circuitry 
that has been designed for the Animatronics project.

This program tests the BUSY pin on the MP3 player in 
order to determine that a clip is finished playing before 
repeating it.  On each repeat, a global variable determines
the clip that is to be played.  To change the clip, use
the cloud function.

Experimentation has revealed the following:
It is necessary to wait about 1 second after starting to play a clip
before testing the BUSY line to see that the MP3 player is actually
busy.  A non-blocking 1 second timer is used in this program.

This software is based upon "DFPlayerBusyPin.ino" which was used to
determine how to use the miniMP3 Player's BUSY pin.  The BUSY
pin is connected to pin D0 on the Photon.  Photon Tx is connected
to miniMP3 Rx and Photon Rx is connected to miniMP3 Tx.  The Photon
Serial1 port is thereby used to communicate with the miniMP3 player
 
Author: Bob Glicksman 
Date: 1/10/21
Version 1.0
*****************************************************/

#include <DFRobotDFPlayerMini.h>  // MP3 player library
DFRobotDFPlayerMini myDFPlayer;

const int BUSY_PIN = D0;
const int LED_PIN = D7;
const unsigned long BUSY_TIMER = 1000;
const int LAST_CLIP_NUM = 10;

int clip = 0; // the clip to play
int volume = 30;   // the mini MP3 player volume

void setup()
{
  Serial1.begin(9600);  // mini MP3 player conected using Serial1; Tx/Rx pins on Photon

  // debug: enable usb serial port and print mini MP3 status
  Serial.begin(9600);   // use Putty for control and status via Serial (USB) port
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(Serial1)) {  //Use Photon Serial1 to communicate with mp3 player
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);  // hang up here to clear out hardware issue
  }
  Serial.println(F("DFPlayer Mini online."));

  pinMode(BUSY_PIN, INPUT); // setup the busy status indicator
  pinMode(LED_PIN, OUTPUT); // the D7 led
  digitalWrite(LED_PIN, LOW);



  // register the cloud function that selects the clip to play
  Particle.function("clip number", clipNum);

  // register the cloud function that selects the volume
  Particle.function("volume", clipVolume);

  delay(5000);  // delay to get putty going

} // end of setup()

void loop() {
  static unsigned long busyDelay = millis();

  digitalWrite(LED_PIN, HIGH); // indicate ready via D7 LED

  // non-blocking timer
  if( (millis() - busyDelay ) >= BUSY_TIMER) {

    // test the busy pin to see if the MP3 player is ready to play a new track
    if(digitalRead(BUSY_PIN) == HIGH) { // HIGH means ready, LOW means busy
      if( (clip > 0) && (clip <= LAST_CLIP_NUM)) {
        Serial.print("\nPlaying clip: "); 
        Serial.println(clip);

        myDFPlayer.volume(volume);  //Set volume value. From 0 to 30
        myDFPlayer.play(clip);  // play the next track

      }
      busyDelay = millis(); // update the busy timer
    }
  }

} // end of loop()

// the cloud function to select the next clip.  0 = no clip
int clipNum(String clipNumber) {
  clip = clipNumber.toInt(); // set the global variable to the next clip number
  return 0;
} // end of clipNum()

// the cloud function to select the clip volume (0 - 30)
int clipVolume(String clipVol) {
  volume = clipVol.toInt(); // set the global variable to the next clip number
  return 0;
} // end of clipVol()
