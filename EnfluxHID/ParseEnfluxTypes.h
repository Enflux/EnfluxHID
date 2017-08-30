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

// Unpacks our 4 byte packed quaternion into an WXYZ quaternion
enfl_quat_t UnpackQuaternion(enfl_packed_quat_t quat)
{
    enfl_quat_t out_quat;

    // The SDK zeros out the buffer when there is not rotation. Return zeros to indicate this.
    if (quat[0] == 0 &&
        quat[1] == 0 &&
        quat[2] == 0 &&
        (quat[3] & 0x3f) == 0) // sensor index is not counted here
    {
        out_quat = QUATERNION_INVALID;
    }
    else
    {
        // The component which is computed
        uint16_t idx = (quat[3] & 0xc0) >> 6;
        // Find the component values in range [0,1024], then linear transform to [-1,1]
        float given[3] =
        {
            (((uint16_t)quat[0] | ((quat[3] & 0x0003) << 8)) / 511.5f) - 1.0f,
            (((uint16_t)quat[1] | ((quat[3] & 0x000c) << 6)) / 511.5f) - 1.0f,
            (((uint16_t)quat[2] | ((quat[3] & 0x0030) << 4)) / 511.5f) - 1.0f
        };
        // Compute the missing component by creating a normalized quaternion
        float computed = (float)sqrt(1.0 - pow(given[0], 2) - pow(given[1], 2) - pow(given[2], 2));

        // Insert the computed component at index idx as if it were a list.
        out_quat.data[0] = (idx == 0) ? computed : given[0];
        out_quat.data[1] = (idx == 1) ? computed : (idx == 0) ? given[0] : given[1];
        out_quat.data[2] = (idx == 2) ? computed : (idx == 3) ? given[2] : given[1];
        out_quat.data[3] = (idx == 3) ? computed : given[2];
    }
    return out_quat;
}

// Converts packed Enflux quaternion to Euler angles
enfl_vector3f QuatToEuler(enfl_packed_quat_t packed_quat)
{
    enfl_quat_t quat = UnpackQuaternion(packed_quat);

    double w = quat.w;
    double x = quat.x;
    double y = quat.y;
    double z = quat.z;

    float roll = (float)atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    float pitch = (float)asin(2 * (w * y - z * x));
    float yaw = (float)atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    return (enfl_vector3f) { roll, pitch, yaw };
}
