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

#include <M5Stack.h>
#include <imu.h>
#include <uart.h>
#include <lcd.h>

IMU IMU1;
LCDscreen LCD1;
UART UART1;

void setup()
{
    M5.begin(LCD, false, true, true);

    Wire.begin();

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

void loop()
{
    bool timeToGo = IMU1.readMPU9250();
    if (timeToGo)
    {
#if SERIAL_DEBUG == true
        UART1.printSerialDebug();
#endif
#if LCD == true
        LCD1.printLCD();
#endif
    }
}
