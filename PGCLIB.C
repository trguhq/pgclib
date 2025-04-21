/*
Original PGCTALK/PGCBMP copyright:

Copyright (c) 2004 John Elliott <jce@seasip.demon.co.uk>

This software is provided 'as-is', without any express or implied warranty. 
In no event will the authors be held liable for any damages arising from the 
use of this software.

Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it 
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not 
claim that you wrote the original software. If you use this software in a 
product, an acknowledgment in the product documentation would be appreciated
but is not required.

    2. Altered source versions must be plainly marked as such, and must not 
be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

PGCLIB:

Copyright (c) 2025 trguhq

Same zLib license.
*/

#define PGCLIB_C

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>
#include <conio.h>
#include "PGCLIB.H"

typedef unsigned char byte;

/* Base address of the PGC transfer buffer*/
#define gl_pgc ((byte far *)MK_FP(0xC600, 0))

/* PGC ring buffer pointers */
#define IN_WRPTR (gl_pgc[0x300])
#define IN_RDPTR (gl_pgc[0x301])
#define OUT_WRPTR (gl_pgc[0x302])
#define OUT_RDPTR (gl_pgc[0x303])
#define ERR_WRPTR (gl_pgc[0x304])
#define ERR_RDPTR (gl_pgc[0x305])

static char ascii_mode;
char pgc_output[PGC_BUFFER_SIZE];
char pgc_error[PGC_BUFFER_SIZE];
int pgc_output_len;
int pgc_error_len;

/* Initialize library */
void pgc_init()
{
	ascii_mode = FALSE;
	pgc_out_len = 0;
	pgc_err_len = 0;

	pgc_say("CX\n");
}

/* Set ASCII mode */
void pgc_mode_ascii()
{
	if (ascii_mode == FALSE)
	{
		ascii_mode = TRUE;
		pgc_say("CA\n");		/* one fewer byte if done in hex? */
	}
}

void pgc_mode_hex()
{
	if (ascii_mode == TRUE)
	{
		ascii_mode = FALSE;
		pgc_say("CX\n");
	}
}

/* Write a byte to the PGC command buffer. */
inline void pgc_write(byte b)
{
	gl_pgc[IN_WRPTR] = b;
	++IN_WRPTR;
}

/* Read output buffer */
int pgc_output_read()
{
	int rv;

	pgc_output_len = 0;

    while (1)
	{
		if (OUT_WRPTR == OUT_RDPTR) return pgc_error_len;

		rv = gl_pgc[0x100 + OUT_RDPTR];
		++OUT_RDPTR;
		if (pgc_output_len > PGC_BUFFER_SIZE - 1 ||
			rv == EOF) return pgc_error_len;
		pgc_output[pgc_output_len++] = rv;
	}

	return pgc_output_len;	
}

/* Read error buffer */
int pgc_error_read()
{
	int rv;

	pgc_error_len = 0;

	while (1)
	{
		if (ERR_RDPTR == ERR_WRPTR) return pgc_error_len;

		rv = gl_pgc[0x200 + ERR_RDPTR];
		++ERR_RDPTR;

		if (pgc_error_len > PGC_BUFFER_SIZE - 1 ||
			rv == EOF) return pgc_error_len;
		pgc_error[pgc_output_len++] = rv;
	}

	return pgc_error_len;	
}

/* Write an ASCII command to the PGC. */
void pgc_command_string(const char *s)
{
	pgc_mode_ascii();

	while (*s)
	{
		pgc_write(*s);
		++s;
	}

	pgc_output_read();
	pgc_error_read();
}

/* 
 * Write a Hex command to the PGC.
 *
 * command = byte command
 * buffer = pointer to command data
 * buffer_len = length of command data
 * 
 */
inline void pgc_command_hex(char command, char* buffer, int buffer_len)
{
	int p;

	pgc_mode_hex();

	pgc_write(command);

	p = 0;
    while (p < buffer_len)
	{
		pgc_write(buffer[p]);
	}
    
	pgc_output_read();
	pgc_error_read();
}
