/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/ox.h,v 1.5 1999/12/16 06:58:01 ohara Exp $ */

#ifndef _OX_H_

#define _OX_H_

#include <gmp.h>  
#include "oxtag.h"

/*
�ؿ���̾���դ�����(����2)
(1) receive_cmo �ؿ���CMO�����ȥǡ������Τ��������. ���δؿ��� CMO������
�ͤ�������ʬ����ʤ��Ȥ��˻��Ѥ���. �֤��ͤȤ��ơ�cmo �ؤΥݥ��󥿤��֤���
(2) receive_cmo_XXX �ؿ���, CMO������Ƥδؿ��Ǽ������Ƥ���ƤӽФ�����
���ǡ��ǡ������ΤΤߤ��������cmo_XXX �ؤΥݥ��󥿤��֤��������⡢
�ؿ������� new_cmo_XXX �ؿ���ƤӽФ���
(3) send_cmo �ؿ���CMO�����ȥǡ������Τ���������.
(4) send_cmo_XXX �ؿ���CMO������Ƥδؿ����������Ƥ���ƤӽФ����ؿ��ǡ�
�ǡ������ΤΤߤ���������.
(5) receive_ox_XXX �ؿ���¸�ߤ��ʤ�(���ʤ�).  receive_cmo �����Ѥ���.

----
(6) send_ox_XXX �ؿ��� OX ������ޤ����������.
(7) ox_XXX �ؿ��ϰ�Ϣ����������ޤ������Ū������ɽ������.
ox_XXX �ؿ��ϡ��������Ȥ��ơ�ox_file_t�����ѿ� sv ��Ȥ�.

(8) YYY_cmo �ؿ��� YYY_cmo_XXX �ؿ��δط��ϼ����̤�:
�ޤ� YYY_cmo �ؿ��� cmo �Υ������������������������Ĥ����ʬ��
YYY_cmo_XXX �ؿ����������롣cmo �������� cmo_ZZZ �ؤΥݥ��󥿤�
����Ȥ��ˤϡ����μ���ˤ�餺�� YYY_cmo �ؿ���ƤӽФ� 
*/

#define LOGFILE  "/tmp/oxtk.XXXXXX"

/* Open Xm File Descripter */
typedef int oxfd;

#if 0
/* ���Τ���������˰ܹԤ�����... */
typedef struct {
    int fd;
    int byteorder;
} oxfile;
typedef oxfile *oxfd;
#endif

/* �����С��Ȥ��̿�ϩ���Ѥ���ե�����ǥ�������ץ��Υڥ�. */
typedef struct {
    oxfd stream;
    oxfd control;
} __ox_file_struct;

typedef __ox_file_struct *ox_file_t;

/*
�ٹ�:
cmo_list ���Υꥹ�Ȥˤ��˲�Ū�������򤷤ƤϤ����ʤ�.
cmo_list �γƥ��Ф�ľ�ܥ����������ƤϤ����ʤ�.
�᥽�å�(..._cmo_list �ؿ�)���Ѥ��뤳��.
*/

/* ��������Ͽ�������� */
typedef struct {
    int tag;
} cmo;

typedef cmo cmo_null;
typedef cmo cmo_zero;
typedef cmo cmo_dms_generic;

typedef struct {
    int tag;
    int i;
} cmo_int32;

typedef struct {
    int tag;
    int size;
    char *body;
} cmo_datum;

typedef struct {
    int tag;
    char *s;
} cmo_string;

typedef struct {
    int tag;
    cmo *ob;
} cmo_mathcap;

typedef cmo_mathcap cmo_error2;
typedef cmo_mathcap cmo_ring_by_name;
typedef cmo_mathcap cmo_indeterminate;

typedef struct cell {
    struct cell *next;
    cmo *cmo;
} cell;

typedef struct {
    int tag;
    int length;   /* �ꥹ�Ȥ�Ĺ��(ɬ��??) */
    cell head[1];
} cmo_list;


typedef struct {
    int tag;
    int length;
    int *exps;
    cmo *coef;
} cmo_monomial32;

typedef struct {
    int tag;
    mpz_t mpz;
} cmo_zz;

typedef struct {
    int tag;
    cmo *num;  /* ʬ�� (cmo_zz) */
    cmo *den;  /* ʬ�� (cmo_zz) */
} cmo_qq;

