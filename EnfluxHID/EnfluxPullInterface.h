/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */
#pragma once

#include "EnfluxTypes.h"


#ifdef WIN32
#include <Windows.h>

#endif
#define THREADFUNCDLL_API __declspec(dllexport)
#ifdef __cplusplus
extern "C"
{
#endif

// Pull SDK :
// This API controls the suit for most types of applications.
// An application is expected to start a streaming loop that polls the device for each update.
// For the lifetime of the stream, the app should pop commands and load rotations in a polling loop.
// The applications must end the streaming thread to put the device back to sleep.

THREADFUNCDLL_API void StartIdlePull(devices device);

// Initializes the device in streaming mode.  Status messages will start to be queued.
// Calling this after the first time will reinitialize the devices.
THREADFUNCDLL_API void StartStreamingPull(devices device);

// Prompts the device to begin calibration.  Return value will be sent via a command.
// When complete, PopCommand will return CALIBRATION_COMPLETE or ERROR_CALIBRATION_FAILED.
THREADFUNCDLL_API void StartCalibrationPull(devices device);

// Checks for any unread status messages.
THREADFUNCDLL_API int HasNewCommand(devices device);

// Pops the latest status messages. It is recommended to handle the following accordingly:
// This should be polled while the device is streaming.
// DEVICE_CONNECTED : Will be called on StartStreamingThread if the device is connected. Start polling.
// DEVICE_DISCONNECTED : Will be called on StartStreamingThread if there is no device or on abrupt disconnect.
// RESET_ORIENTATION : Adjust the user's avatar to the baseline ex. standing straight.
// CALIBRATION_COMPLETE : Allow streaming.
// ERROR_NO_CALIBRATION : Prompt calibration.
// ERROR_CALIBRATION_FAILED : Restart calibration.
THREADFUNCDLL_API int PopCommand(devices device);

// Loads the latest packed orientation values into a buffer. outData must be 5 elements long.
// Returns the number of sensors written to outData.
// This should be polled while the device is streaming.
// The rotation data should be parsed by using the function UnpackQuaternion.
THREADFUNCDLL_API int LoadRotations(devices device, enfl_packed_quat_t* outData);

// Loads the latest unpacked orientation values into a buffer. out_quats must be 5 elements long.
// Returns the number of sensors written to outData.
// This should be polled while the device is streaming.
THREADFUNCDLL_API int LoadRotationsUnpacked(devices device, enfl_quat_t* out_quats);

// Stops the device from streaming data and puts it to sleep.
// This is necessary for the device to hold its battery after the application completes.
THREADFUNCDLL_API int EndStreamingThread();

#ifdef __cplusplus
}
#endif
