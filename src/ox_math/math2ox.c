/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/math2ox.c,v 1.2 1999/11/02 06:11:57 ohara Exp $ */
/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <mathlink.h>
#include <unistd.h>
#include <signal.h>

#include "ox.h"
#include "parse.h"

static char *host    = "localhost";
static char *ctlserv = "ox";
static char *oxprog  = "ox_sm1";

ox_file_t sv;

/* Mathematica ����ľ�ܸƤӽФ����ؿ������. */
/* �ƤӽФ�����ˡ�� math2ox.tm ����������.   */
int OX_executeStringByLocalParser(const char *str)
{
    ox_executeStringByLocalParser(sv, str);
    return 0;
}

char *OX_popString()
{
    return ox_popString(sv, sv->stream);
}

int OX_close()
{
    ox_close(sv);
    return 0;
}

int OX_reset()
{
    ox_reset(sv);
    return 0;
}

/* ʸ���� s �� parse() �ˤ������������줿 cmo �� �����Ф�����. */
/* s �� "...\n" �η��Ǥʤ���Фʤ�ʤ�(??). */
int OX_parse(char *s)
{
    cmo *m;
	int len = strlen(s);
    setmode_mygetc(s, len);

    if(s != NULL && len > 0 && (m = parse()) != NULL) {
		/* ���� m->tag �Υ����å��򤷤� CMO �Ǥ��뤳�Ȥ�
           �Τ���ʤ���Фʤ�ʤ�. */
		send_ox_cmo(sv->stream, m);		
		return 0;
	}
	return -1; /* ���Ԥ������ */
}

int OX_start(char* s)
{
    if (s != NULL && s[0] != '\0') {
        oxprog = s;
    }
    sv = ox_start(host, ctlserv, oxprog);
    fprintf(stderr, "open (%s)\n", "localhost");
    return 0;
}

static char *cp_str(char *src)
{
    char *dest = malloc(strlen(src)+1);
    strcpy(dest, src);
    return dest;
}

int OX_setClientParam(char *h, char* c, char* p)
{
    host    = cp_str(h);
    ctlserv = cp_str(c);
    oxprog  = cp_str(p);
    return 0;
}

int main(int argc, char *argv[])
{
	/* ��ʸ���ϴ������ */
	setflag_parse(PFLAG_ADDREV);
    setgetc(mygetc);

    MLMain(argc, argv);
}
