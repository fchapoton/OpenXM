/* $OpenXM$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ox_toolkit.h"

/*
Usage:

double d;
init_dic();
register_entry("x",1.25);
register_entry("y",2.1);
if(eval_cmo(your_cmo_tree,&d)==0) goto_error();
*/

#define FUNCTION_P(e)      (((e)!=NULL) && ((e)->f != NULL))
#define VALUE_P(e)         (((e)!=NULL) && ((e)->f == NULL))

int eval_cmo(cmo *c, double *retval);

static double op_add(double x,double y)
{
    return x+y;
}

static double op_sub(double x,double y)
{
    return x-y;
}

static double op_mul(double x,double y)
{
    return x*y;
}

static double op_div(double x,double y)
{
    return x/y;
}

static double op_negative(double x) 
{
    return -x;
}

/* 定数は引数なしの関数として実現する。*/
typedef struct {
    char *name;
    double v;
    double (*f)();
    int n_args; /* number of args */
} entry;

/* 
グローバル辞書: sin(x), cos(x), exp(x), lgamma(x) などの関数や、pi,e といった定数を扱うための辞書
(グローバル変数で保持、システム固有)

ローカル辞書: [x -> 1/2, y-> 0.5] などの置き換えのため。
*/
entry global_dic[512] = {
    {"sin",0,sin,1},
    {"cos",0,cos,1},
    {"exp",0,exp,1},
    {"log",0,log,1},
    {"negative",0,op_negative,1},
    {"+",  0,op_add,2},
    {"-",  0,op_sub,2},
    {"*",  0,op_mul,2},
    {"/",  0,op_div,2},
    {"^",  0,pow,2},
    {"e",  M_E, NULL,0},
    {"pi", M_PI,NULL,0},
    {NULL,0,NULL,0}
};

#define LOCAL_DIC_SIZE 256
static entry local_dic[LOCAL_DIC_SIZE] = {
	{NULL,0,NULL,0}
};
static int local_dic_counter;

int register_entry(char *s, double v)
{
    entry *e = &local_dic[local_dic_counter];
    if(local_dic_counter<LOCAL_DIC_SIZE-1) {
        e->name = s;
        e->v = v;
		e->f = NULL;
        local_dic_counter++;
        return 1;
    }
    return 0;
}

void init_dic()
{
    int i;
    for(i=0; i<local_dic_counter; i++) {
        free((local_dic+i)->name);
    }
    local_dic_counter=0;
    memset(local_dic, 0, sizeof(entry)*LOCAL_DIC_SIZE);
}

static entry *find_entry(cmo *node, entry *dic)
{
    char *s;
    entry *e;
    if(node->tag == CMO_STRING) {
        s = ((cmo_string *)node)->s;
    }else if(node->tag == CMO_INDETERMINATE) {
        s = cmo_indeterminate_get_name((cmo_indeterminate *)node);
    }else {
        return NULL;
    }
    for(e=dic; e->name != NULL; e++) {
        if(strcmp(e->name,s)==0) {
            return e;
        }
    }
    return NULL;
}

static int entry_function(entry *e, cmo_list *args, double *retval)
{
	int i,m,n;
	double *dargs;
	n=e->n_args;
	if(n<0 || n>list_length(args)) {
		/* arguments are mismatched */
		return 0;
	}
	if(n>5) {
		/* too many arguments */
		return 0;
	}
	dargs = (double *)alloca(n*sizeof(double));
	for(i=0; i<n; i++) {
		if(eval_cmo(list_nth(args, i), &dargs[i])==0) {
			return 0;
		}
	}
	switch(n) {
	case 0:
		*retval = e->f();
		break;
	case 1:
		*retval = e->f(dargs[0]);
		break;
	case 2:
		*retval = e->f(dargs[0],dargs[1]);
		break;
	case 3:
		*retval = e->f(dargs[0],dargs[1],dargs[2]);
		break;
	case 4:
		*retval = e->f(dargs[0],dargs[1],dargs[2],dargs[3]);
		break;
	case 5:
		*retval = e->f(dargs[0],dargs[1],dargs[2],dargs[3],dargs[4]);
		break;
	default:
		return 0;
	}
	return 1;
}

int entry_value(entry *e, double *retval)
{
	*retval = e->v;
	return 1;
}

/* 
返り値: 評価が成功したか、 1: 成功, 0: 失敗
評価された値: *retval に格納
*/
static int eval_cmo_tree(cmo_tree* t, double *retval)
{
    entry *e = find_entry((cmo *)t->name,global_dic);
	if (FUNCTION_P(e)) {
		return entry_function(e,t->leaves,retval);
	}else if (VALUE_P(e)) {
		return entry_value(e, retval);
	}
    return 0;
}

static int eval_cmo_indeterminate(cmo_indeterminate *c, double *retval)
{
    entry *e = find_entry((cmo *)c,local_dic);
    if (VALUE_P(e)) {
        return entry_value(e,retval);
    }
    return 0;
}

int eval_cmo(cmo *c, double *retval)
{
    int tag = c->tag;
    switch(c->tag) {
    case CMO_ZERO:
        *retval = 0;
        break;
    case CMO_INT32:
        *retval = (double)((cmo_int32 *)c)->i;
        break;
    case CMO_ZZ:
        *retval = mpz_get_d(((cmo_zz *)c)->mpz);
        break;
    case CMO_QQ:
        *retval = mpq_get_d(((cmo_qq *)c)->mpq);
        break;
    case CMO_IEEE_DOUBLE_FLOAT:
    case CMO_64BIT_MACHINE_DOUBLE:
        *retval = ((cmo_double *)c)->d;
        break;
    case CMO_BIGFLOAT:
        *retval = mpfr_get_d(((cmo_bf *)c)->mpfr,GMP_RNDN);
        break;
    case CMO_TREE:
        return eval_cmo_tree((cmo_tree *)c,retval);
        break;
	case CMO_INDETERMINATE:
        return eval_cmo_indeterminate((cmo_indeterminate *)c,retval);
        break;
    default:
        /* 変換できない型 */
        return 0; /* error */
    }
    return 1;
}