/* cmo_list ������. append_cmo_list ��ȤäƤ褤. */
typedef struct {
    int tag;
    int length;    /* number of monomials */
    cell head[1];  /* a list of monomials */
    cmo *ringdef;
} cmo_distributed_polynomial;

typedef cmo ox;

typedef ox ox_sync_ball;

typedef struct {
    int tag;
    int command;
} ox_command;

typedef struct {
    int tag;
    cmo *cmo;
} ox_data;

cmo_null*          new_cmo_null();
cmo_int32*         new_cmo_int32(int i);
cmo_string*        new_cmo_string(char* s);
cmo_mathcap*       new_cmo_mathcap(cmo* ob);
cmo_list*          new_cmo_list();
cmo_monomial32*    new_cmo_monomial32();
cmo_monomial32*    new_cmo_monomial32_size(int size);
cmo_zz*            new_cmo_zz();
cmo_zz*            new_cmo_zz_size(int size);
cmo_zz*            new_cmo_zz_set_si(int integer);
cmo_zz*            new_cmo_zz_set_mpz(mpz_ptr z);
cmo_zz*            new_cmo_zz_noinit();
cmo_zz*            new_cmo_zz_set_string(char* s);
cmo_zero*          new_cmo_zero();
cmo_distributed_polynomial* new_cmo_distributed_polynomial();
cmo_dms_generic*   new_cmo_dms_generic();
cmo_ring_by_name*  new_cmo_ring_by_name(cmo* ob);
cmo_indeterminate* new_cmo_indeterminate(cmo* ob);
cmo_error2*        new_cmo_error2(cmo* ob);

ox_data*           new_ox_data(cmo* c);
ox_command*        new_ox_command(int sm_code);
ox_sync_ball*      new_ox_sync_ball();

char*              new_string_set_cmo(cmo* m);

cmo_error2*        make_error_object(int err_code, cmo* ob);
cmo*               make_mathcap_object(int version, char *id_string);

/* ���� API */
cmo*               receive_cmo(int fd);
int                receive_int32(int fd);
int                receive_ox_tag(int fd);

int                send_cmo(int fd, cmo* m);
int                send_int32(int fd, int integer);
int                send_ox(int fd, ox* m);
int                send_ox_cmo(int fd, cmo* m);
void               send_ox_command(int fd, int sm_command);
int                send_ox_tag(int fd, int tag);

int                decideByteOrderClient(int fd, int order);
int                decideByteOrderServer(int fd, int order);
int                next_serial();
void               setCmotypeDisable(int type);
cell*              new_cell();
cmo*               nth_cmo_list(cmo_list* this, int n);
int                set_current_fd(int fd);

/* ���� API */
ox_file_t          ox_start(char* host, char* prog1, char* prog2);
ox_file_t          ox_start_insecure_nonreverse(char* host, short portControl, short portStream);
void               ox_close(ox_file_t sv);
void               ox_shutdown(ox_file_t sv);
void               ox_reset(ox_file_t sv);
void               ox_execute_string(ox_file_t sv, char* str);
cmo_mathcap*       ox_mathcap(ox_file_t sv);
char*              ox_popString(ox_file_t sv);
int                ox_pops(ox_file_t sv, int num);
cmo*               ox_pop_cmo(ox_file_t sv);
void               ox_push_cmo(ox_file_t sv, cmo *c);
void               ox_push_cmd(ox_file_t sv, int sm_code);
int                ox_cmo_rpc(ox_file_t sv, char *function, int argc, cmo *argv[]);
int                ox_flush(ox_file_t sv);

int                append_cmo_list(cmo_list* this, cmo *ob);
int                length_cmo_list(cmo_list* this);
cell*              next_cell(cell *this);
int                cmolen_cmo(cmo* m);
int                init_dump_buff(char *buff);
int                dump_cmo(cmo* m);
int                dump_ox_command(ox_command* m);
int                dump_ox_data(ox_data* m);

int                print_cmo(cmo* c);

typedef cmo *(*hook_t)(int, cmo *);

int add_hook_before_send_cmo(hook_t func);
int add_hook_after_send_cmo(hook_t func);

#endif /* _OX_H_ */
