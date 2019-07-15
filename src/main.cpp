/**
 * GY-91-test-M5Stack
 */

#include <Arduino.h>

/* MPU9250 Basic Example Code
 by: Kris Winer
 date: April 1, 2014
 license: Beerware - Use this code however you'd like. If you
 find it useful you can buy me a beer some time.
 Modified by Brent Wilkins July 19, 2016

 Demonstrate basic MPU-9250 functionality including parameterizing the register
 addresses, initializing the sensor, getting properly scaled accelerometer,
 gyroscope, and magnetometer data out. Added display functions to allow display
 to on breadboard monitor. Addition of 9 DoF sensor fusion using open source
 Madgwick and Mahony filter algorithms. Sketch runs on the 3.3 V 8 MHz Pro Mini
 and the Teensy 3.1.
 */

#include <WebServerApp.h>
#include <M5Stack.h>
#include <imu.h>
#include <uart.h>
#include <lcd.h>

IMU IMU1;
LCDscreen LCD1;
UART UART1;

AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncWebServer server(80);
AsyncEventSource events("/events");

/**
 * printCompilationDateAndTime
 */
void printCompilationDateAndTime()
{
    Serial.print("###\ncompilation date and time:\n");
    Serial.print(__DATE__);
    Serial.print("\n");
    Serial.print(__TIME__);
    Serial.print("\n###\n\n");
}

/**
 * sendJSON
 */
void sendJSON_IMU()
{
    if (!ws.enabled())
        return;
    static char jsonMsg[200];
    abc_t angles = IMU1.angles;
    const char *formatString =
        R"rawText({"quaternions":{"q0":"%f","q1":"%f","q2":"%f","q3":"%f"},
 "angles":{"A":"%f","B":"%f","C":"%f"}})rawText";

    sprintf(jsonMsg,
            formatString,
            *(getQ() + 0), *(getQ() + 1), *(getQ() + 2), *(getQ() + 3),
            angles.A, angles.B, angles.C);

    ws.textAll(jsonMsg);
}

/**
 *
 */
void setup()
{
    M5.begin(LCD, false, true, true);

    Wire.begin();

    scanNetwork();
    setupWebServer();
    printCompilationDateAndTime();

#if LCD == true
    LCD1.setup1();
#endif

    UART1.begin();

    // Calibrate gyro and accelerometers, load biases in bias registers
    IMU1.calibrateMPU9250(IMU1.gyroBias, IMU1.accelBias);

    IMU1.initMPU9250();

    UART1.printWhoAmI();

    // Get magnetometer calibration from AK8963 ROM
    IMU1.initAK8963(IMU1.magCalibration);

    UART1.printMagCalibration();
}

/**
 *
 */
void loop()
{
    ArduinoOTA.handle();

    bool timeToGo = IMU1.readMPU9250();
    if (!timeToGo)
        return;

#if false
    IMU1.getMaxAngle();
    IMU1.getMinAngle();
#endif

#if SERIAL_DEBUG == true
    UART1.printSerialDebug();
    UART1.printMinMax();
#endif

#if LCD == true
    M5.update();
    // LCD1.printLCD(); // Print values as numbers.
    LCD1.drawBars(); // Print values as bars.

    if (M5.BtnA.wasReleased())
    {
        IMU1.resetMinMax();
        Serial.println("## RESET ##");
        UART1.printMinMax();
        delay(1000);
    }
#endif

    sendJSON_IMU();
    IMU1.resetRateCounter();
}
