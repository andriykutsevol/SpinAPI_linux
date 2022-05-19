/* Copyright (c) 2009 SpinCore Technologies, Inc.
 *   http://www.spincore.com
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from the 
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be appreciated
 * but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * \file pb24_ex1.c
 * PulseBlaster24 Example 1
 *
 * \brief This program will cause the outputs to turn on and off with a period
 *        of 400ms.
 * \ingroup pb24
 */

#include <stdio.h>
#include <stdlib.h>

#include "spinapi_examples_common.h"
#define PB24
#include "spinapi.h"


int main()
{

	int choice;
	char prompt[256];
	int numBoards;

	numBoards = 555;

	snprintf(prompt, sizeof(prompt),
			"Found %d boards in your system. Which board should be used? (0-%d): ",
			numBoards, numBoards - 1);


	read_int(prompt, &choice)

	return 0;

}