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

static char ascii_mode;
static char error_mode;
static char cga_mode;
static char cga_mode_available;

char pgc_output[PGC_BUFFER_SIZE];
char pgc_error[PGC_BUFFER_SIZE];
int pgc_output_len;
int pgc_error_len;

/* Forward delcarations */
int pgc_version_major();
int pgc_version_minor();
char pgc_selftest_pass();
char pgc_selftest_rom_low_pass();
char pgc_selftest_rom_high_pass();
char pgc_selftest_ram_pass();
void pgc_mode_ascii();
void pgc_mode_hex();
void pgc_error_mode(char value);
void pgc_cga_mode(char value);
char pgc_get_ascii_mode();
char pgc_get_error_mode();
char pgc_get_cga_mode();
char pgc_get_cga_mode_avail();
word pgc_get_firmware_ver();
void pgc_write(byte b);
void pgc_output_read();
void pgc_error_read();
char far * pgc_error_string(byte err);
void pgc_command_string(const char far *s);
void pgc_command_hex(char command, char far* buffer, int buffer_len);
void pgc_flagrd(byte flag);
word pgc_flagrd_free_mem();

/* Initialize library */
int pgc_init()
{
	pgc_output_len = 0;
	pgc_error_len = 0;

	ascii_mode = TRUE;		/* set temporarily */
	pgc_mode_hex();

	error_mode = TRUE;		/* temporary */
	pgc_error_mode(FALSE);

	if (gl_pgc[PGC_FLAG_CGA] == 0)
	{
		cga_mode_available = FALSE;
	} else
	{
		cga_mode_available = TRUE;
	}

	if (cga_mode_available)
	{
		/* starts in CGA mode if it has it, verify this */
		cga_mode = TRUE;
	} else
	{
		/* if no CGA mode available starts in PGC mode? */
		cga_mode = FALSE;
	}

	return 1;
}

int pgc_version_major()
{
	return PGCLIB_VERSION_MAJOR;
}

int pgc_version_minor()
{
	return PGCLIB_VERSION_MINOR;
}

/* PGC self-test */
char pgc_selftest_pass()
{
	if (gl_pgc[PGC_PASS] == 0xA5) return 1;

	return 0;
}

/* PGC ROM low self-test */
char pgc_selftest_rom_low_pass()
{
	if (gl_pgc[PGC_ROM_LOW] == 0x5A) return 1;

	return 0;
}

/* PGC ROM high self-test */
char pgc_selftest_rom_high_pass()
{
	if (gl_pgc[PGC_ROM_HIGH] == 0x55) return 1;

	return 0;
}

/* PGC RAM self-test */
char pgc_selftest_ram_pass()
{
	if (gl_pgc[PGC_RAM] == 0xA5) return 1;

	return 0;
}

/* Set ASCII mode */
void pgc_mode_ascii()
{
	if (ascii_mode == FALSE)
	{
		ascii_mode = TRUE;
		pgc_write(PGC_CA);
		/*
		pgc_write(0x43);
		pgc_write(0x41);
		pgc_write(PGC_DELIM);
		*/
	}
}

/* Set Hex mode */
void pgc_mode_hex()
{
	if (ascii_mode == TRUE)
	{
		ascii_mode = FALSE;
		pgc_write(PGC_CX);
		/*
		pgc_write(0x43);
		pgc_write(0x58);
		pgc_write(PGC_DELIM);
		*/
	}
}

/* Set error mode */
void pgc_error_mode(char value)
{
	if (value != error_mode)
	{
		gl_pgc[PGC_CMD_ERROR] = value;
		error_mode = value;
	}
}

/* Set CGA mode */
void pgc_cga_mode(char value)
{
	if (value != cga_mode)
	{
		gl_pgc[PGC_CMD_CGA] = value;
		cga_mode = value;
	}
}

/* these return the stored value rather than read from PGC */
/* Get ASCII mode or not */
char pgc_get_ascii_mode()
{
	return ascii_mode;
}

/* Get error mode */
char pgc_get_error_mode()
{
	return error_mode;
}

/* Get CGA mode */
char pgc_get_cga_mode()
{
	return cga_mode;
}

/* Get CGA mode availability */
char pgc_get_cga_mode_avail()
{
	return cga_mode_available;
}

/* Get firmware version */
word pgc_get_firmware_ver()
{
	/* change this to a macro */
	return ((word) gl_pgc[PGC_FIRM_VER] + 
		((word) gl_pgc[PGC_FIRM_VER + 1] << 8));
}

/* Write a byte to the PGC command buffer. */
void pgc_write(byte b)
{
	gl_pgc[PGC_IN_WRPTR] = b;
	++PGC_IN_WRPTR;
}

/* Read output buffer */
void pgc_output_read()
{
	int rv;

	pgc_output_len = 0;

    while (1)
	{
		if (PGC_OUT_WRPTR == PGC_OUT_RDPTR) return;

		rv = gl_pgc[0x100 + PGC_OUT_RDPTR];
		++PGC_OUT_RDPTR;
		if (pgc_output_len > PGC_BUFFER_SIZE - 1 ||
			rv == EOF) return;
		pgc_output[pgc_output_len++] = rv;
	}
}

/* Read error buffer */
void pgc_error_read()
{
	int rv;

	pgc_error_len = 0;

	while (1)
	{
		if (PGC_ERR_RDPTR == PGC_ERR_WRPTR) return;

		rv = gl_pgc[0x200 + PGC_ERR_RDPTR];
		++PGC_ERR_RDPTR;

		if (pgc_error_len > PGC_BUFFER_SIZE - 1 ||
			rv == EOF) return;
		pgc_error[pgc_output_len++] = rv;
	}
}

/* Generate a text error message. */
char far * pgc_error_string(byte err)
{
	switch (err)
	{
		case PGC_ERR_RANGE:
			return("Parameter out of range.");
		case PGC_ERR_INT:
			return("Need integer.");
		case PGC_ERR_MEM:
			return("Out of memory.");
		case PGC_ERR_OVER:
			return("Arithmetic verflow.");
		case PGC_ERR_DIGIT:
			return("Need digit.");
		case PGC_ERR_OP:
			return("Illegal opcode.");
		case PGC_ERR_REC:
			return("Recursion in command list.");
		case PGC_ERR_STACK:
			return("Nested command lists.");
		case PGC_ERR_LONG:
			return("Command too long.");
		case PGC_ERR_AREA:
			return("Area fill error.");
		case PGC_ERR_MISSING:
			return("Missing paramerter.");
	}

	return("Unknown.");)

}

/* Write an ASCII command to the PGC. */
void pgc_command_string(const char far *s)
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
void pgc_command_hex(char command, char far* buffer, int buffer_len)
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

/* Here commands start */

/* Read data of flag */
void pgc_flagrd(byte flag)
{
	pgc_command_hex(PGC_FLAGRD, &flag, 1);
}

/* Returns free memory */
word pgc_flagrd_free_mem()
{
	pgc_command_hex(PGC_FLAGRD_MEM, NULL, 0);
	return ((word) pgc_output[0] + ((word) pgc_output[1] << 8));
}