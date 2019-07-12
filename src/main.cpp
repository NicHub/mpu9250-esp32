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
#include <lcd.h>

#define SerialDebug true // Set to true to get Serial output for debugging

IMU IMU1;
LCDscreen LCD1;
// Kalman kalmanX, kalmanY, kalmanZ; // Create the Kalman instances

void setup()
{
    M5.begin();
    Wire.begin();

#ifdef LCD
    LCD1.setup1();
#endif

    // Read the WHO_AM_I register, this is a good test of communication
    byte c = IMU1.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    Serial.print("MPU9250 ");
    Serial.print("I AM ");
    Serial.print(c, HEX);
    Serial.print(" I should be ");
    Serial.println(0x71, HEX);



        Serial.println("MPU9250 is online...");

        // Start by performing self test and reporting values
        IMU1.MPU9250SelfTest(IMU1.SelfTest);
        Serial.print("x-axis self test: acceleration trim within : ");
        Serial.print(IMU1.SelfTest[0], 1);
        Serial.println("% of factory value");
        Serial.print("y-axis self test: acceleration trim within : ");
        Serial.print(IMU1.SelfTest[1], 1);
        Serial.println("% of factory value");
        Serial.print("z-axis self test: acceleration trim within : ");
        Serial.print(IMU1.SelfTest[2], 1);
        Serial.println("% of factory value");
        Serial.print("x-axis self test: gyration trim within : ");
        Serial.print(IMU1.SelfTest[3], 1);
        Serial.println("% of factory value");
        Serial.print("y-axis self test: gyration trim within : ");
        Serial.print(IMU1.SelfTest[4], 1);
        Serial.println("% of factory value");
        Serial.print("z-axis self test: gyration trim within : ");
        Serial.print(IMU1.SelfTest[5], 1);
        Serial.println("% of factory value");

        // Calibrate gyro and accelerometers, load biases in bias registers
        IMU1.calibrateMPU9250(IMU1.gyroBias, IMU1.accelBias);

        IMU1.initMPU9250();
        // Initialize device for active mode read of acclerometer, gyroscope, and
        // temperature
        Serial.println("MPU9250 initialized for active data mode....");

        // Read the WHO_AM_I register of the magnetometer, this is a good test of
        // communication
        byte d = IMU1.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
        Serial.print("AK8963 ");
        Serial.print("I AM ");
        Serial.print(d, HEX);
        Serial.print(" I should be ");
        Serial.println(0x48, HEX);

        // Get magnetometer calibration from AK8963 ROM
        IMU1.initAK8963(IMU1.magCalibration);
        // Initialize device for active mode read of magnetometer
        Serial.println("AK8963 initialized for active data mode....");
        if (Serial)
        {
            //  Serial.println("Calibration values: ");
            Serial.print("X-Axis sensitivity adjustment value ");
            Serial.println(IMU1.magCalibration[0], 2);
            Serial.print("Y-Axis sensitivity adjustment value ");
            Serial.println(IMU1.magCalibration[1], 2);
            Serial.print("Z-Axis sensitivity adjustment value ");
            Serial.println(IMU1.magCalibration[2], 2);
        }

}

void printSerialDebug()
{
    Serial.print("ax = ");
    Serial.print((int)1000 * IMU1.ax);
    Serial.print(" ay = ");
    Serial.print((int)1000 * IMU1.ay);
    Serial.print(" az = ");
    Serial.print((int)1000 * IMU1.az);
    Serial.println(" mg");

    Serial.print("gx = ");
    Serial.print(IMU1.gx, 2);
    Serial.print(" gy = ");
    Serial.print(IMU1.gy, 2);
    Serial.print(" gz = ");
    Serial.print(IMU1.gz, 2);
    Serial.println(" deg/s");

    Serial.print("mx = ");
    Serial.print((int)IMU1.mx);
    Serial.print(" my = ");
    Serial.print((int)IMU1.my);
    Serial.print(" mz = ");
    Serial.print((int)IMU1.mz);
    Serial.println(" mG");

    Serial.print("q0 = ");
    Serial.print(*getQ());
    Serial.print(" qx = ");
    Serial.print(*(getQ() + 1));
    Serial.print(" qy = ");
    Serial.print(*(getQ() + 2));
    Serial.print(" qz = ");
    Serial.println(*(getQ() + 3));

    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(IMU1.yaw, 2);
    Serial.print(", ");
    Serial.print(IMU1.pitch, 2);
    Serial.print(", ");
    Serial.println(IMU1.roll, 2);

    Serial.print("rate = ");
    Serial.print((float)IMU1.sumCount / IMU1.sum, 2);
    Serial.println(" Hz");
    Serial.println("");
}


void loop()
{
    bool timeToGo = IMU1.readMPU9250();
    if (timeToGo)
    {
        if (SerialDebug)
            printSerialDebug();
#ifdef LCD
        LCD1.printLCD();
#endif
    }
}
