/* Copyright (c) 2017 Enflux Inc.
 * By downloading, accessing or using this SDK, you signify that you have read,
 * understood and agree to the terms and conditions of the End User License Agreement
 * located at: https://www.getenflux.com/pages/sdk-eula
 */
#pragma once

#include "EnfluxRecordingTypes.h"
#ifdef WIN32
#include <Windows.h>
#endif

#define THREADFUNCDLL_API __declspec(dllexport)

typedef enum
{
    REC_SUCCESS = 0,
    REC_FILE_NOT_OPEN,
    REC_FILE_OPEN_ERROR,
    REC_FILE_WRITE_ERROR,
    REC_FILE_CLOSE_ERROR,
    REC_FILE_DOES_NOT_EXIST,
    REC_FILE_LENGTH_ERROR,
    REC_FILE_NAME_EMPTY,
    REC_FILE_ALREADY_OPEN,
} recording_error_t;


recording_error_t CALLBACK RecordFrame(devices device, byte* rotation_data);

THREADFUNCDLL_API recording_error_t StartRecording(char* filename);

THREADFUNCDLL_API recording_error_t EndRecording();

THREADFUNCDLL_API recording_error_t SetShirtBaseOrientation(enfl_vector_t orientation);

THREADFUNCDLL_API recording_error_t SetPantsBaseOrientation(enfl_vector_t orientation);
