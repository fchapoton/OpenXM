/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.c,v 1.3 1999/11/03 10:56:40 ohara Exp $ */

/* Open Mathematica ������ */
/* �ե�����ǥ�������ץ� 3, 4 �� open ����Ƥ���Ȳ��ꤷ��ư���. */

/* MathLink �Ȥ��̿���ʬ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include <mathlink.h>
#include "ox.h"
#include "serv2.h"

#define UNKNOWN_SM_COMMAND 50000
#define MATH_ERROR         50001

/* MLINK �ϥݥ��󥿷�. */
MLINK lp = NULL;


typedef cmo mlo;
typedef cmo_string mlo_string;
typedef cmo_zz mlo_zz;

/* cmo_list ���������饹*/
typedef struct {
	int tag;
	int length;
	cell *head;
	char *function;
} mlo_function;


mlo *receive_mlo_zz()
{
	char *s;
	mlo  *m;

	MLGetString(lp, &s);
 	fprintf(stderr, "--debug: zz = %s.\n", s);
	m = (mlo *)new_cmo_zz_set_string(s);
	MLDisownString(lp, s);
	return m;
}

mlo *receive_mlo_string()
{
	char *s;
	mlo  *m;
	MLGetString(lp, &s);
	m = (cmo *)new_cmo_string(s);
	MLDisownString(lp, s);
	return m;
}

/* Mathematica ��ư����. */
int MATH_init()
{
    int argc = 2;
    char *argv[] = {"-linkname", "math -mathlink"};

    if(MLInitialize(NULL) != NULL) {
        lp = MLOpen(argc, argv);
        if(lp != NULL) {
            return 0;
        }
    }
    exit(1);
}

int MATH_exit()
{
    /* quit Mathematica then close the link */
    MLPutFunction(lp, "Exit", 0);
    MLClose(lp);
}

char *MATH_getObject()
{
    char *s;

    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(lp) != RETURNPKT) {
        usleep(10);
        MLNewPacket(lp);
    }
    /* ���ޤϥ����פˤ�����餺ʸ������������. */
    switch(MLGetNext(lp)) {
    case MLTKINT:
        fprintf(stderr, "type is INTEGER.\n");
        MLGetString(lp, &s);
        break;
    case MLTKSTR:
        fprintf(stderr, "type is STRING.\n");
        MLGetString(lp, &s);
        break;
    default:
        MLGetString(lp, &s);
    }
    return s;
}


cmo *MATH_getObject2()
{
    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(lp) != RETURNPKT) {
        usleep(10);
        MLNewPacket(lp);
    }
    /* ���ޤϥ����פˤ�����餺ʸ������������. */
	return MATH_getObject3();
}

cmo *MATH_getObject3()
{
    char *s;
    cmo  *m;
    cmo  *ob;
    int  i,n;
	int type;

    /* ���ޤϥ����פˤ�����餺ʸ������������. */
	type = MLGetNext(lp);
    switch(type) {
    case MLTKINT:
        fprintf(stderr, "--debug: MLO == MLTKINT.\n");
		m = receive_mlo_zz();
        break;
    case MLTKSTR:
        fprintf(stderr, "--debug: MLO == MLTKSTR.\n");
		m = receive_mlo_string();
        break;
#if 0
    case MLTKREAL:
        fprintf(stderr, "MLTKREAL is not supported: we use MLTKSTR.\n");
        MLGetString(lp, &s);
        m = (cmo *)new_cmo_string(s);
        break;
    case MLTKERR:
        fprintf(stderr, "--debug: MLO == MLTKERR.\n");
        m = (cmo *)gen_error_object(MATH_ERROR);
        break;
#endif
    case MLTKSYM:
		fprintf(stderr, "--debug: MLO == MLTKSYM.\n");
		/* ������ʬ�����꤬����. */
        MLGetSymbol(lp, &s);
		fprintf(stderr, "--debug: Symbol \"%s\".\n", s);
        m = (cmo *)new_cmo_string(s);
        break;
    case MLTKFUNC:
        fprintf(stderr, "--debug: MLO == MLTKFUNC.\n");
        MLGetFunction(lp, &s, &n);
        fprintf(stderr, "--debug: Function = \"%s\", # of args = %d\n", s, n);
		m = new_cmo_list();
		append_cmo_list(m, new_cmo_string(s));
		fflush(stderr);
        for (i=0; i<n; i++) {
            fprintf(stderr, "--debug: arg[%d]\n", i);
			fflush(stderr);
			ob = MATH_getObject3();
			append_cmo_list(m, ob);
        }
        break;
    default:
        fprintf(stderr, "--debug: MLO(%d) != MLTKINT, MLTKSTR.\n", type);
        fprintf(stderr, "--debug: MLTKFUNC(%d), MLTKSYM(%d).\n", MLTKFUNC, MLTKSYM);

        MLGetString(lp, &s);
		fprintf(stderr, "--debug: \"%s\"\n", s);
        m = (cmo *)new_cmo_string(s);
    }
    return m;
}

