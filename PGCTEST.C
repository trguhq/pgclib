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

PGCTEST:

Copyright (c) 2025 trguhq

Same zLib license.
*/

#include "stdio.h"

#include "PGCLIB.H"
#include "PGCTEST.H"

int main(int argc, char** argv)
{
    int result;

    printf("PGCTEST with PGCLIB\n");
    printf("PGCTEST version %i.%i, PGCLIB version %i.%i\n",
        PGCTEST_VERSION_MAJOR, PGCTEST_VERSION_MINOR,
        pgc_version_major(), pgc_version_minor());
    printf("PGC init: ");
    result = pgc_init();
    printf("%s\n", (result ? "success" : "fail"));
    printf("Firmware version: %i\n", pgc_get_firmware_ver());
    printf("PGC self test: ");
    result = pgc_selftest_pass();
    printf("%s\n", (result ? "success" : "fail"));
    printf("PGC low ROM test: ");
    result = pgc_selftest_rom_low_pass();
    printf("%s\n", (result ? "success" : "fail"));
    printf("PGC high ROM test: ");
    result = pgc_selftest_rom_high_pass();
    printf("%s\n", (result ? "success" : "fail"));
    printf("PGC RAM test: ");
    result = pgc_selftest_ram_pass();
    printf("%s\n", (result ? "success" : "fail"));
    printf("Free memory: %i", pgc_flagrd_free_mem());
    printf("CGA mode available: ");
    result = pgc_get_cga_mode_avail();
    printf("%s\n", (result ? "true" : "false"));

    return 0;
}
