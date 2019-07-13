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

#pragma once

#include <Arduino.h>
#include "utility/MPU9250.h"
#include "utility/quaternionFilters.h"

// abc_t
typedef struct
{
    float A; // pitch
    float B; // roll
    float C; // yaw
} abc_t;

class IMU : public MPU9250
{
public:
    bool readMPU9250();
    abc_t angles;
    abc_t min = {1e6, 2e6, 3e6};
    abc_t max = {-1e6, -2e6, -3e6};
    void getMinAngle();
    void getMaxAngle();
    void resetMinMax();
};
