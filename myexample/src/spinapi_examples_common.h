#ifndef EXAMPLES_SHARED_H
#define EXAMPLES_SHARED_H

#ifndef __linux__
#include <Windows.h>
#include <conio.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Pause until the user responds to the prompt */
static void pause_for_user(void)
{
#ifdef __linux__
	printf("Press enter to continue...");
	while (!feof(stdin) && getchar() != '\n');
#else
	printf("Press any key to continue...");
	while (!_kbhit()) Sleep(100);
	_getch();
#endif
}

/**
 * Reads lines from stdin until a line with a double value is found, returning
 * non-zero on read failures.
 *
 * If prompt is NULL, no prompt is displayed
 *
 * This function preserves the old behavior of discarding unconvertible values,
 * but now also discards the rest of the line on which the convertible value is
 * found.  This works well with subsequent calls to pause_for_user.
 *
 * Lines longer than 255 characters are not allowed in order to avoid split
 * values at the end/beginning of the next read from happening. Arbitrary line
 * lengths could be supported, but support hasn't been added because the common
 * use case is very short lines with single values.
 */
static int read_double(const char *prompt, double *result)
{
    for (;;)
    {
        char buf[256];
        char *fgetsStatus;
        const char delimiters[] = " \n\r";
        char *token;
        char *end;

        if (NULL != prompt)
            printf("%s", prompt);

        fgetsStatus = fgets(buf, sizeof(buf), stdin);
        if (NULL == fgetsStatus)
        {
            printf("read_double: User input read failed\n");
            return -1;
        }
        if ('\n' != buf[strlen(buf) - 1] && !feof(stdin))
        {
            printf("read_double: User input line too long (>%lu)\n",
                   (unsigned long)(sizeof(buf) - 1));
            return -1;
        }

        token = strtok(buf, delimiters);
        while (token != NULL)
        {
            double tmpResult = strtod(token, &end);
            if (tmpResult == 0.0 && end == token)
            {
                token = strtok(NULL, delimiters);
            }
            else
            {
                *result = tmpResult;
                return 0;
            }
        }
    }
}

/** Read an int value from stdin, returning non-zero on failure
 *
 * If prompt is NULL, no prompt is displayed
 */
static int read_int(const char *prompt, int *result)
{
    double resultDouble;
    if (0 != read_double(prompt, &resultDouble))
        return -1;
    *result = (int)resultDouble;
    return 0;
}

static int read_float(const char *prompt, float *result)
{
    double resultDouble;
    if (0 != read_double(prompt, &resultDouble))
        return -1;
    *result = (float)resultDouble;
    return 0;
}

static void read_int_exit_on_error(const char *prompt, int *result)
{
    if (0 != read_int(prompt, result))
        exit(EXIT_FAILURE);
}

static void read_float_exit_on_error(const char *prompt, float *result)
{
    if (0 != read_float(prompt, result))
        exit(EXIT_FAILURE);
}

static void read_double_exit_on_error(const char *prompt, double *result)
{
    if (0 != read_double(prompt, result))
        exit(EXIT_FAILURE);
}

/** Read input clock from stdin, calling exit on failure */
static void read_input_clock(double *retClockMHz)
{
    int status;
    status = read_double("\nPlease enter internal clock frequency (MHz): ", retClockMHz);
    if (0 != status)
        exit(EXIT_FAILURE);
}

#endif
