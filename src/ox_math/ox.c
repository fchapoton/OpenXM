/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/ox.c,v 1.11 1999/11/06 21:39:36 ohara Exp $ */

/*
�ؿ���̾���դ�����(����2):
(1) receive_cmo �ؿ���CMO�����ȥǡ������Τ��������. ���δؿ��� CMO������
�ͤ�������ʬ����ʤ��Ȥ��˻��Ѥ���. �֤��ͤȤ��ơ�cmo �ؤΥݥ��󥿤��֤�.
(2) receive_cmo_XXX �ؿ���, CMO������Ƥδؿ��Ǽ������Ƥ���ƤӽФ�����
���ǡ��ǡ������ΤΤߤ��������cmo_XXX �ؤΥݥ��󥿤��֤�.  �����⡢
�ؿ������� new_cmo_XXX �ؿ���ƤӽФ�.
(3) send_cmo �ؿ���CMO�����ȥǡ������Τ���������.
(4) send_cmo_XXX �ؿ���CMO������Ƥδؿ����������Ƥ���ƤӽФ����ؿ��ǡ�
�ǡ������ΤΤߤ���������.

----
(5) receive_ox_XXX �ؿ���¸�ߤ��ʤ�(���ʤ�).  receive_cmo �����Ѥ���.
(6) send_ox_XXX �ؿ��� OX ������ޤ����������.
(7) ox_XXX �ؿ��ϰ�Ϣ����������ޤ������Ū������ɽ������.
ox_XXX �ؿ��ϡ��������Ȥ��ơ�ox_file_t�����ѿ� sv ��Ȥ�.

(8) YYY_cmo �ؿ��� YYY_cmo_XXX �ؿ��δط��ϼ����̤�:
�ޤ� YYY_cmo �ؿ��� cmo �Υ������������������������Ĥ����ʬ��
YYY_cmo_XXX �ؿ�����������.  cmo �������� cmo_ZZZ �ؤΥݥ��󥿤�
����Ȥ��ˤϡ����μ���ˤ�餺�� YYY_cmo �ؿ���ƤӽФ�.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <gmp.h>
#include <unistd.h>
#include <sys/file.h>

#include "mysocket.h"
#include "ox.h"
#include "parse.h"

static int          cmolen_cmo_int32(cmo_int32* c);
static int          cmolen_cmo_list(cmo_list* c);
static int          cmolen_cmo_mathcap(cmo_mathcap* c);
static int          cmolen_cmo_null(cmo_null* c);
static int          cmolen_cmo_string(cmo_string* c);
static int          cmolen_cmo_zz(cmo_zz* c);
static int          cmolen_cmo_monomial32(cmo_monomial32* c);

static int          dump_cmo_int32(cmo_int32* m);
static int          dump_cmo_list(cmo_list* m);
static int          dump_cmo_mathcap(cmo_mathcap* m);
static int          dump_cmo_null(cmo_null* m);
static int          dump_cmo_string(cmo_string* m);
static int          dump_cmo_monomial32(cmo_monomial32* c);
static int          dump_cmo_zz(cmo_zz* c);
static int          dump_string(char *s, int len);
static int          dump_integer(int x);
static int          dump_mpz(mpz_ptr mpz);

static int          funcs(int cmo_type);

static int          login_with_otp(int fd, char* passwd);
static char         *create_otp();

/* CMO_xxx ���ͽ�ˤʤ�٤뤳��(�ǥХå��Τ���) */
static cmo_null*         receive_cmo_null(int fd);
static cmo_int32*        receive_cmo_int32(int fd);
static cmo_string*       receive_cmo_string(int fd);
static cmo_mathcap*      receive_cmo_mathcap(int fd);
static cmo_list*         receive_cmo_list(int fd);
static cmo_monomial32*   receive_cmo_monomial32(int fd);
static cmo_zz*           receive_cmo_zz(int fd);
static cmo_zero*         receive_cmo_zero(int fd);
static cmo_dms_generic*  receive_cmo_dms_generic(int fd);
static cmo_ring_by_name* receive_cmo_ring_by_name(int fd);
static cmo_distributed_polynomial* receive_cmo_distributed_polynomial(int fd);

static cmo_error2*       receive_cmo_error2(int fd);
static void              receive_mpz(int fd, mpz_ptr mpz);

static int          send_cmo_null(int fd, cmo_null* c);
static int          send_cmo_int32(int fd, cmo_int32* m);
static int          send_cmo_string(int fd, cmo_string* m);
static int          send_cmo_mathcap(int fd, cmo_mathcap* c);
static int          send_cmo_list(int fd, cmo_list* c);
static int          send_cmo_monomial32(int fd, cmo_monomial32* c);
static int          send_cmo_zz(int fd, cmo_zz* c);
static int          send_cmo_error2(int fd, cmo_error2* c);
static int          send_mpz(int fd, mpz_ptr mpz);
static int          send_cmo_distributed_polynomial(int fd, cmo_distributed_polynomial* c);

/* ���顼�ϥ�ɥ�󥰤Τ��� */
static int current_received_serial = 0;

