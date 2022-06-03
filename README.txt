SpinAPI
-------

Table of Contents
-----------------
1. Overview
2. Compilation Support
3. Contact Info

1 - Overview:
=========

This archive contains drivers which will work for PB24, PBDDS-I-300, PBDDS-II, PBDDS-III, 
PulseBlasterESR, PulseBlasterESR-PRO, PulseBlasterESR-PRO-II, PB-DualCore, PB-QuadCore, USB-PTS, 
and RadioProcessor boards.

The contents of each directory is explained below.

include        - contains the SpinAPI headers
src            - contains the source code for the SpinAPI library
examples       - contains the source code for the SpinAPI examples

2 - Compilation Support:
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



3 - Root privileges:
====================

Note that root privileges are required, or the programs may segfault.



4 - libspinapi.so
====================

The build system will build the "libspinapi.so" and link an examples against to it.
You have to link all your programs against this library.
Once built, you will find "libspinapi.so" in the
./build/src
directory.

To let the executable to find the library at a runtime you could add the path to it to the ~/.bashrc
For example:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/Documents/spincore/SpinAPI_linux/build/src


5 - Running examples.
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



6 - Turning on debugging information.
====================

There is a function that allows you to turn on a debugging information:

pb_set_debug(1);

It is defined in the "src/spinapi.h"


4 - Contact Info:
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
