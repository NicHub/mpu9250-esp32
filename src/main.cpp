/**
 * Copyright (C) 2019  Nicolas Jeanmonod, ouilogique.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <Arduino.h>
#include <imu.h>
#include <WebServerApp.h>
#include <utilities.h>
#ifdef BOARD_M5STACK_CORE_ESP32
#include <M5Stack.h>
#endif

AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncWebServer server(80);
AsyncEventSource events("/events");

IMU imu1;

const unsigned short fifoRate = 20U; // IMU refresh rate in Hz.

/**
 *
 */
void setup()
{
#ifdef BOARD_M5STACK_CORE_ESP32
    setupM5Stack();
#endif
    setupSerial();
    scanNetwork();
    setupWebServer();
    imu1.setupIMU(fifoRate);
}

/**
 *
 */
void loop()
{
    static unsigned long T1 = millis();
    if ((millis() - T1) < (1000 / fifoRate))
        return;
    T1 = millis();

    ArduinoOTA.handle();

    if (!ws.enabled())
    {
        Serial.println("NO WebSocket!");
        return;
    }

    // Read IMU.
    static unsigned short status;
    static char jsonMsg[200];
    status = imu1.readIMU(jsonMsg);
    ws.textAll(jsonMsg);
    if (status != 0)
        Serial.println(jsonMsg);

    Serial.println(jsonMsg);
    return;
    // For debug of lag.
    static uint8_t cpt = 0;
    if ((++cpt % 20) == 0)
    {
        cpt = 0;
        Serial.print("\n");
    }
    Serial.print(".");
}
