/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.c,v 1.9 1999/11/19 20:51:36 ohara Exp $ */

/* Open Mathematica ������ */
/* �ե�����ǥ�������ץ� 3, 4 �� open ����Ƥ���Ȳ��ꤷ��ư���. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include <mathlink.h>
#include "ox.h"
#include "parse.h"
#include "serv2.h"

extern int flag_mlo_symbol;

/* MathLink ���¸��ʬ */

#define SIZE_OPERAND_STACK   2048

static cmo* Operand_Stack[SIZE_OPERAND_STACK];
static int Stack_Pointer = 0;

int initialize_stack()
{
    Stack_Pointer = 0;
}

int push(cmo* m)
{
#if DEBUG
    symbol *symp;

    if (m->tag == CMO_STRING) {
        fprintf(stderr, "ox_math:: a CMO_STRING(%s) was pushed.\n", ((cmo_string *)m)->s);
    }else {
        symp = lookup_by_tag(m->tag);
        fprintf(stderr, "ox_math:: a %s was pushed.\n", symp->key);
    }
#endif
    Operand_Stack[Stack_Pointer] = m;
    Stack_Pointer++;
    if (Stack_Pointer >= SIZE_OPERAND_STACK) {
        fprintf(stderr, "stack over flow.\n");
        Stack_Pointer--;
    }
}

/* �����å������ΤȤ���, (CMO_NULL) �򤫤���. */
cmo* pop()
{
    if (Stack_Pointer > 0) {
        Stack_Pointer--;
        return Operand_Stack[Stack_Pointer];
    }
    return new_cmo_null();
}

void pops(int n)
{
    Stack_Pointer -= n;
    if (Stack_Pointer < 0) {
        Stack_Pointer = 0;
    }
}

/* sm_XXX �ؿ����ϡ����顼�ΤȤ��� 0 �ʳ����ͤ��֤����ƤӽФ�����
   ���顼���֥������Ȥ򥻥åȤ��� */
int sm_popCMO(int fd_write)
{
    cmo* m = pop();
#ifdef DEBUG
    symbol *symp = lookup_by_tag(m->tag);
    fprintf(stderr, "ox_math:: opecode = SM_popCMO. (%s)\n", symp->key);
#endif

    if (m != NULL) {
        send_ox_cmo(fd_write, m);
        return 0;
    }
    return SM_popCMO;
}

int sm_pops(int fd_write)
{
    cmo* m = pop();
    if (m != NULL && m->tag == CMO_INT32) {
        pops(((cmo_int32 *)m)->i);
        return 0;
    }
    return ERROR_ID_UNKNOWN_SM;
}

/* MathLink ��¸��ʬ */
int sm_popString(int fd_write)
{
    char *s;
    cmo *err;
    cmo *m;

#ifdef DEBUG
    fprintf(stderr, "ox_math:: opecode = SM_popString.\n");
#endif

    m = pop();
    if (m->tag == CMO_STRING) {
        send_ox_cmo(fd_write, m);
    }else if ((s = convert_cmo_to_string(m)) != NULL) {
        send_ox_cmo(fd_write, (cmo *)new_cmo_string(s));
    }else {
        err = make_error_object(SM_popString, m);
        send_ox_cmo(fd_write, err);
    }
    return 0;
}

int local_execute(char *s)
{
    if(*s == 'i') {
        switch(s[1]) {
        case '+':
            flag_mlo_symbol = FLAG_MLTKSYM_IS_STRING;
            break;
        case '-':
        case '=':
        default:
            flag_mlo_symbol = FLAG_MLTKSYM_IS_INDETERMINATE;
        }
    }
    return 0;
}

/* ���δؿ��ϥ����Ф˰�¸����. */
int sm_executeStringByLocalParser(int fd_write)
{
    symbol *symp;
    cmo* m = pop();
    char *s = NULL;
#ifdef DEBUG
    fprintf(stderr, "ox_math:: opecode = SM_executeStringByLocalParser.\n");
#endif

    if (m->tag == CMO_STRING
        && strlen(s = ((cmo_string *)m)->s) != 0) {
        if (s[0] == ':') {
            local_execute(++s);
        }else {
            /* for mathematica */
            /* mathematica ��ʸ��������ä�ɾ�������� */
            ml_evaluateStringByLocalParser(s);
            push(ml_get_object());
        }
        return 0;
    }
#ifdef DEBUG
    symp = lookup_by_tag(m->tag);
    fprintf(stderr, "ox_math:: error. the top of stack is %s.\n", symp->key);
#endif
    return SM_executeStringByLocalParser;
}

int sm_executeFunction(int fd_write)
{
    int i, argc;
    cmo **argv;
    char* func;
    cmo* m;

    if ((m = pop()) == NULL || m->tag != CMO_STRING) {
        return SM_executeFunction;
    }
    func = ((cmo_string *)m)->s;

    if ((m = pop()) == NULL || m->tag != CMO_INT32) {
        return SM_executeFunction;
    }

    argc = ((cmo_int32 *)m)->i;
    argv = malloc(argc*sizeof(cmo *));
    for (i=0; i<argc; i++) {
        argv[i] = pop();
    }
    ml_executeFunction(func, argc, argv);
    push(ml_get_object());
    return 0;
}

/* ʿ��11ǯ10��13�� */
#define VERSION 0x11102700
#define ID_STRING  "ox_math server 1999/10/28 17:29:25"

int sm_mathcap(int fd_write)
{
    push(make_mathcap_object(VERSION, ID_STRING));
    return 0;
}

int receive_sm_command(int fd_read)
{
    return receive_int32(fd_read);
}

int execute_sm_command(int fd_write, int code)
{
    int err = 0;
#ifdef DEBUG    
    symbol *sp = lookup_by_tag(code);
    fprintf(stderr, "ox_math:: %s received.\n", sp->key);
#endif

    switch(code) {
    case SM_popCMO:
        err = sm_popCMO(fd_write);
        break;
    case SM_popString:
        err = sm_popString(fd_write);
        break;
    case SM_mathcap:
        err = sm_mathcap(fd_write);
        break;
    case SM_pops:
        err = sm_pops(fd_write);
        break;
    case SM_executeStringByLocalParser:
    case SM_executeStringByLocalParserInBatchMode:
        err = sm_executeStringByLocalParser(fd_write);
        break;
    case SM_executeFunction:
        err = sm_executeFunction(fd_write);
        break;
    case SM_shutdown:
        shutdown();
        break;
    case SM_setMathCap:
        pop();  /* ̵�뤹�� */
        break;
    default:
        fprintf(stderr, "unknown command: %d.\n", code);
        err = ERROR_ID_UNKNOWN_SM;
    }

    if (err != 0) {
        push((cmo *)make_error_object(err, new_cmo_null()));
    }
}
