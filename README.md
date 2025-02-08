#OVERVIEW.
This project developed an animatronic head.  The head is capable of detecting and tracking motion in its field of view and of controlling
an eye mechanism to "wake-up" and track this motion.  The head can also play various recorded voice clips and move its mouth to follow the 
audio in a "convincing manner".  The head can detect various events and select an appropriate voice clip to play in response to the event.

## Motivation.

This project was designed to explore various aspects of animatronics, including three specific technologies:

1. TOF Sensor:  The VL53L5CX intelligent sensor 
(https://www.sparkfun.com/sparkfun-qwiic-mini-tof-imager-vl53l5cx.html)
contains a matrix of SPAD diodes that detect the time-of-flight of infrared light
reflected off an object to compute the distance to that object.  The sensor can report out an 8 x 8 array of distance 
measurements up to 15 times per second, making it a "poor man's LIDAR".

2.  Animatronic Eye Mechanism:  We used an eye mechanism built from this project:
https://www.instructables.com/Simplified-3D-Printed-Animatronic-Dual-Eye-Mechani/ of Nilheim Mechatronics. 
They have a YouTube channel: https://goo.gl/7Cle6h. In June 2024, Will released a new version of the 3D files that 
snaps together - no screws! https://www.youtube.com/watch?v=uzPisRAmo2s.

3.  Voice and Mouth Control.  We use a DFPLayer Mini MP3 module from DFRobotics 
(https://www.dfrobot.com/product-1121.html?srsltid=AfmBOornrlH_d-dh73ndTnoJ0Bhm0fcPcsy0bjisI8v6idOF1iMq-ogo)
to play various audio clips that we recorded on a micro-SD card.  A custom-developed analog processing circuit
scales, filters and extracts the envelope from the audio signal and the sampled envelope undergoes further real-time
software digital signal processing in order to drive a mouth servo in a "convincing manner".

## R & D.

In order to best perform the R&D associated with this project, a laser cut skeleton was developed so that the electronics and servos
can be positioned flexibly.  The "head" itself is 3D printed and flexibly attached to the skeleton via 3D printed ears with magnets
glued onto the mounting ears and the head pieces.  All CAD files for the laser cut and 3D printed pieces are included in this repository.

### Eye Movement.

In order to explore using the complex eye mechanism to create realistic eye movement scenarios, a flexible software system was
developed.

1. Scene:  A "scene" is the combination of the movements of the 6 eye servos to produce some specific effect, e.g. blink the eyes.

2. Scenario:  A "scenario" is a timed script of various scenes that produce some overall effect, e.g. a "sleepy dog" whose eyes 
periodically open just a crack and the eyeballs move around to survey the landscape, and then the eyes close and the dog goes 
back to sleep.

Scenes and Scenarios can be easily custom-defined and combined to create a wide variety of overall eye motion effects.  In addition,
scenarios are defined and implemented to effect eye movement to realistically track motion detected by the TOF sensor.

### Voice and Mouth Movement.

The mouth control electronics can play pre-recorded MP3 voice clips under software command by the onboard microcontroller.
A custom-designed analog signal processing circuit provides the microcontroller's A/D converter with analog signals that 
it can sample and process digitally.

1. Scaled audio:  the left audio MP3 player channel is shifted and scaled to be within the microcontroller's A/D 
converter range of 0 - 3.3 volts.  The signal is also filtered to a 3.3 KHz bandwidth (telephone audio quality) so 
that the microcontroller can sample this signal at 8 KHz (> the Nyquist criteria) and perform whatever digital signal 
processing is desired (at 125 microseconds per sample).
The analog AC signal is offset for the single supply A/D converter and the zero-offset voltage is provided on another A/D converter input
so that this offset can be subtracted (in software) from each sample.  These inputs are provided by the hardware but are not used
in the currently released software.

2. Envelope Extraction:  The scaled audio signal is further processed to extract the "envelope" of the audio signal by AM demodulation.
The bandwidth of the extracted envelope signal is approximately 40 Hz, which was empirically determined to preserve the full
information about the vocal elements of the voice clip while rejecting the "carrier" frequencies.  The envelope signal is 
connected to a 3rd A/D converter channel and the software samples this at 100 Hz.

The sampled envelope signal is processed in software to control the mouth servo.  In order to explore the nuances of controlling
the mouth servo to be "convincing", various configurable processing stages are provided in the released software:

a. Linear and non-linear processing:  The input envelope samples are scaled using either a linear or a non-linear scaling
algorithm.  The released software is configured for linear scaling, but realistic non-linear algorithms may be explored in
the future.

b. Averaging:  The 40 Hz envelope bandwidth and 100 Hz sampling are currently used to drive the mouth servo.  However,
1 to 5 samples may be averaged before being mapped to the servo drive.  The servo that we are using seems to work best
when driven without averaging the samples, and extreme averaging creates a noticeable lag between the audio and mouth movements.
However, averaging may be useful with other mechanical mouth linkages.

c.  Servo scaling: the processed samples are scaled to the desired servo range of travel before being used to control the mouth
servo.

### TOF Sensor.

The TOF sensor is very sophisticated and required a substantial amount of experimentation before we could use it successfully.
After figuring out what all of the controllable parameters do and how best to set them, we then worked on the actual application
of the sensor to this project.  The application includes:

1.  Calibrating the room:  Upon powering up the head, the room should be empty.  The software records the distance measurements
for the empty room so that it can later determine if anything has entered the room and respond to it.

2.  Tracking movement:  Tracking movement within the sensor's field of view turned out to be a challenge, as an 8 x 8 matrix of
distance measurements is complex.  In order to fix a point in space that is closet to the sensor, we had to combine both
spatial and temporal filters to reject "noise" in the data.  This filtering was necessary to produce non-jittery positioning
of the eyes to focus on the closest subject area in space.

3.  Event detection:  The software further processes the TOF data in order to detect various "events" for the head to
respond to:  (a) person(s) entering the TOF field of view, (b) person(s) leaving the TOF field of view, (c) person(s)
coming too close to the head.  Other events might be explored in the future.

# WHAT IS IN TNIS REPOSITORY.

## Top Level.

1.  This README file.

2.  License information.

## Data Folder.

This folder contains digitized audio test files that were used with analysis and circuit simulation software to develop the analog
signal processing parameters for the circuit design.  These files are preserved for historical purposes and for further research;
they are not needed to implement the current animatronic head design.

## Documents Folder.

This folder contains the documentation for this project including a User Manual and development related documentation to memorialize
the R&D performed on this project.

## Hardware Folder.

This folder contains information about the electronic design, including PCB CAD files and hookup data

## Legacy Folder.

This folder contains source code for various test programs that were used to exercise and research the various components
of the animatronic head prior to integrating the complete project.  These files are not part of the released head design but
are retained for possible use in future R&D.

## Mechanical Folder.

This folder contains the 2D and 3D CAD files for the skeleton, the head, the eye mechanism and other 3D printed mechanical
components of this project.

## Software Folder.

This folder contains source code for the two Particle Photon microcontrollers used on this project.  The source code includes
the released operational code for each of the Photons, as well as some test and calibration code that needs to be used for
construction and calibration of the animatronic head.




