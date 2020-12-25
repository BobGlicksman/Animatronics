/***************************************************
DFPlayerFullClip.ino:  Demo program that loops through
all clips on the mini MP3 player SD card.  This demo program
tests that a clip is finished playing before advancing to
the next clip.

Experimentation has revealed the following:
After initialization, the MP3 player is ready to play a clip.  However,
you cannot command the MP3 player again until it is "available".  
Furthermore, if you want to test to see if a clip is done playing,
you can test the return value of "myDFPlayer.readType()" to be the constant
"DFPlayerPlayFinished" (defined in the library header file).  HOWEVER,
if you play the next clip, "DFPlayerPlayFinished" comes back
immediately so you will then play the next clip, i.e. skipping
every other clip.  The reason appears to be that the MP3 player
thinks that there are two "tracks" per clip, but there is only one.
Here is a quote from the DFRobot mini-MP3 player user manual (section
3.3.2, item #4; interpret at your own risk):

"If the currently finish the playing the first song, the track pointer
automatically point to second song, If you send a "play next one" command,
then the module will playback the third song... "

This software is based upon "DFRPlayerTest.ino" which, in turn, is based 
upon the examples in the DFRobotDFPlayerMini library.
 
Author: Bob Glicksman 
Date: 12/25/20
Version 1.0.1
*****************************************************/

#include <DFRobotDFPlayerMini.h>  // MP3 player library
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
  Serial1.begin(9600);  // mini MP3 player conected using Serial1; Tx/Rx pins on Photon
  Serial.begin(9600);   // use Putty for control and status via Serial (USB) port
  
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
  static boolean firstTrack = false;  // flag to indicate skip alternate tracks of clip
  static boolean firstTime = true;  // flag to denote first time through loop()

  if(firstTime == true) {
    // play the first clip
    firstTime = false;  // clear the first time through loop() flag
    Serial.print("\nPlaying clip: "); 
    Serial.println(clip);
    myDFPlayer.play(clip);  // play the next track

  } else {  // not the first time throu so wait for clip to finish
    if (myDFPlayer.available()) { // MP3 player must be available before communicating with it
      
      // wait for prevous clip to finish playing
      if(myDFPlayer.readType() == DFPlayerPlayFinished) {
        
        // make sure that we process altrernate tracks
        if(firstTrack == true) {  // process the track toggle flag for alternate tracks of a clip
          // play the next clip
          clip++;
          if(clip > 5) {
            clip = 1;
          }
          firstTime = false;  // clear the first time through loop() flag
          Serial.print("\nPlaying clip: "); 
          Serial.println(clip);
          myDFPlayer.play(clip);  // play the next track
        }
        firstTrack = !firstTrack; // toggle the track flag
      }      
    }

  }

} // end of loop()
