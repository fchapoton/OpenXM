/*
  $OpenXM: OpenXM/src/hgm/mh/src/sfile.c,v 1.1 2013/02/19 08:03:14 takayama Exp $
 */
#include <stdio.h>
#include "sfile.h"
#define SSIZE 5
void *mh_malloc(int size) { return((void *)malloc(size)); }

struct SFILE *mh_fopen(char *name,char *mode,int byFile) {
  struct SFILE *sfp;
  sfp = mh_malloc(sizeof(struct SFILE));
  sfp->byFile=0; sfp->s=NULL; sfp->pt=0; sfp->len=0;sfp->limit=0; sfp->fp=NULL;
  sfp->forRead=1; sfp->copied=0;
  
  if (byFile) {
	sfp->byFile = 1;
	sfp->fp = fopen(name,mode);
	if (sfp->fp == NULL) return(NULL);
	else return(sfp);
  }else if (strcmp(mode,"r")==0) {
	sfp->byFile=0;
	sfp->s=name;
	sfp->pt=0;
	sfp->len=strlen(name);
	sfp->limit=sfp->len+1;
	sfp->forRead=1;
	return(sfp);
  }else if (strcmp(mode,"w")==0) {
	sfp->byFile=0;
	sfp->s=mh_malloc(SSIZE);
	sfp->pt=0;
	sfp->len=0;
	sfp->limit= SSIZE;
	sfp->forRead=0;
	return(sfp);
  }else{
	fprintf(stderr,"Error: unknown mode %s in the string mode.\n",mode);
	return NULL;
  }
}

char *mh_fgets(char *str,int size,struct SFILE *sfp) {
  int i,pt,len;
  char *s;
  if (sfp->byFile) return fgets(str,size,sfp->fp);
  s = sfp->s; len = sfp->len; pt = sfp->pt;
  if (s[pt] == 0) return NULL;
  if (pt >= len) return NULL;
  if (size != 0) str[0]=0;
  for (i=0; i<size-1; i++) {
	if (s[pt] != 0) {
	  str[i] = s[pt]; str[i+1] = 0;
	  pt++; sfp->pt = pt;
	  if (s[pt] == 0) return str;
	  if (pt >= len) return str;
	  if (str[i] == '\n') return str;
	}
  }
  return str;
}
int mh_fputs(char *str,struct SFILE *sfp) {
  int i,pt,len,limit;
  int inputLen;
  char *s;
  if (sfp->byFile) return fputs(str,sfp->fp);
  s = sfp->s; len = sfp->len; pt = sfp->pt; limit=sfp->limit;
  inputLen=strlen(str);
  if (inputLen+len+1 > limit) {
	limit *= 2;
	s = (char *) mh_malloc(limit);
	if (s == NULL) return(EOF);
	strcpy(s,sfp->s);
    free(sfp->s);
  }
  strcpy(&(s[len]),str);
  len += inputLen;
  sfp->s=s; sfp->len=len; sfp->limit=limit;
  return(0);
}

/* Note: copy the resulting string sfp->s before mh_fclose */
int mh_fclose(struct SFILE *sfp) {
  if (sfp->byFile) return fclose(sfp->fp);
  if (! (sfp->forRead)) {
	if (!sfp->copied) fprintf(stderr,"Warning in mh_fclose. sfp->s has not been copied, but deallocated.\n"); 
	if (sfp->s != NULL) { free(sfp->s); sfp->s = NULL; }
  }
  free(sfp);
}

int mh_outstr(char *str,int size,struct SFILE *sfp) {
  int i;
  if (sfp->byFile) {
	fprintf(stderr,"Error in mh_outstr. mh_outstr is called in the file i/o mode.\n");
	return 0;
  }
  if (size) str[0] = 0;
  for (i = 0; (i<size-1) && (i<sfp->len); i++) {
	str[i] = (sfp->s)[i]; str[i+1] = 0;
  }
  sfp->copied=1;
  return(i);
}


/* for debugging */
dump(struct SFILE *sfp) {
  printf("byFile=%d\n",sfp->byFile);
  if (sfp->s) printf("sfp->s=%s\n",sfp->s);
  printf("pt=%d\n",sfp->pt);
  printf("len=%d\n",sfp->len);
  printf("limit=%d\n",sfp->limit);
  printf("fp=%p\n",sfp->fp);
}

#define TESTSIZE 1024
main() {
  struct SFILE *sfp;
  char str[TESTSIZE];
  int i;
/*
  sfp = mh_fopen("hoge\nafo\nbho\ncat\ndot\ndolphin\n","r",0);
  while (mh_fgets(str,1024,sfp)) {
	printf(str);
  }
  mh_fclose(sfp);

  sfp = mh_fopen("sfile.h","r",1);
  while (mh_fgets(str,1024,sfp)) {
	printf(str);
  }
  mh_fclose(sfp);
*/
  sfp = mh_fopen("","w",0);
  for (i=0; i<3; i++) {
	mh_fputs("hoge\n",sfp);
	mh_fputs("hage\n",sfp);
	dump(sfp);
  }
  mh_fputs("end end\n",sfp);
  printf("result=%s\n",sfp->s);
  mh_outstr(str,TESTSIZE,sfp);
  mh_fclose(sfp);
}