/* ���顼�򵯤������Ȥ��˥����Фϼ���ƤӽФ�.  */
cmo_error2* make_error_object(int err_code, cmo *ob)
{
    cmo_list* li = new_cmo_list();
    append_cmo_list(li, (cmo *)new_cmo_int32(current_received_serial));
    append_cmo_list(li, (cmo *)new_cmo_int32(err_code));
    append_cmo_list(li, ob);
    /* ¾�ξ����ä���ʤ饳�� */
    return new_cmo_error2((cmo *)li);
}

/* add at Mon Sep  7 15:51:28 JST 1998 */
#define DEFAULT_SERIAL_NUMBER 0x0000ffff
#define receive_serial_number(x)   (receive_int32(x))

/* ���������ꥢ���ֹ������ */
int next_serial()
{
    static int serial_number = DEFAULT_SERIAL_NUMBER;
    return serial_number++;
}

/* int32 ���Υ��֥������Ȥ���������.  */
int send_int32(int fd, int int32)
{
    int32 = htonl(int32);
    return write(fd, &int32, sizeof(int));
}

/* int32 ���Υ��֥������Ȥ��������.  */
int receive_int32(int fd)
{
    int tag;
    read(fd, &tag, sizeof(int));
    return ntohl(tag);
}

/* (OX_tag, serial number) ���������.  */
int receive_ox_tag(int fd)
{
    int serial;
    int tag = receive_int32(fd);
    current_received_serial = receive_serial_number(fd);
    return tag;
}

/* (OX_tag, serial number) ����������.   */
int send_ox_tag(int fd, int tag)
{
    send_int32(fd, tag);
    return send_int32(fd, next_serial());
}

/* CMO_LIST �ط��δؿ��� */
cell* new_cell()
{
    cell* h = malloc(sizeof(cell));
    h->next = NULL;
    h->cmo  = NULL;
    return h;
}

cell* next_cell(cell* this)
{
    return this->next;
}

static cell *tail(cmo_list* this) {
    cell *cp = this->head;
    while (cp->next != NULL) {
        cp = cp->next;
    }
    return cp;
}

int append_cmo_list(cmo_list* this, cmo* newcmo)
{
    cell *cp = tail(this);
    cp->cmo  = newcmo;
    cp->next = new_cell();
    this->length++;
    return 0;
}

int length_cmo_list(cmo_list* this)
{
    return this->length;
}

/** receive_cmo_XXX �ؿ��� **/
static cmo_null* receive_cmo_null(int fd)
{
    return new_cmo_null();
}

static cmo_int32* receive_cmo_int32(int fd)
{
    int i = receive_int32(fd);
    return new_cmo_int32(i);
}

static cmo_string* receive_cmo_string(int fd)
{
    int len = receive_int32(fd);
    char* s = malloc(len+1);
    memset(s, '\0', len+1);
    if (len > 0) {
        read(fd, s, len);
    }
    return new_cmo_string(s);
}

static cmo_mathcap* receive_cmo_mathcap(int fd)
{
    cmo* ob = receive_cmo(fd);
    return new_cmo_mathcap(ob);
}

static cmo_list* receive_cmo_list(int fd)
{
    cmo* ob;
    cmo_list* c = new_cmo_list();
    int len = receive_int32(fd);

    while (len>0) {
        ob = receive_cmo(fd);
        append_cmo_list(c, ob);
        len--;
    }
    return c;
}

static cmo_monomial32* receive_cmo_monomial32(int fd)
{
    int i;
    int len = receive_int32(fd);
    cmo_monomial32* c = new_cmo_monomial32(len);

    for(i=0; i<len; i++) {
        c->exps[i] = receive_int32(fd);
    }
    c->coef = receive_cmo(fd);
    return c;
}

static cmo_zz* receive_cmo_zz(int fd)
{
    cmo_zz* c = new_cmo_zz();
    receive_mpz(fd, c->mpz);
    return c;
}

static cmo_zero* receive_cmo_zero(int fd)
{
    return new_cmo_zero();
}

static cmo_dms_generic* receive_cmo_dms_generic(int fd)
{
    return new_cmo_dms_generic();
}

static cmo_ring_by_name* receive_cmo_ring_by_name(int fd)
{
    cmo* ob = receive_cmo(fd);
    /* ��̣Ū�����å���ɬ�� */
    return new_cmo_ring_by_name(ob);
}

static cmo_distributed_polynomial* receive_cmo_distributed_polynomial(int fd)
{
    cmo* ob;
    cmo_distributed_polynomial* c = new_cmo_distributed_polynomial();
    int len = receive_int32(fd);
    c->ringdef = receive_cmo(fd);

    while (len>0) {
        ob = receive_cmo(fd);
        append_cmo_list(c, ob);
        len--;
    }
    return c;
}

static cmo_error2* receive_cmo_error2(int fd)
{
    cmo* ob = receive_cmo(fd);
    return new_cmo_error2(ob);
}

