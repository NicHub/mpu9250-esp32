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
#include <uart.h>

void UART::begin()
{
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
}

void UART::printWhoAmI()
{
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
}

void UART::printMagCalibration()
{
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

void UART::printSerialDebug()
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
