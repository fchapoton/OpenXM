/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/bconv.c,v 1.3 1999/11/02 21:15:02 ohara Exp $ */

/* 
OX  expression -> バイト列
CMO expression -> バイト列
コンバータ
*/

#include <stdio.h>
#include <stdlib.h>
#include "ox.h"
#include "parse.h"

static int display(ox *m)
{
    int i;
    int len = 0;
    unsigned char* d_buff;

    switch(m->tag) {
    case OX_DATA:
        len = sizeof(int) + sizeof(int) + cmolen_cmo(((ox_data *)m)->cmo);
        d_buff = malloc(len);
        init_dump_buffer(d_buff);
        dump_ox_data((ox_data *)m);
        break;
    case OX_COMMAND:
        len = sizeof(int) + sizeof(int) + sizeof(int);
        d_buff = malloc(len);
        init_dump_buffer(d_buff);
        dump_ox_command((ox_command *)m);
        break;
    default:
        len = cmolen_cmo((cmo *)m);
        d_buff = malloc(len);
        init_dump_buffer(d_buff);
        dump_cmo((cmo *)m);
    }

    for(i=0; i<len; i++) {
        fprintf(stdout, "%02x ", d_buff[i]);
        if(i%20 == 19) {
            fprintf(stdout, "\n");
        }
    }
    if(i%20 != 19) {
        fprintf(stdout, "\n");
    }
    free(d_buff);
}

#define SIZE_CMDLINE  8192

static int  size = SIZE_CMDLINE;
static char cmdline[SIZE_CMDLINE];

static int prompt()
{
    fprintf(stdout, "> ");
    fgets(cmdline, size, stdin);
    setmode_mygetc(cmdline, size);
}

int main()
{
    cmo *m;
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);

    setflag_parse(PFLAG_ADDREV);
    setgetc(mygetc);

    for(prompt(); (m = parse()) != NULL; prompt()) {
        display(m);
    }
    return 0;
}