/* receive_ox_tag() == OX_DATA �θ�˸ƤӽФ���� */
/* �ؿ��ݥ��󥿤�Ȥä��������줤�˽񤱤�褦�ʵ�������.  */
/* if (foo[tag] != NULL) foo[tag](fd); �Ȥ� */

cmo* receive_cmo(int fd)
{
    cmo* m;
    int tag;
    tag = receive_int32(fd);

    switch(tag) {
    case CMO_NULL:
        m = receive_cmo_null(fd);
        break;
    case CMO_INT32:
        m = (cmo *)receive_cmo_int32(fd);
        break;
    case CMO_STRING:
        m = (cmo *)receive_cmo_string(fd);
        break;
    case CMO_MATHCAP:
        m = (cmo *)receive_cmo_mathcap(fd);
        break;
    case CMO_LIST:
        m = (cmo *)receive_cmo_list(fd);
        break;
    case CMO_MONOMIAL32:
        m = (cmo *)receive_cmo_monomial32(fd);
        break;
    case CMO_ZZ:
        m = (cmo *)receive_cmo_zz(fd);
        break;
    case CMO_ZERO:
        m = (cmo *)receive_cmo_zero(fd);
        break;
    case CMO_DMS_GENERIC:
        m = (cmo *)receive_cmo_dms_generic(fd);
        break;
    case CMO_RING_BY_NAME:
        m = (cmo *)receive_cmo_ring_by_name(fd);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        m = (cmo *)receive_cmo_distributed_polynomial(fd);
        break;
    case CMO_ERROR2:
        m = (cmo *)receive_cmo_error2(fd);
        break;
    case CMO_DATUM:
    case CMO_QQ:
    default:
        fprintf(stderr, "unknown cmo-type: tag = (%d)\n", m->tag);
    }
    return m;
}

static void receive_mpz(int fd, mpz_ptr mpz)
{
    int i;
    int size  = receive_int32(fd);
    int len   = abs(size);
    resize_mpz(mpz, size);

    for(i=0; i<len; i++) {
        mpz->_mp_d[i] = receive_int32(fd);
    }
}

void resize_mpz(mpz_ptr mpz, int size)
{
    _mpz_realloc(mpz, abs(size));
    mpz->_mp_size = size;
}

/** new_cmo_XXX �ؿ��� **/
cmo_null* new_cmo_null()
{
    cmo_null* m = malloc(sizeof(cmo_null));
    m->tag = CMO_NULL;
    return m;
}

cmo_int32* new_cmo_int32(int i)
{
    cmo_int32* c;
    c = malloc(sizeof(cmo_int32));
    c->tag     = CMO_INT32;
    c->i = i;
    return c;
}

cmo_string* new_cmo_string(char* s)
{
    cmo_string* c = malloc(sizeof(cmo_string));
    c->tag = CMO_STRING;
    if (s != NULL) {
        c->s = malloc(strlen(s)+1);
        strcpy(c->s, s);
    }else {
        c->s = NULL;
    }
    return c;
}

cmo_mathcap* new_cmo_mathcap(cmo* ob)
{
    cmo_mathcap* c = malloc(sizeof(cmo_mathcap));
    c->tag = CMO_MATHCAP;
    c->ob  = ob;
    return c;
}

cmo_list* new_cmo_list()
{
    cmo_list* c = malloc(sizeof(cmo_list));
    c->tag    = CMO_LIST;
    c->length = 0;
    c->head->next = NULL;
    return c;
}

cmo_monomial32* new_cmo_monomial32()
{
    cmo_monomial32* c = malloc(sizeof(cmo_monomial32));
    c->tag  = CMO_MONOMIAL32;
    return c;
}

cmo_monomial32* new_cmo_monomial32_size(int size)
{
    cmo_monomial32* c = new_cmo_monomial32();
    if (size>0) {
        c->length = size;
        c->exps = malloc(sizeof(int)*size);
    }
    return c;
}

cmo_zz* new_cmo_zz()
{
    cmo_zz* c = malloc(sizeof(cmo_zz));
    c->tag  = CMO_ZZ;
    mpz_init(c->mpz);
    return c;
}

cmo_zz* new_cmo_zz_noinit()
{
    cmo_zz* c = malloc(sizeof(cmo_zz));
    c->tag  = CMO_ZZ;
    return c;
}

cmo_zz* new_cmo_zz_set_si(int i)
{
    cmo_zz* c = new_cmo_zz();
    mpz_set_si(c->mpz, i);
    return c;
}

cmo_zz *new_cmo_zz_set_string(char *s)
{
    cmo_zz* c = new_cmo_zz_noinit();
    mpz_init_set_str(c->mpz, s, 10);
    return c;
}

cmo_zz* new_cmo_zz_size(int size)
{
    cmo_zz* c = new_cmo_zz();
    resize_mpz(c->mpz, size);
    return c;
}

cmo_zero* new_cmo_zero()
{
    cmo_zero* m = malloc(sizeof(cmo_zero));
    m->tag = CMO_ZERO;
    return m;
}

