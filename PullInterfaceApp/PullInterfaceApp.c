/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */

// PullInterfaceApp.cpp : Uses the Enflux pull interface to control an Enflux device
//

#include "stdio.h"

#include "ParseEnfluxTypes.h"
#include "EnfluxPullInterface.h"
#include "Recording.h"

// Change this to a valid file name on your PC
char* recording_file_name = "D:\\Recording.enfl";

// Choose which device to operate on
devices streaming_device = devices_shirt;
//devices streaming_device = devices_pants;
//devices streaming_device = devices_head;


int main()
{
    devices devs = streaming_device;
    // Starts streaming the filtered data for the shirt.
    StartStreamingPull(devs);
    enfl_quat_t rotations[ENFL_SENSOR_COUNT];
    int quit = 0;
    while (!quit)
    {
        // Input a one character command for each operation.
        // Press enter to send the command.
        int ch = getchar();
        switch (ch)
        {
        case 'q': //quit
            quit = 1;
            break;
        case 'r': // Read input commands in the order they are received.
            if (HasNewCommand(streaming_device))
            {
                int command = PopCommand(streaming_device);
                printf("Status received: %d\n", command);
            }
            else
            {
                printf("No new status\n");
            }
            break;
        case 'p': // Pull the latest orientations

            // Prints the quaternion as {W,X,Y,Z}

            LoadRotationsUnpacked(streaming_device, rotations);
            // Prints out the quaternion in the format {w,x,y,z}
            printf("New Orientation. Sensor 0 is {%.02f %.02f %.02f %.02f}\n",
                rotations[0].w, rotations[0].x, rotations[0].y, rotations[0].z);
            break;
        case 'c': // Start calibration
            StartCalibrationPull(devs);
            break;
        case 'e': // End streaming
            StartIdlePull(devs);
            break;
        case 's': // start streaming
            StartStreamingPull(devs);
            break;
        case 'j': // Begin recording
            {
                int rec_result = StartRecording(recording_file_name);
                if (rec_result != REC_SUCCESS)
                {
                    printf("Recording Error: %d\n", rec_result);
                }
            }
            break;
        case 'k': // End recording
            {
                int end_rec_result = EndRecording();
                if (end_rec_result != REC_SUCCESS)
                {
                    printf("End recording Error: %d\n", end_rec_result);
                }
            }
            break;
        case 'b':
            {
                // Requires Firmware V126
                int pct = GetBatteryLevel(streaming_device);
                printf("battery pct: %d\n", pct);

            }
            break;
        }
    }
    // Finish the app by ending streaming, otherwise the device will continue to stream data to the PC.
    EndStreamingThread();
    return 0;
}