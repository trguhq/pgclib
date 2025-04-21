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

/* other */
#define FLAG_COLD (gl_pgc[0x306]);		/* cold start flag */
#define FLAG_WARM (gl_pgc[0x307]);		/* warm start flag */
#define CMD_ERROR (gl_pgc[0x308]);		/* set to !0 for errors */
#define FLAG_WARM2 (gl_pgc[0x309]);		/* set when warm start flag is */
#define FLAG_CGA (gl_pgc[0x30B]);		/* !0 if CGA mode available */
#define CMD_CGA (gl_pgc[0x30C]);		/* set to 1 for CGA, 0 PGC */
#define ACK_DISP (gl_pgc[0x30D]);
#define REQ_CGA_BUF (gl_pgc[0x30E]);
#define ACK_CGA_BUF (gl_pgc[0x30F]);
#define PGC_REG (gl_pgc[0x310]);		/* 0x11 bytes */
#define CGA_VERT_TOTAL (gl_pgc[0x322]);
#define CGA_VERT_DISP (gl_pgc[0x323]);
#define CGA_VERT_ADJ (gl_pgc[0x324]);
#define CGA_VERT_SYNC (gl_pgc[0x325]);
#define CGA_CUR_SIZE (gl_pgc[0x327]);	/* 2 bytes */
#define CGA_CUR_ADD (gl_pgc[0x329]);	/* 2 bytes */
#define CGA_SCR_START (gl_pgc[0x32B]);	/* 2 bytes */
#define PORT_03D8 (gl_pgc[0x3D8]);
#define PORT_03D9 (gl_pgc[0x3D9]);
#define PGC_PRES (gl_pgc[0x3DB]);		/* presence test byte */
#define CGA_CRTC (gl_pgc[0x3E0]);		/* 0x13 bytes */
#define PGC_FIRM_VER (gl_pgc[0x3F8]);	/* 2 bytes */
#define PGC_PASS (gl_pgc[0x3FB]);		/* 0xA5 pass */
#define PGC_ROM_LOW (gl_pgc[0x3FC]);    /* 0xFF fail 0x5A pass */
#define PGC_ROM_HIGH (gl_pgc[0x3FD]);	/* 0xFF fail 0x55 pass */
#define PGC_RAM (gl_pgc[0x3FE]);		/* 0xFF fail 0xAA pass */
#define CMD_REBOOT (gl_pgc[0x3FF]);		/* write 0x50 */
										/* wait 2 system clock */
										/* write 0xA0 */
static char ascii_mode;
char pgc_output[PGC_BUFFER_SIZE];
char pgc_error[PGC_BUFFER_SIZE];
int pgc_output_len;
int pgc_error_len;

/* Initialize library */
int pgc_init()
{
	pgc_out_len = 0;
	pgc_err_len = 0;

	ascii_mode = FALSE;
	pgc_mode_hex();
}

/* PGC self-test */
int pgc_selftest_pass()
{
	if (gl_pgc[PGC_PASS] == 0xA5) return 1;

	return 0;
}

/* PGC ROM low self-test */
int pgc_selftest_rom_low_pass()
{
	if (gl_pgc[PGC_ROM_LOW] == 0x5A) return 1;

	return 0;
}

/* PGC ROM high self-test */
int pgc_selftest_rom_high_pass()
{
	if (gl_pgc[PGC_ROM_HIGH] == 0x55) return 1;

	return 0;
}

/* PGC RAM self-test */
int pgc_selftest_ram_pass()
{
	if (gl_pgc[PGC_RAM] == 0xA5) return 1;

	return 0;
}

/* Set ASCII mode */
inline void pgc_mode_ascii()
{
	if (ascii_mode == FALSE)
	{
		ascii_mode = TRUE;
		/* pgc_write(PGC_CA); */
		pgc_write(0x43);
		pgc_write(0x41);
		pgc_write(PGC_DELIM);
	}
}

/* Set Hex mode */
inline void pgc_mode_hex()
{
	if (ascii_mode == TRUE)
	{
		ascii_mode = FALSE;
		/* pgc_write(PGC_CX); */
		pgc_write(0x43);
		pgc_write(0x58);
		pgc_write(PGC_DELIM);
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
