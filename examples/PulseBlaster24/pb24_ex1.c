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

int detect_boards();
int select_board(int numBoards);

int main()
{
	int start, status;
	int numBoards;
	double clock;

	//Uncommenting the line below will generate a debug log in your current
	//directory that can help debug any problems that you may be experiencing   
	pb_set_debug(1); 

	printf("Copyright (c) 2010 SpinCore Technologies, Inc.\n\n");

	printf("Using SpinAPI library version %s\n", pb_get_version());

	/*If there is more than one board in the system, have the user specify. */
	if ((numBoards = detect_boards()) > 1) {
		select_board(numBoards);
	}

	if (pb_init() != 0) {
		printf("Error initializing board: %s\n", pb_get_error());
		pause_for_user();
		return -1;
	}
	
	// User input clock
	read_input_clock(&clock);

	printf("\nClock frequency: %lfMHz\n\n", clock);
	printf("All outputs should now be turning on and off with a period of "
	       "400ms and \n 50 percent duty cycle.\n\n");

	// Tell the driver what clock frequency the board has (in MHz)
	pb_core_clock(clock);

	pb_start_programming(PULSE_PROGRAM);

	// Instruction 0 - Continue to instruction 1 in 200ms
	// Flags = 0xFFFFFF, OPCODE = CONTINUE
	start = pb_inst(0xFFFFFF, CONTINUE, 0, 200.0 * ms);

	// Instruction 1 - Continue to instruction 2 in 100ms
	// Flags = 0x0, OPCODE = CONTINUE
	pb_inst(0x0, CONTINUE, 0, 100.0 * ms);

	// Instruction 2 - Branch to "start" (Instruction 0) in 100ms
	// 0x0, OPCODE = BRANCH, Target = start
	pb_inst(0x0, BRANCH, start, 100.0 * ms);

	pb_stop_programming();

	
	// Trigger the pulse program
	pb_reset();
	pb_start();

	//Read the status register
	status = pb_read_status();

	printf("status: %d \n", status);
	printf("%s\n", pb_status_message());

	pause_for_user();

	pb_stop();
	pb_close();

	return 0;
}

int detect_boards()
{
	int numBoards;

	numBoards = pb_count_boards();	/*Count the number of boards */

	if (numBoards <= 0) {
		printf
		    ("No Boards were detected in your system. Verify that the board "
		     "is firmly secured in the PCI slot.\n\n");
		pause_for_user();
		exit(-1);
	}

	return numBoards;
}

int select_board(int numBoards)
{
	int choice;
	char prompt[256];

	snprintf(prompt, sizeof(prompt),
			"Found %d boards in your system. Which board should be used? (0-%d): ",
			numBoards, numBoards - 1);
	do {
		if (0 != read_int(prompt, &choice))
			continue;

		if (choice < 0 || choice >= numBoards) {
			printf("Invalid Board Number (%d).\n", choice);
		}
	} while (choice < 0 || choice >= numBoards);

	pb_select_board(choice);
	printf("Board %d selected.\n", choice);

	return choice;
}
