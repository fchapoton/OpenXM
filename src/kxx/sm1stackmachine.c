#include <stdio.h>
#include <setjmp.h>
#include "ox_kan.h"
#include "serversm.h"
extern int OXprintMessage;

/*  server stack machine */

int Sm1_start(int argc, char *fnames[],char *myname) {
  int i;
  char cmd[4092];
  extern int ErrorMessageMode;
  extern char *VersionString;
  extern int CmoClientMode;
  CmoClientMode = 0;
  KSstart();
  fprintf(stderr,"sm1 version : %s\n",VersionString);
  fprintf(stderr,"sm1 url : http://www.math.kobe-u.ac.jp/KAN\n");
  fprintf(stderr,"name = %s\n",myname);

  /* Initialize for ox_sm1 */
  ErrorMessageMode = 2;
  KSexecuteString(" [ [(Strict) 1] system_variable ] pop ");

  /* Write a system start-up script here. */
  KSexecuteString(" [(cmoLispLike) 1] extension pop  ");

  /* if argc > 0, execute the system startup files fnames and
     set oxSystemName to myname. */
  if (argc > 0) {
    for (i=0; i<argc; i++) {
      /* load files from the search path */
      if (strlen(fnames[i]) > 1024) {
	fprintf(stderr,"Too long name for sm1 library file to load.\n");
	exit(10);
      }
      sprintf(cmd," [(parse) (%s) pushfile ] extension pop ",fnames[i]);
      KSexecuteString(cmd);
    }
    sprintf(cmd," [(cmoOxSystem) (ox_sm1_%s) ] extension pop ",myname);
    KSexecuteString(cmd);
  }
  KSexecuteString(" (---------------------------------------------------) message ");
  KSexecuteString(" [(flush)] extension pop ");

  return(0);
}

void *Sm1_mathcap() {
  int n,i;
  struct mathCap *mathcap;
  mathcap = KSmathCapByStruct();
  return((void *)mathcap);
}

int Sm1_setMathCap(ox_stream os) {
  /* Set the mathcap data of the client in the server. */
  /*  sm1 <====== ox_sm1  mathcap is set in ox_stream = FILE2 * */
  /* The mathcap data is on the stack. */
  struct object ob;
  struct object ob2;
  int n,i;
  ob = KSpop();
  KSpush(ob);  KSexecuteString(" (mathcap data is ) message message ");
  Kan_setMathCapToStream(os,ob);
  /* set the math cap data associated to the ox_stream. */
}
void Sm1_pops(void) {
  char data[100];
  sprintf(data," 1 1 3 -1 roll { pop pop } for ");
  KSexecuteString(data);
}
int Sm1_executeStringByLocalParser(void) {
  int i;
  char *s;
  s = Sm1_popString();
  if (s == NULL) {
    printf("NULL argument for executeString.\n");
    return(-1);
  }else{
    if (OXprintMessage) fprintf(stderr,"KSexecuteString(%s)\n",s);
    i = KSexecuteString(s);
    return(i);
  }
}
char *Sm1_popString(void) {
  char *KSpopString();
  KSexecuteString(" toString ");
  return(KSpopString());
}


int Sm1_setName(void)
{
  char *s;
  struct object ob;
  s = Sm1_popString();
  if (s == NULL) {
    printf("NULL argument for setName.\n");
    return(-1);
  }else{
    ob = KSpop();
    printf("/%s tag=%d def\n",s,ob.tag);
    KputUserDictionary(s,ob);
    return(0);
  }
}

int Sm1_evalName(void)
{
  char *s;
  struct object ob;
  s = Sm1_popString();
  if (s == NULL) {
    printf("NULL argument for evalName.\n");
    return(-1);
  }else{
    ob = KfindUserDictionary(s);
    if (ob.tag == -1) {
      printf("findUserDictionary(%s)--> tag=%d Not found.\n",s,ob.tag);
      return(-1);
    }
    printf("findUserDictionary(%s)--> tag=%d\n",s,ob.tag);
    KSpush(ob);
    return(0);
  }
}

int Sm1_pushCMO(ox_stream fp)
{
  return(Kan_pushCMOFromStream(fp));
}
int Sm1_popCMO(ox_stream fp,int serial)
{
  return(Kan_popCMOToStream(fp,serial));
}

int Sm1_pushError2(int serial, int no, char *s)
{
  struct object ob;
  ob = KnewErrorPacket(serial,no,s);
  KSpush(ob);
}

char *Sm1_popErrorMessage(char *s) {
  char *e;
  char *a;
  extern int ErrorMessageMode;
  /* Set ErrorMessageMode = 2 to use this function. */
  if (ErrorMessageMode != 2) return(s);
  e = popErrorStackByString();
  if (e == NULL ) {
    a = (char *) sGC_malloc(sizeof(char)*(strlen(s)+80));
    if (a == NULL) {
      fprintf(stderr,"No more memory in Sm1_popErrorMessage.\n");
      exit(10);
    }
    strcpy(a,s); strcat(a,"No error message on the error stack.");
    return(a);
  }else{
    a = (char *) sGC_malloc(sizeof(char)*(strlen(s)+strlen(e)+2));
    if (a == NULL) {
      fprintf(stderr,"No more memory in Sm1_popErrorMessage.\n");
      exit(10);
    }
    strcpy(a,s); strcat(a,e);
    return(a);
  }
}

void Sm1_getsp(void) {
  KSpush(KpoInteger(KSstackPointer()));
}

void Sm1_dupErrors(void) {
  KSpush(KSdupErrors());
}



