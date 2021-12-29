/* 
 * TPPTOF.h
 * Team Practical Projects
 * Time of Flight sensor
 * 
 * This is our implementation of an API to call the Time of Flight Sensor.
 * 
 * Part of the animatronic exploration of Team Practical Projects
 * https://github.com/TeamPracticalProjects
*/
#ifndef _TPP_TOF_H
#define _TPP_TOF_H

#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

typedef struct {
    int azimuth;
    int altitude;
} attentionPoint;

typedef struct {
    int numPoints;
    attentionPoint  apoint[64];
} attentionPoints;


// Called once to initialize the TOF
void TOFinititialize();

// Called every time through the main loop so the module can decide to read the TOF if it wants
void TOFsense();

// Called to get a list of current points that are of interest
void TOFgetAttentionPoints(attentionPoints *currentPoints);

#endif