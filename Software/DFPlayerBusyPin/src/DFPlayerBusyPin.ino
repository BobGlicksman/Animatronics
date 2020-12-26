/***************************************************
DFPlayerBusyPin.ino:  Demo program that loops through
all clips on the mini MP3 player SD card.  This demo program
tests the BUSY pin on the MP3 player in order to determine
that a clip is finished playing before advancing to the next clip.

Experimentation has revealed the following:
It is necessary to wait about 1 second after starting to play a clip
before testing the BUSY line to see that the MP3 player is actually
busy.  A non-blocking 1 second timer is used in this program.

This software is based upon "DFRPlayerTest.ino" which, in turn, is based 
upon the examples in the DFRobotDFPlayerMini library.
 
Author: Bob Glicksman 
Date: 12/26/20
Version 1.0
*****************************************************/

#include <DFRobotDFPlayerMini.h>  // MP3 player library
DFRobotDFPlayerMini myDFPlayer;

const int BUSY_PIN = D0;
const unsigned long BUSY_TIMER = 1000;
const int LAST_CLIP_NUM = 5;

void setup()
{
  Serial1.begin(9600);  // mini MP3 player conected using Serial1; Tx/Rx pins on Photon
  Serial.begin(9600);   // use Putty for control and status via Serial (USB) port
  pinMode(BUSY_PIN, INPUT);
  
  delay(5000);  // delay to get putty going
  
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
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30

} // end of setup()

void loop() {
  static int clip = 4;  // start playing this clip
  static unsigned long busyDelay = millis();

  // non-blocking timer
  if( (millis() - busyDelay ) >= BUSY_TIMER) {

    // test the busy pin to see if the MP3 player is ready to play a new track
    if(digitalRead(BUSY_PIN) == HIGH) { // HIGH means ready, LOW means busy
      Serial.print("\nPlaying clip: "); 
      Serial.println(clip);
      myDFPlayer.play(clip);  // play the next track

      // advance to the next clip
      clip++;
      if(clip > LAST_CLIP_NUM) {
        clip = 1;
      }
      busyDelay = millis(); // update the busy timer
    }
  }

} // end of loop()
