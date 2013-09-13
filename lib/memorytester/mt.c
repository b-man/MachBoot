/*
 * memtester version 4
 *
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2007 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#define __version__ "4.0.8"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "types.h"
#include "sizes.h"
#include "tests.h"

#define mlock(args...)
#define munlock(args...)
#define exit return
#define fprintf(fp, args...) printf(args)
#define fflush(args...)

#define EXIT_FAIL_NONSTARTER    0x01
#define EXIT_FAIL_ADDRESSLINES  0x02
#define EXIT_FAIL_OTHERTEST     0x04

struct test tests[] = {
    { "Random Value", test_random_value },
    { "Compare XOR", test_xor_comparison },
    { "Compare SUB", test_sub_comparison },
    { "Compare MUL", test_mul_comparison },
    { "Compare DIV",test_div_comparison },
    { "Compare OR", test_or_comparison },
    { "Compare AND", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { "Walking Ones", test_walkbits1_comparison },
    { "Walking Zeroes", test_walkbits0_comparison },
    { NULL, NULL }
};

#undef _SC_PAGE_SIZE
#define _SC_PAGE_SIZE

#ifdef _SC_VERSION
void check_posix_system(void) {
}
#else
#define check_posix_system()
#endif

#ifdef _SC_PAGE_SIZE
int memtester_pagesize(void) {
    return 4096;
}
#endif

int command_memorytest(int argc, char **argv) {
    ul loops, loop, i;
    size_t pagesize, wantmb, wantbytes, wantbytes_orig, bufsize, halflen, count;
    void volatile *buf, *aligned = (void*)DRAM_BASE;
    ulv *bufa, *bufb;
    int do_mlock = 0, done_mem = 0;
    int exit_code = 0;
    size_t maxbytes = -1; /* addressable memory, in bytes */
    size_t maxmb = (maxbytes >>20) + 1; /* addressable memory, in MB */

    printf("memtester version " __version__ " (%d-bit)\n", UL_LEN);

    check_posix_system();
    pagesize = memtester_pagesize();

    if (argc < 3) {
        fprintf(stderr, "usage: %s [region to test] [mb-of-memory-to-test] [loops]\n", argv[0]);
        exit(EXIT_FAIL_NONSTARTER);
    }
    aligned = (void*)strtoul(argv[1], NULL, 16);
    wantmb = (size_t) strtoul(argv[2], NULL, 0);
    if (wantmb > maxmb) {
	fprintf(stderr, "This system can only address %llu MB.\n", (ull) maxmb);
	exit(EXIT_FAIL_NONSTARTER);
    }
    wantbytes_orig = wantbytes = (size_t) (wantmb << 20);
    if (wantbytes < pagesize) {
        fprintf(stderr, "bytes < pagesize -- memory argument too large?\n");
        exit(EXIT_FAIL_NONSTARTER);
    }
    wantbytes_orig += 0;

    if (argc < 4) {
        loops = 1;
    } else {
        loops = strtoul(argv[3], NULL, 10);
    }

    printf("want %lluMB (%llu bytes)\n", (ull) wantmb, (ull) wantbytes);
    buf = NULL;

    while (!done_mem) {
        while (!buf && wantbytes) {
            buf = (void volatile *) malloc(wantbytes);
            if (!buf) wantbytes -= pagesize;
        }
        bufsize = wantbytes;
        fflush(stdout);
            done_mem = 1;
            printf("\n"); 
    }


    halflen = bufsize / 2;
    count = halflen / sizeof(ul);
    bufa = (ulv *) aligned;
    bufb = (ulv *) ((size_t) aligned + halflen);

    for(loop=1; ((!loops) || loop <= loops); loop++) {
        printf("Loop %lu", loop);
        if (loops) {
            printf("/%lu", loops);
        }
        printf(":\n");
        printf("  %-20s: ", "Stuck Address");
        fflush(stdout);
        if (!test_stuck_address(aligned, bufsize / sizeof(ul))) {
             printf("ok\n");
        } else {
            exit_code |= EXIT_FAIL_ADDRESSLINES;
        }
        for (i=0;;i++) {
            if (!tests[i].name) break;
            printf("  %-20s: ", tests[i].name);
            if (!tests[i].fp(bufa, bufb, count)) {
                printf("ok\n");
            } else {
                exit_code |= EXIT_FAIL_OTHERTEST;
            }
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    if (do_mlock) munlock((void *) aligned, bufsize);
    printf("Done.\n");
    fflush(stdout);
    return(exit_code);
}