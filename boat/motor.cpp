/**
 * @file motor.cpp
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Motor control functions for the boat
 * @version 0.1
 * @date 17-04-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "motor.h"




/**
 * @brief Construct a new Motor:: Motor object
 * @note: An initialiser list is used to set the pin
 * 
 * @param pin 
 */
Motor::Motor(mbed::PwmOut pin) : _pin(pin)
{
    _speed = 0;
    _direction = 0;
    _servo.attach(_pin);
    _servo.writeMicroseconds(SERVO_NEUTRAL);
}

/**
 * @brief Construct a new Motor::set Speed object
 * 
 * @param speed - value between -1 and 1 where 0 is stopped and 100 is full speed forward and -100 is full speed reverse
 *
 
 */
void Motor::setSpeed(int speed)
{
    // map the speed to the servo values
    float mappedSpeed = mapFloat(speed, -1, 1, SERVO_MIN, SERVO_MAX);

    #ifdef CLAMP_SPEED
    // clamp the speed to the 30% of the max speed
    if (mappedSpeed > SERVO_NEUTRAL + SERVO_MAX * 0.3)
    {
        mappedSpeed = SERVO_NEUTRAL + SERVO_MAX * 0.3;
    }
    else if (mappedSpeed < SERVO_NEUTRAL - SERVO_MAX * 0.3)
    {
        mappedSpeed = SERVO_NEUTRAL - SERVO_MAX * 0.3;
    }
    #endif


    _servo.writeMicroseconds(mappedSpeed);
    _speed = speed;

}

void Motor::stop()
{
    _servo.writeMicroseconds(SERVO_NEUTRAL);
    _speed = 0;

}


int Motor::getSpeed()
{
    return _speed;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}