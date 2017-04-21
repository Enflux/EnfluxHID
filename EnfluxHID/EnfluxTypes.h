/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */


// Contains the Enflux enumerations and data types

#ifndef BLE_ENFLUX_H__
#define BLE_ENFLUX_H__

#include "stdint.h"

typedef enum
{
    devices_none = 0,
    devices_shirt = 1,
    devices_pants = 2,
    devices_both = 3
} devices;

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
    flag_rsvd7      = 0x40, // Reserved for future use.
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
    // Errors
    ERROR_NO_CALIBRATION = -1,
    ERROR_NO_SHIRTPANTS = -2,
    ERROR_CALIBRATION_FAILED = -3
} enfl_inputs_cmd;

// SDK supplements to Enflux device codes
typedef enum
{
    DEVICE_CONNECTED = 128,
    DEVICE_DISCONNECTED = 129,
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
typedef struct
{
    // Timestamp & 0x0FFF : A timestamp in milliseconds.
    // Timestamp & 0xF000 : The enfl_sensor for this report.
    uint16_t timestamp;
    enfl_vector_t acc;
    enfl_vector_t gyro;
    enfl_vector_t mag;
} enfl_raw_data_t;

// Quaternion rotation.
typedef struct
{
    int8_t w;
    int8_t x;
    int8_t y;
    int8_t z;
} enfl_quat_t;

typedef enfl_quat_t enfl_rotations[ENFL_SENSOR_COUNT];

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

// This should map to most quaternion types with structure {w,x,y,z}
// Or can be unpacked to a 4 value array of real numbers such as double[4]
#define UNPACK_ENFL_QUAT_T(quat) {(1.0 * (quat.w) / 127.5), \
                           (1.0 * (quat.x) / 127.5), \
                           (1.0 * (quat.y) / 127.5), \
                           (1.0 * (quat.z) / 127.5)}


#endif
