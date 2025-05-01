/*
 * Original PGCTALK/PGCBMP copyright:
 * 
 * Copyright (c) 2004 John Elliott <jce@seasip.demon.co.uk>
 * 
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from the 
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not 
 * claim that you wrote the original software. If you use this software in a 
 * product, an acknowledgment in the product documentation would be appreciated
 * but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not 
 * be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 * PGCLIB:
 *
 * Copyright (c) 2025 trguhq
 *
 * Same zLib license.
 *
 * Documentation:
 *
 * IBM technical reference and https://www.seasip.info/VintagePC/pgc.html
*/

#ifndef PGCLIB_H
#define PGCLIB_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned char byte;
typedef short int word;
typedef struct coord {word integer; unsigned int fraction;} coord;

/* externs */
#ifndef PGCLIB_C
extern char pgc_output[];       /* output buffer */
extern char pgc_error[];        /* error buffer */
extern int pgc_output_len;      /* length of output */
extern int pgc_error_len;       /* length of error */

extern int pgc_version_major();
extern int pgc_version_minor();
extern char pgc_selftest_pass();
extern char pgc_selftest_rom_low_pass();
extern char pgc_selftest_rom_high_pass();
extern char pgc_selftest_ram_pass();
extern void pgc_mode_ascii();
extern void pgc_mode_hex();
extern void pgc_error_mode(char value);
extern void pgc_cga_mode(char value);
extern char pgc_get_ascii_mode();
extern char pgc_get_error_mode();
extern char pgc_get_cga_mode();
extern char pgc_get_cga_mode_avail();
extern word pgc_get_firmware_ver();
extern void pgc_write(byte b);
extern void pgc_output_read();
extern void pgc_error_read();
extern char far * pgc_error_string(byte err);
extern void pgc_command_string(const char far *s);
extern void pgc_command_hex(char command, char far* buffer, int buffer_len);
extern void pgc_flagrd(byte flag);
extern word pgc_flagrd_free_mem();

#endif /* PGCLIB_C */

#define PGCLIB_VERSION_MAJOR 0
#define PGCLIB_VERSION_MINOR 1

#define PGC_BUFFER_SIZE 0xFF
#define PGC_DELIM 0x20

/* Base address of the PGC transfer buffer*/
#define gl_pgc ((byte far *)MK_FP(0xC600, 0))

/* PGC ring buffer pointers */
#define PGC_IN_WRPTR (gl_pgc[0x300])
#define PGC_IN_RDPTR (gl_pgc[0x301])
#define PGC_OUT_WRPTR (gl_pgc[0x302])
#define PGC_OUT_RDPTR (gl_pgc[0x303])
#define PGC_ERR_WRPTR (gl_pgc[0x304])
#define PGC_ERR_RDPTR (gl_pgc[0x305])

