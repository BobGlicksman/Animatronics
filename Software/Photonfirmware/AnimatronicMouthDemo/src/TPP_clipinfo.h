/*
 *   TPP_clipinfo.h 
 *   
 * Structures to hold the information about the audio clips used by the Animatronic Mouth code 
 * 
 * Copyright (c) 2022 Bob Glicksman, Jim Schrempp
 * https://github.com/TeamPracticalProjects/Animatronics
 *    
*/

#include "Particle.h"
#include <TPP_Animatronic_Global.h>

#ifndef TPP_clipinfo_h
#define TPP_clipinfo_h

// structure definition for clip data
struct  ClipData{
    int personalityNum; // this clip is for this personality
    TOF_detect TOFEvent; // this clip is for this TOF event
    String clipNumber; // the track number on the SD card
    String volume;     // the playback volume setting on the mini MP3 player
    String nlproc;        // the non-linear processing type for clip data
    String avSamples;  // the number of samples to average
    String aMax;       // the largest analog value to map to servo upper limit
    String aMin;       // the smallest analog value to map to the servo lower limit
} ;

extern ClipData audioClips[];




#endif



