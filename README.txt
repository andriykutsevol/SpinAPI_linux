Table of Contents
-----------------
1. Overview
2. PCI-e
3. Compilation Support
4. Root privileges
5. libspinapi.so
6. Running examples
7. Turning on debugging information
8. Contact Info

1 - Overview:
=========

This archive contains drivers which will work for PB24, PBDDS-I-300, PBDDS-II, PBDDS-III, 
PulseBlasterESR, PulseBlasterESR-PRO, PulseBlasterESR-PRO-II, PB-DualCore, PB-QuadCore, USB-PTS, 
and RadioProcessor boards.


The contents of each directory is explained below.

examples		- contains the source code for the SpinAPI examples
include			- contains the SpinAPI headers
interpreter		- SpinAPI CLI
src				- contains the source code for the SpinAPI library



2 - PCI-e
=========

Linux driver supports products with pid "0x887A" only.



3 - Compilation Support:
====================

Required packages, on Ubuntu 18.04: build-essential cmake

If you are cross compiling, also install: gcc-multilib g++multilib

CMake version 3.6 or later is required.

To perform a full rebuild of SpinAPI and the examples:

	cd SpinAPI
	mkdir build
	cd build
	cmake ..
	make

The freshly compiled programs will be inside the build directory, for example:

	cd SpinAPI
	sudo ./build/examples/General/pb_read_firmware



4 - Root privileges/udev:
====================

Note that root privileges are required, or the programs may segfault.


Device permissions must be set properly in order run SpinAPI programs. 
It is recommended that users set device permissions using a udev rules file. 
An example udev rules file can be found here: 99-spincore.rules. 
This file should typically be placed in "/etc/udev/rules.d/", but this path may vary depending on your Linux distribution. 
This file is created for the RadioProcessorUSB and PulseBlasterUSB boards. 
Once the rules file has been added, 
you can update the system rules using the command "udevadm control --reload-rules". 
If the SpinCore board is plugged in while performing this step, power the board off and on. 
Any user in group "spincore" should now have access to the board. 

Alternatively, applications may be run as root, but this is not recommended.




5 - libspinapi.so
====================

The build system will build the "libspinapi.so" and link an examples against to it.
You have to link all your programs against this library.
Once built, you will find "libspinapi.so" in the
./build/src
directory.

To let the executable to find the library at a runtime you could add the path to it to the ~/.bashrc
For example:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/Documents/spincore/SpinAPI_linux/build/src


6 - Running examples.
====================

The examples will be built in:
./build/examples

There is a 'General' folder.

-----
./build/examples/General/pb_debug
-----
Will create a "log.txt" file in the current directory.
Make sure that it does not have any errors.

-----
./build/examples/General/pb_read_firmware
-----

Make sure it produces a reasonable result.
For example:
"Board 0 Firmware ID: 23-1"
But not soemthink like:
"Board 0 Firmware ID: 255-255"
or
"Board 0 Firmware ID: 0-0"

-----
./build/examples/General/pb_start
-----
Make sure it does not produce any errors.

-----
./build/examples/General/pb_start
-----
Make sure it does not produce any errors.



After checking the "./build/examples/General/" directory
you could run other tests.



7 - Turning on debugging information.
====================

There is a function that allows you to turn on a debugging information:

pb_set_debug(1);

It is defined in the "src/spinapi.h"


8 - Contact Info:
=============

Thank you for choosing a design from SpinCore Technologies, Inc. We appreciate your business!
At SpinCore we try to fully support the needs of our customers. If you are in need of assistance,
please contact us and we will strive to provide the necessary support.
Please see our contact information below for any questions/comments you may have.

SpinCore Technologies, Inc.
4631 NW 53rd Avenue, SUITE 103
Gainesville, FL 32653
USA

Phone:    (USA) 352-271-7383 
Internet: http://www.spincore.com
