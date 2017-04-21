/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */

// Uses the HID library to expose our reports

#pragma once

#define ENFLUX_VID 0x1915
#define ENFLUX_PID 0xEEEE

#include <stdint.h>
#include <stdbool.h>

#include "EnfluxTypes.h"

#include "EnfluxPullInterface.h"

#ifdef WIN32
#include <Windows.h>
#endif

// The push interface is defined below.
// For most uses of the SDK, just the following is sufficient:
// A callback to data streams
// Passing the callback to StartStreamingThread
// It is necessary for the app to call EndStreamingThread or the device will waste battery.

// The rotation data should be parsed by dividing the signed bytes by 127.5; See UNPACK_ENFL_QUAT_T.
typedef void (CALLBACK *streamingReportCallback)(devices dev, enfl_quat_t[5]);
// The status is defined in enumeration enfl_inputs_cmd.
typedef void (CALLBACK *deviceStatusCallback)(devices dev, int status);

typedef void (CALLBACK *rawdataReportCallback)(devices dev, int sensor, enfl_raw_data_t* data);
typedef struct
{
    deviceStatusCallback deviceStatusCB;
    streamingReportCallback reportCB;
} DeviceCallbacks;

// Starts a thread that starts the suit and pings a callback on every message.
THREADFUNCDLL_API void StartStreamingThread(devices device, DeviceCallbacks callbacks);

THREADFUNCDLL_API void StartCalibrationThread(devices device, DeviceCallbacks callbacks);

// Stops the device from streaming data.
THREADFUNCDLL_API int EndStreamingThread();

// The following region is used to stream raw data.
// The sensors will stream back raw sensor values.
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

// Ends a connection to the suit.
THREADFUNCDLL_API void Disconnect(devices status);

// Sets the rate at which the device communicates to the client.
THREADFUNCDLL_API void SetInterval(devices dev, uint16_t interval_ms);

// Starts streaming data packets.
THREADFUNCDLL_API int SetFlags(devices device, bool streaming, bool isRawData, bool calibrate);

THREADFUNCDLL_API int StartRecording(char* filename);

THREADFUNCDLL_API int EndRecording();
