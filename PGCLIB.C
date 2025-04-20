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

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>
#include <conio.h>

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

/* Initialize library */
void pgc_init()
{
	ascii_mode = FALSE;
	pgc_say("CX\n");
}

/* Set ASCII mode */
void pgc_mode_ascii()
{
	if (ascii_mode == FALSE)
	{
		ascii_mode = TRUE;
	    pgc_say("CA\n");
	}
}

void pgc_mode_hex()
{
	if (ascii_mode == TRUE)
	{
		ascii_mode = FALSE;
		pgc_sac("CX\n");
	}
}

/* Write a byte to the PGC command buffer. */
void pgc_write(byte b)
{
	gl_pgc[IN_WRPTR] = b;
	++IN_WRPTR;
}

/* Read a byte from the PGC output buffer. Returns EOF if no data available. */
int pgc_rdout()
{
	int rv;

	if (OUT_WRPTR == OUT_RDPTR) return EOF;

	rv = gl_pgc[0x100 + OUT_RDPTR];
	++OUT_RDPTR;
	return rv;	
}

/* Read a byte from the PGC error buffer. Returns EOF if no data available. */
int pgc_rderr()
{
	int rv;

	if (ERR_RDPTR == ERR_WRPTR) return EOF;

	rv = gl_pgc[0x200 + ERR_RDPTR];
	++ERR_RDPTR;
	return rv;	
}

/* Write an ASCII command to the PGC. */
void pgc_cmd(const char *s)
{
	while (*s)
	{
		pgc_write(*s);
		++s;
	}
}

/* Write an ASCII command to the PGC. Display any results returned. */
void pgc_say(const char *s)
{
	int ch, n;

    pgc_mode_ascii();
	pgc_cmd(s);

	n = 0;
	while ((ch = pgc_rdout()) != EOF)
	{
		if (!n) { printf("Out: "); ++n; }
		putchar(ch);
	}
	if (n) printf("\n");
	n = 0;
	while ((ch = pgc_rderr()) != EOF)
	{
		if (!n) { printf("Err: "); ++n; }
		putchar(ch);
	}
	if (n) printf("\n");

}

/* Write a Hex command to the PGC. Display any results returned as Hex.
 *
 */
void pgc_xsay(const char *s)
{
	char buf[128], *p;
	char x[3];
	int hexv, n, ch;

	pgc_mode_hex();

/* Extract only hex digits from the passed string */
	p = buf;
	while (*s)
	{
		if (isxdigit(*s)) *p++ = *s;
		++s;	
	}
	*p = 0;
	p = buf;
/* Parse each pair of digits as a hex byte, and feed that to the PGC */
	while (*p)
	{
		sprintf(x, "%-2.2s", p);
		if (strlen(p) < 2) p = "";
		else p += 2;
		sscanf(x, "%x", &hexv);
		printf("%02x ", hexv);
		pgc_write(hexv);
	}
	printf("\n");
/* Read output and error, and render them as Hex. */
	n = 0;
	while ((ch = pgc_rdout()) != EOF)
	{
		if (!n) { printf("Out: "); ++n; }
		printf("%02x ", ch);
	}
	if (n) printf("\n");
	n = 0;
	while ((ch = pgc_rderr()) != EOF)
	{
		if (!n) { printf("Err: "); ++n; }
		printf("%02x ", ch);
	}
	if (n) printf("\n");
}