/* other */
#define PGC_FLAG_COLD (gl_pgc[0x306])	    /* cold start flag */
#define PGC_FLAG_WARM (gl_pgc[0x307])	    /* warm start flag */
#define PGC_CMD_ERROR (gl_pgc[0x308])	    /* set to !0 for errors */
#define PGC_FLAG_WARM2 (gl_pgc[0x309])	    /* set when warm start flag is */
#define PGC_FLAG_CGA (gl_pgc[0x30B])        /* !0 if CGA mode available */
#define PGC_CMD_CGA (gl_pgc[0x30C])         /* set to 1 for CGA, 0 PGC */
#define PGC_ACK_DISP (gl_pgc[0x30D])
#define PGC_REQ_CGA_BUF (gl_pgc[0x30E])
#define PGC_ACK_CGA_BUF (gl_pgc[0x30F])
#define PGC_REG (gl_pgc[0x310])             /* 0x11 bytes */
#define PGC_CGA_VERT_TOTAL (gl_pgc[0x322])
#define PGC_CGA_VERT_DISP (gl_pgc[0x323])
#define PGC_CGA_VERT_ADJ (gl_pgc[0x324])
#define PGC_CGA_VERT_SYNC (gl_pgc[0x325])
#define PGC_CGA_CUR_SIZE (gl_pgc[0x327])    /* 2 bytes */
#define PGC_CGA_CUR_ADD (gl_pgc[0x329])     /* 2 bytes */
#define PGC_CGA_SCR_START (gl_pgc[0x32B])   /* 2 bytes */
#define PGC_PORT_03D8 (gl_pgc[0x3D8])
#define PGC_PORT_03D9 (gl_pgc[0x3D9])
#define PGC_PRES (gl_pgc[0x3DB])            /* presence test byte */
#define PGC_CGA_CRTC (gl_pgc[0x3E0])        /* 0x13 bytes */
#define PGC_FIRM_VER (gl_pgc[0x3F8])        /* 2 bytes */
#define PGC_PASS (gl_pgc[0x3FB])            /* 0xA5 pass */
#define PGC_ROM_LOW (gl_pgc[0x3FC])         /* 0xFF fail 0x5A pass */
#define PGC_ROM_HIGH (gl_pgc[0x3FD])        /* 0xFF fail 0x55 pass */
#define PGC_RAM (gl_pgc[0x3FE])             /* 0xFF fail 0xAA pass */
#define PGC_CMD_REBOOT (gl_pgc[0x3FF])      /* write 0x50 */
                                            /* wait 2 system clock */
                                            /* write 0xA0 */

/* errors */
#define PGC_ERR_RANGE 0x1
#define PGC_ERR_INT 0x2         /* ascii mode only */
#define PGC_ERR_MEM 0x3
#define PGC_ERR_OVER 0x4
#define PGC_ERR_DIGIT 0x5       /* ascii mode only */
#define PGC_ERR_OP 0x6
#define PGC_ERR_REC 0x7
#define PGC_ERR_STACK 0x8
#define PGC_ERR_LONG 0x9
#define PGC_ERR_AREA 0xA
#define PGC_ERR_MISSING 0xB     /* ascii mode only */

