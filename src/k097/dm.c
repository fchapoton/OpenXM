/* $OpenXM$ */
#include <stdio.h>
#include "d.h"

int K00_verbose = 0;
extern int DebugMode;

/* dm.c main program of d. */
main(int argc, char *argv[]) {
  int i;
  DebugMode = 0;
  if (argc >= 2) {
    for (i=1; i < argc; i++) {
      if (strcmp(argv[i],"-DebugMode") == 0) {
	/* fprintf(stderr,"DebugMode = 1\n"); */
	DebugMode = 1;
      }else{
	fprintf(stderr,"d0 -DebugMode\n");
      }
    }
  }
  parseAfile(stdin);
  /*  parseAstring("print(1+2);\nx=1+2;\n"); tests */
  KCparse();
  exit(0);
}


void pkkan(s)
char *s;
{
  printf("%s",s);
}

void *mymalloc(int size) {
  return((void *)malloc(size));
}

void *GC_malloc(int size) {
  return((void *)malloc(size));
}

sendKan(int p) { /* do nothing */ }

/*           test new function           */
void testNewFunction(objectp op)
{
  if (op->tag != Sstring) {
    fprintf(stderr,"The argument must be given as an argment of load.\n");
    return;
  }
  fprintf(stderr,"Now execute .. <<%s>> \n",op->lc.str);
  parseAstring(op->lc.str);
  fprintf(stderr,"\nDone.\n");
}

struct object KSpop() {
  struct object obj;
  obj.tag = 0;
  fprintf(stderr," Do not use Test.\n");
  return(obj);
}

void pkdebug(char *s0,char *s1, char *s2,char *s3) {
  if (DebugMode) {
    pkkan(" db.DebugStack setstack $");
    pkkan(s0); pkkan(s1); pkkan(s2); pkkan(s3);
    pkkan("$  stdstack \n");
  }
}
    
void pkdebug2(void) {
  if (DebugMode) {
    pkkan(" db.DebugStack setstack pop stdstack \n");
  }
}
    
