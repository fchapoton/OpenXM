/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/ox.h,v 1.3 1999/11/02 18:58:25 ohara Exp $ */
/* $Id$ */

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

----
(5) receive_ox_XXX �ؿ���¸�ߤ��ʤ�(���ʤ�).  receive_cmo �����Ѥ���.
(6) send_ox_XXX �ؿ��� OX ������ޤ����������.
(7) ox_XXX �ؿ��ϰ�Ϣ����������ޤ������Ū������ɽ������.
ox_XXX �ؿ��ϡ��������Ȥ��ơ�ox_file_t�����ѿ� sv ��Ȥ�.

(8) YYY_cmo �ؿ��� YYY_cmo_XXX �ؿ��δط��ϼ����̤�:
�ޤ� YYY_cmo �ؿ��� cmo �Υ������������������������Ĥ����ʬ��
YYY_cmo_XXX �ؿ����������롣cmo �������� cmo_ZZZ �ؤΥݥ��󥿤�
����Ȥ��ˤϡ����μ���ˤ�餺�� YYY_cmo �ؿ���ƤӽФ� 
*/


#define DEFAULT_LOGFILE  "/tmp/result"

/* �����С��Ȥ��̿�ϩ���Ѥ���ե�����ǥ�������ץ��Υڥ�. */
typedef struct {
    int stream;
    int control;
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
    cell *head;
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
    int length;  /* number of monomials */
    cell *head;  /* a list of monomials */
	cmo *ringdef;
} cmo_distributed_polynomial;

typedef cmo ox;

typedef struct {
    int tag;
    int command;
} ox_command;

typedef struct {
    int tag;
    cmo *cmo;
} ox_data;

cell*         new_cell(cmo* newcmo);
cmo_null*     new_cmo_null();
cmo_int32*    new_cmo_int32(int i);
cmo_string*   new_cmo_string(char* s);
cmo_mathcap*  new_cmo_mathcap(cmo* ob);
cmo_list*     new_cmo_list();
cmo_monomial32* new_cmo_monomial32();
cmo_monomial32* new_cmo_monomial32_size(int size);
cmo_zz*       new_cmo_zz();
cmo_zz*       new_cmo_zz_size(int size);
cmo_zz*       new_cmo_zz_set_si(int integer);
cmo_zz*       new_cmo_zz_noinit();
cmo_zero*     new_cmo_zero();
cmo_dms_generic* new_cmo_dms_generic();
cmo_ring_by_name* new_cmo_ring_by_name(cmo* ob);
cmo_indeterminate* new_cmo_indeterminate(cmo* ob);
cmo_error2*   new_cmo_error2(cmo* ob);

ox_data*      new_ox_data(cmo* c);
ox_command*   new_ox_command(int sm_code);

cmo_error2*   gen_error_object(int err_code);
cmo*          make_mathcap_object(int version, char *id_string);

void          resize_mpz(mpz_ptr mpz, int size);
cmo*          receive_cmo(int fd);
cmo*          receive_cmo2(int fd);
int           receive_int32(int fd);
int           receive_ox_tag(int fd);

int           send_cmo(int fd, cmo* m);
int           send_int32(int fd, int integer);
int           send_ox(ox_file_t s, ox* m);
int           send_ox_cmo(int fd, cmo* m);
void          send_ox_command(int fd, int sm_command);
int           send_ox_tag(int fd, int tag);

int           append_cmo_list(cmo_list* this, cmo *newcmo);
int           length_cmo_list(cmo_list* this);
cell*         next_cell(cell *this);
int           cmolen_cmo(cmo* m);

void          ox_close(ox_file_t sv);
void          ox_executeStringByLocalParser(ox_file_t sv, char* str);
cmo_mathcap*  ox_mathcap(ox_file_t sv);
char*         ox_popString(ox_file_t sv, int fd);
cmo*          ox_pop_cmo(ox_file_t sv, int fd);
void          ox_reset(ox_file_t sv);
ox_file_t     ox_start(char* host, char* prog1, char* prog2);

int           init_dump_buff(char *buff);
int           dump_cmo(cmo* m);
int           dump_ox_command(ox_command* m);
int           dump_ox_data(ox_data* m);

int           print_cmo(cmo* c);
int           print_cmo_int32(cmo_int32* c);
int           print_cmo_list(cmo_list* li);
int           print_cmo_mathcap(cmo_mathcap* c);
int           print_cmo_string(cmo_string* c);

int           decideByteOrder(int fd_read, int fd_write, int order);
int           decideByteOrder2(int fd_read, int fd_write, int order);
int           next_serial();
void          setCmotypeDisable(int type);

cmo_zz*       new_cmo_zz_set_string(char *s);
char*         convert_zz_to_cstring(cmo_zz *c);
char*         convert_cmo_to_cstring(cmo *m);
char*         convert_null_to_cstring();
char*         convert_int_to_cstring(int integer);

#endif /* _OX_H_ */
