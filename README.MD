Library and samples for streaming Enflux module data over a PC HID connection.

# Enflux HID Library

The EnfluxHID library provides push and pull interfaces for connecting to the module and streaming data. To operate the modules using the Enflux SDK, first connect the device using the Windows bluetooth manager. After connecting, this library should be able to perform all operations such as setting properties, calibration, and streaming.

## Pull Interface

This is the most likely API to use for game engines and most applications. It is a simple and safe interface.
After starting streaming, calibration, or raw data stream, continually call the poll function to refresh the latest device status and streamed data.
At the end of the application, end the streaming thread.

## Push interface

This API allows the lowest latency for streamed data by using callbacks. The callbacks will need to be handled promptly, or the program will be disrupted. Misuse of this API may result in unexpected behavior.
When starting streaming on a module, pass the callback functions to handle the events.
The callbacks will be hit on new device status and rotations.
At the end of the application, end the streaming thread.