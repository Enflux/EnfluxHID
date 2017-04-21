/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */
#pragma once

#include "EnfluxTypes.h"
#include "stdint.h"

// Version 1 : Define recording playback format
// Version 2 : Increase maximum number of frames. Change storage format of Base Shirt/Pants Orientations.
#define RECORDING_HEADER_VERSION 2
// Version 1 : Roll Pitch Yaw packets
// Version 2 : Use quaternions instead of Euler angle packets
#define RECORDING_FRAME_VERSION 2

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} enfl_recording_vector3;


#pragma pack (push)
#pragma pack (1)
// Recorded data header information
typedef struct
{
    //The first 2 fields should never be changed or it will break version checking.
    // Version of the header structure
    int16_t header_version;
    // Version of the frame structure
    int16_t frame_version;
    // number of shirt frames recorded
    uint64_t shirt_frames;
    // number of pants frames recorded
    uint64_t pants_frames;
    // duration of the recording in milliseconds
    uint32_t duration;
    // Orientation used in the viewer as the base initial heading of the shirt
    enfl_recording_vector3 shirt_base_orientation;
    // Orientation used in the viewer as the base initial heading of the pants
    enfl_recording_vector3 pants_base_orientation;
} enfl_recording_header;
#pragma pack (pop)

// Format of each frame in a recording file
typedef union
{
#pragma pack (push)
#pragma pack (1)
    struct
    {
        // Number of milliseconds since the start of the recording
        uint32_t timestamp;
        // Device used to record these frame rotations
        uint8_t device_type;
        // Rotation data for each sensor in this frame
        enfl_rotations frame;
    };
#pragma pack (pop)
} enfl_recording_frame;
