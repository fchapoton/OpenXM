/* $OpenXM: OpenXM/src/kxx/ox_texmacs.c,v 1.18 2004/03/25 01:37:14 takayama Exp $ */

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include "ox_kan.h"
#include "serversm.h"

#if defined(__CYGWIN__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  sigsetjmp(env,1)
#define LONGJMP(env,p)  siglongjmp(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  setjmp(env)
#define LONGJMP(env,p)  longjmp(env,p)
#endif

/*
#define DEBUG 
*/
/* #define DEBUG2 */

#ifdef DEBUG
#define DATA_BEGIN_V  "<S type=verbatim>"     /* "\002verbatim:" */
#define DATA_BEGIN_L  "<S type=latex>"        /* "\002latex:" */
#define DATA_BEGIN_P  "<S type=prompt>"        /* "\002channel:prompt " */
#define DATA_BEGIN_PS  "<S type=postscript>"        /* "\002ps: " */
#define DATA_END      "</S>"    /* "\005" */
#else
#define DATA_BEGIN_V  "\002verbatim:"
#define DATA_BEGIN_L  "\002latex:"
#define DATA_BEGIN_P  "\002prompt:"
#define DATA_BEGIN_PS  "\002ps:"
#define DATA_END      "\005" 
#endif

/*
#define END_OF_INPUT  '#'
*/
#define END_OF_INPUT '\n'

/* Table for the engine type. */
#define ASIR          1
#define SM1           2
#define K0            3

extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
extern int Calling_ctrlC_hook;
int Format=1;  /* 1 : latex mode */
int OutputLimit_for_TeXmacs = (1024*10);

int TM_Engine  = ASIR ;
int TM_asirStarted = 0;
int TM_sm1Started  = 0;
int TM_k0Started  = 0;
int TM_do_not_print = 0;

int Xm_noX = 0;
int NoCopyright = 0;

void ctrlC();
struct object KpoString(char *s);
char *KSpopString(void);

static char *readString(FILE *fp,char *prolog, char *eplog);
static void printv(char *s);
static void printl(char *s);
static void printp(char *s);
static void printps(char *s);
static void printCopyright(char *s);
static int startEngine(int type,char *msg);
static int isPS(char *s);

/* tail -f /tmp/debug-texmacs.txt 
   Debug output to understand the timing problem of pipe interface.
*/
FILE *Dfp;

main(int argc,char *argv[]) {
  char *s;
  char *r;
  char *sys;
  struct object ob;
  int irt=0;
  int vmode=1;
  char *openxm_home;
  char *asir_config;
  int i;

  openxm_home = (char *) getenv("OpenXM_HOME");
  asir_config = (char *) getenv("ASIR_CONFIG");
  if (openxm_home == NULL || asir_config == NULL) {
    printv("The environmental variables OpenXM_HOME/ASIR_CONFIG are not set.\nStart the texmacs with openxm texmacs or ox_texmacs by openxm ox_texmacs\nBye...");
    exit(10);
  }

  
#ifdef DEBUG2
  Dfp = fopen("/tmp/debug-texmacs.txt","w");
#endif
  
  /* Set consts */
  Quiet = 1;
  for (i=1; i<argc; i++) {
	if (strcmp(argv[i],"--sm1") == 0) {
	  TM_Engine = SM1;
	}else if (strcmp(argv[i],"--asir") == 0) {
	  TM_Engine = ASIR;
	}else if (strcmp(argv[i],"--k0") == 0) {
	  TM_Engine = K0;
    }else if (strcmp(argv[i],"--outputLimit") == 0) {
      i++;
      sscanf(argv[i],"%d",&OutputLimit_for_TeXmacs);
    }else if (strcmp(argv[i],"--noLogWindow") == 0) {
	  Xm_noX = 1;
    }else if (strcmp(argv[i],"--noCopyright") == 0) {
	  NoCopyright = 1;
	}else{
	  /* printv("Unknown option\n"); */
	}
  }

  /* Initialize kanlib (gc is also initialized) */
  KSstart();

  /* Main loop */
  printf("%s",DATA_BEGIN_V);
  printCopyright("");

  /* Load ox engine here */
  /* engine id should be set to ox.engine */
  KSexecuteString(" [(parse) (ox.sm1) pushfile] extension ");
  if (Xm_noX) KSexecuteString(" /Xm_noX 1 def ");
  startEngine(TM_Engine," ");

  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
    signal(SIGINT,ctrlC);
  }

  irt = 0;
  while(1) {
    /* printp(sys);  no prompt */
    if (SETJMP(EnvOfStackMachine)) {
      if (!Calling_ctrlC_hook) {
        Calling_ctrlC_hook = 1;
        KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
      }
      Calling_ctrlC_hook = 0; 
      if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
        signal(SIGINT,ctrlC);
      }
      irt = 1;
      continue;
    } else {  }
    if (!irt) {
      printf("%s",DATA_END); fflush(stdout);
    }
    irt = 0;

    /* Reading the input. */
    if (TM_Engine == K0) {
      s=readString(stdin, " ", " "); /* see test data */
    }else if (TM_Engine == SM1) {
      s=readString(stdin, " ", " "); /* see test data */
    }else{
      s=readString(stdin, "if (1) { ", " ; }else{ }"); /* see test data */
    }

    if (s == NULL) { irt = 1; continue; }
    if (!irt) printf("%s",DATA_BEGIN_V);
    /* Evaluate the input on the engine */
    KSexecuteString(" ox.engine ");
    ob = KpoString(s);  
    KSpush(ob);
    KSexecuteString(" oxsubmit ");
    
    /* Get the result in string. */
    if (Format == 1 && (! TM_do_not_print)) {
      /* translate to latex form */
      KSexecuteString(" ox.engine oxpushcmotag ox.engine oxpopcmo ");
      ob = KSpop();
      vmode = 0;
      /* printf("id=%d\n",ob.tag); bug: matrix return 17 instead of Sinteger
       or error. */
      if (ob.tag == Sinteger) {
        /* printf("cmotag=%d\n",ob.lc.ival);*/
        if (ob.lc.ival == CMO_ERROR2) {
          vmode = 1;
        }
        if (ob.lc.ival == CMO_STRING) {
          vmode = 1;
        }
      }
      if (vmode) {
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
      }else{
        KSexecuteString(" ox.engine 1 oxpushcmo ox.engine (print_tex_form) oxexec  ");
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
      }
      if (isPS(r)) {
        printps(r);
      }else{
        if (vmode) printv(r);
        else{
          if (strlen(r) < OutputLimit_for_TeXmacs) {
            printl(r);
          } else printv("Output is too large.\n");
        }
      }
    }else{
      if (!TM_do_not_print) {
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
        printv(r);
      }else{
        KSexecuteString(" ox.engine 1 oxpops "); /* Discard the result. */
        /* Push and pop dummy data to wait until the computation finishes. */
        KSexecuteString(" ox.engine 0 oxpushcmo ox.engine oxpopcmo ");
        ob = KSpop();
        printv("");
      }
    }
  }
}

