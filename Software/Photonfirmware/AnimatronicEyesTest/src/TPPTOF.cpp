/* 
 * TPPTOF.cpp
 * Team Practical Projects
 * Time of Flight sensor
 * 
 * This is our implementation of an API to call the Time of Flight Sensor.
 * 
 * Part of the animatronic exploration of Team Practical Projects
 * https://github.com/TeamPracticalProjects
*/

#include <Wire.h>
#include <TPPTOF.h>

SparkFun_VL53L5CX mg_myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

attentionPoints currentAttentionPoints;

// Called once to initialize the TOF
void TOFinititialize() {

};

// Called every time through the main loop so the module can decide to read the TOF if it wants
void TOFsense() {

}

// Called to get a list of current points that are of interest
void TOFgetAttentionPoints(attentionPoints *currentPoints) {

}