cmo_dms_generic* new_cmo_dms_generic()
{
    cmo_dms_generic* m = malloc(sizeof(cmo_dms_generic));
    m->tag = CMO_DMS_GENERIC;
    return m;
}

cmo_ring_by_name* new_cmo_ring_by_name(cmo* ob)
{
    cmo_ring_by_name* c = malloc(sizeof(cmo_ring_by_name));
    c->tag = CMO_RING_BY_NAME;
    c->ob  = ob;
    return c;
}

cmo_indeterminate* new_cmo_indeterminate(cmo* ob)
{
    cmo_indeterminate* c = malloc(sizeof(cmo_indeterminate));
    c->tag = CMO_INDETERMINATE;
    c->ob  = ob;
    return c;
}

cmo_distributed_polynomial* new_cmo_distributed_polynomial()
{
    cmo_distributed_polynomial* c = malloc(sizeof(cmo_distributed_polynomial));
    c->tag     = CMO_DISTRIBUTED_POLYNOMIAL;
    c->length  = 0;
    c->head->next = NULL;
    c->ringdef = NULL;
    return c;
}

cmo_error2* new_cmo_error2(cmo* ob)
{
    cmo_error2* c = malloc(sizeof(cmo_error2));
    c->tag = CMO_ERROR2;
    c->ob  = ob;
    return c;
}

void send_ox_command(int fd, int sm_command)
{
    send_ox_tag(fd, OX_COMMAND);
    send_int32(fd, sm_command);
}

int print_cmo(cmo* c)
{
    int tag = c->tag;

#ifdef DEBUG
    symbol* symp = lookup_by_tag(tag);
    if (symp != NULL) {
        fprintf(stderr, "local::tag = %s: ", symp->key);
    }else {     
        fprintf(stderr, "local::tag = %d: ", tag);
    }
#endif

    switch(tag) {
    case CMO_LIST:
        print_cmo_list((cmo_list *)c);
        break;
    case CMO_INT32:
        print_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_MATHCAP:
        print_cmo_mathcap((cmo_mathcap *)c);
        break;
    case CMO_STRING:
        print_cmo_string((cmo_string *)c);
        break;
    case CMO_NULL:
        fprintf(stderr, "\n");
        break;
    default:
        fprintf(stderr, "print_cmo() does not know how to print.\n");
    }
}

int print_cmo_int32(cmo_int32* c)
{
    fprintf(stderr, "cmo_int32 = (%d)\n", c->i);
}

int print_cmo_list(cmo_list* li)
{
    cell* cp = li->head;
    fprintf(stderr, "length = (%d)\nlist:\n", li->length);
    while(cp != NULL) {
        print_cmo(cp->cmo);
        cp=cp->next;
    }
    fprintf(stderr, "end of list\n");
}

int print_cmo_mathcap(cmo_mathcap* c)
{
    fprintf(stderr, "\n");
    print_cmo(c->ob);
}

int print_cmo_string(cmo_string* c)
{
    fprintf(stderr, "cmo_string = (%s)\n", c->s);
}

void ox_close(ox_file_t sv)
{
    send_ox_command(sv->control, SM_control_kill);
#ifdef DEBUG
    sleep(2); /* OpenXM server �ν�λ���Ԥ�. ���ޤ��̣�Ϥʤ�. */
    fprintf(stderr, "I have closed the connection to an Open XM server.\n");
#endif
}

void ox_executeStringByLocalParser(ox_file_t sv, char* s)
{
    if (s != NULL) {
        /* ʸ�����򥹥��å��˥ץå���. */
        send_ox_cmo(sv->stream, (cmo *)new_cmo_string(s));
        /* �����Ф˼¹Ԥ�����. */
        send_ox_command(sv->stream, SM_executeStringByLocalParser);
    }
}

/* ox_mathcap() �򥳡��뤹��.  */
cmo_mathcap* ox_mathcap(ox_file_t sv)
{
    send_ox_command(sv->stream, SM_mathcap);
    send_ox_command(sv->stream, SM_popCMO);
    receive_ox_tag(sv->stream);          /* OX_DATA */
    return (cmo_mathcap *)receive_cmo(sv->stream);
}

char* ox_popString(ox_file_t sv, int fd)
{
    cmo_string* m = NULL;

    send_ox_command(fd, SM_popString);
    receive_ox_tag(fd); /* OX_DATA */
    m = (cmo_string *)receive_cmo(fd);
    return m->s;
}

cmo* ox_pop_cmo(ox_file_t sv, int fd)
{
    send_ox_command(fd, SM_popCMO);
    receive_ox_tag(fd); /* OX_DATA */
    return receive_cmo(fd);
}

/* ��ȴ��. (��ǲ������褦...) */
static char *create_otp()
{
    static char otp[] = "otpasswd";
    return otp;
}