/* hex commands */
#define PGC_ARC	0x3C        /* radius,angle1,angle2 (1 coord, 2 words) */
#define PGC_AREA 0xC0       /* none */
#define PGC_AREABC 0xC1     /* color (1 byte) */
#define PGC_AREAPT 0xE7     /* word,word... (16 words) */
#define PGC_BUFFER 0x4F     /* p,q (2 bytes) */
#define PGC_CA 0xD2         /* or 0x43 0x41 */
#define PGC_CIRCLE 0x38     /* radius (1 coord) */
#define PGC_CLBEG 0x70      /* id (1 byte) */
#define PGC_CLDEL 0x74      /* id (1 byte) */
#define PGC_CLEARS 0x0F     /* color (1 byte) */
#define PGC_CLEND 0x71      /* none */
#define PGC_CLIPH 0xAA      /* mode (1 byte) */
#define PGC_CLIPY 0xAB      /* mode (1 byte) */
#define PGC_CLOOP 0x73      /* id,reps (1 byte, 1 word) */
#define PGC_CLRD 0x75       /* id (1 byte) */
#define PGC_CLRUN 0x72      /* id (1 byte) */
#define PGC_COLOR 0x06      /* color (1 byte) */
#define PGC_CONVRT 0xAF     /* none */
#define PGC_CX 0xD1         /* or 0x43 0x58 */
#define PGC_DISPLA 0xD0     /* mode (1 byte) */
#define PGC_DISTAN 0xB1     /* distance (1 coord) */
#define PGC_DISTH 0xA8      /* distance (1 coord) */
#define PGC_DISTY 0xA9      /* distance (1 coord) */
#define PGC_DRAW 0x28       /* x,y (2 coords) */
#define PGC_DRAWR 0x29      /* dx,yx (2 coords) */
#define PGC_DRAW3 0x2A      /* x,y,z (3 coords) */
#define PGC_DRAWR3 0x2B     /* dx,dy,dz (3 coords) */
#define PGC_ELIPSE 0x39     /* width,height (2 coords) */
#define PGC_FILMSK 0xEF     /* mask (1 byte) */
#define PGC_FLAGRD 0x51     /* flag num (1 byte) */
#define PGC_FLAGRD_AREAPT 1     /* 16 words */
#define PGC_FLAGRD_CLIPH 2      /* 1 byte */
#define PGC_FLAGRD_CLIPY 3      /* 1 byte */
#define PGC_FLAGRD_COLOR 4      /* 1 byte */
#define PGC_FLAGRD_DISPLAY 5    /* 1 byte */
#define PGC_FLAGRD_DISTAN 6     /* 1 coord */
#define PGC_FLAGRD_DISTH 7      /* 1 coord */
#define PGC_FLAGRD_DISTY 8      /* 1 coord */
#define PGC_FLAGRD_FILMSK 9     /* 1 byte */
#define PGC_FLAGRD_LINFUN 10    /* 1 byte */
#define PGC_FLAGRD_LINPAT 11    /* 1 word */
#define PGC_FLAGRD_MASK 12      /* 1 byte */
#define PGC_FLAGRD_MDORG 13     /* 3 coords */
#define PGC_FLAGRD_2DPOS 14     /* 2 coords */
#define PGC_FLAGRD_3DPOS 15     /* 3 coords */
#define PGC_FLAGRD_PRMFIL 16    /* 1 byte */
#define PGC_FLAGRD_PROJCT 17    /* 1 byte */
#define PGC_FLAGRD_TANGLE 18    /* 1 word */
#define PGC_FLAGRD_TJUST 19     /* 2 bytes */
#define PGC_FLAGRD_TSIZE 20     /* 1 coord */
#define PGC_FLAGRD_VIEW 21      /* 4 words */
#define PGC_FLAGRD_VWPRT 22     /* 3 coords */
#define PGC_FLAGRD_WINDOW 23    /* 4 coords */
#define PGC_FLAGRD_3DPOINT 24   /* 3 coords */
#define PGC_FLAGRD_MEM 25       /* 1 word */
#define PGC_FLOOD 0x07      /* color (1 byte) */
#define PGC_IMAGER 0xD8     /* row,col1,col2 (3 words) */
#define PGC_IMAGEW 0xD9     /* row,col1,col2,data (3 words)*/
#define PGC_LINFUN 0xEB     /* mode (1 byte) */
#define PGC_LINPAT 0xEA     /* pattern (1 word) */
#define PGC_LUT 0xEE        /* ink,r,g,b (4 bytes) */
#define PGC_LUTINT 0xEC     /* palette (1 byte) */
#define PGC_LUTINT_DEF 0
#define PGC_LUTINT_16 1
#define PGC_LUTINT_233 2
#define PGC_LUTINT_323 3
#define PGC_LUTINT_332 4
#define PGC_LUTINT_666 5
#define PGC_LUTINT_LUTSAV 255
/* The standard palettes are:
 *
 * 0 - Default 256-colour palette
 * Low 4 bits of ink give intensity, high 4 bits give colour
 * 0=Grey, 1=Red, 2=Red/Magenta, 3=Magenta, 4=Violet, 5=Blue, 
 * 6=Blue/Cyan, 7=Cyan, 8=Cyan/Green, 9=Green, 10=Green/Yellow, 
 * 11=Yellow, 12=Orange, 13=Red, 14=Green, 15=Blue
 * 
 * 1 - 16-colour palette (for making the PGC behave as two 16-colour planes)
 * If high 4 bits of ink are 0, low 4 bits give colour;
 * otherwise, high 4 bits give colour.
 * 0=Cyan, 1=Black, 2=Dark Brown, 3=Light Brown, 4=Red/Brown, 5=Red,
 * 6=Orange, 7=Yellow, 8=Yellow/Green, 9=Green, 10=Dark Green, 11=Dark Cyan,
 * 12=Blue, 13=Salmon, 14=Grey, 15=White
 *
 * 2 - 2-3-3 truecolour palette
 * Bits 0-2 of ink give blue; bits 3-5 give green; bits 6,7 give red.
 * 
 * 3 - 3-2-3 truecolour palette
 * Bits 0-2 of ink give blue; bits 3-4 give green; bits 5-7 give red.
 * 
 * 4 - 3-3-2 truecolour palette
 * Bits 0,1 of ink give blue; bits 2-4 give green; bits 5-7 give red.
 * 
 * 5 - 6x6x6 colour cube
 * 
 * 255 - Restore palette previously saved with the LUTSAV command
 */
