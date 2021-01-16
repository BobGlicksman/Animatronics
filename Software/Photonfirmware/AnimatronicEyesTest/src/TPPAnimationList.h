/*
 * TPPAnimationList.h
 * 
 * Team Practical Project animation scene control
 * 
 * This library uses the TPPAnimateHead library to control the mechanisms in the head.
 * It is initialized with a set of "scenes". Each scene is listed in the eScene enumeration. 
 * Given a series of scenes this library will then move the mechanism objects through those
 * scenes. Each scene that is added to the list also contains a "speed" used to move
 * the objects from the previous scene to the new scene.
 * 
 * The scene control will ask each object how long it will take to reach the new position and
 * wait that long before moving to the next scene. 
 * 
 * When adding a scene you specify a "delay". A delay of 0 means to move to the next scene as 
 * soon as the time specified by the objects is exceeded. 
 * 
 * Specify a delay > 0 if you want the scene to dwell for some 
 * time before moving on to the next scene in the list. 
 * 
 * Note that a delay of -1 tells the
 * scene contol to not wait for the objects to reach their positions. This is useful when
 * you want to make a scene of several sub-scenes. For example, eyes moving left slowly
 * while the eyelids also close slowly. To do this, add a scene to move the eyes with 
 * delay -1 followed by a scene to close the lids.
 *  
 * Instantiate this class, and it will create an instance of the TPPAnimateHead library.
 * 
 * Key methods
 *      .process()  called over and over to cause the objects to move from current
 *              position to the new target position. This function in turn calls process()
 *              on each of the other control objects
 *      .addScene()  as described above, adds a new scene to the end of the scene list
 *      .startRunning()  starts the animation list running from the first scene
 * 
 * still to come
 *      .stopRunning()
 *      .clearList()
 * 
 * 
 * For full documentation see https://github/TeamPracticalProjects/XXXX
 * 
 * (cc) Non-Commercial Share-Alike Attribution 2021 Bob Glicksman, Jim Schrempp
 * 
 */

#ifndef _TPP_ANIMATION_LIST
#define _TPP_ANIMATION_LIST

#define MAX_SCENE 100

#include <TPPAnimateHead.h>
//#include <Wire.h> // DO NOT USE Serial.anything, it is not thread safe. Use Log.

enum eScene {
    sceneEyesAheadOpen,
    sceneEyesAhead,
    sceneEyesRight,
    sceneEyesLeft,
    sceneEyesUp,
    sceneEyesDown,
    sceneEyesClosed,
    sceneEyesOpen,
    sceneEyesOpenWide,
    sceneWinkLeft,
    sceneWinkRight,
    sceneBlink
};



class animationList {
    public:
        struct sceneInfo {
            eScene scene;
            int speed;
            int delayAfterMoveMS;
        };
        TPP_Head head;
        int addScene(eScene scene, int speed, int delayAfterMoveMS);
        void process();
        void startRunning();

        
    private: 
        int currentSceneIndex = 0;
        int lastSceneIndex = -1;
        int startTimeMS = 0;
        int nextSceneChangeMS = 0;
        bool isRunning = false;
        sceneInfo sceneList[MAX_SCENE]; 

        int setScene(eScene newScene, int speed, TPP_Head *theHead);
};

#endif