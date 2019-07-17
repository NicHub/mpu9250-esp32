/************************************************************
MPU9250_DMP_Quaternion
 Quaternion example for MPU-9250 DMP Arduino Library
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

The MPU-9250's digital motion processor (DMP) can calculate
four unit quaternions, which can be used to represent the
rotation of an object.

This exmaple demonstrates how to configure the DMP to
calculate quaternions, and prints them out to the serial
monitor. It also calculates pitch, roll, and yaw from those
values.

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
*************************************************************/
#include <Arduino.h>
#include <M5Stack.h>
#include <SparkFunMPU9250-DMP.h>

#include <WebServerApp.h>
#include <M5Stack.h>

AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncWebServer server(80);
AsyncEventSource events("/events");

#define SerialPort Serial

MPU9250_DMP imu;

typedef struct
{
    double qw, qx, qy, qz;
} quaternion_t;

typedef struct
{
    double eA, eB, eC;
} euler_angles_t;

/**
 *
 */
euler_angles_t quatToEulerWikipedia(quaternion_t q)
{
    euler_angles_t angles;

    // angle A, roll (x-axis rotation)
    double sinr_cosp = +2.0 * (q.qw * q.qx + q.qy * q.qz);
    double cosr_cosp = +1.0 - 2.0 * (q.qx * q.qx + q.qy * q.qy);
    angles.eA = atan2(sinr_cosp, cosr_cosp);

    // angle B, pitch (y-axis rotation)
    double sinp = +2.0 * (q.qw * q.qy - q.qz * q.qx);
    if (fabs(sinp) >= 1)
        angles.eB = copysign(HALF_PI, sinp); // use 90 degrees if out of range
    else
        angles.eB = asin(sinp);

    // angle C, yaw (z-axis rotation)
    double siny_cosp = +2.0 * (q.qw * q.qz + q.qx * q.qy);
    double cosy_cosp = +1.0 - 2.0 * (q.qy * q.qy + q.qz * q.qz);
    angles.eC = atan2(siny_cosp, cosy_cosp);

    angles.eA *= RAD_TO_DEG;
    angles.eB *= RAD_TO_DEG;
    angles.eC *= RAD_TO_DEG;

    return angles;
}

/**
 * Source:
 * http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
 *
 * airplane        telescope  symbol     angular    velocity
 * applied first   heading    azimuth    θ (theta)  yaw
 * applied second  attitude   elevation  φ (phi)    pitch
 * applied last    bank       tilt       ψ (psi)    roll
 */
euler_angles_t quatToEulerEuclideanspace(quaternion_t q1)
{
    euler_angles_t angles;
    double test = q1.qx * q1.qy + q1.qz * q1.qw;
    if (test > 0.499)
    { // singularity at north pole
        angles.eC = 2 * atan2(q1.qx, q1.qw);
        angles.eB = HALF_PI;
        angles.eA = 0;
        return angles;
    }
    if (test < -0.499)
    { // singularity at south pole
        angles.eC = -2 * atan2(q1.qx, q1.qw);
        angles.eB = -HALF_PI;
        angles.eA = 0;
        return angles;
    }
    double sqx = q1.qx * q1.qx;
    double sqy = q1.qy * q1.qy;
    double sqz = q1.qz * q1.qz;
    angles.eC = atan2(2 * q1.qy * q1.qw - 2 * q1.qx * q1.qz, 1 - 2 * sqy - 2 * sqz);
    angles.eB = asin(2 * test);
    angles.eA = atan2(2 * q1.qx * q1.qw - 2 * q1.qy * q1.qz, 1 - 2 * sqx - 2 * sqz);
    return angles;
}

/**
 *
 */
