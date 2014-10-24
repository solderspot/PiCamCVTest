PiCamObjDetect Demo
===================

This is a modified version of a demo created by Chris Cummings.

Original project is at: http://robotblogging.blogspot.co.uk/2013/10/an-efficient-and-simple-c-api-for.html

To Build Project
================


First make sure cmake is installed:

	$ sudo apt-get install cmake


Install Userland
----------------

From your home directory get the latest userland zip ball

	$ wget https://github.com/raspberrypi/userland/zipball/master

You should now have a zip file called 'master'. Unzip it.
 
	$ unzip master

And you should now have a folder called something like 'raspberrypi-userland-xxxxxx'

We need to move and rename this folder to be '/opt/vc/userland-master':

	$ sudo mv raspberrypi-userland-4333d6d /opt/vc/userland-master

Now we build it:

	$ cd /opt/vc/userland-master/
	$ mkdir build
	$ cd build
	$ sudo cmake -DCMAKE_BUILD_TYPE=Release ..
	$ sudo make
	$ sudo make install

This part can take a while.

Installing OpenCV
-----------------

	$ sudo apt-get install libopencv-dev

Install and Build Project
-------------------------

Move back to you home folder and clone the project:

	$ git clone https://github.com/solderspot/PiCamObjDetect.git

Then build:

	$ cd PiCamObjDetect
	$ mkdir build
	$ cd build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ make

And run:

	$ ./pidetect
	
The demo does not parse command line arguments. You must modify the code, rebuild and run. Might add args at a later date.

Press ctrl-c to exit the frame loop. 
	
 


