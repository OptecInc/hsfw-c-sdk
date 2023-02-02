# HSFW C SDK

The HSFW C sdk enables cross platform development for the Optec HSFW. This project uses HIDAPI to provide the low level HID communication. This is tested on Windows, Linux (using hidraw or libusb, amd64, armhf and aarch64 are tested) and macOS.

Here is a summary of each folder and several key files.

## hidapi

This is a submodule linked to the main HIDAPI project. A recent version of HIDAPI is required to use the linux hidraw target.

## include

This contains the header file for libhsfw. This should work for most C and C++ projects.

## libusb

This contains a very simple bash script to build the sample application using the libusb backend.

## linux

This contains a very simple bash script to build the sample application using the linux hidraw backend.

## mac

This contains a very simple bash script to build the sample application using the macOS backend.

## src

### hsfw-app.c

This is a very simple example on using the libhsfw to run an HSFW. If you are developing an application using libhsfw this is a good place to start.

### libhsfw.c

The source file for the libhsfw library.

## windows

This contains a sample vsproj project to build the hsfw-app.

## 50-usb-hsfw.rules

A sample rules file to grant access to the HSFW on most linux systems.
