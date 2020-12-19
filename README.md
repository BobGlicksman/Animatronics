# AnimatronicsMouth
This repository contains experimental hardware, software and documentation for figuring out
ways to control an animatronic device's mouth to simulate speech.  The ultimate goal is to
be able to record speech/music and have a robotic mouth move along with the sound in a 
somwhat realistic manner.

The operating assumptions are:
1. the robotic mouth will be driven by a single servo.  The servo controls the mouth open/closed
position in synch with the overall envelope of the sound source.  The envelope of the sound source
is the overall pattern of sound amplitude with the "carrier" removed; i.e. extracting the
envelope from an audible sound source is equivalent to AM demodulation.

2. the sound source will be pre-stored sound/music files, nominally MP3 files (but the sound format
may be any common format (MP4A, WAV, etc; these formats can esily be converted from one format to another).

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

 