#define PGC_LUTRD 0x50      /* ink (1 byte) */
#define PGC_LUTSAV 0xED     /* none */
#define PGC_MASK 0xE8       /* mask (1 byte) */
#define PGC_MATXRD 0x52     /* id (1 byte) */
#define PGC_MDIDEN 0x90     /* none */
#define PGC_MDMATX 0x97     /* coord,coord... (16 coords) */
#define PGC_MDORG 0x91      /* x,y,z (3 coords) */
#define PGC_MDROTX 0x93     /* angle (1 word) */
#define PGC_MDROTY 0x94     /* angle (1 word) */
#define PGC_MDROTZ 0x95     /* angle (1 word) */
#define PGC_MDSCAL 0x92     /* x,y,z (3 coords) */
#define PGC_MDTRAN 0x96     /* dx,dy,dz (3 coords) */
#define PGC_MOVE 0x10       /* x,y (2 coords) */
#define PGC_MOVER 0x11      /* dx,dy (2 coords) */
#define PGC_MOVE3 0x12      /* x,y,z (3 coords) */
#define PGC_MOVER3 0x13     /* dx,dy,dz (3 coords) */
#define PGC_POINT 0x08      /* none */
#define PGC_POINT3 0x09     /* none */
#define PGC_POLY 0x30       /* count,x1,y1,x2,y2... */
#define PGC_POLYR 0x31      /* count,x1,y1,x2,y2... */
#define PGC_POLY3 0x32      /* count,x1,y1,z1... */
#define PGC_POLYR3 0x33     /* count,x1,y1,z1... */
#define PGC_PRMFIL 0xE9     /* fill (1 byte) */
#define PGC_PROJCT 0xB0     /* angle (1 word) */
#define PGC_RECT 0x34       /* x,y (2 coords) */
#define PGC_RECTR 0x35      /* dx,dy (2 coords) */
#define PGC_RESETF 0x04     /* none */
#define PGC_SCAN 0x5F       /* search (1 byte) */
#define PGC_SECTOR 0x3D     /* radius,angle1,angle2 */
#define PGC_TANGLE 0x82     /* angle (1 word) */
#define PGC_TDEFIN 0x84     /* id,width,height,bits*/
#define PGC_TEXT 0x80       /* "text" or 'text' */
#define PGC_TEXTP 0x83      /* "text" or 'text' */
#define PGC_TJUST 0x85      /* 2 bytes */
#define PGC_TSIZE 0x81      /* size (1 coord) */
#define PGC_VWIDEN 0xA0     /* none */
#define PGC_VWMATX 0xA7     /* coord1,coord2... (16 coords) */
#define PGC_VWPORT 0xB2     /* x1,x2,y1,y2 (4 words) */
#define PGC_VWRPT 0xA1      /* x,y,z (3 coords) */
#define PGC_VWROTX 0xA3     /* angle */
#define PGC_VWROTY 0xA4     /* angle */
#define PGC_VWROTZ 0xA5     /* angle */
#define PGC_WAIT 0x05       /* delay (1 word) */
#define PGC_WINDOW 0xB3     /* x1,x2,y1,y2 (4 words) */

/* 
 * Undocumented hex mode command for 8088 to jump to location in buffer
 * argument: segment, offset, checksum (3 words)
 * If segment + offset + checksum = 0, the PGC's processor will do a far
 * jump to specified address in transfer buffer which is at location
 * 28000h to 283FFh hfrom the PGC's 8088's point of view.
 * To return control to the normal firmware, the IBM diagnostic program 
 * jumps to FFFF:0008h; INT 3 may also work.
 */
#define PGC_JUMP1 0x5B 
#define PGC_JUMP2 0x71 
#define PGC_JUMP3 0x8C

#endif /* PGCLIB_H */