/* OneTimePassword �ν��� */
static int login_with_otp(int fd, char* passwd)
{
    int len   = strlen(passwd)+1;
    char *buf = alloca(len);
    int n     = read(fd, buf, len);
    int ret   = strcmp(passwd, buf);

#ifdef DEBUG
    if (ret != 0) {
        fprintf(stderr, "Socket#%d: Login incorrect.\n", fd);
    }else {
        fprintf(stderr, "Socket#%d: login!.\n", fd);
    }
    fprintf(stderr, "password = (%s), %d bytes.\n", passwd, len);
    fprintf(stderr, "received = (%s), %d bytes.\n", buf, n);
    fflush(stderr);
#endif

    return ret;
}

static int exists_ox(char *dir, char *prog)
{
    char *path = alloca(strlen(dir)+strlen(prog)+6);
    sprintf(path, "%s/%s", dir, prog);
    return access(path, X_OK|R_OK);
}

static char *search_ox(char *prog)
{
    char *env = getenv("OpenXM_HOME");
    char *dir;
    if (env != NULL) {
        dir = malloc(strlen(env)+5);
        sprintf(dir, "%s/bin", env);
        if (exists_ox(dir, prog) == 0) {
            return dir;
        }
        free(dir);
    }
    dir = "/usr/local/OpenXM/bin";
    if (exists_ox(dir, prog) == 0) {
        return dir;
    }
    dir = ".";
    if (exists_ox(dir, prog) == 0) {
        return dir;
    }
    return NULL;
}

static int mysocketAccept2(int fd, char *pass)
{
    fd = mysocketAccept(fd);
    if(login_with_otp(fd, pass)==0) {
        decideByteOrderClient(fd, 0);
        return fd;
    }
    close(fd);
    return -1;
}

/*
   (-reverse �Ǥ� ox_start)
   ox_start �� ���饤����Ȥ��ƤӽФ�����δؿ��Ǥ���.
   �����ФǤϻȤ��ʤ�.  ctl_prog �� ����ȥ��륵���ФǤ���,
   -ox, -reverse, -data, -control, -pass, -host
   �Ȥ������ץ��������򤹤뤳�Ȥ��ꤹ��. dat_prog �Ϸ׻������ФǤ���.
   ��³���ˤ�, sv->control ����˥����ץ󤹤�.
*/

ox_file_t ox_start(char* host, char* ctl_prog, char* dat_prog)
{
    char *pass;
    char ctl[16], dat[16];
    short portControl = 0; /* short �Ǥ��뤳�Ȥ���� */
    short portStream  = 0;
    ox_file_t sv = NULL;
    char *dir;

    if ((dir = search_ox(ctl_prog)) == NULL) {
        fprintf(stderr, "client:: %s not found.\n", ctl_prog);
        return NULL;
    }
    sv = malloc(sizeof(__ox_file_struct));
    sv->control = mysocketListen(host, &portControl);
    sv->stream  = mysocketListen(host, &portStream);

    sprintf(ctl, "%d", portControl);
    sprintf(dat, "%d", portStream);
    pass = create_otp();

    if (fork() == 0) {
        dup2(2, 1);
        dup2(open(DEFAULT_LOGFILE, O_RDWR|O_CREAT|O_TRUNC, 0644), 2);
        chdir(dir);
        execl(ctl_prog, ctl_prog, "-reverse", "-ox", dat_prog,
              "-data", dat, "-control", ctl, "-pass", pass,
              "-host", host, NULL);
    }

    if ((sv->control = mysocketAccept2(sv->control, pass)) == -1) {
        close(sv->stream);
        return NULL;
    }
    /* 10�ޥ�������, ���ֲԤ�����. */
    usleep(10);
    if((sv->stream  = mysocketAccept2(sv->stream, pass)) == -1) {
        return NULL;
    }
    return sv;
}

/*
   (-insecure �Ǥ� ox_start)  �ޤ�����ȤϤ���ޤ���
   ox_start_insecure_nonreverse �� ���饤����Ȥ��ƤӽФ�����δؿ��Ǥ���.
   ��³���ˤ�, sv->control ����˥����ץ󤹤�.
   ������:
   portControl = 1200
   portStream  = 1300
*/

ox_file_t ox_start_insecure_nonreverse(char* host, short portControl, short portStream)
{
    ox_file_t sv = malloc(sizeof(__ox_file_struct));

    sv->control = mysocketOpen(host, portControl);
#if 0
    /* ox �� insecure �ΤȤ� byte order �η��꤬�������Ǥ��ʤ��褦��... */
    decideByteOrderClient(sv->control, 0);
#endif
    /* 10�ޥ�������, ���ֲԤ�����. */
    usleep(10);
    sv->stream  = mysocketOpen(host, portStream);
    decideByteOrderClient(sv->stream, 0);
    return sv;
}

void ox_reset(ox_file_t sv)
{
    send_ox_command(sv->control, SM_control_reset_connection);

    receive_ox_tag(sv->control);      /* OX_DATA */
    receive_cmo(sv->control);         /* (CMO_INT32, 0) */

    while(receive_ox_tag(sv->stream) != OX_SYNC_BALL) {
        receive_cmo(sv->stream); /* skipping a message. */
    }

    send_ox_tag(sv->stream, OX_SYNC_BALL);
#ifdef DEBUG
    fprintf(stderr, "I have reset an Open XM server.\n");
#endif
}