#define SB_SIZE 1024
#define INC_BUF     if (n >= limit-3) { \
      tmp = s; \
      limit *= 2;  \
      s = (char *) sGC_malloc(limit); \
      if (s == NULL) { \
        fprintf(stderr,"No more memory.\n"); \
        exit(10); \
      } \
      strcpy(s,tmp); \
    } 
/*   */
static char *readString(FILE *fp, char *prolog, char *epilog) {
  int n = 0;
  static int limit = 0;
  static char *s;
  int c;
  char *tmp;
  int i;
  int m;
  int start;
  struct object ob;
  if (limit == 0) {
    limit = 1024;
    s = (char *)sGC_malloc(limit);
    if (s == NULL) {
      fprintf(stderr,"No more memory.\n");
      exit(10);
    }
  }
  s[0] = 0; n = 0; m = 0;
  for (i=0; i < strlen(prolog); i++) {
    s[n++] = prolog[i];  s[n] = 0;
    INC_BUF ;
  }
  start = n;
  while ((c = fgetc(fp)) != EOF) {
#ifdef DEBUG2
    fprintf(Dfp,"[%x] ",c); fflush(Dfp); 
#endif
    if (c == END_OF_INPUT) {
      /* If there remains data in the stream,
         read the remaining data. */
	  /*
      if (oxSocketSelect0(0,1)) {
        if (c == '\n') c=' ';
        s[n++] = c; s[n] = 0;  m++;
        INC_BUF ;
        continue;
      }
      */
      break;
    }
    if ( c == '\v') c=' ';
    s[n++] = c; s[n] = 0;  m++;
    INC_BUF ;
  }
  /* Check the escape sequence */
  if (strcmp(&(s[start]),"!quit;") == 0) {
    printv("Terminated the process ox_texmacs.\n"); 
    exit(0);
  }
  /* Check the escape sequence to change the global env. */
  if (strcmp(&(s[start]),"!verbatim;") == 0) {
    printv("Output mode is changed to verbatim mode.");
    Format=0;
    return NULL;
  }
  if (strcmp(&(s[start]),"!latex;") == 0) {
    printv("Output mode is changed to latex/verbose.");
    Format = 1;
    return NULL;
  }
  if (strcmp(&(s[start]),"!asir;") == 0) {
    Format=1;
    TM_Engine=ASIR; startEngine(TM_Engine,"Asir");
    return NULL;
  }
  if (strcmp(&(s[start]),"!sm1;") == 0) {
    Format=0;
    TM_Engine=SM1; startEngine(TM_Engine,"sm1");
    return NULL;
  }
  if (strcmp(&(s[start]),"!k0;") == 0) {
    Format=0;
    TM_Engine=K0; startEngine(TM_Engine,"k0");
    return NULL;
  }
  if (strcmp(&(s[start]),"!reset;") == 0) {
	printf("%s",DATA_BEGIN_V);
    KSexecuteString(" ox.engine oxreset ox.engine oxpopcmo ");
	ob = KSpop();
	printf("%s",DATA_END); fflush(stdout);
    return NULL;
  }

  /* Set TM_do_no_print */
  if (s[n-1] == '$' && TM_Engine == ASIR) {
	TM_do_not_print = 1; s[n-1] = ' ';
  } else if (s[n-1] == ';' && TM_Engine == SM1) {
	TM_do_not_print = 1; s[n-1] = ' ';
  } else TM_do_not_print = 0;

  for (i=0; i < strlen(epilog); i++) {
    s[n++] = epilog[i];  s[n] = 0;
    INC_BUF ;
  }
  return s;
}

