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

lib            - contains the SpinAPI library
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

Note that root privileges are required, or the programs may segfault.

3 - Contact Info:
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
