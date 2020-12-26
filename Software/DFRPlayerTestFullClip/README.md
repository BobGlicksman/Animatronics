# DFRPlayerTestFullClip

A Particle project named DFRPlayerTestFullClip.  This program loops through the clips on a DFRobot mini MP3 Player, quering the Player
to see if it is available and if the previous clip has finished.  The program works, but is complex due to many artifacts of the Player's
serial I/O.  One artifact is that the Player appears to play some null track of a clip after playing the actual clip, so the Player reports
a second ready right after the first.  This causes clips to be skipped.  This program uses an extra flag to make sure that skipped clips
don't happen.  The program also uses a second flag to determine if this is the first time through loop(), as the Player status won't show
that the previous clip is complete if there was no previous clip.

Another problem is that querying Player status via serial I/O may cause a clip in process to play distorted.  At least, this has been reported
on the Internet.  Other than this possible issue, the software works.

## Welcome to your project!

Every new Particle project is composed of 3 important elements that you'll see have been created in your project directory for DFRPlayerTestFullClip.

#### ```/src``` folder:  
This is the source folder that contains the firmware files for your project. It should *not* be renamed. 
Anything that is in this folder when you compile your project will be sent to our compile service and compiled into a firmware binary for the Particle device that you have targeted.

If your application contains multiple files, they should all be included in the `src` folder. If your firmware depends on Particle libraries, those dependencies are specified in the `project.properties` file referenced below.

#### ```.ino``` file:
This file is the firmware that will run as the primary application on your Particle device. It contains a `setup()` and `loop()` function, and can be written in Wiring or C/C++. For more information about using the Particle firmware API to create firmware for your Particle device, refer to the [Firmware Reference](https://docs.particle.io/reference/firmware/) section of the Particle documentation.

#### ```project.properties``` file:  
This is the file that specifies the name and version number of the libraries that your project depends on. Dependencies are added automatically to your `project.properties` file when you add a library to a project using the `particle library add` command in the CLI or add a library in the Desktop IDE.

## Adding additional files to your project

#### Projects with multiple sources
If you would like add additional files to your application, they should be added to the `/src` folder. All files in the `/src` folder will be sent to the Particle Cloud to produce a compiled binary.

#### Projects with external libraries
If your project includes a library that has not been registered in the Particle libraries system, you should create a new folder named `/lib/<libraryname>/src` under `/<project dir>` and add the `.h`, `.cpp` & `library.properties` files for your library there. Read the [Firmware Libraries guide](https://docs.particle.io/guide/tools-and-features/libraries/) for more details on how to develop libraries. Note that all contents of the `/lib` folder and subfolders will also be sent to the Cloud for compilation.

## Compiling your project

When you're ready to compile your project, make sure you have the correct Particle device target selected and run `particle compile <platform>` in the CLI or click the Compile button in the Desktop IDE. The following files in your project folder will be sent to the compile service:

- Everything in the `/src` folder, including your `.ino` application file
- The `project.properties` file for your project
- Any libraries stored under `lib/<libraryname>/src`