static void printv(char *s) {
  int i;
  printf("%s",DATA_BEGIN_V);
  printf("%s",s);
  printf("%s",DATA_END);
#ifdef DEBUG2
  fprintf(Dfp,"<%s>",s); fflush(Dfp);
#endif
  fflush(NULL);
}
static void printl(char *s) {
  printf("%s",DATA_BEGIN_L);
  printf(" $ %s $ ",s);
  printf("%s",DATA_END);
  fflush(NULL);
}
static int isPS(char *s) {
  if (s[0] != '%') return 0;
  if (s[1] != '%') return 0;
  if (s[2] != '!') return 0;
  if (s[3] != 'P') return 0;
  if (s[4] != 'S') return 0;
  return 1;
}
static void printps(char *s) {
  printf("%s",DATA_BEGIN_PS);
  printf("%s",s);
  printf("%s",DATA_END);
  fflush(NULL);
}
static void printp(char *s) {
  printf("%s",DATA_BEGIN_P);
  printf("%s",DATA_END);
  printf("%s] ",s);
  fflush(NULL);
}
static void printCopyright(char *s) {
  printf("%s",DATA_BEGIN_V);
  if (! NoCopyright) {
    printf("OpenXM engine (ox engine) interface for TeXmacs\n2004 (C) openxm.org");
    printf(" under the BSD license.  !asir; !sm1; !k0; !verbatim;\n");
    printf("Type in      !reset;     when the engine gets confused. ");
    printf("%s",s);
  }
  printf("%s",DATA_END);
  fflush(NULL);
}

static int startEngine(int type,char *msg) {
  struct object ob;
  printf("%s",DATA_BEGIN_V);
  if (type == SM1) {
    if (!TM_sm1Started) KSexecuteString(" sm1connectr ");
    KSexecuteString(" /ox.engine oxsm1.ccc def ");
    /* Initialize the setting of sm1. */
    KSexecuteString("  oxsm1.ccc ( [(cmoLispLike) 0] extension ) oxsubmit ");
    KSexecuteString("  oxsm1.ccc ( ox_server_mode ) oxsubmit ");
    KSexecuteString("  oxsm1.ccc ( ( ) message (------------- Message from sm1 ----------------)message ) oxsubmit ");
    TM_sm1Started = 1;
	/* Welcome message.  BUG. Copyright should be returned by a function. */
    if (! NoCopyright) {
      printf("Kan/StackMachine1                         1991 April --- 2004.\n");
      printf("This software may be freely distributed as is with no warranty expressed. \n");
      printf("See OpenXM/Copyright/Copyright.generic\n");
      printf("Info: http://www.math.kobe-u.ac.jp/KAN, kan@math.kobe-u.ac.jp.\n");
      printf("0 usages to show a list of functions. \n(keyword) usages to see a short description\n");
    }
    printf("%s\n",msg);
  }else if (type == K0) {
    if (!TM_k0Started) KSexecuteString(" k0connectr ");
    KSexecuteString(" /ox.engine oxk0.ccc def ");
    TM_k0Started = 1;
    printf("%s\n",msg);
  }else{
    if (!TM_asirStarted) KSexecuteString(" asirconnectr ");
    KSexecuteString(" /ox.engine oxasir.ccc def ");
    TM_asirStarted = 1;
    printf("%s\n",msg);
    if ( ! NoCopyright) {
      KSexecuteString(" oxasir.ccc (copyright()+asir_contrib_copyright();) oxsubmit oxasir.ccc oxpopstring ");
      ob = KSpop();
      if (ob.tag == Sdollar) {
        printf("%s",ob.lc.str);
      }
    }
    /* Initialize the setting of asir. */
    KSexecuteString(" oxasir.ccc (if(1) {  Xm_server_mode = 1; Xm_helpdir = \"help-eg\";  } else { ; } ;) oxsubmit oxasir.ccc oxpopcmo ");
    KSexecuteString(" oxasir.ccc (if(1) {  ctrl(\"message\",0);  } else { ; } ;) oxsubmit oxasir.ccc oxpopcmo ");
    /* bug; if ctrl is written with Xm_helpdir = ... without oxpopcmo, then it does
       not work. */
    KSexecuteString(" oxasir.ccc (print(\"----------- Messages from asir ------------------------------\")$ ) oxsubmit oxasir.ccc oxpopcmo ");
  }
  printf("%s",DATA_END);
  fflush(NULL);
}

/* test data

1.  print("hello"); print("afo");

    1+2;

2. def foo(N) { for (I=0; I<10; I++) {   --> error

   3+5;

4.  print("hello"); shift+return print("afo");

*/


  
