/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */

// Use the pull interface to handle input commands when they are received by the PC.

#include "stdio.h"
#include "EnfluxDeviceControls.h"
#include "math.h"

// These callbacks are handled on the Windows thread.
// Move any expensive handling operations to another thread.

// Print each input command received by the SDK.
static void CALLBACK HandleStatusCallback(devices dev, int status)
{
    printf("Status device : %d Message : %d\n", dev, status);
}

// Handle a new raw data packet.
static void CALLBACK HandleRawData(devices dev, int sensor, enfl_raw_data_t* data)
{
    if (sensor == S_CENTER)
    {
        printf("Center Gyro Y Value: %d\n", data->gyro.y);
    }
}

// Handle a new filtered data packet.
static void CALLBACK HandleFilteredData(devices dev, enfl_quat_t* data)
{
    // Convert from quaternion format to roll/pitch/yaw
    double w = (1.0 * ((int8_t)(data[S_CENTER].w)) / 127.5);
    double x = (1.0 * ((int8_t)(data[S_CENTER].x)) / 127.5);
    double y = (1.0 * ((int8_t)(data[S_CENTER].y)) / 127.5);
    double z = (1.0 * ((int8_t)(data[S_CENTER].z)) / 127.5);

    double roll = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    double pitch = asin(2 * (w * y - z * x));
    double yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    printf("Center Orientation Roll %f \n", roll);
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