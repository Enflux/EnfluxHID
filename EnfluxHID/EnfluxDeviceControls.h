/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */

// Uses the HID library to expose our reports

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "EnfluxTypes.h"

#ifdef WIN32
#include <Windows.h>
#endif

#define ENFLUX_VID 0x1915
#define ENFLUX_PID 0xEEEE

#define THREADFUNCDLL_API __declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif
// The push interface is defined below.
// For most uses of the SDK, the following operations are sufficient:
// Define callbacks for status and data reports.
// Recomended : Call StartIdleThread to receive devices ready to stream.
// Call StartStreamingThread with the callbacks above.
// Call EndStreamingThread to gracefully end streaming and avoid battery drain.

// The rotation data should be parsed using UnpackQuaternion.
typedef void (CALLBACK *streamingReportCallback)(devices dev, enfl_packed_quat_t[5]);
// The status is defined in enumeration enfl_inputs_cmd.
typedef void (CALLBACK *deviceStatusCallback)(devices dev, int status);

typedef void (CALLBACK *rawdataReportCallback)(devices dev, int sensor, enfl_raw_data_t* data);

typedef struct
{
    deviceStatusCallback deviceStatusCB;
    streamingReportCallback reportCB;
} DeviceCallbacks;


// Starts a thread that starts listening for devices and triggers a callback on every message.
THREADFUNCDLL_API void StartIdleThread(devices device, deviceStatusCallback status_cb);

// Attempt to stream data from the modules specified with the device flag.
THREADFUNCDLL_API void StartStreamingThread(devices device, DeviceCallbacks callbacks);

// Attempt to startteh calibration sequence on the modules specified with the device flag.
THREADFUNCDLL_API void StartCalibrationThread(devices device, DeviceCallbacks callbacks);

// Set all devices to idle mode and closes their threads and handles.
// Failure to call this function at the end of a program can cause battery drain on the module.
THREADFUNCDLL_API int EndStreamingThread();

// The following region is used to stream raw data.
// The sensors will report raw data sequentially.
#pragma region ENFL_RAWDATA

typedef struct
{
    deviceStatusCallback deviceStatusCB;
    rawdataReportCallback rawDataCB;
} RawDataCallbacks;

THREADFUNCDLL_API void StartRawDataThread(devices device, RawDataCallbacks raw_callbacks);
#pragma endregion

typedef struct
{
    devices attampted_devices;
    bool start_stream;
    bool raw_data;
    bool calibration;
} DeviceListenerParams;

// Register callbacks
THREADFUNCDLL_API void RegisterCallbacks(DeviceCallbacks _callbacks);
THREADFUNCDLL_API void RegisterRawCallbacks(RawDataCallbacks _rawcallbacks);

// Configure the device listener with
THREADFUNCDLL_API void StartListenerThread(DeviceListenerParams params, DeviceCallbacks* callbacks, RawDataCallbacks* raw_callbacks);

// Individually tear down device threads and handles.
THREADFUNCDLL_API void Disconnect(devices status);

// Sets the rate at which the device communicates to the client.
THREADFUNCDLL_API void SetInterval(devices dev, uint16_t interval_ms);

// Starts streaming data packets.
THREADFUNCDLL_API int SetFlags(devices device, bool streaming, bool isRawData, bool calibrate);

// Query battery level.
THREADFUNCDLL_API int GetBatteryLevel(devices dev);


#ifdef __cplusplus
}
#endif
