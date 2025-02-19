/*
 *   clipinfo.cpp
 *   
 * Structures to hold the information about the audio clips used by the Animatronic Mouth code 
 * 
 * Copyright (c) 2022 Bob Glicksman, Jim Schrempp
 * https://github.com/TeamPracticalProjects/Animatronics
 *    
*/

#include "Particle.h"
#include <TPP_clipinfo.h>

// This array has the entries of parameters to play each clip. At start up
// the array is scanned and loaded into the personalities structure. The
// entries here can be in any order. A particular clipNum could be in this
// array more than once, if that's what you want.

// Be sure to correctly set NUM_PERSONALITIES in the main code file.

ClipData audioClips[] = 
{

    // 0: Bob's set up
    {0, Person_entered_fov, "11", "23", "1", "1", "2500", "0"},  // welcome
    {0, Person_left_fov, "15", "23", "1", "1", "2000", "0"},   // thanks for coming  
    {0, Person_too_close, "14", "23", "1", "1", "2500", "0"},   // backoff you're too close
    {0, Person_left_quickly, "13", "23", "1", "1", "3000", "0"},   // don't walk away when I'm talking to you
    
    // 1: Bob's 2nd set up

    // 2: Jim's set up
    {2, No_change_in_fov, "100", "25", "2", "1", "3700", "0"},  // Snoring
    {2, No_change_in_fov, "102", "25", "2", "1", "3500", "0"},  // Where's that pencil
    {2, No_change_in_fov, "103", "25", "2", "1", "3600", "0"},  // Guess your weight
    {2, No_change_in_fov, "104", "25", "2", "1", "3300", "0"},  // I'm lonely
    {2, Person_entered_fov, "110", "26", "2", "1", "3700", "0"},  // Hello there
    {2, Person_entered_fov, "111", "27", "2", "1", "3100", "0"},  // Nice to see you
    {2, Person_entered_fov, "112", "27", "2", "1", "3600", "0"},   // Come a little closer
    {2, Person_left_fov, "120", "27", "2", "1", "3300", "0"},  // See you later
    {2, Person_left_fov,"121", "27", "2", "1", "3700", "0"},  // Come back again
    {2, Person_left_fov,"122", "26", "2", "1", "3700", "0"},   // Thanks for stopping by
    {2, Person_too_close,"130", "27", "2", "1", "3800", "0"},  // Take off my head
    {2, Person_too_close,"131", "28", "2", "1", "3500", "0"},   // You're a little too close
    {2, Person_left_quickly,"140", "27", "2", "1", "3700", "0"},  // Come back when you have time
    {2, Person_left_quickly,"141", "28", "2", "1", "3900", "0"},  // Alright see you, can't be friends
    {2, Person_left_quickly,"142", "28", "2", "1", "3700", "0"},   // Hey, where are you going  

    // 3: Jim's 2nd set up

    // end marker, always the last array entry
    {-1, (TOF_detect) 0, "no", "no", "no", "no", "no", "no"} 
};

ClipData placeholderClip = {-1, (TOF_detect) 0, "no", "no", "no", "no", "no", "no"};

