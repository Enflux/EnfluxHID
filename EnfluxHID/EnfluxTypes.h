/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */

// Contains the Enflux enumerations and data types

#pragma once

#include "stdint.h"
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

// The total number of module configurations.
#define NUM_ENFLUX_DEVICES 5

// Flags for different module configurations.
typedef enum
{
    devices_none = 0,
    devices_shirt = 1,
    devices_pants = 2,
    devices_left_foot = 4,
    devices_right_foot = 8,
    devices_head = 16,
    devices_both = 3,
    devices_all =
#ifdef FEET_ENABLED
    devices_left_foot | devices_right_foot |
#endif // FEET_ENABLED
    devices_shirt | devices_pants | devices_head
} devices;

inline int device_index(devices dev)
{
    switch (dev)
    {
    case devices_shirt: return 0;
    case devices_pants : return 1;
    case devices_left_foot: return 2;
    case devices_right_foot: return 3;
    case  devices_head: return 4;
    }
    return -1;
}

#ifdef FEET_ENABLED
static_assert(devices_all == (1 << NUM_ENFLUX_DEVICES) - 1,"NUM_ENFLUX_DEVICES and devices_all are out of sync");
#endif // FEET_ENABLED

#ifdef __cplusplus
inline devices operator|(devices a, devices b)
{
    return static_cast<devices>(static_cast<int>(a) | static_cast<int>(b));
}

inline devices& operator |=(devices& a, devices b)
{
    return a = a | b;
}
#endif

// The number of sensors used in each device type;
inline int NumSensors(devices dev)
{
    switch(dev)
    {
    case devices_shirt:
    case devices_pants:
        return 5;
#ifdef FEET_ENABLED
    case devices_left_foot:
    case devices_right_foot:
#endif // FEET_ENABLED
    case devices_head:
        return 1;
    default:
        return 0;
    }
}


typedef enum
{
    FEATURE_REPORT_FLAGS,
    FEATURE_REPORT_COUNT
} feature_report;

const static int FEATURE_REPORT_LEN[FEATURE_REPORT_COUNT] = { 1 };

typedef enum
{
    OUTPUT_REPORT_COMMAND,
    OUTPUT_REPORT_COUNT
} output_report;

const static int OUTPUT_REPORT_LEN[OUTPUT_REPORT_COUNT] = { 1
};

typedef enum
{
    //INPUT_REPORT_COMMAND,
    INPUT_REPORT_FILTERED,
    INPUT_REPORT_COUNT
} input_report;

const static int INPUT_REPORT_LEN[INPUT_REPORT_COUNT] = { 1
#ifdef USE_RAW_INPUT_REPORT
,20
#endif
};

// Device state stored in the module's flags feature report.
typedef enum
{
    flag_streaming  = 0x01, // The device is streaming input reports.
    flag_rawdata    = 0x02, // The reports are raw unfiltered data.
    flag_shirtpants = 0x04, // Device type : 0 = shirt, 1 = pants.
    flag_calibrate  = 0x08, // The device is performing an onboard calibration.
    flag_rsvd5      = 0x10, // Reserved for future use.
    flag_rsvd6      = 0x20, // Reserved for future use.
    flag_headmod    = 0x40, // Reserved for future use.
    flag_changing   = 0x80, // The flag is set but not read.
} enfl_flags;

// Octopus sensor enumeration.
typedef enum
{
    S_CENTER,
    S_L_UPPER,
    S_L_LOWER,
    S_R_UPPER,
    S_R_LOWER,
    ENFL_SENSOR_COUNT
} enfl_sensor;

// Output commands from the client to the module.
typedef enum
{
    SET_AS_SHIRT = 1,
    SET_AS_PANTS,
    LOG_CALIBRATION,
    ERASE_SHIRT_PANTS,
    ERASE_CALIBRATION,
    USE_QUATS = 0xd0,
    USE_RPY = 0xd1
} enfl_outputs_cmd;

// Input status reports from the module to the client.
typedef enum
{
    RESET_ORIENTATION = 1,
    WROTE_CALIBRATION,
    WROTE_SHIRTPANTS,
    CALIBRATION_COMPLETE,
    BEGIN_CALIBRATION,
    // battery incicators
    BATTERY_LEVEL_0 = 0x20,
    BATTERY_LEVEL_1,
    BATTERY_LEVEL_2,
    BATTERY_LEVEL_3,
    BATTERY_LEVEL_4,
    BATTERY_LEVEL_5,
    BATTERY_LEVEL_6,
    BATTERY_LEVEL_7,
    BATTERY_LEVEL_8,
    BATTERY_LEVEL_9,
    BATTERY_LEVEL_10,
    // Errors
    ERROR_NO_CALIBRATION = -1,
    ERROR_NO_SHIRTPANTS = -2,
    ERROR_CALIBRATION_FAILED = -3
} enfl_inputs_cmd;

// SDK supplements to Enflux device codes
typedef enum
{
    DEVICE_BEGAN_STREAMING = 128,
    DEVICE_DISCONNECTED = 129,
    DEVICE_AVAILABLE = 130,
    DEVICE_UNAVAILABLE = 131,
    DEVICE_REQUESTING_CALIBRATION = 132,
    ERROR_DECLINED_TOS = 192,
    ERROR_CALIBRATION_DISCONNECTED = 193

} sdk_inputs_cmd;

// Vector unit used in raw data packets
typedef union
{
    struct
    {
        int16_t z;
        int16_t y;
        int16_t x;
    };

    uint8_t data[3];
} enfl_vector_t;

typedef struct
{
    float x;
    float y;
    float z;
} enfl_vector3f;

// A collection of all the raw data received from the sensor with a timestamp in milliseconds.
// See ConvertEngineeringUnits to convert readings to meaningful units.
typedef struct
{
    // Timestamp & 0x0FFF : A timestamp in milliseconds.
    // Timestamp & 0xF000 : The enfl_sensor for this report.
    uint16_t timestamp;
    enfl_vector_t acc;
    enfl_vector_t gyro;
    enfl_vector_t mag;
} enfl_raw_data_t;

// Packaged data sent by the module.
typedef uint8_t enfl_packed_quat_t[4];

// When converting this type, be aware some other programs use {x, y, z, w} order. 
typedef union
{
    float data[4];
    struct
    {
        float w, x, y, z;
    };
} enfl_quat_t;

// Full packet containing the maximum number of compressed orientations.
typedef enfl_packed_quat_t enfl_rotations[ENFL_SENSOR_COUNT];

static enfl_quat_t QUATERNION_IDENTITY = { 1.f,0.f,0.f,0.f };
static enfl_quat_t QUATERNION_INVALID = { NAN,NAN,NAN,NAN };

typedef struct
{
    // Garment of type enum devices. 0 : Shirt, 1 : Pants
    uint16_t garment;
    // Sensor of type enum enfl_sensor
    uint16_t sensor;
    // Rolling timestamp in milliseconds, range [0,4095]
    uint16_t timestamp;
    // meters / second squared
    enfl_vector3f acc;
    // radians / second
    enfl_vector3f gyro;
    // tesla
    enfl_vector3f mag;
} enfl_engineering_data_t;


#ifdef __cplusplus
}
#endif
