/*
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

Adaption to PGCLIB Copyright (c) 2025 trguhq

Same zLib license.

*/

/*************************** INTRODUCTION *********************************/
/* This is a rather rough-and-ready BMP loader for the IBM Professional 
 * Graphics Controller. It supports 256-colour BMP files with no compression.
 *
 * ASCII mode is used to set up the controller; binary mode is used for speed
 * when drawing the bitmap.
 */

 #include <stdio.h>
 #include <string.h>
 #include <dos.h>
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
 
 int gl_pgc_present;
 
 /* Bitmap structures */
 typedef struct
 {
     char bfType[2];
     long bfSize;
     long bfReserved;
     long bfOffBits;
 } BITMAPFILEHEADER;
 
 typedef struct
 {
     long biSize;
     long biWidth;
     long biHeight;
     short biPlanes;
     short biBitCount;
     long  biCompression;
     long  biSizeImage;
     long  biXPelsPerMeter;
     long  biYPelsPerMeter;
     long  biClrUsed;
     long  biClrImportant;
 } BITMAPIMAGEHEADER;
 
 byte palette[1024];
 
 static BITMAPFILEHEADER  bmfh;
 static BITMAPIMAGEHEADER bmih;
 
  /* C Programmer's Disease: Allocating fixed-length buffers rather than 
  * doing proper dynamic memory sizing */
 static char cmdbuf[256];
 static byte monsterbuf[3000];
 static byte pic_line[1024];
 static int gl_monstercount;
  
 void wrword(byte *buf, unsigned short v)
 {
     buf[0] = v & 0xFF;
     buf[1] = (v >> 8) & 0xFF;
 }
  
 /* Write the start of the IMAGEW command. */
 void prelude(long y)
 {
     monsterbuf[0] = 0xD9;	/* IMAGEW */
     wrword(monsterbuf+1, bmih.biHeight - y);
     wrword(monsterbuf+3, 0);
     wrword(monsterbuf+5, bmih.biWidth - 1);
     gl_monstercount = 7;
 }
 
  /* If the next 3 bytes in the line are the same, return nonzero; else zero */
 int adjacent3(int x)
 {
     if (x >= (bmih.biWidth - 2)) return 0;
     return ((pic_line[x]   == pic_line[x+1]) &&
             (pic_line[x+1] == pic_line[x+2]));
 }
 
 /* If the next 2 bytes in the line are the same, return nonzero; else zero */
 int adjacent(int x)
 {
     if (x >= (bmih.biWidth - 1)) return 0;
     return (pic_line[x] == pic_line[x+1]);
 }
  
 /* Bitmap viewer main loop */
 int main(int argc, char **argv)
 {
     long y,x;
     int wb;
     int n;
     int blockstart;
     FILE *fp = fopen(argv[1], "rb");
     if (!fp)
     {
         perror(argv[1]);
         return 1;
     }
     fread(&bmfh, 1, sizeof(bmfh), fp);
     if (bmfh.bfType[0] != 'B' || bmfh.bfType[1] != 'M')
     {
         fprintf(stderr, "%s: Not a BMP file\n", argv[1]);
         fclose(fp);
         return 1;
     }
     fread(&bmih, 1, sizeof(bmih), fp);
     if (bmih.biSize != sizeof(bmih) || bmih.biPlanes != 1 ||
         bmih.biBitCount != 8 || bmih.biCompression != 0)
     {
         fprintf(stderr, "Unsupported BMP file %s\n", argv[1]);
         fclose(fp);
         return 1;
     }
     fread(palette, 1, sizeof(palette), fp);
     fseek(fp, bmfh.bfOffBits, 0);
 
     gl_pgc_present = 1; /* pgc_present(); */
     if (!pgc_init())
     {
        fprintf(stderr, "Could not initialize PGC.");
        fclose(fp);
        return 1;
     };

     pgc_mode_cga(FALSE);
     pgc_mode_hex();

 /* Load the palette */
     for (n = 0; n < 256; n++)
     {
        pgc_lut(n, palette[4*n+2] / 16,
                    palette[4*n+1] / 16,
                    palette[4*n]   / 16);
     }
 
 /* Switch the PGC screen in */
     wb = (bmih.biWidth + 3) & 0xFFFC;
     for (y = bmih.biHeight; y > 0; y--)
     {
         prelude(y);
 
         fread(pic_line, 1, wb, fp);
         for (x = 0; x < bmih.biWidth;)
         {
 /* If the next 3 bytes are not all the same, output a block of bytes 
  * as-are. If they are all the same, output a compressed sequence. */
             if (!adjacent3(x))
             {
                 blockstart = gl_monstercount++;
                 monsterbuf[blockstart] = 0x7F;	
                 while (monsterbuf[blockstart] != 0xFF &&
                        !adjacent3(x) && x < bmih.biWidth) 
                 {
                     monsterbuf[gl_monstercount++] 
                         = pic_line[x];
                     ++monsterbuf[blockstart];
                     ++x;	
                 }
                 continue;
             }
             else /* adjacent3(x) */
             {
                 blockstart = gl_monstercount++;
                 monsterbuf[blockstart] = 0;
                 monsterbuf[gl_monstercount++] = pic_line[x];
                 while (monsterbuf[blockstart] != 0x7F &&
                         adjacent(x) &&
                         x < bmih.biWidth)
                 {
                     ++x;	
                     ++monsterbuf[blockstart];
                 }
                 ++x;	
             }
         }
 /* monsterbuf now holds the IW command. Execute it. */
         pgc_write_len(monsterbuf, gl_monstercount);
         gl_monstercount = 0;
 /* Check for user abort. */
         if (kbhit())
         {
             char c = getch();
             if (c == 3 || c == 0x1B) break;
         }
     }
     getch();

 /* Switch back to CGA display */
     pgc_mode_cga(true);
 
     return 0;
 }