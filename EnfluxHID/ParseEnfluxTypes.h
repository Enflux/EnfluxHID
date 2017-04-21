/* Copyright (c) 2017 Enflux Inc.
* By downloading, accessing or using this SDK, you signify that you have read,
* understood and agree to the terms and conditions of the End User License Agreement
* located at: https://www.getenflux.com/pages/sdk-eula
*/
#pragma once
#include "EnfluxTypes.h"
#include <math.h>

// Accelerometer measured in meters /(seconds ^2).
#define SENSOR_ACC_INCREMENT 0.0024188f
// Gyroscope measured in radians / sec.
#define SENSOR_GYRO_INCREMENT +0.0002864f
// Magnetometer measured in tesla.
#define SENSOR_MAG_INCREMENT 1.220703e-8f

// Converts raw data packets to engineering unit type.
enfl_engineering_data_t ConvertEngineeringUnits(enfl_raw_data_t data, int isShirt) {
    enfl_engineering_data_t dest;
    if (isShirt)
    {
        dest.garment = 0;
    }
    else
    {
        dest.garment = 1;
    }

    dest.sensor = data.timestamp >> 12;
    dest.timestamp = data.timestamp & 0xFFF;

    int isCore = (dest.timestamp >> 12) == S_CENTER;

    //Shirt and Waist Module
    if (isCore) {
        dest.acc.x = -SENSOR_ACC_INCREMENT * data.acc.x;
        dest.acc.y = -SENSOR_ACC_INCREMENT * data.acc.y;
        dest.acc.z = -SENSOR_ACC_INCREMENT * data.acc.z;
        dest.gyro.x = +SENSOR_GYRO_INCREMENT * data.gyro.x;
        dest.gyro.y = +SENSOR_GYRO_INCREMENT * data.gyro.y;
        dest.gyro.z = +SENSOR_GYRO_INCREMENT * data.gyro.z;
        dest.mag.x = +SENSOR_MAG_INCREMENT * data.mag.x;
        dest.mag.y = +SENSOR_MAG_INCREMENT * data.mag.y;
        dest.mag.z = -SENSOR_MAG_INCREMENT * data.mag.z;
    }
    //Shirt Nodes
    if ((!isCore) && (isShirt)) {
        dest.acc.x = -SENSOR_ACC_INCREMENT * data.acc.y;
        dest.acc.y = +SENSOR_ACC_INCREMENT * data.acc.z;
        dest.acc.z = +SENSOR_ACC_INCREMENT * data.acc.x;
        dest.gyro.x = +SENSOR_GYRO_INCREMENT * data.gyro.y;
        dest.gyro.y = -SENSOR_GYRO_INCREMENT * data.gyro.z;
        dest.gyro.z = -SENSOR_GYRO_INCREMENT * data.gyro.x;
        dest.mag.x = +SENSOR_MAG_INCREMENT * data.mag.y;
        dest.mag.y = +SENSOR_MAG_INCREMENT * data.mag.z;
        dest.mag.z = -SENSOR_MAG_INCREMENT * data.mag.x;
    }
    //Pants Nodes
    if ((!isCore) && (!isShirt)) {
        dest.acc.x = -SENSOR_ACC_INCREMENT * data.acc.x;
        dest.acc.y = +SENSOR_ACC_INCREMENT * data.acc.z;
        dest.acc.z = -SENSOR_ACC_INCREMENT * data.acc.y;
        dest.gyro.x = +SENSOR_GYRO_INCREMENT * data.gyro.x;
        dest.gyro.y = -SENSOR_GYRO_INCREMENT * data.gyro.z;
        dest.gyro.z = +SENSOR_GYRO_INCREMENT * data.gyro.y;
        dest.mag.x = +SENSOR_MAG_INCREMENT * data.mag.x;
        dest.mag.y = +SENSOR_MAG_INCREMENT * data.mag.z;
        dest.mag.z = +SENSOR_MAG_INCREMENT * data.mag.y;
    }
    return dest;
}

// Converts packed Enflux quaternion to Euler angles
enfl_vector3f QuatToEuler(enfl_quat_t packed_quat)
{
    double quat[4] = UNPACK_ENFL_QUAT_T(packed_quat);;

    double w = quat[0];
    double x = quat[1];
    double y = quat[2];
    double z = quat[3];

    float roll = (float)atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    float pitch = (float)asin(2 * (w * y - z * x));
    float yaw = (float)atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    return (enfl_vector3f) { roll, pitch, yaw };
}
