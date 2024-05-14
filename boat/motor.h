/**
 * @file motor.h
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Motor control functions for the boat
 * @version 0.1
 * @date 17-04-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <Servo.h>
#include <mbed.h>
#include <stdio.h>

//Clamp the speed to 30% of the max speed
#define CLAMP_SPEED


/*
@note: Viper 15 motor controllers are being used for this boat
they use the servo library to control the motors with a PWM signal
1500 is the neutral position, 1000 is full reverse and 2000 is full forward in theory
however the messured output from the remote was more like 1100 - 1480 - 1880 which is what is used in the code
*/

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

#define SERVO_MIN 1100
#define SERVO_MAX 1880
#define SERVO_NEUTRAL 1480


class Motor
{
    public:
        Motor(mbed::PwmOut pin);
        void setSpeed(int speed);
        void stop();
        int getSpeed();
        
    private:
        int _pin;
        int _speed;
        int _direction;
        bool _PIDenabled;
        Servo _servo;
        mbed::Ticker _PIDtimer;


};


#endif // MOTOR_H


