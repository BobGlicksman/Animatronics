/*
 * TPPAnimationList.cpp
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

#include <TPPAnimationList.h>

Logger logAnilist("app.anilist");

// The order of these must correspond to the order in the eScene enumeration
const char* eSceneNames[12] {
    "sceneEyesAheadOpen",
    "sceneEyesAhead",
    "sceneEyesRight",
    "sceneEyesLeft",
    "sceneEyesUp",
    "sceneEyesDown",
    "sceneEyesClosed",
    "sceneEyesOpen",
    "sceneEyesOpenWide",
    "sceneWinkLeft",
    "sceneWinkRight",
    "sceneBlink"
};

/* ------ addScene
 * Adds a scene to the end of the animation scene list
 * parameters
 *    scene: one of the scenes in the eScene enumeration
 *    speed: 1-10.  10 is fast
 *    daylayAfterMoveMS: how long to delay before the next scene,
 *       after the servos should be finished with this scene. 
 *       -1: do not delay at all. Don't even wait for the servos
 *           to finish moving into this scene. Useful for a compound
 *           scene set. e.g. open eyes but don't wait to finish, start
 *           moving eyes right away.
 */
int animationList::addScene(eScene sceneIn, int speedIn, int delayAfterMoveMSIn){

    // is there room for another scene?
    if (lastSceneIndex == MAX_SCENE - 1) {
        logAnilist.warn("Too many scenes.");
        return 1;
    }

    // add new scene to end of list
    lastSceneIndex++;
    sceneList[lastSceneIndex].scene = sceneIn;
    sceneList[lastSceneIndex].speed = speedIn;
    sceneList[lastSceneIndex].delayAfterMoveMS = delayAfterMoveMSIn;

    return 0;

}

void animationList::startRunning(){
    
    isRunning = true;
    startTimeMS = millis();
    nextSceneChangeMS = millis();
    currentSceneIndex = -1;
    logAnilist("starting animation run");

}

/* --------- process()
 * Works through the animation list setting each scene when the
 * previous scene should be done. 
 */
void animationList::process() {

    bool sceneChangeNow = false;
    int runTime = millis() - startTimeMS;
    //why do the following two generate warnings?
    int sceneToSet = 0; 
    int timeToFinishScene = 0;

    // if not running, then exit
    if (!isRunning) {
        return;
    }

    // Is it time to change to the next scene?
    if (runTime > nextSceneChangeMS) {

        currentSceneIndex++;
        if (currentSceneIndex < lastSceneIndex) {
            sceneChangeNow = true;
            logAnilist.trace("moving to scene list # %d ", currentSceneIndex);
        } else {
            logAnilist.trace("Last Scene has played");
            isRunning = false;
        }

    }

    // We have to set the next scene
    if (sceneChangeNow) {

        sceneChangeNow = false;

        logAnilist.trace("Changing scene now to %s", eSceneNames[sceneList[currentSceneIndex].scene]);

        sceneToSet = sceneList[currentSceneIndex].scene;

        eScene thisScene = sceneList[currentSceneIndex].scene;
        float thisSpeed = sceneList[currentSceneIndex].speed;

        timeToFinishScene = setScene(thisScene, thisSpeed, &head);

        // Should we wait for the servos to finish moving?
        if (sceneList[currentSceneIndex].delayAfterMoveMS > -1 ){

            nextSceneChangeMS = millis() + timeToFinishScene + sceneList[currentSceneIndex].delayAfterMoveMS;

        }  // else the scene will change on the very next call to this process() routine
        
    }

    head.process();

}

// setScene
// Positions the objects to their positions for the scene
// Returns the estimated time to reach the scene
int animationList::setScene(eScene newScene, int speed, TPP_Head *theHead){


    int timeForSceneChange = 0;
    

    logAnilist.trace("now setting scene %s with speed %d ", eSceneNames[newScene], speed);

    // For each scene in the eNum scene, we set the servos to their positions
    switch (newScene) {

        case sceneEyesAheadOpen:
            logAnilist.trace("Scene: eyes ahead and open   ===========================================");
            timeForSceneChange = theHead->eyeballs.lookCenter(speed) ;
            timeForSceneChange = theHead->eyesOpen(50, speed);
            break;

        case sceneEyesAhead: 
            logAnilist.trace("Scene: eyes ahead   ===========================================");
            timeForSceneChange = theHead->eyeballs.lookCenter(speed) ;
            break;

        case sceneEyesOpenWide:
            logAnilist.trace("Scene: eyes open wide   ==========================================="); 
            timeForSceneChange = theHead->eyesOpen(100,speed); 
            break;

        case sceneEyesOpen:
            logAnilist.trace("Scene: eyes open   ==========================================="); 
            timeForSceneChange = theHead->eyesOpen(50,speed); 
            break;

        case sceneEyesClosed:
            logAnilist.trace("Scene: eyes closed   ==========================================="); 
            timeForSceneChange = theHead->eyesOpen(0,speed);    
            break;

        case sceneEyesRight:
            logAnilist.trace("Scene: eyes right  ===========================================");
            timeForSceneChange = theHead->eyeballs.positionX(100,speed); 
            break;
        
        case sceneEyesLeft:
            logAnilist.trace("Scene: eyes left  ===========================================");
            timeForSceneChange = theHead->eyeballs.positionX(0,speed); 
            break;

        case sceneEyesUp:
            logAnilist.trace("Scene: eyes up    ===========================================");
            timeForSceneChange = theHead->eyeballs.positionY(100,speed);
            break;

        case sceneEyesDown:
            logAnilist.trace("Scene: eyes down    ===========================================");
            timeForSceneChange = theHead->eyeballs.positionY(0,speed);
            break;

        case sceneBlink:
            logAnilist.trace("Scene: blink   ===========================================");
            timeForSceneChange = theHead->blink();
            break;

        case sceneWinkLeft:
            logAnilist.trace("Scene: wink left   ===========================================");
            timeForSceneChange = theHead->wink(true);
            break;

        case sceneWinkRight:
            logAnilist.trace("Scene: wink right   ===========================================");
            timeForSceneChange = theHead->wink(false);
            break;

        default:
            logAnilist.trace("Unknown Scene");
            break;
    }

    return timeForSceneChange;
}