/* �ʲ��� bconv.c ��ɬ�פȤ���ؿ����Ǥ���. */

/* cmolen �ؿ��� cmo ��(��������)�Х���Ĺ���֤�. */
/* cmolen_XXX �ؿ��� cmo_XXX �� tag ��������Х���Ĺ���֤�. */

static int cmolen_cmo_null(cmo_null* c)
{
    return 0;
}

static int cmolen_cmo_int32(cmo_int32* c)
{
    return sizeof(int);
}

static int cmolen_cmo_string(cmo_string* c)
{
    return sizeof(int)+strlen(c->s);
}

static int cmolen_cmo_mathcap(cmo_mathcap* c)
{
    return cmolen_cmo(c->ob);
}

static int cmolen_cmo_list(cmo_list* c)
{
    int size = sizeof(int);
    cell* cp = c->head;

    while(cp->next != NULL) {
        size += cmolen_cmo(cp->cmo);
        cp = cp->next;
    }
    return size;
}

static int cmolen_cmo_monomial32(cmo_monomial32* c)
{
    int len = (c->length + 1)*sizeof(int);
    return len + cmolen_cmo(c->coef);
}

static int cmolen_cmo_zz(cmo_zz* c)
{
    int len = abs(c->mpz->_mp_size);
    return sizeof(len) + len*sizeof(int);
}

static int cmolen_cmo_distributed_polynomial(cmo_distributed_polynomial* c)
{
    return cmolen_cmo_list((cmo_list *)c) + cmolen_cmo(c->ringdef);
}

/* CMO ���Х��ȥ��󥳡��ɤ��줿���ΥХ������Ĺ������� */
int cmolen_cmo(cmo* c)
{
    int size = sizeof(int);

    switch(c->tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        size += cmolen_cmo_null(c);
        break;
    case CMO_INT32:
        size += cmolen_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_STRING:
        size += cmolen_cmo_string((cmo_string *)c);
        break;
    case CMO_MATHCAP:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
    case CMO_ERROR2:
        size += cmolen_cmo_mathcap((cmo_mathcap *)c);
        break;
    case CMO_LIST:
        size += cmolen_cmo_list((cmo_list *)c);
        break;
    case CMO_MONOMIAL32:
        size += cmolen_cmo_monomial32((cmo_monomial32 *)c);
        break;
    case CMO_ZZ:
        size += cmolen_cmo_zz((cmo_zz *)c);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        size += cmolen_cmo_distributed_polynomial((cmo_distributed_polynomial *)c);
        break;
    default:
    }
    return size;
}

static int  d_ptr;
static char *d_buf;

int init_dump_buffer(char *s)
{
    d_buf = s;
    d_ptr = 0;
}

static int dump_cmo_null(cmo_null* m)
{
    return 0;
}

static int dump_cmo_int32(cmo_int32* m)
{
    dump_integer(m->i);
}

static int dump_cmo_string(cmo_string* m)
{
    int len = strlen(m->s);
    dump_integer(len);
    dump_string(m->s, len);
}

static int dump_cmo_mathcap(cmo_mathcap* c)
{
    dump_cmo(c->ob);
}

static int dump_cmo_list(cmo_list* m)
{
    cell* cp = m->head;
    int len = length_cmo_list(m);
    dump_integer(len);

    while(cp->next != NULL) {
        dump_cmo(cp->cmo);
        cp = cp->next;
    }
}

static int dump_cmo_monomial32(cmo_monomial32* c)
{
    int i;
    int length = c->length;
    dump_integer(c->length);
    for(i=0; i<length; i++) {
        dump_integer(c->exps[i]);
    }
    dump_cmo(c->coef);
}

static int dump_cmo_zz(cmo_zz* c)
{
    dump_mpz(c->mpz);
}

static int dump_cmo_distributed_polynomial(cmo_distributed_polynomial* m)
{
    cell* cp = m->head;
    int len = length_cmo_list((cmo_list *)m);
    dump_integer(len);
    dump_cmo(m->ringdef);
    while(cp != NULL) {
        dump_cmo(cp->cmo);
        cp = cp->next;
    }
}

/* ������񤭽Ф��Ƥ��顢�ƴؿ���ƤӽФ� */
int dump_cmo(cmo* m)
{
    dump_integer(m->tag);
    switch(m->tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        dump_cmo_null(m);
        break;
    case CMO_INT32:
        dump_cmo_int32((cmo_int32 *)m);
        break;
    case CMO_STRING:
        dump_cmo_string((cmo_string *)m);
        break;
    case CMO_MATHCAP:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
    case CMO_ERROR2:
        dump_cmo_mathcap((cmo_mathcap *)m);
        break;
    case CMO_LIST:
        dump_cmo_list((cmo_list *)m);
        break;
    case CMO_MONOMIAL32:
        dump_cmo_monomial32((cmo_monomial32 *)m);
        break;
    case CMO_ZZ:
        dump_cmo_zz((cmo_zz *)m);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        dump_cmo_distributed_polynomial((cmo_distributed_polynomial *)m);
        break;
    default:
    }
}

