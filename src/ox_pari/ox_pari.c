/*  $OpenXM: OpenXM/src/ox_pari/ox_pari.c,v 1.13 2016/08/01 01:35:01 noro Exp $  */

#include "ox_pari.h"

OXFILE *fd_rw;

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;
extern int debug_print;
long paristack=10000000;

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

void *gc_realloc(void *p,size_t osize,size_t nsize)
{
  return (void *)GC_realloc(p,nsize);        
}

void gc_free(void *p,size_t size)
{
  GC_free(p);
}

void init_gc()
{
  GC_INIT();
  mp_set_memory_functions(GC_malloc,gc_realloc,gc_free);
}

void init_pari()
{
  pari_init(paristack,2);
}

int initialize_stack()
{
  stack_pointer = 0;
   stack_size = INIT_S_SIZE;
  stack = MALLOC(stack_size*sizeof(cmo*));
  return 0;
}

static int extend_stack()
{
  int size2 = stack_size + EXT_S_SIZE;
  cmo **stack2 = MALLOC(size2*sizeof(cmo*));
  memcpy(stack2, stack, stack_size*sizeof(cmo *));
  free(stack);
  stack = stack2;
  stack_size = size2;
  return 0;
}

int push(cmo* m)
{
  stack[stack_pointer] = m;
  stack_pointer++;
  if(stack_pointer >= stack_size) {
    extend_stack();
  }
  return 0;
}

cmo* pop()
{
  if(stack_pointer > 0) {
    stack_pointer--;
    return stack[stack_pointer];
  }
  return new_cmo_null();
}

void pops(int n)
{
  stack_pointer -= n;
  if(stack_pointer < 0) {
    stack_pointer = 0;
  }
}

#define OX_PARI_VERSION 20150731
#define ID_STRING  "2015/07/31 15:00:00"

int sm_mathcap()
{
  char *opts[] = {"no_ox_reset", NULL};
  mathcap_init2(OX_PARI_VERSION, ID_STRING, "ox_pari", NULL, NULL, opts);
  push((cmo*)oxf_cmo_mathcap(fd_rw));
  return 0;
}

int sm_popCMO()
{
  cmo* m = pop();

  if(m != NULL) {
    send_ox_cmo(fd_rw, m);
    return 0;
  }
  return SM_popCMO;
}

cmo_error2 *make_error2(char *message)
{
  return new_cmo_error2((cmo *)new_cmo_string(message));
}

int get_i()
{
  cmo *c = pop();
  if(c->tag == CMO_INT32) {
    return ((cmo_int32 *)c)->i;
  }else if(c->tag == CMO_ZZ) {
    return mpz_get_si(((cmo_zz *)c)->mpz);
  }
  make_error2("get_i : invalid object");
  return 0;
}

char *get_str()
{
  cmo *c = pop();
  if(c->tag == CMO_STRING) {
    return ((cmo_string *)c)->s;
  }
  make_error2("get_str : invalid object");
  return "";
}

int ismatrix(GEN z)
{
  int len,col,i;

  if ( typ(z) != t_VEC ) return 0;
  if ( typ((GEN)z[1]) != t_VEC ) return 0;
  len = lg(z); col = lg((GEN)z[1]);
  for ( i = 2; i < len; i++ )
    if ( lg((GEN)z[i]) != col ) return 0;
  return 1;
}

