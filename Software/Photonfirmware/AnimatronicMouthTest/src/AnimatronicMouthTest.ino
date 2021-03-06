/******************************************************************************
 * AnimatronicMouthTest.ino:  Test program to experiment with using a servo
 *  controlled "mouth" to mimic speech in an animatronic figure.
 * 
 * This program uses an array of 201 integers as the source of "envelope"
 * data from sampling a .wav file containing a welcoming message.  The
 * array was generated by processing the file "Welcome_high.wav" through
 * an analog circuit simulated in LTSpice: "Analog_wavefile_input.asc".  The
 * LTSpice simulation captured the output of this circuit which is the envelope 
 * data for the input .wav file.  The simulation was was copied to an Excel
 * spreadsheet and then edited so that only samples of the data at 250 ms
 * intervals were retained.  These samples (4 seconds worth of speech data) were
 * then copied inbto the array "env_data" which is contained in the file "env_data.dat".
 * This file also contains the number of elements in the array in the const int "env_data_size"
 * (in this case, 201 elements in the array representing about 4 seconds worth of
 * sample envelope data).  The data in the array is scaled where 0 = 0 volts and
 * 4095 = 3.3 volts.
 * 
 * This program loops through the array, averaging the last NUM_AVERAGE number
 * of samples, scaling the result to the servo values 0 to 90 degrees, and operating
 * the servo accordingly.  The idea is to experimnent with various number of
 * samples averaged to determine a realistic way to operate the servo that
 * simulates mouth movements.
 * 
 * The program runs on a Particle Photon that is used on a TPP "Wireless_IO_Board".
 * This board has a servo control attachment on Photon pin D1.
 * 
 * The program reads through the array, averaging and controlling the servo, until
 * all of the array has been read through.  The program then waits loops forever until
 * the Photon is reset.
 * 
 * (c) 2020, 2021 Team Practical Projects, Bob Glicksman, Jim Schrempp
 * version: 1.0; 12-18-20
 * 
 * *********************************************************************************/

// Data file
const int env_data_size = 201;
uint16_t env_data[] = 
	{34,34,34,34,34,34,34,34,34,34,34,34,43,2439,3374,2481,1845,1937,1681,1534,
	2152,671,138,49,36,34,37,219,1224,1026,744,790,1557,1671,587,405,315,676,1605,1258,
	1216,1996,2155,2004,1492,1043,371,110,45,36,34,68,808,297,718,995,672,215,127,82,
	58,331,304,658,1714,1553,1581,1459,784,337,168,53,37,34,41,732,266,224,1502,2757,
	1809,352,78,69,84,50,753,2165,2499,2787,2833,2712,3142,1920,969,827,435,574,632,128,
	47,61,40,327,1433,1610,1439,1755,2452,2471,1441,443,350,234,222,112,45,36,38,46,
	81,85,58,39,39,38,40,49,46,51,53,42,37,34,34,34,196,165,89,60,
	97,228,318,289,140,52,37,34,34,35,39,55,61,90,86,80,52,41,39,37,
	41,101,115,387,573,505,519,198,224,75,41,35,34,34,34,35,57,89,209,394,
	560,465,118,49,38,35,43,43,52,61,93,166,228,194,192,122,59,43,37,34,34};

// Photon pin definitions
const int SERVO_PIN = D1;

// Constants
const int NUMBER_SAMPLES_TO_AVERAGE = 2;    // this number of samples is averaged to drive the servo
const int MOUTH_CLOSED = 90;    // servo position for the mouth closed
const int MOUTH_OPENED = 180;   // servo position for wide open mouth
const int MIN_DATA = 0;         // lowest value of averaged data
const int MAX_DATA = 3500;      // highest value of averaged data

// define the mouth servo object
Servo mouthServo;

// Globals for console display
    int dataPointIndex = 0;     // variable to hold the index into the data array
    int servoPoints = 0;        // number of servo control points

void setup() {
    pinMode(D7, OUTPUT);
    mouthServo.attach(SERVO_PIN);
    Particle.variable("index", dataPointIndex);
    Particle.variable("servoPoints", servoPoints);

    // flash D7 LED twice to indocate setup is complete
    flashLED(D7);
    delay(500);
    flashLED(D7);
    delay(500);
    flashLED(D7);
    delay(500);
    flashLED(D7);
}   // end of setup()

void loop() {

    static unsigned long sampleTime = millis();
    static unsigned int averagedData = 0;
    static int numberAveragedPoints = 0;
    int servoCommand;


    // process the data array
    if(dataPointIndex < env_data_size) {
        //  walk through the data samples array, one point every 20 ms
        if( (millis() - sampleTime) >= 20) {  // wait for 50 ms to elapse
            if(numberAveragedPoints < NUMBER_SAMPLES_TO_AVERAGE) { // get another point into the average
                averagedData += env_data[dataPointIndex++];
                numberAveragedPoints++;
            } else { // process the average, control the servo, and reset for another average
                averagedData = averagedData / NUMBER_SAMPLES_TO_AVERAGE;
                // scale the data
                servoCommand = map(averagedData, MIN_DATA, MAX_DATA, MOUTH_OPENED, MOUTH_CLOSED);
                // control the servo
                mouthServo.write(servoCommand);
                servoPoints++;  // increment the count of outputs to the servo in the run
                // reset averaged data
                averagedData = 0;
                numberAveragedPoints = 0;
            }

            flashLED(D7); // toogle the D7 LED to show things are working
            sampleTime = millis();  // reset the sample interval time
        }
        


    } else {    // we have walked through the entire data array
        while(true){
            // loop forever until reset the Photon
            flashLED(D7);
            delay(500);
            flashLED(D7);
            delay(500);
        }
    }

}   // end of loop()

void flashLED(int LEDpin) {
    static  bool flashLed = true;

    if(flashLed == true) {
        digitalWrite(LEDpin, HIGH);
        flashLed = false;

    } else {
        digitalWrite(LEDpin, LOW);
        flashLed = true;
    }
}   // end of flashLED()