int MATH_sendObject(cmo *m)
{
    char *s;
    switch(m->tag) {
    case CMO_INT32:
        MLPutInteger(lp, ((cmo_int32 *)m)->i);
        break;
    case CMO_STRING:
        s = ((cmo_string *)m)->s;
        MLPutString(lp, s);
        fprintf(stderr, "put %s.", s);
        break;
    default:
        MLPutFunction(lp, "ToExpression", 1);
        s = convert_cmo_to_string(m);
        MLPutString(lp, s);
        fprintf(stderr, "put %s.", s);
        break;
    }
}

int MATH_evaluateStringByLocalParser(char *str)
{
    MLPutFunction(lp, "ToExpression", 1);
    MLPutString(lp, str);
    MLEndPacket(lp);
}

int MATH_executeFunction(char *function, int argc, cmo *argv[])
{
    int i;
    MLPutFunction(lp, function, argc);
    for (i=0; i<argc; i++) {
        MATH_sendObject(argv[i]);
    }
    MLEndPacket(lp);
}

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
    fprintf(stderr, "server:: a cmo is pushed: tag == %d.\n", m->tag);
    if (m->tag == CMO_STRING) {
        fprintf(stderr, "server:: %s\n", ((cmo_string *)m)->s);
    }
#endif
    Operand_Stack[Stack_Pointer] = m;
    Stack_Pointer++;
    if (Stack_Pointer >= SIZE_OPERAND_STACK) {
        fprintf(stderr, "stack over flow.\n");
        exit(1);
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

    fprintf(stderr, "code: SM_popCMO.\n");
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
    return UNKNOWN_SM_COMMAND;
}

/* MathLink ��¸��ʬ */
int sm_popString(int fd_write)
{
    char* s;
    cmo*  m;

#ifdef DEBUG
    fprintf(stderr, "code: SM_popString.\n");
#endif

    if ((m = pop()) != NULL && (s = convert_cmo_to_string(m)) != NULL) {
        send_ox_cmo(fd_write, (cmo *)new_cmo_string(s));
        return 0;
    }
    return SM_popString;
}

/* ���δؿ��ϥ����Ф˰�¸����. */
int sm_executeStringByLocalParser(int fd_write)
{
    cmo* m = NULL;
#ifdef DEBUG
    fprintf(stderr, "code: SM_executeStringByLocalParser.\n");
#endif
    if ((m = pop()) != NULL && m->tag == CMO_STRING) {
        /* for mathematica */
        /* mathematica ��ʸ��������ä�ɾ�������� */
        MATH_evaluateStringByLocalParser(((cmo_string *)m)->s);
        push(MATH_getObject2());
        return 0;
    }
    fprintf(stderr, "cannot execute: top of stack is not string!(%p, %d)\n", m, m->tag);
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
    argv = malloc(sizeof(cmo *)*argc);
    for (i=0; i<argc; i++) {
        if ((argv[i] = pop()) == NULL) {
            return SM_executeFunction;
        }
    }
    MATH_executeFunction(func, argc, argv);
    push(MATH_getObject2());
    return 0;
}

/* ʿ��11ǯ10��13�� */
#define VERSION 0x11102700
#define ID_STRING  "ox_math server 1999/10/28 17:29:25"

int sm_mathcap(int fd_write)
{
    cmo* c = make_mathcap_object(VERSION, ID_STRING);
    push(c);
    return 0;
}

int receive_sm_command(int fd_read)
{
    return receive_int32(fd_read);
}

int execute_sm_command(int fd_write, int code)
{
    int err = 0;

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
        err = sm_executeStringByLocalParser(fd_write);
        break;
    case SM_executeFunction:
        err = sm_executeFunction(fd_write);
        break;
    case SM_setMathCap:
        pop();  /* ̵�뤹�� */
        break;
    default:
        fprintf(stderr, "unknown command: %d.\n", code);
        err = UNKNOWN_SM_COMMAND;
    }

    if (err != 0) {
        push((cmo *)gen_error_object(err));
    }
}