int sm_executeFunction()
{
  pari_sp av0;
  int ac,i;
  cmo_int32 *c;
  cmo *av[PARI_MAX_AC];
  cmo *ret;
  GEN z,m;
  struct parif *parif;
  unsigned long prec;
  char buf[BUFSIZ];

  if ( setjmp(GP_DATA->env) ) {
    sprintf(buf,"sm_executeFunction : an error occured in PARI.");
    push((cmo*)make_error2(buf));
    return -1;
  }
  cmo_string *func = (cmo_string *)pop();
  if(func->tag != CMO_STRING) {
    sprintf(buf,"sm_executeFunction : func->tag=%d is not CMO_STRING",func->tag);
    push((cmo*)make_error2(buf));
    return -1;
  }

  c = (cmo_int32 *)pop();
  ac = c->i;
  if ( ac > PARI_MAX_AC ) {
    push((cmo*)make_error2("sm_executeFunction : too many arguments"));
    return -1;
  }
  for ( i = 0; i < ac; i++ ) {
    av[i] = (cmo *)pop();
//    fprintf(stderr,"arg%d:",i);
//    print_cmo(av[i]);
//    fprintf(stderr,"\n");
  }
  if( strcmp( func->s, "exit" ) == 0 )
    exit(0);

  parif =search_parif(func->s);
  if ( !parif ) {
    sprintf(buf,"%s : not implemented",func->s);
    push((cmo*)make_error2(buf));
    return -1;
 } else if ( parif->type == 0 ) {
    /* one long int variable */ 
    int a = cmo_to_int(av[0]);
    a = (int)(parif->f)(a);
    ret = (cmo *)new_cmo_int32(a);
    push(ret);
    return 0;
  } else if ( parif->type == 1 ) {
    /* one number/poly/matrix argument possibly with prec */ 
    av0 = avma;
    z = cmo_to_GEN(av[0]); 
    prec = ac==2 ? cmo_to_int(av[1])*3.32193/32+3 : precreal;
    if ( ismatrix(z) ) {
      int i,len;
      len = lg(z); 
      for ( i = 1; i < len; i++ )
        settyp(z[i],t_COL);
      settyp(z,t_MAT);
      z = shallowtrans(z);
    }
    printf("input : "); output(z);
    m = (*parif->f)(z,prec);
    ret = GEN_to_cmo(m);
    avma = av0;
    push(ret);
    return 0;
  } else if ( parif->type == 2 ) {
    /* one number/poly/matrix argument with flag=0 */ 
    av0 = avma;
    z = cmo_to_GEN(av[0]); 
    if ( ismatrix(z) ) {
      int i,len;
      len = lg(z); 
      for ( i = 1; i < len; i++ )
        settyp(z[i],t_COL);
      settyp(z,t_MAT);
      z = shallowtrans(z);
    }
    printf("input : "); output(z);
    m = (*parif->f)(z,0);
    ret = GEN_to_cmo(m);
    avma = av0;
    push(ret);
    return 0;
  } else {
    sprintf(buf,"%s : not implemented",func->s);
    push((cmo*)make_error2(buf));
    return -1;
  }
}

int receive_and_execute_sm_command()
{
  int code = receive_int32(fd_rw);
  switch(code) {
  case SM_popCMO:
    sm_popCMO();
    break;
  case SM_executeFunction:
    sm_executeFunction();
    break;
  case SM_mathcap:
    sm_mathcap();
    break;
  case SM_setMathCap:
    pop();
    break;
  case SM_shutdown:
    exit(0);
    break;
  default:
    printf("receive_and_execute_sm_command : code=%d\n",code);fflush(stdout);
    break;
  }
  return 0;
}

int receive()
{
  int tag;

  tag = receive_ox_tag(fd_rw);
  switch(tag) {
  case OX_DATA:
    printf("receive : ox_data %d\n",tag);fflush(stdout);
    push(receive_cmo(fd_rw));
    break;
  case OX_COMMAND:
    printf("receive : ox_command %d\n",tag);fflush(stdout);
    receive_and_execute_sm_command();
    break;
  default:
    printf("receive : tag=%d\n",tag);fflush(stdout);
  }
  return 0;
}

int main()
{
  init_gc();
  ox_stderr_init(stderr);
  initialize_stack();
  init_pari();

  fprintf(stderr,"ox_pari\n");

  fd_rw = oxf_open(3);
  oxf_determine_byteorder_server(fd_rw);

  while(1){
    receive();
  }
}
