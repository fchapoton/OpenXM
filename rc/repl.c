/* $OpenXM: OpenXM/rc/repl.c,v 1.4 2001/08/29 11:18:58 takayama Exp $ */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 10000
main(int argc,char *argv[]) {
  char s[BUFSIZE];
  char cwd[BUFSIZE];
  char *slash;
  char type = 'b';
  FILE *fp;

  if (argc >= 2) {
	if (strcmp(argv[1],"csh")==0) {
	  type = 'c';
	}
  }

  getcwd(cwd,BUFSIZE);
  slash = strrchr(cwd,'/');
  *slash = 0;
  while (fgets(s,BUFSIZE,stdin) != NULL) {
	if (strcmp(s,"OpenXM_HOME=$HOME/OpenXM\n") == 0) {
	  printf("OpenXM_HOME=%s\n",cwd);
	}else if (strcmp(s,"setenv OpenXM_HOME $HOME/OpenXM\n") == 0) {
	  printf("setenv OpenXM_HOME %s\n",cwd);
	}else{
	  printf("%s",s);
	}
  }

  /* Configuring environmental variables. */
  /* Check if pstoimg (src/asir-contrib) supports png format. */
  fp = fopen("/tmp/repl_test.ps","w");
  if (fp == NULL) {
	fprintf(stderr,"Open error of /tmp/repl_test.ps\n");
	exit(10);
  }
  fprintf(fp,"/Times-Roman findfont 10 scalefont setfont\n");
  fprintf(fp," 390 290 moveto  (F) show \n");
  fprintf(fp,"showpage \n");
  fclose(fp);
  if (!system("pstoimg -type png /tmp/repl_test.ps -out /tmp/repl_test.png >/dev/null")) {
	if (type == 'b') {
	  printf("export OpenXM_PSTOIMG_TYPE=png\n");
	}else{
	  printf("setenv OpenXM_PSTOIMG_TYPE png\n");
	}
  }else if (!system("pstoimg -type gif /tmp/repl_test.ps -out /tmp/repl_test.gif >/dev/null")) {
	if (type == 'b') {
	  printf("OpenXM_PSTOIMG_TYPE=gif\n");
      printf("export OpenXM_PSTOIMG_TYPE\n");
	}else{
	  printf("setenv OpenXM_PSTOIMG_TYPE=gif\n");
	}
  }else {
	printf("OpenXM_PSTOIMG_TYPE=no\n");
	printf("export OpenXM_PSTOIMG_TYPE\n");
  }
  /* system("rm -f /tmp/repl_test.*"); */

  
  exit(0);
}

	  
  
