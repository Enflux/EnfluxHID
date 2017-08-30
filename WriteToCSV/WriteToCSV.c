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

const static char* DEFAULT_FILE_NAME = "D:\\DataStream.csv";

bool unpack = TRUE;

enum
{
    QUATERNION_ANGLES,
    EULER_ANGLES,
    RAW_DATA
}
LOG_TYPE = QUATERNION_ANGLES;

FILE* f;

const float bleScaling = 325.631013566f;

ULONGLONG InitialTicks;


// Choose which device to operate on
devices streaming_device = devices_shirt;
//devices streaming_device = devices_pants;
//devices streaming_device = devices_head;

// Print each input command received by the SDK.
static void CALLBACK HandleStatusCallback(devices dev, int status)
{
    printf("Status device : %d Message : %d\n", dev, status);
}

// Handle a new raw data packet.
static void CALLBACK HandleRawData(devices dev, int sensor, enfl_raw_data_t* raw_data)
{


    int cursor = 0; //Points to the next place in line to write to.
    char strdata[512] = { '\0' };
    if (unpack)
    {
        enfl_engineering_data_t data = ConvertEngineeringUnits(*raw_data,dev == devices_shirt);

        cursor += sprintf_s(strdata + cursor, 20, "%d,", data.garment);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", data.sensor);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", data.timestamp);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.acc.x);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.acc.y);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.acc.z);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.gyro.x);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.gyro.y);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.gyro.z);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.mag.x);
        cursor += sprintf_s(strdata + cursor, 20, "%f,", data.mag.y);
        cursor += sprintf_s(strdata + cursor, 20, "%f\n", data.mag.z);
    }
    else
    {
        cursor += sprintf_s(strdata + cursor, 20, "%d,", dev);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", sensor);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", (raw_data->timestamp & 0x0FFF));
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->acc.x);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->acc.y);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->acc.z);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->gyro.x);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->gyro.y);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->gyro.z);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->mag.x);
        cursor += sprintf_s(strdata + cursor, 20, "%d,", raw_data->mag.y);
        cursor += sprintf_s(strdata + cursor, 20, "%d\n", raw_data->mag.z);
    }
    fwrite(strdata, 1, cursor, f);
}


// Handle a new filtered data packet.
static void CALLBACK HandleFilteredData(devices dev, enfl_rotations data)
{
    // Euler Angles
    size_t cursor = 0; //Points to the next place in line to write to.
    char strdata[512] = { '\0' };

    ULONGLONG ticks = GetTickCount64() - InitialTicks;
    cursor += sprintf_s(strdata + cursor, 16, "%f,", ticks / 1000.f);

    for (int i = 0; i < ENFL_SENSOR_COUNT; ++i)
    {
        if (LOG_TYPE == EULER_ANGLES)
        {
            enfl_vector3f RPY = QuatToEuler(data[i]);
            cursor += sprintf_s(strdata + cursor, 20, "%f,", RPY.x);
            cursor += sprintf_s(strdata + cursor, 20, "%f,", RPY.y);
            cursor += sprintf_s(strdata + cursor, 20, "%f,", RPY.z);
        }
        else
        {
            if (unpack)
            {
                enfl_quat_t unpacked = UnpackQuaternion (data[i]);
                cursor += sprintf_s(strdata + cursor, 20, "%f,", unpacked.w);
                cursor += sprintf_s(strdata + cursor, 20, "%f,", unpacked.x);
                cursor += sprintf_s(strdata + cursor, 20, "%f,", unpacked.y);
                cursor += sprintf_s(strdata + cursor, 20, "%f,", unpacked.z);
            }
            else
            {
                uint8_t* bleData = (uint8_t*)(&data[i]);
                cursor += sprintf_s(strdata + cursor, 20, "%d,", bleData[0]);
                cursor += sprintf_s(strdata + cursor, 20, "%d,", bleData[1]);
                cursor += sprintf_s(strdata + cursor, 20, "%d,", bleData[2]);
                cursor += sprintf_s(strdata + cursor, 20, "%d,", bleData[3]);

            }
        }
    }
    strdata[cursor - 1] = '\n'; // Overwrite comma
    fwrite(strdata, 1, cursor, f);
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

int main(int argc,char** argv)
{
    // Define the connection parameters.
    DeviceListenerParams params;
    params.attampted_devices = streaming_device;
    params.calibration = false;
    params.raw_data = (LOG_TYPE == RAW_DATA);
    params.start_stream = true;

    InitialTicks = GetTickCount();

    char* file_name = (argc >= 2) ? argv[1] : DEFAULT_FILE_NAME;

    // Use the first argument, otherwise hardcoded value.
    errno_t err = fopen_s(&f, file_name, "w");
    if (err != 0)
    {
        printf("Error opening file %s : %d", file_name, err);
        printf("\nPress any key to exit.");
        int ch = getchar();
    }
    else
    {
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
        fclose(f);
    }
    return 0;
}
