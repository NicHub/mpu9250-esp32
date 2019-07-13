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
#include <lcd.h>

void LCDscreen::setup1()
{
    // Start device display with ID of sensor
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK); // Set pixel color; 1 on the monochrome screen
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("MPU9250");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.print("9-DOF 16-bit");
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.print("motion sensor");
    M5.Lcd.setCursor(20, 40);
    M5.Lcd.print("60 ug LSB");
    delay(1000);

    // Set up for data display
    M5.Lcd.setTextSize(1);    // Set text size to normal, 2 is twice normal etc.
    M5.Lcd.fillScreen(BLACK); // clears the screen and buffer
    M5.Lcd.setTextColor(GREEN, BLACK);
}

void LCDscreen::printLCD()
{
    // M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextFont(2);

    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("     x       y       z ");
    M5.Lcd.setCursor(0, 24);
    M5.Lcd.printf("% 6d  % 6d  % 6d     mg   \r\n", (int)(1000 * IMU1.ax), (int)(1000 * IMU1.ay), (int)(1000 * IMU1.az));
    M5.Lcd.setCursor(0, 44);
    M5.Lcd.printf("% 6d  % 6d  % 6d      o/s  \r\n", (int)(IMU1.gx), (int)(IMU1.gy), (int)(IMU1.gz));
    M5.Lcd.setCursor(0, 64);
    M5.Lcd.printf("% 6d  % 6d  % 6d     mG    \r\n", (int)(IMU1.mx), (int)(IMU1.my), (int)(IMU1.mz));

    M5.Lcd.setCursor(0, 100);
    M5.Lcd.printf("  yaw: % 5.2f    pitch: % 5.2f    roll: % 5.2f   \r\n", (IMU1.yaw), (IMU1.pitch), (IMU1.roll));

    // With these settings the filter is updating at a ~145 Hz rate using the
    // Madgwick scheme and >200 Hz using the Mahony scheme even though the
    // display refreshes at only 2 Hz. The filter update rate is determined
    // mostly by the mathematical steps in the respective algorithms, the
    // processor speed (8 MHz for the 3.3V Pro Mini), and the magnetometer ODR:
    // an ODR of 10 Hz for the magnetometer produce the above rates, maximum
    // magnetometer ODR of 100 Hz produces filter update rates of 36 - 145 and
    // ~38 Hz for the Madgwick and Mahony schemes, respectively. This is
    // presumably because the magnetometer read takes longer than the gyro or
    // accelerometer reads. This filter update rate should be fast enough to
    // maintain accurate platform orientation for stabilization control of a
    // fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
    // produced by the on-board Digital Motion Processor of Invensense's MPU6050
    // 6 DoF and MPU9150 9DoF sensors. The 3.3 V 8 MHz Pro Mini is doing pretty
    // well!

    // M5.Lcd.setCursor(0, 60);
    // M5.Lcd.printf("yaw:%6.2f   pitch:%6.2f   roll:%6.2f  ypr \r\n",(IMU1.yaw), (IMU1.pitch), (IMU1.roll));
    M5.Lcd.setCursor(12, 144);
    M5.Lcd.print("rt: ");
    M5.Lcd.print((float)IMU1.sumCount / IMU1.sum, 2);
    M5.Lcd.print(" Hz");
}

void LCDscreen::drawB(int32_t angle, int32_t rectY, uint32_t rectColor)
{
#define BKG_COL BLACK
    if (angle >= 0)
    {
        M5.Lcd.fillRect(0, rectY, 160, 50, BKG_COL);
        M5.Lcd.fillRect(160 + angle, rectY, 160 - angle, 50, BKG_COL);
        M5.Lcd.fillRect(160, rectY, angle, 50, rectColor);
    }
    else
    {
        M5.Lcd.fillRect(160, rectY, 160, 50, BKG_COL);
        M5.Lcd.fillRect(0, rectY, 160 + angle, 50, BKG_COL);
        M5.Lcd.fillRect(160 + angle, rectY, -angle, 50, rectColor);
    }
}

void LCDscreen::drawBars()
{
    abc_t angles = IMU1.angles;
    const float scale = 160.0 / 180.0;
    angles.A = (int32_t)(angles.A * scale);
    angles.B = (int32_t)(angles.B * scale);
    angles.C = (int32_t)(angles.C * scale);

    drawB(angles.A, 10, BLUE);
    drawB(angles.B, 70, GREEN);
    drawB(angles.C, 130, CYAN);
}
