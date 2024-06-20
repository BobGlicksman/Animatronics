# Animatronics

## Overview.
This repository contains experimental hardware, software and documentation for figuring out
ways to control an animatronic device's mouth and eyes to simulate a human-like puppet. 

## The ultimate goal is to
A. Be able to record speech/music and have a robotic mouth move along with the sound in a 
somewhat realistic manner.
B. Trigger some realistic animation behavior when a person approaches the puppet. 


## The operating assumptions are:

### A. Mouth

1. the robotic mouth will be driven by a single servo.  The servo controls the mouth open/closed
position in synch with the overall envelope of the sound source.  The envelope of the sound source
is the overall pattern of sound amplitude with the "carrier" removed; i.e. extracting the
envelope from an audible sound source is equivalent to AM demodulation.

2. the sound source will be pre-stored sound/music files, nominally MP3 files (but the sound format
may be any common format -- MP4A, WAV, etc; these formats can esily be converted from one format to another).

3. an MP3 player will contain a number of pre-recorded sound files.  The selected MP3 player will be capable
of having a sound file selected and played via remote commands from a microcontroller (e.g. Particle Photon).
The MP3 player analog output will be connected to a set of amplified speakers and also to an analog
processing ciruit that will pre-filter the analog audio data and then extract the envelope from the data.

4. the analog audio circuit envelope output will be sampled by the ADC of the microcontroller.  The nominal
sampling period is 20 ms per sample (50 samples/second). Therefore, the bandwidth of the envelope must be < 25 Hz
in order to meet the Nyquist criterion.  A margin needs to be provided, so the envelope bandwidth should be on
the order of 15 - 20 Hz. This seems reasonable for realistic mouth movements.  The 20 ms sample rate provides
an ample number of samples while still leaving plenty of headroom for the microcontroller to process each sample.

5. it is assumed that a realistic mouth movement algorithm might average a number of these samples and then drive
the mouth servo at a lower rate (than 50 movements per second).  A key goal of this project is to determine
how many samples should be averaged to drive the mouth servo in a realistic manner.  Another key goal of this 
project to to determine how to properly scale (averaged) sample values to servo position movements.

### B. Eyes

1. We use an eye mechanism built from this project https://www.instructables.com/Simplified-3D-Printed-Animatronic-Dual-Eye-Mechani/
of Nilheim Mechatronics.  They have a YouTube channel:  https://goo.gl/7Cle6h   (In June 2024 Will released a new version of
the 3D files that snaps together - no screws!  https://www.youtube.com/watch?v=uzPisRAmo2s )

3. We will modify the eye mechanism as needed to suit our project. Changed STL files will be published in this GitHub repository.

4. The mouth will have sensors used to detect people in the area of the puppet. The eyes will be triggered from the mouth mechanism to take action appropriate to a speaking person.

5. When not triggered by the mouth, the eyes will often be "asleep" but occasionally "wake" to look around.


## The overall project concept is:

MP3 board --> analog processing circuit --> Photon ADC --> Photon firmware processing --> Servo control

       |
        ---> amplified speakers

Additionally:
Sensor (TBD) --> Photon --> firmware --> Photon Tx/Rx ---> MP3 board track select and control

And: 
Photon firmware processing of mouth --> Photon firmware of eyes


## Repository contents.
### Data.
#### AnimatronicMouth.mp4: 
this is a video of the mouth movement (roughly) in sync with
a sound file.  The clip is about 4 seconds long.  This was done
using the spice simulation of the old V1 circuit.

#### Welcome_Waveform_High_Data.xlsx:  
This is a spreadsheet containing 201 samples of the
envelope of the voice file, each sample at a 20 ms interval (~ 4 seconds of data).

#### Welcome_high.wav:  
the voice data file that is input to the analog processing circuit that
produces the envelope information.

### Documents.
#### Analog Processor V2 Analysis.pptx:  
MS Powerpoint file that shows the analog processing circuit, the 
concept (block diagram) for the circuit, and Spice simulation and analysis.  Produced using LTspice.

### Hardware.
#### Analog.asc: 
LTspice file of the analog processing circuit with simple sine wave input.

#### Analog_wavefile_input.asc:  
LTspice file of the analog processing circuit with input from a .wav file.

#### DFPlayer Mini Manual.pdf:  
manual for a candidate MP3 player for this project.

### Software/Photonfirmware/AnimatronicMouthTest/src.
#### AnimatronicMouthTest.ino:  
Photon source firmware for testing out a robotic mouth driven
by a servo. Pin connections are for the Team Practical Projects "Wireless I/O Board"
(https://github.com/TeamPracticalProjects/Wireless_IO_Board). 
The data array included in this sorce code file is the data from the spreadsheet
"Welcome_Waveform_High_Data.xlsx" in the "Data" folder of this repository.

#### workspace.code-workspace: 
"workspace" file for the Particle Workbench.  This is needed only if
viewing/editing "AnimatronicMouthTest.ino" using the Particle Workbench.

### Software/Photonfirmware/AnimatronicEyesTest
#### AnimatronicEyes.ino
Photon source firmware for controlling the eyes
#### TPPAnimatePuppet.h/.cpp
A layer to link behaviors between several physical mechanisms. Perhaps to have the head rotate when the eyes move in a particular direction. This module calls TPPAnimateServo.
#### TPPAnimateServo.h/.cpp
Wraps the AdaFruit servo board to allow non blocking calls to move a servo over a certain distance over a certain time frame. Sample operation: Move servo 2 from 70 degrees to 120 degrees over 6 seconds.
#### TPPAnimationList.h/.cp
A module to maintain a sequence of "scenes" (positions of a different physical mechanisms) and transition between them at a time delay specified by the caller. Sample operation: move eyes left 80% and head down by 10%, wait 100 milliseconds, then move eyelids open 100% and head up to 50%, wait 300 milliseconds, then move the head left 60%, etc, etc. This module calls TPPAnimatePuppet.