static int dump_mpz(mpz_ptr mpz)
{
    int i;
    int len = abs(mpz->_mp_size);
    dump_integer(mpz->_mp_size);
    for(i=0; i<len; i++) {
        dump_integer(mpz->_mp_d[i]);
    }
    return;
}

static int dump_string(char *s, int len)
{
    memcpy(&d_buf[d_ptr], s, len);
    d_ptr += len;
}

static int dump_integer(int x)
{
    int nx = htonl(x);
    dump_string((char *)&nx, sizeof(int));
}

int dump_ox_data(ox_data* m)
{
    dump_integer(OX_DATA);
    dump_integer(-1);
    dump_cmo(m->cmo);
}

int dump_ox_command(ox_command* m)
{
    dump_integer(OX_COMMAND);
    dump_integer(-1);
    dump_integer(m->command);
}

int send_ox(ox_file_t s, ox *m)
{
    int tag = m->tag;
    int code;
    if (tag == OX_DATA) {
        send_ox_cmo(s->stream, ((ox_data *)m)->cmo);
    }else if (tag == OX_COMMAND) {
        code = ((ox_command *)m)->command;
        if (code >= 1024) {
            /* control command */
            send_ox_command(s->control, code);
        }else {
            send_ox_command(s->stream, code);
        }
    }else {
        /* CMO?? */
        send_ox_cmo(s->stream, (cmo *)m);
    }
}

int send_ox_cmo(int fd, cmo* m)
{
    send_ox_tag(fd, OX_DATA);
    send_cmo(fd, m);
}

/* send_cmo_xxx �ؿ��� */
static int send_cmo_null(int fd, cmo_null* c)
{
    return 0;
}

static int send_cmo_int32(int fd, cmo_int32* m)
{
    send_int32(fd, m->i);
}

static int send_cmo_string(int fd, cmo_string* m)
{
    int len = (m->s != NULL)? strlen(m->s): 0;
    send_int32(fd, len);
    if (len > 0) {
        write(fd, m->s, len);
    }
    return 0;
}

static int send_cmo_mathcap(int fd, cmo_mathcap* c)
{
    send_cmo(fd, c->ob);
    return 0;
}

static int send_cmo_list(int fd, cmo_list* c)
{
    cell* cp = c->head;
    int len = length_cmo_list(c);
    send_int32(fd, len);

    while(cp->next != NULL) {
        send_cmo(fd, cp->cmo);
        cp = cp->next;
    }
    return 0;
}

static int send_cmo_distributed_polynomial(int fd, cmo_distributed_polynomial* c)
{
    cell* cp = c->head;
    int len = length_cmo_list((cmo_list *)c);
    send_int32(fd, len);
    send_cmo(fd, c->ringdef);

    while(cp->next != NULL) {
        send_cmo(fd, cp->cmo);
        cp = cp->next;
    }
    return 0;
}

static int send_cmo_monomial32(int fd, cmo_monomial32* c)
{
    int i;
    int len = c->length;
    send_int32(fd, len);
    for(i=0; i<len; i++) {
        send_int32(fd, c->exps[i]);
    }
    send_cmo(fd, c->coef);
    return 0;
}

static int send_cmo_zz(int fd, cmo_zz* c)
{
    send_mpz(fd, c->mpz);
    return 0;
}

static int send_cmo_error2(int fd, cmo_error2* c)
{
    send_cmo(fd, c->ob);
    return 0;
}

/* CMO������.  OX_tag ��������*/
int send_cmo(int fd, cmo* c)
{
    int tag = c->tag;

    send_int32(fd, tag);
    switch(tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        send_cmo_null(fd, c);  /* ���δؿ� */
        break;
    case CMO_INT32:
        send_cmo_int32(fd, (cmo_int32 *)c);
        break;
    case CMO_STRING:
        send_cmo_string(fd, (cmo_string *)c);
        break;
    case CMO_MATHCAP:
    case CMO_ERROR2:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
        send_cmo_mathcap(fd, (cmo_mathcap *)c);
        break;
    case CMO_LIST:
        send_cmo_list(fd, (cmo_list *)c);
        break;
    case CMO_MONOMIAL32:
        send_cmo_monomial32(fd, (cmo_monomial32 *)c);
        break;
    case CMO_ZZ:
        send_cmo_zz(fd, (cmo_zz *)c);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        send_cmo_distributed_polynomial(fd, (cmo_distributed_polynomial *)c);
        break;
    default:
    }
}

static int send_mpz(int fd, mpz_ptr mpz)
{
    int i;
    int len = abs(mpz->_mp_size);
    send_int32(fd, mpz->_mp_size);
    for(i=0; i<len; i++) {
        send_int32(fd, mpz->_mp_d[i]);
    }
    return 0;
}

ox_data* new_ox_data(cmo* c)
{
    ox_data* m = malloc(sizeof(ox_data));
    m->tag = OX_DATA;
    m->cmo = c;
    return m;
}

