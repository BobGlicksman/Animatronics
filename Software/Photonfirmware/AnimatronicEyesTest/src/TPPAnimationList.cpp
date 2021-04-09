/*
 * TPPAnimationList.cpp
 * 
 * Team Practical Project animation scene control
 * 
 * This library uses the TPPAnimatepuppet library to control the mechanisms in the puppet.
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
 * Instantiate this class, and it will create an instance of the TPPAnimatepuppet library.
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
const char* eSceneNames[8] {
    "sceneEyesAheadOpen",
    "sceneEyesAhead",
    "sceneEyesRight",
    "sceneEyesUpDown",
    "sceneEyesOpen",
    "sceneEyelidsLeft",
    "sceneEyelidsRight"
    "sceneBlink"
};

/* ------ addScene
 * Adds a scene to the end of the animation scene list
 * parameters
 *    scene: one of the scenes in the eScene enumeration
 *    modifier: an int passed down to the scene setting routine. No standard 
 *      definition.
 *    speed: 1-10.  10 is fast
 *    daylayAfterMoveMS: how long to delay before the next scene,
 *       after the servos should be finished with this scene. 
 *       -1: do not delay at all. Don't even wait for the servos
 *           to finish moving into this scene. Useful for a compound
 *           scene set. e.g. open eyes but don't wait to finish, start
 *           moving eyes right away.
 */
int animationList::addScene(eScene sceneIn, int modifierIn, float speedIn, int delayAfterMoveMSIn){

    // is there room for another scene?
    if (lastSceneIndex_ == MAX_SCENE - 1) {
        logAnilist.warn("Too many scenes.");
        return 1;
    }

    // add new scene to end of list
    lastSceneIndex_++;
    sceneList_[lastSceneIndex_].scene = sceneIn;
    sceneList_[lastSceneIndex_].modifier = modifierIn;
    sceneList_[lastSceneIndex_].speed = speedIn;
    sceneList_[lastSceneIndex_].delayAfterMoveMS = delayAfterMoveMSIn;

    return 0;

}

/* ----- startRunning ----
 * starts an animation run, beginning at the first scene
 */
void animationList::startRunning(){
    
    isRunning_ = true;
    nextSceneChangeMS_ = millis();
    currentSceneIndex_ = -1;
    logAnilist("starting animation run");

}

/* ----- isRunning -----
 * returns true if an animation run is in progress
 */
bool animationList::isRunning(){
    return isRunning_;
}

/* ----- stopRunning -----
 *  stops the current animation run but does not 
 *  reset the scene pointer. If you startRunning
 *  after calling this, the animation list will 
 *  continue
 */
void animationList::stopRunning(){
    isRunning_ = false;
}

/* ----- clearSceneList -----
 *  resets the scene list. If startRunning
 *  is called immediately after this it will
 *  essentially have no effect on the mechanisms.
 */
void animationList::clearSceneList(){
    isRunning_ = false;
    currentSceneIndex_ = -1;
    lastSceneIndex_ = -1;
}

/* --------- process()
 * Works through the animation list setting each scene when the
 * previous scene should be done. 
 */
void animationList::process() {

    bool sceneChangeNow = false;
    int runTime = millis();
    int timeToFinishScene_ = 0;    

    // if not running, then exit
    if (!isRunning_) {
        return;
    }

    // Is it time to change to the next scene?
    if (runTime > nextSceneChangeMS_) {

        currentSceneIndex_++;
        if (currentSceneIndex_ <= lastSceneIndex_) {
            sceneChangeNow = true;
            logAnilist.trace("moving to scene list # %d ", currentSceneIndex_);
        } else {
            logAnilist.trace("Last Scene has played");
            isRunning_ = false;
        }

    }

    // We have to set the next scene
    if (sceneChangeNow) {

        sceneChangeNow = false;

        logAnilist.trace("Changing scene now to %s", eSceneNames[sceneList_[currentSceneIndex_].scene]);

        eScene thisScene = sceneList_[currentSceneIndex_].scene;
        int thisModifier = sceneList_[currentSceneIndex_].modifier;
        float thisSpeed = sceneList_[currentSceneIndex_].speed;

        timeToFinishScene_ = setScene(thisScene, thisModifier, thisSpeed); //XXX, &puppet);

        // Should we wait for the servos to finish moving?
        if (sceneList_[currentSceneIndex_].delayAfterMoveMS > -1 ){

            nextSceneChangeMS_ = millis() + timeToFinishScene_ + sceneList_[currentSceneIndex_].delayAfterMoveMS;

        }  // else the scene will change on the very next call to this process() routine
        
        logAnilist.trace("Next scene at: %d",nextSceneChangeMS_);
    }

    puppet.process();

}

// setScene
// Positions the objects to their positions for the scene
// Returns the estimated time to reach the scene
int animationList::setScene(eScene newScene, int modifier, float speed) { //, TPP_puppet *thepuppet){ XXX

    int timeForSceneChange = 0;

    logAnilist.info("now setting scene %s with speed %.2f ", eSceneNames[newScene], speed);

    // For each scene in the eNum scene, we set the servos to their positions
    switch (newScene) {

        case sceneEyesAheadOpen:
            timeForSceneChange = puppet.eyeballs.lookCenter(speed) ;
            timeForSceneChange = puppet.eyesOpen(50, speed);
            break;

        case sceneEyesAhead: 
            timeForSceneChange = puppet.eyeballs.lookCenter(speed) ;
            break;

        case sceneEyesOpen:
            // modifier is how far open. 0:closed 100:open wide
            timeForSceneChange = puppet.eyesOpen(modifier,speed); 
            break;

        case sceneEyesLeftRight:
            timeForSceneChange = puppet.eyeballs.positionX(modifier,speed); 
            break;

        case sceneEyesUpDown:
            timeForSceneChange = puppet.eyeballs.positionY(modifier,speed);
            break;

        case sceneBlink:
            timeForSceneChange = puppet.blink();
            break;

        case sceneEyelidsLeft:
            timeForSceneChange = puppet.eyelidLeftUpper.position(modifier, speed);
            timeForSceneChange = puppet.eyelidLeftLower.position(modifier, speed);
            break;

        case sceneEyelidsRight:
            timeForSceneChange = puppet.eyelidRightUpper.position(modifier, speed);
            timeForSceneChange = puppet.eyelidRightLower.position(modifier, speed);
            break;

        default:
            logAnilist.error("Unknown Scene");
            timeForSceneChange = 10000;
            break;
    }

    return timeForSceneChange;
}