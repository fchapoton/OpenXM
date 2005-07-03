/* $OpenXM: OpenXM/src/kan96xx/plugin/cmotagToName.c,v 1.3 2001/05/04 01:06:30 takayama Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "cmotag.h"
#define BSIZE 1024

main() {
  FILE *fp;
  char s[BSIZE];
  int i, start;
  char *a;
  fp = fopen("cmotag.h","r");
  if (fp == NULL) {
    fprintf(stderr,"Inputfile cmotag.h could be found.\n");
    exit(10);
  }

  fprintf(stderr,"cmotagToName.c is generating the function cmotagToName.\n");
  printf("/* This is automatically generated by cmotagToName.c. DO NOT EDIT.*/\n");
  printf("static char *cmotagToName(int p) {\n");
  printf("if (0) { ; \n");
  while (fgets(s,BSIZE-11,fp) != NULL) {
    for (i=start=0; i<BSIZE-10; i++) {
      if (s[i] == '\0') goto aa ;
      if (s[i] == '#') {
        s[i+7] = '\0';
        if (strcmp(&(s[i+1]),"define") != 0) goto aa ;
        start = i+8;
        break;
      }
    }
    i = start;
    while (s[i] <= ' ') {
      if (s[i] == '\0') goto aa ;
      i++;
    }
    start = i;
    while (s[i] > ' ') {
      i++;
    }
    s[i] = '\0';
    /* CMO_XXXX is stored in s[start], ..., s[i]='\0' */
    a = &(s[start]);
    /* printf("%s\n",a); */
    printf("} else if (p == %s) {\n",a);
    printf("  return(\"%s\");\n",a);

  aa: ;
  }
  printf("} else { return(\"Unknown tag\"); }\n");
  printf("}\n");
  exit(0);
}

    
    