ox_command* new_ox_command(int sm_code)
{
    ox_command* m = malloc(sizeof(ox_command));
    m->tag = OX_COMMAND;
    m->command = sm_code;
    return m;
}

#define  MAX_TYPES  8
static int known_types[] = {
    -1,   /* gate keeper */
    CMO_NULL,
    CMO_INT32,
    CMO_STRING,
    CMO_MATHCAP,
    CMO_LIST,
    CMO_ZZ,
    CMO_ERROR2,
};

#define ID_TEMP   "(CMO_LIST, (CMO_INT32, %d), (CMO_STRING, \"%s\"), (CMO_STRING, \"%s\"), (CMO_STRING, \"%s\"))"


static cmo_list* make_list_of_id(int ver, char* ver_s, char* sysname)
{
    cmo_list *cap;
    char buff[512];

    setgetc(mygetc);
    sprintf(buff, ID_TEMP, ver, sysname, ver_s, getenv("HOSTTYPE"));
    setmode_mygetc(buff, 512);
    cap = parse();
    resetgetc();

    return cap;
}

static cmo_list *make_list_of_tag(int type)
{
    cmo_list *li = new_cmo_list();
    symbol *symp;
    int i = 0;
    while((symp = lookup(i++))->key != NULL) {
        if (symp->type == type) {
            append_cmo_list(li, (cmo *)new_cmo_int32(symp->tag));
        }
    }
    return li;
}

cmo* make_mathcap_object(int version, char *id_string)
{
    char sysname[]   = "ox_math";
    cmo_list *li     = new_cmo_list();
    cmo_list *li_ver = make_list_of_id(version, id_string, sysname);
    cmo_list *li_cmo = make_list_of_tag(IS_CMO);
    cmo_list *li_sm  = make_list_of_tag(IS_SM);

    append_cmo_list(li, (cmo *)li_ver);
    append_cmo_list(li, (cmo *)li_cmo);
    append_cmo_list(li, (cmo *)li_sm);

    return (cmo *)new_cmo_mathcap((cmo *)li);
}

static int funcs(int cmo_type)
{
    int i;
    for(i=0; i<MAX_TYPES; i++) {
        if (known_types[i] == cmo_type) {
            return i;
        }
    }
    return 0;
}

void setCmotypeDisable(int type)
{
    int i = funcs(type);
    known_types[i] = -1;
}

#if 0
cmo* (*received_funcs[])(int fd) = {
    NULL,  /* gate keeper */
    receive_cmo_null,
    receive_cmo_int32,
    receive_cmo_string,
    receive_cmo_mathcap,
    receive_cmo_list,
    receive_cmo_zz,
    receive_cmo_error2
};

cmo* receive_cmo2(int fd)
{
    int tag;
    cmo* (*foo)() = received_funcs[funcs(tag)];
    if (foo != NULL) {
        return foo(fd);
    }
}
#endif

/* �ե�����ǥ�������ץ� fd ���̿�ϩ�Ǥ� integer �� byte order ����ꤹ�� */
/* �ºݤˤ� order (0,1,or 0xFF)��ߤƤϤ��ʤ� */
int decideByteOrderClient(oxfd fd, int order)
{
    char zero = OX_BYTE_NETWORK_BYTE_ORDER;
    char dest;
    read(fd, &dest, sizeof(char));
    write(fd, &zero, sizeof(char));
    return 0;
}

/* Server ¦�ǤϤ�������Ѥ��� */
/* ���ޤμ����� dup ����Ƥ��뤳�Ȥ�����ˤʤäƤ��� */
int decideByteOrderServer(oxfd fd, int order)
{
    char zero = OX_BYTE_NETWORK_BYTE_ORDER;
    char dest;
    write(fd, &zero, sizeof(char));
    read(fd, &dest, sizeof(char));
    return 0;
}

/* cmo �� string (�����Ǥ�C�����string) ���Ѵ��ؿ��� */
char *convert_zz_to_string(cmo_zz *c)
{
    return mpz_get_str(NULL, 10, c->mpz);
}

char *convert_cmo_to_string(cmo *m)
{
    symbol *symp;
    switch(m->tag) {
    case CMO_ZZ:
        return convert_zz_to_string((cmo_zz *)m);
    case CMO_INT32:
        return convert_int_to_string(((cmo_int32 *)m)->i);
    case CMO_STRING:
        return ((cmo_string *)m)->s;
    case CMO_NULL:
        return convert_null_to_string();
    default:
#ifdef DEBUG
        symp = lookup_by_tag(m->tag);
        fprintf(stderr, "I do not know how to convert %s to a string.\n", symp->key);
#endif
        /* �ޤ��������Ƥ��ޤ���. */
        return NULL;
    }
}

char *convert_null_to_string()
{
    static char* null_string = "";
    return null_string;
}

char *convert_int_to_string(int integer)
{
    char buff[1024];
    char *s;

    sprintf(buff, "%d", integer);
    s = malloc(strlen(buff)+1);
    strcpy(s, buff);

    return s;
}
