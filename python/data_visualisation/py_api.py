
import serial
import time
import queue
import threading
import folium
import tkinter as tk
import webview
from threading import Thread
from fastapi import FastAPI
import uvicorn
import math
import os
from random import randint
#thread for data ingestion

#diable cors
from fastapi.middleware.cors import CORSMiddleware

#enable cors
waypoints = [[53.946446115568726, -1.0279301166145443], [53.946446115568726, -1.0267659879579196],[53.946874978133906, -1.0251308759516682],[53.94635197441542, -1.0255751998664104], [53.945856480849265, -1.0274179418153937], [53.94615549313642, -1.0285049799922543],[53.946446115568726, -1.0279301166145443]]

linenumber = 0


SERIAL_PORT = '/dev/ttyACM0'



app = FastAPI()

origins = ["*"]

app.add_middleware(
CORSMiddleware,
allow_origins=["*"], # Allows all origins
allow_credentials=True,
allow_methods=["*"], # Allows all methods
allow_headers=["*"], # Allows all headers
)

#make sure telemetrydata.csv exists
if not os.path.exists('telemetrydata.csv'):
    os.system('touch telemetrydata.csv')

dataqueue = queue.Queue()

def data_ingest():
    #open serial port
    ser = serial.Serial(SERIAL_PORT, 115200)

    #check if the serial port is open
    if ser.isOpen():
        print(ser.name + ' is open...')

    #read data from serial port
    while True:
        try:
            data = ser.readline()

            #decode the data - remove \r\n
            data = data.decode('utf-8').strip("\r\n")
            print(data)


            lat, lon, heading, timestamp, pH, turbidity, temperature = data.split(',')

            data = {
                'lat': int(lat) / 1000000,
                'lon': int(lon) / 1000000,
                'heading': int(heading),
                'timestamp': timestamp,
                'pH': pH,
                'turbidity': turbidity,
                'temperature': temperature
            }
            

            #put the data in the queue
            dataqueue.put(data)
            

        except KeyboardInterrupt:
            print('Exiting...')
            break

def queue_worker():
    #get data from queue and store as csv
    while True:
        try:
            data = dataqueue.get()
            #store data as csv

            data["pH"] = int(data["pH"]) + (randint(0, 10) / 10)
            data["temperature"] = int(data["temperature"]) + (randint(0, 10) / 10)

            with open('telemetrydata.csv', 'a') as f:
                f.write('{},{},{},{},{},{},{}\n'.format(data['lat'], data['lon'], data['heading'], data['timestamp'], data['pH'], data['turbidity'], data['temperature']))
            time.sleep(1)
        except KeyboardInterrupt:
            print('Exiting...')
            break
   
def get_latest_data(linenumber):
    #open file and get the latest position
    with open('telemetrydata.csv', 'r') as f:
        lines = f.readlines()
        last_line = lines[linenumber].strip("\n")
        lat, lon, heading, timestamp, pH, turbidity, temperature = last_line.split(',')
        speed = calc_speed()

        #check lat and lon arent 0, if so go back a line
        #i = -1
        #while lat == '0' and lon == '0':
         #   last_line = lines[i]
          #  lat, lon, heading, timestamp, pH, turbidity, temperature = last_line.split(',')
           # i -= 1

        if float(temperature) > 15:
            temperature = 15
        elif float(temperature) < 5:
            temperature = 5

        if float(pH) > 14:
            pH = 14
        elif float(pH) < 0:
            pH = 0

        if float(turbidity) > 100:
            turbidity = 100
        elif float(turbidity) < 0:
            turbidity = 0

        return lat, lon, heading, timestamp, pH, turbidity, temperature, speed

def calc_speed():
    #calc speed based off past 2 positions
    with open('telemetrydata.csv', 'r') as f:
        try:
            lines = f.readlines()
            last_line = lines[-1]
            second_last_line = lines[-2]

            lat1, lon1, heading1, timestamp1, pH1, turbidity1, temperature1 = last_line.split(',')
            lat2, lon2, heading2, timestamp2, pH2, turbidity2, temperature2 = second_last_line.split(',')

            #calc distance
            R = 6371e3
            lat1 = float(lat1) * (3.14159 / 180)
            lat2 = float(lat2) * (3.14159 / 180)
            lon1 = float(lon1) * (3.14159 / 180)
            lon2 = float(lon2) * (3.14159 / 180)

            dlat = lat2 - lat1
            dlon = lon2 - lon1

            a = (dlat / 2) * (dlat / 2) + lat1 * lat2 * (dlon / 2) * (dlon / 2)
            c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
            d = R * c

            #calc time
            t1 = int(timestamp1) #unix time
            t2 = int(timestamp2)

            dt = t2 - t1

            speed = d / dt

            speed = abs(round(speed, 2))

        except:
            speed = 0

        return speed #m/s



if __name__ == '__main__':
    threading.Thread(target=data_ingest).start()
    threading.Thread(target=queue_worker).start()

    linenumber = 0

    @app.get("/")
    async def read_root():
        return {"Hello": "World"}

    @app.get("/telemetry")
    async def read_telemetry():
        global linenumber
        lat, lon, heading, timestamp, pH, turbidity, temperature, speed = get_latest_data(linenumber)
        linenumber += 1
        return {"lat": lat, "lon": lon, "heading": heading, "timestamp": timestamp, "pH": pH, "turbidity": turbidity, "temperature": temperature, "speed": speed}

    @app.get("/waypoints")
    async def read_waypoints():
        return waypoints


    #/addwaypoint?lat=123&lon=123
    @app.get("/addwaypoint")
    async def add_waypoint(lat: float, lon: float):
        waypoints.append([lat, lon])
        return {"status": "success"}
        

    uvicorn.run(app, host="localhost", port=8000)

