/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */
/* OX expression, CMO expression �ѡ��� */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <setjmp.h>
#include "oxtag.h"
#include "ox.h"
#include "parse.h"

/* --- ��ʸ������ --- */
/* (����)���ޥ�ƥ��å����ˤĤ��Ƥ����.
   CMO_LIST, CMO_STRING �ϡ����餫����Ϳ����줿���ǤθĿ���̵�뤹��.
   CMO_MONOMIAL32 ��̵�뤷�ʤ�. (�Ĥޤꤪ�������Ȥ��Ϲ�ʸ���顼�ˤʤ�)
*/


/* parse.c, lex.c �Ǥ�, Lisp ɽ�����줿 CMO ʸ������ɤ߹���,
   �Х��������Ϥ���.  ���ɽ���Ȥ��ơ�cmo *�����Ѥ���.
   parse() �ϥȡ�������󤫤� cmo *(�λؤ���¤��)����������.  
*/

/* ���פʤ��Ȥϥѡ���(�γƥ��֥롼����)��
   ��˥ȡ������ҤȤ����ɤߤ��Ƥ���ȸ������ȤǤ���.
*/

/* �����ɤ߹�����Υȡ������ɽ��. */
static int token = 0;   

/* �ȡ������°����. yylval �� lex() �ˤ�äƥ��åȤ����. */
static union{
    int   d;
    char* sym;
} yylval;

/* pflag_cmo_addrev �����åȤ���Ƥ���С���̩�ˤ� CMO expression �Ǥ�
   �ʤ����, �㤨�� (CMO_STRING, "hello") �� CMO ���Ѵ������. */

static int pflag_cmo_addrev = 1;  /* CMO �ξ�ά��ˡ��������ݤ��Υե饰 */

/* �ؿ������ */
static int parse_error(char *s);
static int parse_lf();
static int parse_right_parenthesis();
static int parse_left_parenthesis();
static int parse_comma();
static int parse_integer();
static cmo *parse_cmo_null();
static cmo *parse_cmo_int32();
static cmo *parse_cmo_string();
static cmo *parse_cmo_mathcap();
static cmo *parse_cmo_dms_generic();
static cmo *parse_cmo_ring_by_name();
static cmo *parse_cmo_error2();
static cmo *parse_cmo_zero();
static cmo *parse_cmo_zz();
static cmo *parse_cmo_list();
static cmo *parse_cmo();
static char *parse_string();
static int parse_sm();
static ox* parse_ox();
static ox* parse_ox_command();
static ox* parse_ox_data();

static int is_t_cmo(int token)
{
    return (token >= MIN_T_CMO && token < MAX_T_CMO) || token == TOKEN(CMO_ERROR2);
}

static int is_t_sm(int token)
{
    return token >= MIN_T_SM && token < MAX_T_SM;
}

static int is_t_ox(int token)
{
    return token >= MIN_T_OX && token < MAX_T_OX;
}

static jmp_buf env_parse;

/* ��ʸ���Ϥ˼��Ԥ������Ȥ��̣����. */
static int parse_error(char *s)
{
	fprintf(stderr, "%s\n", s);
	longjmp(env_parse, 1);
}

/* ������ʬ�Ͻ񤭴���ͽ��. */
cmo *parse()
{
    cmo *m;

	if (setjmp(env_parse) != 0) {
		return NULL; /* ��ʸ���Ϥ˼��Ԥ����� NULL ���֤�. */
	}

    do{
        token = lex();
    }while (token == '\n');

    if (token == '(') {
        token = lex();
        if (is_t_cmo(token)) {
            m = parse_cmo();
        }else if(is_t_ox(token)) {
            m = parse_ox();
        }else {
            parse_error("syntax error: unknown symbol.");
        }
        parse_lf();
        return m;
    }
    return NULL;
}

/* �ȡ���������ɤߤ��ʤ�(����). */
static int parse_lf()
{
    if (token != '\n') {
        parse_error("syntax error: not new line.");
    }
    return 0;
}


