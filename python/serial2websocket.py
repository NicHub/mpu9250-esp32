#!/usr/bin/env python
# https://websockets.readthedocs.io/en/stable/intro.html
# WS server that sends messages at random intervals


# JavaScript
# var ws = new WebSocket("ws://127.0.0.1:5678/");

import asyncio
import datetime
import random
import websockets
import serial
from time import sleep

port = "/dev/tty.SLAB_USBtoUART"
baud = 230400
ser = serial.Serial(port, baud, timeout=0)
sleep_time = 1 / 20 / 2 # 1/2 1/fifoRate in main.cpp
print("START")

async def time(websocket, path):
    quat = ""
    euler = ""
    json = ""
    while True:

        # Serial
        try:
            line = ser.readline().decode("ascii")
        except:
            pass

        if len(line) > 0:
            if(line[2:5] == "qua"):
                quat = line
            elif(line[2:5] == "eul"):
                euler = line
                json = quat + euler
                # print(json)

                # Websocket
                await websocket.send(json)
                # await asyncio.sleep(sleep_time)
                sleep(sleep_time)

start_server = websockets.serve(time, "127.0.0.1", 5678)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
