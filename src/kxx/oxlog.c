/*$OpenXM: OpenXM/src/kxx/oxlog.c,v 1.6 2000/02/09 12:02:56 takayama Exp $*/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* In some distributions of linux you need to include the following file
#include <linux/sched.h>
*/
#include <unistd.h> 

/* oxlog /usr/X11R6/bin/xterm -icon -e /home/nobuki/kxx/ox
    0     1                    2     3  4
         -ox /home/nobuki/kxx/ox_asir
          5   6
*/
/* oxlog /home/nobuki/kxx/ox
    0     1     
         -ox /home/nobuki/kxx/ox_asir  >& /dev/null
          2     3
*/
static int Debug_which = 1;
char *toFullPath(char *s);
char *which(char *s,char *env);
main(int argc, char *argv[]) {
  sigset_t sss;
  int i;
  char *oxname ="???";
  /* char *env[2];
  env[0] = NULL;*/
  sigemptyset(&sss);
  sigaddset(&sss,SIGINT);
  sigprocmask(SIG_BLOCK,&sss,NULL);
  if (argc >= 2) {
    oxname = argv[1];
    oxname = toFullPath(oxname);
	if (oxname == NULL) oxname="NULL";
  }
  if (argc == 2) {
    execl(oxname,oxname,NULL);
  }else if (argc == 3) {
    execl(oxname,oxname,argv[2],NULL);
  }else if (argc == 4) {
    execl(oxname,oxname,argv[2],argv[3],NULL);
  }else if (argc == 5) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],NULL);
  }else if (argc == 6) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],NULL);
  }else if (argc == 7) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],NULL);
  }else if (argc == 8) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],NULL);
  }else if (argc == 9) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],NULL);
  }else if (argc == 10) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],NULL);
  }else if (argc == 11) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],NULL);
  }else if (argc == 12) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11],NULL);
  }else if (argc == 13) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],NULL);
  }else if (argc == 14) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],NULL);
  }else if (argc == 15) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],NULL);
  }else if (argc == 16) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],NULL);
  }else if (argc == 17) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],argv[16],NULL);
  }else if (argc == 18) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],argv[16],argv[17],NULL);
  }else if (argc == 19) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],argv[16],argv[17],argv[18],NULL);
  }else if (argc == 20) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],argv[16],argv[17],argv[18],argv[19],NULL);
  }else if (argc == 21) {
    execl(oxname,oxname,argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],argv[16],argv[17],argv[18],argv[19],argv[20],NULL);
  }else {
    fprintf(stderr,"Error in oxlog: cannot handle argc=%d\n",argc);
	fprintf(stderr,"oxname=%s\n",oxname);
    for (i=0; i<argc; i++) {
      fprintf(stderr,"argv[%d]=%s ",i,argv[i]);
    }
    fprintf(stderr,"\n");
    fprintf(stderr,"Usage example 1: oxlog /usr/X11R6/bin/xterm -icon -e /home/nobuki/kxx/ox -ox /home/nobuki/kxx/ox_asir\n");
    fprintf(stderr,"      example 2: oxlog /home/nobuki/kxx/ox -ox /home/nobuki/kxx/ox_asir >& /dev/null\n");
    exit(10);
  }
  fprintf(stderr,"\nError in oxlog: Failed to start the process.\n");
  fprintf(stderr,"oxname=%s\n",oxname);
  for (i=0; i<argc; i++) {
    fprintf(stderr,"argv[%d]=%s ",i,argv[i]);
  }
  fprintf(stderr,"\n");
  exit(20);
}
mainold1() {
  sigset_t sss;
  int i;
  /* char *env[2];
  env[0] = NULL;*/
  sigemptyset(&sss);
  sigaddset(&sss,SIGINT);
  sigprocmask(SIG_BLOCK,&sss,NULL);
  /* for (i=0; i<1000; i++) {
     sleep(1);
     printf("Hello\n");
     }
     exit(0); */
  execl("/usr/X11R6/bin/xterm","/usr/X11R6/bin/xterm",
	"-e","/home/nobuki/kxx/ox","-ox","/home/nobuki/kxx/ox_asir",NULL);
/*  execl("/home/nobuki/kxx/ox","/home/nobuki/kxx/ox","-ox","/home/nobuki/kxx/ox_asir",NULL);*/
}

char *toFullPath(char *s) {
  extern int Debug_which;
  char *path ="/bin:/usr/bin:/usr/local/bin:/usr/X11R6/bin:/usr/openwin/bin:/usr/X/bin";
  char *s2;
  if (strlen(s) == 0) {
    return(s);
  }
  if (strchr(s,'/') != NULL) return(s);
  else {
    s2 = which(s,getenv("PATH"));
  }
  if (s2 == NULL) {
	if (Debug_which) fprintf(stderr,"Could not find arg[1] in your search path.\n");
	if (Debug_which) fprintf(stderr,"You path is %s.\n",getenv("PATH"));
	if (Debug_which) fprintf(stderr,"Trying to find in the path: %s\n",path);
	s2 = which(s,path);
  }
  return(s2);
}

/*which("xterm", getenv("PATH"));*/
char *which(char *prog, char *path_env)
{
  extern int Debug_which ;
  char *tok;
  char *path;
  char delim[] = ":";
  char *e = alloca(strlen(path_env)+1);
  strcpy(e, path_env);
  tok = strtok(e, delim);
  if (Debug_which) fprintf(stderr,"PATH=%s\n",path_env);
  while (tok != NULL) {
    char *path = malloc(strlen(tok)+strlen(prog)+2);
    sprintf(path, "%s/%s", tok, prog);
    if (access(path, X_OK&R_OK) == 0) {
	  if (Debug_which) fprintf(stderr,"Found: %s\n",path);
      return path;
    }
    free(path);
    tok = strtok(NULL, delim);
  }
  return NULL;
}

