/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/oxf_old.c,v 1.2 2000/10/12 15:53:25 ohara Exp $ */

/* ���Υ⥸�塼��ϸߴ����Τ���Τ�ΤǤ���*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/param.h>
#include <time.h>

#include "mysocket.h"
#include "ox_toolkit.h"

OXFILE *oxf_control_set(OXFILE *oxfp, OXFILE *ctl)
{
	oxfp->control = ctl;
	return oxfp;
}

static char *concat_openxm_home_bin(char *s);

OXFILE *         ox_start(char* host, char* prog1, char* prog2);
OXFILE *         ox_start_insecure_nonreverse(char* host, short portControl, short portStream);
OXFILE *         ox_start_remote_with_ssh(char *dat_prog, char* host);

#if 0
/* ����ײ�: start �ؿ��ϼ��Τ褦���߷פǤ���٤����� */

OXFILE *oxf_connect_client()
{
    short port   = 0; /* Listen() �˷�ᤵ����. */
    int listened = Listen(&port);

    execute_server(port);
    return oxf_connect_passive(listened); /* �����ǡ�ǧ��, �Х��ȥ������η��� */
}

OXFILE *ox_start_calc(char *cserver)
{
    short port   = 0; /* Listen() �˷�ᤵ����. */
    int listened;

    listened = Listen(&port);
    oxc_open(oxfp_ctl, port, cserver); /* ������password�������⤢�뤬... */
    return oxf_connect_passive(listened); /* ǧ�ڤ��ʤ�. �Х��ȥ������Ϸ��ꤹ��. */

}

int ox_start()
{
    OXFILE *oxfp_ctl, *oxfp_calc;

    oxfp_ctl = oxf_connect_client();
    /* mathcap �θ� */
    oxfp_calc = ox_start_calc("ox_sm1");
    return oxf_control_set(oxfp_calc, oxfp_ctl);
}
#endif

static OXFILE *mysocketAccept2(int listened, char *passwd)
{
    OXFILE *oxfp = oxf_connect_passive(listened);
    if(oxf_confirm_client(oxfp, passwd)) {
        oxf_determine_byteorder_client(oxfp);
        return oxfp;
    }
    oxf_close(oxfp);
    return NULL;
}

/* The environment variable OpenXM_HOME must be defined. */
static char *concat_openxm_home_bin(char *s)
{
    char *path;
    char *base;

    /* if s includes '/' then it is not concaticated. */
    if (strchr(s, '/') != NULL) {
        return s;
    }

    base = getenv("OpenXM_HOME");
    path = malloc(strlen(base)+6+strlen(s));
    sprintf(path, "%s/bin/%s", base, s);
    return path;
}

void set_OpenXM_HOME()
{
    /* Solaris does not have the setenv(). */
    if (getenv("OpenXM_HOME") == NULL) {
        putenv("OpenXM_HOME=/usr/local/OpenXM");
    }
}

void ox_exec_local(char* ctl_prog, char* dat_prog, int portControl, int portStream, char *passwd)
{
    char  ctl[128], dat[128];
    char localhost[MAXHOSTNAMELEN];


    sprintf(ctl, "%d", portControl);
    sprintf(dat, "%d", portStream);

    set_OpenXM_HOME();
    ctl_prog = concat_openxm_home_bin(ctl_prog);
    dat_prog = concat_openxm_home_bin(dat_prog);

    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
		if (fork() == 0) {
			execlp("oxlog", "oxlog", "xterm", "-icon", "-e", ctl_prog,
				   "-reverse", "-ox", dat_prog,
				   "-data", dat, "-control", ctl, "-pass", passwd,
				   "-host", localhost, NULL);
			exit(1);
		}
	}
}

/*
   (-reverse �Ǥ� ox_start)
   ox_start �� ���饤����Ȥ��ƤӽФ�����δؿ��Ǥ���.
   �����ФǤϻȤ��ʤ�.  ctl_prog �ϥ���ȥ��륵���ФǤ���,
   -ox, -reverse, -data, -control, -pass, -host
   �Ȥ������ץ��������򤹤뤳�Ȥ��ꤹ��. dat_prog �Ϸ׻������ФǤ���.
   ��³���ˤ�, ct ����˥����ץ󤹤�.
*/

OXFILE *ox_start(char* host, char* ctl_prog, char* dat_prog)
{
    OXFILE *st, *ct;
    char *passwd = generate_otp();
    int   listen[2];
    int   ports[2] = {0};  /* short! */

	/* host ��̵�� */
    listen[0] = oxf_listen(&ports[0]);
    listen[1] = oxf_listen(&ports[1]);

    ox_exec_local(ctl_prog, dat_prog, ports[0], ports[1], passwd);
    ct = mysocketAccept2(listen[0], passwd);
    if (ct != NULL) {
        usleep(10);   /* zzz... */
        st = mysocketAccept2(listen[1], passwd);
        if (st != NULL) {
            return oxf_control_set(st, ct);
        }
    }
    return NULL;
}

/*
   (-insecure �Ǥ� ox_start)  �ޤ�����ȤϤ���ޤ���
   ox_start_insecure �� ���饤����Ȥ��ƤӽФ�����δؿ��Ǥ���.
   ��³���ˤ�, ct ����˥����ץ󤹤�.
   ������:
   portControl = 1200
   portStream  = 1300
*/

OXFILE *ox_start_insecure(char* host, short portControl, short portStream)
{
    OXFILE *ct, *st;
    ct = oxf_connect_active(host, portControl);
    /* ox �� insecure �ΤȤ� byte order �η��꤬�������Ǥ��ʤ��褦��... */
    /* oxf_determine_byteorder_client(ct); */
    /* wainting 10 micro second. */
    usleep(10);
    st = oxf_connect_active(host, portStream);
    oxf_determine_byteorder_client(st);
    return oxf_control_set(st, ct);
}

/* ssh -f host oxlog xterm -e ox -ox ox_asir ... */
void ssh_ox_server(char *remote_host, char *ctl_prog, char *dat_prog, short portControl, short portStream)
{
    ctl_prog = concat_openxm_home_bin(ctl_prog);
    dat_prog = concat_openxm_home_bin(dat_prog);

    if (fork() == 0) {
        execlp("ssh", "ssh", "-f", remote_host, "oxlog", "xterm", "-icon",
              "-e", ctl_prog, "-insecure", "-ox", dat_prog,
              "-data", portStream, "-control", portControl,
              "-host", remote_host, NULL);
        exit(1);
    }
}

OXFILE *ox_start_remote_with_ssh(char *dat_prog, char* remote_host)
{
    ssh_ox_server(remote_host, "ox", dat_prog, 1200, 1300);
    return ox_start_insecure(remote_host, 1200, 1300);
}

