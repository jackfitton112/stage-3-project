#####################################
# File: data_logger.py
# Brief: This file is responsible for logging data to a file from the boat via BLE / Serial
# Author: Jack Fitton
# Date: 23/02/2024
#####################################

import time
from bleak import BleakClient, BleakScanner
import asyncio
import struct

BLE_ADDR = "DB:8A:EC:0E:2F:CD"

# BLE Characteristics from the boat
#BLEService boatService("19B10000-E8F2-537E-4F6C-D104768A1214");
#BLEIntCharacteristic boatCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
#BLEDoubleCharacteristic boatGPSLat("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
#BLEDoubleCharacteristic boatGPSLon("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
#BLEIntCharacteristic boatGPSHead("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
#BLEFloatCharacteristic boatGPSSpeed("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

# ble scanner
async def scan():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)


def bigedian_to_littleedian(data):
    return data[::-1]

def bytes_to_double(b):
    # Assuming b is in little-endian format
    return struct.unpack('<d', b)[0]  # Use '<d' for double-precision float

def bytes_to_float(b):
    # Assuming b is in little-endian format
    return struct.unpack('<f', b)[0]  # Use '<f' for single-precision float

def bytes_to_int(b):
    # Assuming b is in little-endian format
    return struct.unpack('<i', b)[0]  # Use '<i' for single-precision float

# ble client
async def connect():
    async with BleakClient(BLE_ADDR) as client:
        x = await client.is_connected()
        print("Connected: {0}".format(x))


        while client.is_connected():

            # read the data from the characteristics
            lat = await client.read_gatt_char("19B10002-E8F2-537E-4F6C-D104768A1214")
            lat = bytes_to_double(lat)

            lon = await client.read_gatt_char("19B10003-E8F2-537E-4F6C-D104768A1214")
            lon = bytes_to_double(lon)

            heading = await client.read_gatt_char("19B10004-E8F2-537E-4F6C-D104768A1214")
            heading = bytes_to_int(heading)

            speed = await client.read_gatt_char("19B10005-E8F2-537E-4F6C-D104768A1214")
            speed = bytes_to_float(speed)

            #log data to file
            with open('data_log.csv', 'a') as file:
                file.write(f"{time.time()}{lat},{lon},{heading},{speed}\n")
            
            print(f"Lat: {lat}, Lon: {lon}, Heading: {heading}, Speed: {speed}")

            await asyncio.sleep(1)
                






# main
async def main():
    await connect()


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())
    loop.close()