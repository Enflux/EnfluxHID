/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */

// Use the pull interface to handle input commands when they are received by the PC.

#include "stdio.h"
#include "ParseEnfluxTypes.h"
#include "EnfluxDeviceControls.h"
#include "math.h"

// These callbacks are handled on the Windows thread.
// Move any expensive handling operations to another thread.

// Quaternion with component values in range [-1,1]
struct quaternion
{
    double w, x, y, z;
};

// Print each input command received by the SDK.
static void CALLBACK HandleStatusCallback(devices dev, int status)
{
    printf("Status device : %d Message : %d\n", dev, status);
}

// Handle a new raw data packet.
static void CALLBACK HandleRawData(devices dev, int sensor, enfl_raw_data_t* raw_data)
{
    enfl_engineering_data_t data = ConvertEngineeringUnits(*raw_data,dev == devices_shirt);
    if (sensor == S_CENTER)
    {
        printf("Center Gyro X Value: %f\n", data.gyro.x);
    }
}

// Handle a new filtered data packet.
static void CALLBACK HandleFilteredData(devices dev, enfl_rotations data)
{
    struct quaternion center_rotation = UNPACK_ENFL_QUAT_T(data[S_CENTER]);;

    // Convert from quaternion format to roll/pitch/yaw
    double w = center_rotation.w;
    double x = center_rotation.x;
    double y = center_rotation.y;
    double z = center_rotation.z;

    double roll = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    double pitch = asin(2 * (w * y - z * x));
    double yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    enfl_vector3f center_euler = QuatToEuler(data[S_CENTER]);

    printf("Center Orientation Roll %f \n", center_euler.x);
}


RawDataCallbacks raw_callbacks =
{
    HandleStatusCallback,
    HandleRawData
};

DeviceCallbacks filtered_callbacks =
{
    HandleStatusCallback,
    HandleFilteredData
};

int main()
{
    // Define the connection parameters.
    DeviceListenerParams params;
    params.attampted_devices = devices_shirt;
    params.calibration = false;
    params.raw_data = false;
    params.start_stream = true;

    if (params.raw_data)
    {
        StartListenerThread(params, NULL, &raw_callbacks);
    }
    else
    {
        StartListenerThread(params, &filtered_callbacks, NULL);
    }

    printf("Streaming Beginning. Press any key to terminate.\n");

    int ch = getchar();
    EndStreamingThread();
    return 0;
}
