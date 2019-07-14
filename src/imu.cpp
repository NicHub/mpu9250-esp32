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

#include <imu.h>

bool IMU::readMPU9250()
{
    // If intPin goes high, all data registers have new data
    // On interrupt, check if data ready interrupt
    if (this->readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
    {
        this->readAccelData(this->accelCount); // Read the x/y/z adc values
        this->getAres();

        // Now we'll calculate the accleration value into actual g's
        // This depends on scale being set
        this->ax = (float)this->accelCount[0] * this->aRes; // - accelBias[0];
        this->ay = (float)this->accelCount[1] * this->aRes; // - accelBias[1];
        this->az = (float)this->accelCount[2] * this->aRes; // - accelBias[2];

        this->readGyroData(this->gyroCount); // Read the x/y/z adc values
        this->getGres();

        // Calculate the gyro value into actual degrees per second
        // This depends on scale being set
        this->gx = (float)this->gyroCount[0] * this->gRes;
        this->gy = (float)this->gyroCount[1] * this->gRes;
        this->gz = (float)this->gyroCount[2] * this->gRes;

        this->readMagData(this->magCount); // Read the x/y/z adc values
        this->getMres();
        // User environmental x-axis correction in milliGauss, should be
        // automatically calculated
        this->magbias[0] = +470.;
        // User environmental x-axis correction in milliGauss TODO axis??
        this->magbias[1] = +120.;
        // User environmental x-axis correction in milliGauss
        this->magbias[2] = +125.;

        // Calculate the magnetometer values in milliGauss
        // Include factory calibration per data sheet and user environmental
        // corrections
        // Get actual magnetometer value, this depends on scale being set
        this->mx = (float)this->magCount[0] * this->mRes * this->magCalibration[0] -
                   this->magbias[0];
        this->my = (float)this->magCount[1] * this->mRes * this->magCalibration[1] -
                   this->magbias[1];
        this->mz = (float)this->magCount[2] * this->mRes * this->magCalibration[2] -
                   this->magbias[2];
    } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

    // Must be called before updating quaternions!
    this->updateTime();

    // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
    // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
    // (+ up) of accelerometer and gyro! We have to make some allowance for this
    // orientationmismatch in feeding the output to the quaternion filter. For the
    // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
    // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
    // modified to allow any convenient orientation convention. This is ok by
    // aircraft orientation standards! Pass gyro rate as rad/s
    //  MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
    MahonyQuaternionUpdate(this->ax, this->ay, this->az, this->gx * DEG_TO_RAD,
                           this->gy * DEG_TO_RAD, this->gz * DEG_TO_RAD, this->my,
                           this->mx, this->mz, this->deltat);

    // Serial print and/or display at 0.5 s rate independent of data rates
    this->delt_t = millis() - this->count;

    // update LCD independent of read rate
    bool timeToGo = this->delt_t > IMU_REFRESH_MS;
    if (!timeToGo)
        return false;

    // Define output variables from updated quaternion---these are Tait-Bryan
    // angles, commonly used in aircraft orientation. In this coordinate system,
    // the positive z-axis is down toward Earth. Yaw is the angle between Sensor
    // x-axis and Earth magnetic North (or true North if corrected for local
    // declination, looking down on the sensor positive yaw is counterclockwise.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the
    // Earth is positive, up toward the sky is negative. Roll is angle between
    // sensor y-axis and Earth ground plane, y-axis up is positive roll. These
    // arise from the definition of the homogeneous rotation matrix constructed
    // from quaternions. Tait-Bryan angles as well as Euler angles are
    // non-commutative; that is, the get the correct orientation the rotations
    // must be applied in the correct order which for this configuration is yaw,
    // pitch, and then roll.
    // For more see
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // which has additional links.
    this->yaw = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() *
                                                                  *(getQ() + 3)),
                      *getQ() * *getQ() + *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) - *(getQ() + 3) * *(getQ() + 3));
    this->pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() *
                                                                    *(getQ() + 2)));
    this->roll = atan2(2.0f * (*getQ() * *(getQ() + 1) + *(getQ() + 2) *
                                                             *(getQ() + 3)),
                       *getQ() * *getQ() - *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) + *(getQ() + 3) * *(getQ() + 3));
    this->pitch *= RAD_TO_DEG;
    this->yaw *= RAD_TO_DEG;
    // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
    // 	8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
    // - http://www.ngdc.noaa.gov/geomag-web/#declination
    this->yaw -= 8.5;
    this->roll *= RAD_TO_DEG;

    this->angles.A = this->pitch;
    this->angles.B = this->roll;
    this->angles.C = this->yaw;

    return true;
}

void IMU::getMinAngle()
{
    if (this->pitch < this->min.A)        this->min.A = this->pitch;
    if (this->roll < this->min.B)        this->min.B = this->roll;
    if (this->yaw < this->min.C)        this->min.C = this->yaw;
}

void IMU::getMaxAngle()
{
    if (this->pitch > this->max.A)        this->max.A = this->pitch;
    if (this->roll > this->max.B)        this->max.B = this->roll;
    if (this->yaw > this->max.C)        this->max.C = this->yaw;
}

void IMU::resetMinMax()
{
    this->min = {1e6, 2e6, 3e6};
    this->max = {-1e6, -2e6, -3e6};
}

void IMU::resetRateCounter()
{
    this->count = millis();
    this->sumCount = 0;
    this->sum = 0;
}