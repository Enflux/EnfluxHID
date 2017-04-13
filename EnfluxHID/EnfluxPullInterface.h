/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */
#pragma once

#include "EnfluxTypes.h"

#define THREADFUNCDLL_API __declspec(dllexport)

// Pull SDK :
// This API controls the suit for most types of applications.
// An application is expected to start a streaming loop that polls the device for each update.
// For the lifetime of the stream, the app should pop commands and load rotations in a polling loop.
// The applications must end the streaming thread to put the device back to sleep.

// Initializes the device in streaming mode.  Status messages will start to be queued.
// Calling this after the first time will reinitialize the devices.
THREADFUNCDLL_API void StartStreamingPull(devices device);

// Prompts the device to begin calibration.  Return value will be sent via a command.
// When complete, PopCommand will return CALIBRATION_COMPLETE or ERROR_CALIBRATION_FAILED.
THREADFUNCDLL_API void StartCalibrationPull(devices device);

// Checks for any unread status mesages.
THREADFUNCDLL_API int HasNewCommand(devices device);

// Pops the latest status messages. It is reccomended to handle the following accordingly:
// This should be polled while the devuice is streaming.
// DEVICE_CONNECTED : Will be called on StartStreamingThread if the devcice is connected. Start polling.
// DEVICE_DISCONNECTED : Will be called on StartStreamingThread if there is no device or on abrupt disconnect.
// RESET_ORIENTATION : Adjust the user's avatar to the baseline ex. standing straight.
// CALIBRATION_COMPLETE : Allow streaming.
// ERROR_NO_CALIBRATION : Prompt calibration.
// ERROR_CALIBRATION_FAILED : Restart calibration.
THREADFUNCDLL_API int PopCommand(devices device);

// Loads the latest rotation values into a buffer. outData must be 5 elements long.
// This should be polled while the devuice is streaming.
THREADFUNCDLL_API void LoadRotations(devices device, enfl_quat_t* outData);

// Stops the device from streaming data and puts it to sleep.
// This is necessary for the device to hold its battery after the application completes.
THREADFUNCDLL_API int EndStreamingThread();

// Records device rotations to a file.
// An existing file will be overwritten or a new file will be created at filename.
// The file format is stored as a series of repeating frames of type enflux_recording_frame
THREADFUNCDLL_API int StartRecording(char* filename);

// Ends recording. The file can now be opened for playback.
THREADFUNCDLL_API int EndRecording();
