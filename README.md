# AnimatronicsMouth

## Overview.
This repository contains experimental hardware, software and documentation for figuring out
ways to control an animatronic device's mouth to simulate speech.  The ultimate goal is to
be able to record speech/music and have a robotic mouth move along with the sound in a 
somewhat realistic manner.

The operating assumptions are:
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

6. the overall project concept is:

MP3 board --> analog processing circuit --> Photon ADC --> Photon firmware processing --> Servo control

       |
        ---> amplified speakers

Additionally:
Sensor (TBD) --> Photon --> firmware --> Photon Tx/Rx ---> MP3 board track select and control

## Repository contents.
### Data.
#### AnimatronicMouth.mp4: 
this is a video of the mouth movement (roughly) in sync with
a sound file.  The video and sound start at about 10 seconds into the video and end
about 4 seconds later.

#### Welcome_Waveform_High_Data.xlsx:  
This is a spreadsheet containing 201 samples of the
envelope of the voice file, each sample at a 20 ms interval (~ 4 seconds of data).

#### Welcome_high.wav:  
the voice data file that is input to the analog processing circuit that
produces the envelope information.

### Documents.
#### Analog Processor Analysis.pptx:  
MS Powerpoint file that shows the analog processing circuit, the 
concept (block diagram) for the circuit, and Spice simulation and analysis.  Produced using LTSpice.

### Hardware.
#### Analog.asc: 
LTspice file of the analog processing circuit with simple sine wave input.

#### Analog_wavefile_input.asc:  
LTspice file of the analog processing circuit with input from a .wav file.

#### DFPlayer Mini Manual.pdf:  
manual for a candidate MP3 player for this project.

### Software/Photon Firmware/AnimatronicMouthTest/src.
#### AnimatronicMouthTest.ino:  
Photon source firmware for testing out a robotic mouth driven
by a servo. Pin connections are for the Team Practical Projects "Wireless I/O Board"
(). The data array included in this sorce code file is the data from the spreadsheet
"Welcome_Waveform_High_Data.xlsx" in the "Data" folder of this repository.

#### workspace.code-workspace: 
"workspace" file for the Particle Workbench.  This is needed only if
viewing/editing "AnimatronicMouthTest.ino" using the Particle Workbench.