void printIMUData(void)
{
    // After calling dmpUpdateFifo() the ax, gx, mx, etc. values
    // are all updated.
    // Quaternion values are, by default, stored in Q30 long
    // format. calcQuat turns them into a float between -1 and 1
    quaternion_t quat = {
        imu.calcQuat(imu.qw),
        imu.calcQuat(imu.qx),
        imu.calcQuat(imu.qy),
        imu.calcQuat(imu.qz)};

    SerialPort.println("Q: " + String(quat.qw, 4) + ", " +
                       String(quat.qx, 4) + ", " + String(quat.qy, 4) +
                       ", " + String(quat.qz, 4));
    SerialPort.println("R/P/Y: " + String(imu.roll) + ", " + String(imu.pitch) + ", " + String(imu.yaw));
    SerialPort.println("Time: " + String(imu.time) + " ms");
    SerialPort.println();

    // Send JSON
    if (!ws.enabled())
        return;

#define ALGO EUCLIDEANSPACE
#if ALGO == SPARKFUN
    euler_angles_t eulerAngles = {imu.roll, imu.pitch, imu.yaw};
#elif ALGO == WIKIPEDIA
    euler_angles_t eulerAngles = quatToEulerWikipedia(quat);
#elif ALGO == EUCLIDEANSPACE
    euler_angles_t eulerAngles = quatToEulerEuclideanspace(quat);
#endif

    static char jsonMsg[200];
    const char *formatString =
        R"rawText({"quat":{"qw":%f,"qx":%f,"qy":%f,"qz":%f},
 "euler":{"eA":%f,"eB":%f,"eC":%f}})rawText";
    sprintf(jsonMsg,
            formatString,
            quat.qw, quat.qx, quat.qy, quat.qz,
            eulerAngles.eA, eulerAngles.eB, eulerAngles.eC);
    ws.textAll(jsonMsg);
}

/**
 *
 */
void setupSerial()
{
    Serial.begin(BAUD_RATE);
    Serial.print("\n\n##########################");
    Serial.print("\nCOMPILATION DATE AND TIME:\n");
    Serial.print(__DATE__);
    Serial.print("\n");
    Serial.print(__TIME__);
    Serial.print("\n##########################\n\n");
}

/**
 *
 */
void setupIMU()
{
    // Call imu.begin() to verify communication and initialize
    if (imu.begin() != INV_SUCCESS)
    {
        SerialPort.println("Unable to communicate with MPU-9250");
        SerialPort.println("Check connections, and try again.");
        SerialPort.println();
        while (1)
        {
            yield();
        }
    }

    imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT |  // Enable 6-axis quat
                     DMP_FEATURE_GYRO_CAL, // Use gyro calibration
                 10);                      // Set DMP FIFO rate to 10 Hz
                                           // DMP_FEATURE_LP_QUAT can also be used. It uses the
                                           // accelerometer in low-power mode to estimate quat's.
                                           // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive
}

/**
 *
 */
unsigned short readIMU()
{
    unsigned short fifoAvailable = imu.fifoAvailable();

    // Check for new data in the FIFO
    if (fifoAvailable)
    {
        // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
        if (imu.dmpUpdateFifo() == INV_SUCCESS)
        {
            // computeEulerAngles can be used -- after updating the
            // quaternion values -- to estimate roll, pitch, and yaw
            imu.computeEulerAngles();
            printIMUData();
        }
    }

    return fifoAvailable;
}

/**
 *
 */
void setupM5Stack()
{
#ifdef _M5STACK_H_
    M5.begin(true, false, true, true);

    // Display compilation date and time on M5STACK.
    M5.Lcd.setTextSize(3);
    M5.Lcd.fillScreen(BLACK);

    M5.Lcd.setTextColor(BLUE, BLACK);
    M5.Lcd.setCursor(20, 10);
    M5.Lcd.print("COMPILATION");

    M5.Lcd.setCursor(20, 40);
    M5.Lcd.print("DATE AND TIME");

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(20, 100);
    M5.Lcd.print(__DATE__);

    M5.Lcd.setCursor(20, 130);
    M5.Lcd.print(__TIME__);
#endif
}

/**
 *
 */
void setup()
{
    setupM5Stack();
    setupSerial();
    scanNetwork();
    setupWebServer();
    setupIMU();
}

/**
 *
 */
void loop()
{
    ArduinoOTA.handle();
    readIMU();
    delay(10);
}
