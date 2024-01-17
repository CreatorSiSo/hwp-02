# HTWK - Hardware Lab Assignment 2

Four devices are connected to the DMX cable: a moving head in 14-channel mode is located at addresses 9 and 23.
There is a spotlight in 8-bit mode at addresses 1 and 37.
Select a piece of music and program the lighting sequence for the first 30 seconds.

We were allowed to use the Arduino program located in `src/DMX/DmxSimple.h`.

## Project Structure

- `src/main.rs` cli for starting the DMX sequence (via USB) and music at the same time.
- `src/DMX` for Arduino Nano, used for controlling the lights.
- `src/serial` for ESP32-S3 and Arduino Nano, used for testing the serial connection and remote control.
