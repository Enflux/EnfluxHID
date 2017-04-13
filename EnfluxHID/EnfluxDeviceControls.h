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
// It is necesary for the app to call EndStreamingThread or the device will waste battery.

// The callback is sent a byte array of ...
typedef void (CALLBACK *streamingReportCallback)(devices dev, enfl_quat_t[5]);
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
// The sensor will cycle once every five input reports, significantly increasing the latency.
#pragma region ENFL_RAWDATA

typedef struct
{
    deviceStatusCallback deviceStatusCB;
    rawdataReportCallback rawDataCB;
} RawDataCallbacks;

THREADFUNCDLL_API void StartRawDataThread(devices device, RawDataCallbacks raw_callbacks);
#pragma endregion

// The following functions are useful for a pull interface and advanced SDK usage.
#pragma region ENFL_ADVANCED_SDK

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

#ifdef ADDRESS_REPORT //removed for HID driver error issue
// Returns the MAC address of the device.
THREADFUNCDLL_API int GetAddress(devices dev, uint8_t* buffer);
#endif

#ifdef OUTPUT_CALIBRATION
THREADFUNCDLL_API int SetCalibration(devices dev, enfl_calibration_t calibrations[]);
#endif

#pragma endregion