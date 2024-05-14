/**
 * @file control.cpp
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Implementation file for the control module
 * @version 0.1
 * @date 18-04-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "control.h"

boatControl::boatControl() {
    _targetLat = 0;
    _targetLon = 0;
    // Headings are in RADS
    _currentHeading = 0;
    _desiredHeading = 0;
    _motorOutputLeft = 0;
    _motorOutputRight = 0;
    _error = 0;
    _integral = 0;
    _derivative = 0;
}

void boatControl::setup() {
    // setup the PID controller ticker
    _pidTicker.attach(mbed::callback(this, &boatControl::_PID), 0.1);
}

void boatControl::setTargetPosition(float targetLat, float targetLon) {
    _targetLat = targetLat;
    _targetLon = targetLon;
}

void boatControl::_calculateTargetHeading() {
    // calculate the heading to the target position

    //get the current position from the gps module
    double currentLat = gpsData.lat * DEG_TO_RAD;
    double currentLon = gpsData.lon * DEG_TO_RAD;

    //_targetLat and _targetLon are the target position

    double dlon = _targetLon - currentLon;

    double y = sin(dlon) * cos(_targetLat);
    double x = cos(currentLat) * sin(_targetLat) - sin(currentLat) * cos(_targetLat) * cos(dlon);
    double bearingRad = atan2(y, x);

    _desiredHeading = bearingRad;

}

void boatControl::_PID() {
    // calculate the error
    _error = _desiredHeading - _currentHeading;

    // calculate the integral
    _integral += _error;

    // calculate the derivative
    _derivative = _error - _lastError;
    _lastError = _error;

    // calculate the output
    float output = KP * _error + KI * _integral + KD * _derivative;

    // calculate the motor outputs
    
    //This output will either be positive or negative, positive means the boat needs to turn right
    //negative means the boat needs to turn left, this then needs to be mapped to the motor outputs
    _calculateMotorOutputs(output);
}

void boatControl::_calculateMotorOutputs(float input){;

    //if input is positive, the boat needs to turn right (left motor faster)
    //if input is negative, the boat needs to turn left (right motor faster)
    //if input is 0, the boat is going straight
    //setSpeed takes a value between -1 and 1, so we need to map the input to this range
    




    // set the motor speeds
    leftMotor.setSpeed(_motorOutputLeft);
    rightMotor.setSpeed(_motorOutputRight);

}