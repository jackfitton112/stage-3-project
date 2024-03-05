/**
 * @file errorno.h
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Error number definitions
 * @version 0.1
 * @date 04-03-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ERRORNO_H
#define ERRORNO_H

#define OK 0

//GPS error codes
#define GPS_INIT_FAIL 1
#define GPS_READ_FAIL 2


//BLE error codes
#define BLE_INIT_FAIL 21
#define BLE_ADVERTISE_FAIL 22
#define BLE_SERVICE_ADD_FAIL 23

//Radio error codes
#define RADIO_INIT_FAIL 41


#endif // ERRORNO_H