static ox* parse_ox()
{
    ox *m = NULL;

    switch(token) {
    case TOKEN(OX_COMMAND):
        token = lex();
        m = parse_ox_command();
        break;
    case TOKEN(OX_DATA):
        token = lex();
        m = parse_ox_data();
        break;
    default:
        parse_error("syntax error: invalid ox_tag.");
    }
    return m;
}

static ox* parse_ox_data()
{
    ox* m;

    parse_comma();
    parse_left_parenthesis();
    m = (ox *)new_ox_data(parse_cmo());
    parse_right_parenthesis();
    return m;
}

static int parse_sm()
{
    int sm_code;
    if (!is_t_sm(token)) {
        parse_error("syntax error: invalid sm code.");
    }
    sm_code = token - T_MAGIC;
    token = lex();
    return sm_code;
}


static ox* parse_ox_command()
{
    ox* m;

    parse_comma();
    m = (ox *)new_ox_command(parse_sm());
    parse_right_parenthesis();
    return m;
}

/* ���������Ϥʤ��, parse_cmo ��Ƥֻ�����, token �ˤ� 
   TOKEN(CMO_xxx), TOKEN(OX_xxx) �Τ����줫�����åȤ���Ƥ���. */
static cmo *parse_cmo()
{
    cmo *m = NULL;

    switch(token) {
    case TOKEN(CMO_NULL):
        token = lex();
        m = parse_cmo_null();
        break;
    case TOKEN(CMO_INT32):
        token = lex();
        m = parse_cmo_int32();
        break;
    case TOKEN(CMO_STRING):
        token = lex();
        m = parse_cmo_string();
        break;
    case TOKEN(CMO_MATHCAP):
        token = lex();
        m = parse_cmo_mathcap();
        break;
    case TOKEN(CMO_LIST):
        token = lex();
        m = parse_cmo_list();
        break;
    case TOKEN(CMO_MONOMIAL32):
        token = lex();
        m = parse_cmo_monomial32();
        break;
    case TOKEN(CMO_ZZ):
        token = lex();
        m = parse_cmo_zz();
        break;
    case TOKEN(CMO_ZERO):
        token = lex();
        m = parse_cmo_zero();
        break;
    case TOKEN(CMO_DMS_GENERIC):
        token = lex();
        m = parse_cmo_dms_generic();
        break;
    case TOKEN(CMO_RING_BY_NAME):
        token = lex();
        m = parse_cmo_ring_by_name();
        break;
    case TOKEN(CMO_ERROR2):
        token = lex();
        m = parse_cmo_error2();
        break;
    default:
        parse_error("syntax error: invalid cmo_tag.");
    }
    return m;
}

static int parse_left_parenthesis()
{
    if (token != '(') {
        parse_error("syntax error: no left parenthesis.");
    }
    token = lex();
}

static int parse_right_parenthesis()
{
    if (token != ')') {
        parse_error("syntax error: no right parenthesis.");
    }
    token = lex();
}

static int parse_comma()
{
    if (token != ',') {
        parse_error("syntax error: no comma.");
    }
    token = lex();
}

static int parse_integer()
{
    int val;
    if (token != T_INTEGER) {
        parse_error("syntax error: no integer.");
    }
    val = yylval.d;
    token = lex();
    return val;
}

static char *parse_string()
{
    char *s;
    if (token != T_STRING) {
        parse_error("syntax error: no string.");
    }
    s = yylval.sym;
    token = lex();
    return s;
}

static cmo *parse_cmo_null()
{
    parse_right_parenthesis();
    return (cmo *)new_cmo_null();
}

static cmo *parse_cmo_int32()
{
    int i;

    parse_comma();
    i = parse_integer();
    parse_right_parenthesis();
    return (cmo *)new_cmo_int32(i);
}

