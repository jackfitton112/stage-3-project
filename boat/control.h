/**
 * @file control.h
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Header file for the control module
 * @version 0.1
 * @date 18-04-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef CONTROL_H
#define CONTROL_H

#include <mbed.h>
#include "errorno.h"
#include "gps.h"
#include <cmath>
#include "boat.h"

/*
Boat is a trimaran design with two motors, this allows differential thrust to be used to control the boat.
the boat is controlled by a PID controller, the PID controller takes in the desired heading and the current heading
and outputs a value to be sent to the motors.

the control class will take a target pos, work out the required heading and then pass this to the PID controller
the pid controller will look at the current heading and the desired heading and output a value to be sent to the motors
it will also have to have to calulate outputs for two motors based off the output of the PID controller

*/

#define DEG_TO_RAD M_PI / 180
#define RAD_TO_DEG 180 / M_PI

#define KP 1
#define KI 0
#define KD 0 // not used


class boatControl {

    public:
        boatControl();
        void setup();
        void setTargetPosition(float targetLat, float targetLon);
    
    private:
        float _targetLat;
        float _targetLon;

        void _calculateTargetHeading();
        void _calculateMotorOutputs(float input);

        void _PID();
        float _error;
        float _lastError;
        float _integral;
        float _derivative;

        float _currentHeading;
        float _desiredHeading;

        float _motorOutputLeft;
        float _motorOutputRight;

        //tickers for PID and motor control
        mbed::Ticker _pidTicker;
        mbed::Ticker _motorTicker;

    


};


#endif // CONTROL_H

