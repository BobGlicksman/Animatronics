/*
 *    TPP_Animatronic_Global.h
 * 
 * Header file for the animatronic project defines that are shared among all modules.
 * 
 * Copyright (c) 2022 Bob Glicksman, Jim Schrempp
 * 
*/

#ifndef TPP_Animatronic_Global_H
#define  TPP_Animatronic_Global_H

// define enumerated events reported from the eyes code
enum TOF_detect {
    No_change_in_fov = 0,
    Person_entered_fov = 1,   // empty FOV goes to a valid detection in any zone
    Person_left_fov = 2,      // valid detection in any zone goes to empty FOV
    Person_too_close = 3,     // smallest distance is < TOO_CLOSE mm
    Person_left_quickly = 4   // same as #2 but FOV was vacated in a short time period
};
#define NUM_TOF_EVENTS 5  // if you add more events, update this 


#endif