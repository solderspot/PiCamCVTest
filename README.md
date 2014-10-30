PiCamCVTest 
===========

This is a modified version of a demo created by Chris Cummings.

Original project is at: http://robotblogging.blogspot.co.uk/2013/10/an-efficient-and-simple-c-api-for.html

Setting up the Pi and Camera for the Test Code
----------------------------------------------

I'm going to detail how to prep the Pi in order to build and run PiCamCVTest.

To make things easier I'm going to start with a fresh Raspbian instance.

First get the Raspbian image from the [downloads page](http://www.raspberrypi.org/downloads).

Refer to the [image installation page](http://www.raspberrypi.org/documentation/installation/installing-images/README.md) on how to write the Raspbian image to an appropriate SD card.

Power up the Pi with the new card.

You should eventually get the config screen. If not then type 'raspi-config' at a command line prompt.

On the config screen do the following:

 * Set a login password.
 * Extend SD partition to use whole disk.
 * Enable the PiCamera hardware.

Re-boot and login as "pi" with the password you just set.

**From the command line perform the following actions:**

Ensure PiCamera support is enabled by checking the folder '/opt/vc' exists and looks something like:

    $ ls /opt/vc
    include lib sbin src

Update all existing packages - this is very important:

    $ sudo apt-get update
    $ sudo apt-get upgrade

And finally we need these packages for PiCamCVTest to build:

    $ sudo apt-get install cmake libopencv-dev

Installing and Building PiCamCVTest
-----------------------------------

It is simplest to just clone PiCamCVTest locally:

    $ git clone https://github.com/solderspot/PiCamCVTest.git

And then build using cmake:

    $ cd PiCamCVTest
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release ..
    $ make

You should now have the executable 'PiCamVCTest' in the build folder.

Using PiCamCVTest
-----------------

Command line options:

    $ ./PiCamCVTest help
    Usage: ./PiCamCVTest [options]
       Where options are:
          -fh              :  flip image horizontally
          -fv              :  flip image vertically
          -w <pixels>      :  capture image width - defailt 320
          -h <pixels>      :  capture image height - defailt 320
          -r               :  don't process input
          -H <min>-<max>   :  hue range (0..360) - defailt 0..60
          -S <min>-<max>   :  saturation range (0..255) - defailt 100..255
          -V <min>-<max>   :  value range (0..255) - defailt 100..255
     example: ./PiCamCVTest -H 10..50 -S 50..255 -V 100..200


[Note that you cannot use arbitrary image dimensions. The camera will round-up any sizes given to the nearest ones it can support. However, the rendering code will not be aware of this and so the image will not display correctly.]

If you run the code without any arguments then it will use the defaults:

    $ ./PiCamCVTest
    Init camera output with 320/320
    Creating pool with 3 buffers of size 409600
    Camera successfully created
    Starting capture: 320x320
    Applying color threshold: hue 0..60, sat 100..255, val 100..255
    Frame rate: 10.46 fps
    Frame rate: 10.15 fps
    Frame rate: 10.15 fps
    Frame rate: 10.15 fps

At regular interval it will output the effective frame rate.

You can use the -r option to disable any OpenCV processing and display the video feed directly. This is so you can verify the camera is working correctly, view the image quality and see if you need to flip the image orientation using the -fh and -fv flags.

Performance
-----------

Running the code on my Pi, which is over clocked at 900Mhz, I get the following results:

|     Image Size      | 160x160 | 320x320 | 640x640 | 
|---------------------|---------|---------|---------|
| no image processing |  40 fps |  30 fps |  15 fps |
| image processing    |  20 fps |  10 fps |   3 fps |
    
    
