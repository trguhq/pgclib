#ifndef PGCLIB_H
#define PGCLIB_H

#ifndef PGCLIB_C

/* externs */
extern char pgc_output[];       /* output buffer */
extern char pgc_error[];        /* error buffer */
extern int pgc_output_len;      /* length of output */
extern int pgc_error_len;       /* length of error */

#endif

#define PGC_BUFFER_SIZE 0xFF
#define PGC_DELIM 0x20

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

#define PGC_ARC	0x3C
#define PGC_AREA 0xC0
#define PGC_AREABC 0xC1
#define PGC_AREAPT 0xE7
#define PGC_BUFFER 0x4F
#define PGC_CA 0xD2         /* or 43 41 */
#define PGC_CIRCLE 0x38
#define PGC_CLBEG 0x70
#define PGC_DLDEL 0x74
#define PGC_CLEARS 0x0F
#define PGC_CLEND 0x71
#define PGC_CLIPH 0xAA
#define PGC_CLIPY 0xAB
#define PGC_CLOOP 0x73
#define PGC_CLRD 0x75
#define PGC_CLRUN 0x72
#define PGC_COLOR 0x06
#define PGC_CONVRT 0xAF
#define PGC_CX 0xD1         /* or 43 58 */
#define PGC_DISPLA 0xD0
#define PGC_DISTAN 0xB1
#define PGC_DISTH 0xA8
#define PGC_DISTY 0xA9
#define PGC_DRAW 0x28
#define PGC_DRAWR 0x29
#define PGC_DRAW3 0x2A
#define PGC_DRAWR3 0x2B
#define PGC_ELIPSE 0x39
#define PGC_FILMSK 0xEF
#define PGC_FLAGRD 0x51
#define PGC_FLOOD 0x07
#define PGC_IMAGER 0xD8
#define PGC_IMAGEW 0xD9
#define PGC_LINFUN 0xEB
#define PGC_LINPAT 0xEA
#define PGC_LUT 0xEE
#define PGC LUTINT 0xEC
#define PGC_LUTRD 0x50
#define PGC_LUTSAV 0xED
#define PGC_MASK 0xE8
#define PGC_MATXRD 0x52
#define PGC_MDIEN 0x90
#define PGC_MDMATX 0x97
#define PGC_MDORG 0x91
#define PGC_MDROTX 0x93
#define PGC_MDROTY 0x94
#define PGC_MDROTZ 0x95
#define PGC_MDSCAL 0x92
#define PGC_MDTRAN 0x96
#define PGC_MOVE 0x10
#define PGC_MOVER 0x11
#define PGC_MOVE3 0x12
#define PGC_MOVER3 0x13
#define PGC_POINT 0x08
#define PGC_POINT3 0x09
#define PGC_POLY 0x30
#define PGC_POLYR 0x31
#define PGC_POLY3 0x32
#define PGC_PRMFIL 0xE9
#define PGC_PROJCT 0xB0
#define PGC_RECT 0x34
#define PGC_RECTR 0x35
#define PGC_RESETF 0x04
#define PGC_SCAN 0x5F
#define PGC_SECTOR 0x3D
#define PGC_TANGLE 0x82
#define PGC_TDEFIN 0x84
#define PGC_TEXT 0x80
#define PGC_TEXTP 0x83
#define PGC_TJUST 0x85
#define PGC_TSIZE 0x81
#define PGC_VWIDEN 0xA0
#define PGC_VWMATX 0xA7
#define PGC_VWPORT 0xB2
#define PGC_VWRPT 0xA1
#define PGC_VWROTX 0xA3
#define PGC_VWROTY 0xA4
#define PGC_VWROTZ 0xA5
#define PGC_WAIT 0x05
#define PGC_WINDOW 0xB3

#endif