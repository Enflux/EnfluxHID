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

devices streaming_device = devices_shirt;
//devices streaming_device = devices_pants;
//devices streaming_device = devices_head;

// These callbacks are handled on the Windows thread.
// Move any expensive handling operations to another thread.

// Print each input command received by the SDK.
static void CALLBACK HandleStatusCallback(devices dev, int status)
{
    printf("Status device : %d Message : %d\n", dev, status);
}

// Handle a new raw data packet.
static void CALLBACK HandleRawData(devices dev, int sensor, enfl_raw_data_t* raw_data)
{
    enfl_engineering_data_t data = ConvertEngineeringUnits(*raw_data,dev == streaming_device);
    if (sensor == S_CENTER)
    {
        printf("Center Gyro X Value: %f\n", data.gyro.x);
    }
}

// Handle a new filtered data packet.
static void CALLBACK HandleFilteredData(devices dev, enfl_rotations data)
{
    float quat[4];

    for (int i = 0; i < ENFL_SENSOR_COUNT; ++i)
    {
        enfl_vector3f euler_angles = QuatToEuler(data[i]);

        float roll = euler_angles.x;
        float pitch = euler_angles.y;
        float yaw = euler_angles.z;

        printf("Sensor %d Orientation %f %f %f \n", i, roll,pitch,yaw);
    }
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
    params.attampted_devices = streaming_device;
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
