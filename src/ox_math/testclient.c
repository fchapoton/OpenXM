/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/testclient.c,v 1.3 1999/11/06 21:39:37 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "ox.h"

ox_file_t sv;

int dumpx(int fd, int n)
{
    unsigned char buff[2048]; 
    int i;
    int len = read(fd, buff, n);

    fprintf(stderr, "I have read %d byte from socket.\n", len);
    for(i = 0; i < len; i++) {
        fprintf(stderr, "%02x ", buff[i]);
        if (i%20 == 19) {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
    return len;
}

/* ʿ��11ǯ11��7�� */
#define VERSION 0x11110700
#define ID_STRING  "testclient version 0.11110700"

int prompt()
{
    printf("> ");
}

int test_0()
{
    cmo* c = NULL;

	ox_mathcap(sv);
	send_ox_cmo(sv->stream, make_mathcap_object(VERSION, ID_STRING));

    ox_reset(sv);
    send_ox_cmo(sv->stream, new_cmo_string("N[ArcTan[1]]"));
    send_ox_command(sv->stream, SM_executeStringByLocalParser);
    send_ox_command(sv->stream, SM_popCMO);
    receive_ox_tag(sv->stream);
    c = receive_cmo(sv->stream);
    fprintf(stderr, "testclient:: cmo received.\n");
    print_cmo(c);
}

int main(int argc, char* argv[])
{
    ox* m = NULL;
    cmo* c = NULL;
    int code;
    char *server = "ox_sm1";

    setbuf(stderr, NULL);

    if (argc>1) {
        server = argv[1];
    }
    fprintf(stderr, "testclient:: I use %s as an OX server.\n", server);
    sv = ox_start("localhost", "ox", server); 
    if (sv == NULL) {
        fprintf(stderr, "testclient:: I cannot connect to servers.\n");
        exit(1);
    }

    if (strcmp(argc, "ox_math")==0) {
		test_0();
    }

    while(prompt(), (m = parse()) != NULL) {
        send_ox(sv, m);
        if (m->tag == OX_COMMAND) {
            code = ((ox_command *)m)->command;
            if (code >= 1024) {
                break;
            }else if (code == SM_popCMO || code == SM_popString) {
                receive_ox_tag(sv->stream);
                c = receive_cmo(sv->stream);
                fprintf(stderr, "testclient:: cmo received.\n");
                print_cmo(c);
            }
        }
    }

    ox_reset(sv);
    fprintf(stderr, "The testclient resets.\n");
    ox_close(sv);
    fprintf(stderr, "The testclient halts.\n");

    return 0;
}