static cmo *parse_cmo_string()
{
    cmo_string *m;
    char *s;

    parse_comma();
    if (token == T_INTEGER) {
        parse_integer();
        parse_comma();
    }else if (!pflag_cmo_addrev) {
        parse_error("syntax error: not a cmo string.");
    }
	s = parse_string();
    m = new_cmo_string(s);
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_mathcap()
{
	cmo *ob;

    parse_comma();
	parse_left_parenthesis();
	ob = parse_cmo();
    parse_right_parenthesis();
    return (cmo *)new_cmo_mathcap(ob);
}

static cmo *parse_cmo_list()
{
    int length=0;
    int i=0;
    cmo_list *m = new_cmo_list();
    cmo *newcmo;

	if (token == ',') {
		parse_comma();

		if (token == T_INTEGER) {
			parse_integer();
			parse_comma();
		}else if (!pflag_cmo_addrev) {
			parse_error("syntax error: not a list.");
		}

		while(token == '(') {
			parse_left_parenthesis();
			newcmo = parse_cmo();
			append_cmo_list(m, newcmo);
			if (token != ',') {
				break;
			}
			parse_comma();
		}
	}else if (!pflag_cmo_addrev) {
		parse_error("syntax error: not a list.");
	}
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_monomial32()
{
	int size;
	int *exps;
	int i;
	cmo_monomial32 *m;

	parse_comma();
	size = parse_integer();
	if (size <= 0) {
		parse_error("syntax error: invalid value.");
	}
	m = new_cmo_monomial32_size(size);

	for(i=0; i<size; i++) {
		parse_comma();
		m->exps[i] = parse_integer();
	}
	parse_comma();
	parse_left_parenthesis();
	m->coef = parse_cmo(); 
    /* ��̣Ū�����å���ɬ�פ��� */
    parse_right_parenthesis();
    return (cmo *)m;
}

/* cmo_zz ��������ľ�ܤ�����. */
static cmo *parse_cmo_zz()
{
    int length;
    int i=0;
    cmo_zz *m= NULL;

    parse_comma();
    length = parse_integer();
    if (token == ',') {
        m = new_cmo_zz_size(length);
        
        length = abs(length);
        for(i=0; i<length; i++) {
            parse_comma();
            m->mpz->_mp_d[i] = parse_integer();
        }
    }else if (pflag_cmo_addrev) {
        m = new_cmo_zz_set_si(length);
    }else {
        parse_error("syntax error: invalid symbol.");
    }

    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_zero()
{
    parse_right_parenthesis();
    return (cmo *)new_cmo_zero();
}

static cmo *parse_cmo_dms_generic()
{
    parse_right_parenthesis();
    return (cmo *)new_cmo_dms_generic();
}

static cmo *parse_cmo_ring_by_name()
{
	cmo *ob;

    parse_comma();
	parse_left_parenthesis();
	ob = parse_cmo();  	
    /* ��̣Ū�����å���ɬ��(ob->tag == CMO_STRING�Ǥʤ���Ф����ʤ�) */
    parse_right_parenthesis();
    return (cmo *)new_cmo_ring_by_name(ob);
}

static cmo *parse_cmo_error2()
{
	cmo *ob;

    parse_comma();
	parse_left_parenthesis();
	ob = parse_cmo();
    parse_right_parenthesis();
    return (cmo *)new_cmo_error2(ob);
}

/* --- ��������� --- */

/* lexical analyzer ���ɤ����Ф����ʸ���ʤ鲿�����ͤˤ��Ƥ�褤 */
static int c = ' ';    

/* ��ʸ���ɤ߹���ؿ� */
static int (*GETC)() = getchar;

int setgetc(int (*foo)())
{
    GETC = foo;
}

int resetgetc()
{
    GETC = getchar;
}

#define SIZE_BUFFER  8192
static char buffer[SIZE_BUFFER];

/* ����ξ����к��Ϥʤ� */
static int lex_digit()
{
    int d = 0;
    do {
        d = 10*d + (c - '0');
        c = GETC();
    } while(isdigit(c));
    return d;
}

/* �Хåե����դ줷�������к�������ȹͤ���٤� */
static char *lex_quoted_string()
{
    int i;
    char c0 = ' ';
    char *s = NULL;
    for (i=0; i<SIZE_BUFFER; i++) {
        c = GETC();
        if(c == '"') {
            s = malloc(i+1);
            buffer[i]='\0';
            strcpy(s, buffer);
      
            c = GETC();
            return s;
        }else if (c == '\\') {
            c0 = c;
            c = GETC();
            if (c != '"') {
                buffer[i++] = c0;
            }
        }
        buffer[i]=c;
    }
    fprintf(stderr, "buffer overflow!\n");
    exit(1);
    /* return NULL; */
}

typedef struct {
	char *key;
	int  token;
} symbol; 

#define MK_KEY(x)  { #x  , TOKEN(x) }

static symbol symbol_list[] = {
	MK_KEY(CMO_NULL),
    MK_KEY(CMO_INT32), 
	MK_KEY(CMO_DATUM),
	MK_KEY(CMO_STRING), 
	MK_KEY(CMO_MATHCAP),
	MK_KEY(CMO_LIST), 
	MK_KEY(CMO_MONOMIAL32),
	MK_KEY(CMO_ZZ),
	MK_KEY(CMO_ZERO), 
	MK_KEY(CMO_DMS_GENERIC),
	MK_KEY(CMO_RING_BY_NAME),
	MK_KEY(CMO_INDETERMINATE),
	MK_KEY(CMO_ERROR2),
    MK_KEY(SM_popCMO),   
	MK_KEY(SM_popString), 
	MK_KEY(SM_mathcap),  
	MK_KEY(SM_pops), 
	MK_KEY(SM_executeStringByLocalParser), 
	MK_KEY(SM_executeFunction), 
	MK_KEY(SM_setMathCap),
    MK_KEY(SM_control_kill), 
	MK_KEY(SM_control_reset_connection),
    MK_KEY(OX_COMMAND),  MK_KEY(OX_DATA),
	{NULL, 0}        /* a gate keeper */
}; 

static int token_of_symbol(char *key)
{
	symbol *kp;
	for(kp = symbol_list; kp->key != NULL; kp++) {
		if (strcmp(key, kp->key)==0) {
			return kp->token;
		}
	}
#if DEBUG
    fprintf(stderr, "lex error\n");
#endif
    return 0;
}

static int lex_symbol()
{
    int i;
    for (i=0; i<SIZE_BUFFER; i++) {
        if (!isalnum(c) && c != '_') {
            buffer[i]='\0';
            return token_of_symbol(buffer);
        }
        buffer[i]=c;
        c = GETC();
    }
    fprintf(stderr, "buffer overflow!\n");
	return 0;
}

/* return �������˰�ʸ�����ɤߤ��Ƥ����� */
int lex()
{
    int c_dash = 0;
  
    /* ����򥹥��åפ���. */
    while (isspace(c) && c != '\n') {
        c = GETC();
    }

    switch(c) {
    case '(':
    case ')':
    case ',':
    case '\n':
        c_dash = c;
        c = ' ';
        return c_dash;
    case EOF:
        c = GETC();
        return c_dash;
    case '"':      /* a quoted string! */
        yylval.sym = lex_quoted_string();
        return T_STRING;
    default:
    }

    if (isalpha(c)) {    /* ���̻� */
        return lex_symbol();
    }

    /* 32bit ������ */
    if (isdigit(c)){
        yylval.d = lex_digit();
        return T_INTEGER;
    }
    if (c == '-') {
        c = GETC();
        while (isspace(c) && c != '\n') {
            c = GETC();
        }
        if (isdigit(c)){
            yylval.d = - lex_digit();
            return T_INTEGER;
        }
        return 0;
    }

    c = GETC();
    return 0;
}

static char *mygetc_line;
static int  mygetc_counter;
static int  mygetc_counter_max;

int mygetc()
{
    if (mygetc_counter <= mygetc_counter_max) {
        return mygetc_line[mygetc_counter++];
    }
    return 0;
}

int setmode_mygetc(char *s, int len)
{
    mygetc_counter=0;
    mygetc_counter_max=len;
    mygetc_line=s;
}

int setflag_parse(int flag)
{
	pflag_cmo_addrev = flag;
}
