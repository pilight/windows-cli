/*
	Copyright (C) 2013 CurlyMo

	This file is part of the Raspberry Pi 433.92Mhz transceiver.

    Raspberry Pi 433.92Mhz transceiver is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.

    Raspberry Pi 433.92Mhz transceiver is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Raspberry Pi 433.92Mhz transceiver. If not, see
	<http://www.gnu.org/licenses/>
*/

#include "mingw.h"
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "gc.h"

#ifdef _WIN32
#define sigjmp_buf jmp_buf
#define sigsetjmp(env, savemask) setjmp(env)
#define siglongjmp(env, val) longjmp(env, val)
#define sigemptyset(x) (void)0
#endif

static sigjmp_buf gc_cleanup;

/* The gc uses a observer pattern to
   easily call function when exiting
   the daemon */

void gc_handler(int sig) {
    siglongjmp(gc_cleanup, sig);
}

/* Removed function from GC */
void gc_detach(int (*fp)(void)) {
	unsigned i;

	for(i=0; i<gc.nr; ++i) {
		if(gc.listeners[i] == fp) {
			gc.nr--;
			gc.listeners[i] = gc.listeners[gc.nr];
		}
	}
}

/* Add function to gc */
void gc_attach(int (*fp)(void)) {
	gc_detach(fp);
	gc.listeners[gc.nr++] = fp;
}

/* Initialize the catch all gc */
void gc_catch(void) {

    unsigned int i, s;

    if(sigsetjmp(gc_cleanup, 0) == 0)
		return;

	/* Call all GC functions */
	for(i=0; i<gc.nr; ++i) {
		if(gc.listeners[i]() != 0) {
			s=1;
		}
	}
	if(s)
		exit(255);
	else
		exit(0);
}
