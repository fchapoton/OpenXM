/*&eg
\documentclass{article}
\title{On oxweave.c}
\author{} \date{}
\begin{document}
\maketitle
\section{Introduction}
*/
/*&jp 
\documentclass{jarticle}
\title{oxweave $B$N%=!<%9%3!<%I$K$D$$$F$N2r@b(B}
\author{} \date{}
\begin{document}
\maketitle
\section{$BA0=q$-(B}
*/
/* $OpenXM: OpenXM/src/kxx/oxweave.c,v 1.4 1999/12/13 14:47:41 takayama Exp $ */
#include <stdio.h>

/* Modify here to change the begin tag and EndComment. Less than 9 characters.
*/
char *BeginTag0="/*\x026";  /* 0x26 = & */
char *BeginTag1="//\x026";  /* 0x26 = & */
char *EndComment0="*/";
char *EndComment1="\n";

#define BSIZE 256
#define VSIZE 256
static int Debug = 0;
static int Plain = 0;
/*&jp \noindent
  $B:F5"(B option $B$r(B on $B$K$7$?>l9g(B ({\tt Recursive = 1},
  {\tt LevelState1} $B$G(B, $B0u:~$9$Y$-(B comment $B$N%M%9%H$N%l%Y%k$rI=$9(B.
  {\tt LevelState2} $B$G(B, $B:o=|$9$Y$-(B comment $B$N%M%9%H$N%l%Y%k$rI=$9(B.
*/
static int Recursive = 0;
static int LevelState1 = 0;
static int LevelState2 = 0;
/*&jp \noindent
  {\tt Buf} $B$OI8=`=PNO$h$j$N%G!<%?$r0l;~3JG<$7$F$*$/NN0h(B.
  {\tt Head} $B$,:G=i$NJ8;z$r$5$7(B, {\tt Tail} $B$O:G8e$NJ8;z$r$5$9(B.
  {\tt Buf} $B$r%j%s%0>u$K;HMQ$9$k$N$G(B, $B%j%s%0%P%C%U%!$H$h$V(B
$B$3$H$K$9$k(B.
 */
int Buf[BSIZE];
int Head = 0;
int Tail = 0;
char *Tag = NULL;
/*&jp \noindent {\tt OutputNoTaggedSegment = 1}
  $B$J$i(B $B%3%a%s%H5-9f$N30$O(B verbatim $B$rMQ$$$F=PNO(B. 
  {\tt --source} $B%*%W%7%g%s$G$3$NJQ?t$r(B1$B$K$G$-$k(B.
  {\tt --plain} $B$G$O(B,  verbatim $B$rMxMQ$;$:$K@8$G=PNO(B.
  */
int OutputNoTaggedSegment = 0;
/*&jp \noindent 1 $B$J$i%?%0$N$D$$$?>l=j$r=PNOCf(B. */
int OutputtingTaggedSegment = 0;
int BeginVerbatim = 0;


/*&jp \section{$B%W%m%0%i%`K\BN(B} */
main(int argc,char *argv[]) {
  extern char *BeginTag;
  extern char *EndComment0;
  extern char *EndComment1;
  extern int Plain;
  int c;
  int tagc,i;
  char *tagv[VSIZE];
  int tagc2;
  char *tagv2[VSIZE];
  int pos;
  Head = Tail = 0; Buf[0] = ' ';  /* initialize */

  /*&jp  {\tt tagv[]} $B$K%?%0$N$"$D$^$j$r$$$l$k(B. 
    {\tt tagv2[]} $B$KBP1~$9$k%?%0$N$*$o$j$N5-9f$r$$$l$k(B.
    */
  tagc = tagc2 = 0;
  if (argc <= 1 || argc >= VSIZE) {
    usage();
    exit();
  }else{
    for (i=1; i< argc ; i++) {
      if (strcmp(argv[i],"--source") == 0) {
	OutputNoTaggedSegment = 1;
      }else if (strcmp(argv[i],"--plain") == 0) {
	Plain = 1; OutputNoTaggedSegment = 1;
      }else if (strcmp(argv[i],"--recursive") == 0) {
	Recursive = 1;
      } else{
	if (strcmp(argv[i]," ") == 0) {
	  argv[i] = "";
	}
	tagv[tagc] = (char *) malloc(sizeof(char)*(strlen(argv[i])+10));
	tagv2[tagc2] = (char *) malloc(sizeof(char)*10);
	strcpy(tagv[tagc],BeginTag0);
	strcat(tagv[tagc],argv[i]);
	tagv2[tagc] = EndComment0; 
	/* $B%3%a%s%H$N$*$o$j$N5-9f(B.  */
	tagc2++;
	tagc++;

	tagv[tagc] = (char *) malloc(sizeof(char)*(strlen(argv[i])+10));
	tagv2[tagc2] = (char *) malloc(sizeof(char)*10);
	strcpy(tagv[tagc],BeginTag1);
	strcat(tagv[tagc],argv[i]);
	tagv2[tagc] = EndComment1;
        tagc2++;
	tagc++;
      }
    }
  }
  /*&jp $B%W%m%0%i%`$O#3$D$N>uBV$r;}$D(B. $B>uBV(B 0 $B$O%?%0IU$-%3%a%s%H5-9f$N30(B.
    $B>uBV(B 1 $B$O;XDj$5$l$?%?%0$NIU$$$?%3%a%s%H$NCf(B.
    $B>uBV(B 2 $B$O;XDj$5$l$F$$$J$$%?%0$NIU$$$?%3%a%s%H$NCf(B
    ($B>uBV#2$K$"$k$H$-$O0u:~$5$l$J$$(B.) */
  /*
    state 0  -- / * & jp  --->  state 1  
    if ( BeginVerbatim & OutputNoTaggedSegment ) end-verbatim
    <---  * /    ---   state 1
    if ( OutputNoTaggedSegment ) begin-verbatim

    state 0  -- / * & unknown  --->  state 2  
    <---  * /    ---   state 2

    state 0  & OutputNoTaggedSegment  ==> putchar()
    state 1                           ==> putchar()
    state 2                           ==> skip
    */
  while (notEOF()) {
    /* We are in the state 0. */
    pos = findNextTag(tagc,tagv,tagc2,tagv2);
    /* printf(" ===pos=%d=== ",pos); */
    /* We are in the state 1. */
    findEndTag(tagc2,tagv2,pos);
  }
  if (BeginVerbatim) {
    if (!Plain) printf("\n\\end{verbatim\x07d}\n");
  }
  exit(0);
}

/*&jp \noindent $B<!$N4X?t$OMxMQK!$rI=<($9$k(B. */
usage() {
#include "oxweaveUsage.h"
}

#define inc(a) ((a+1) % BSIZE)
/*&jp \noindent {\tt wread()} $B$O(B $BI8=`F~NO$h$j$N%G!<%?$rFI$a$k$@$1(B
$B%j%s%0%P%C%U%!(B {\tt Buf} $B$XFI$_9~$`(B.*/
wread() {
  int c,i;
  static int eof = 0;
  if (eof) return(-1);
  while (inc(Tail) != Head) {
    c = getchar();
    Tail = inc(Tail);
    Buf[Tail] = c;
    if (c == EOF) { eof = 1; return(-1); }
  }
  if (Debug == 1) {
    fprintf(stderr,"[Head=%d, Tail=%d, ",Head,Tail);
    for (i = Head; inc(i) != Tail; i = inc(i)) {
      fprintf(stderr,"%c",Buf[i]);
    }
    fprintf(stderr,"]\n");
  }
  return(0);
}

/*&jp \noindent {\tt wgetc(p)} $B$O(B
{\tt p} $BJ8;zFI$_9~$s$G:G8e$N(B 1 $BJ8;z$N%"%9%-!<%3!<%I$rLa$9(B.
 */
int wgetc(int p) {
  int c;
  int i;
  wread();
  if (p < 1) p = 1;
  for (i=0; i<p; i++) {
    c = Buf[Head];
    if (c == EOF) return(c);
    Head = inc(Head);
    wread();
  }
  return(c);
}

/*&jp  \noindent {\tt findNextTag()} $B$O<!$N(B {\tt / *\&} $B$J$k%?%0$r$5$,$9(B.
   ( $B$3$l$O(B, {\tt BeginTag0} $B$NCM$rJQ$($k$HJQ99$G$-$k(B.)
  {\tt OutputNoTaggedSegment} $B$,(B 1 $B$J$i%G!<%?$r$=$N$^$^$J$,$9(B.
  $BL5;k$9$Y$-%?%0$N$H$-$O(B, $B%?%0FbIt$r%9%-%C%W$7$?$N$A(B
  {\tt findNextTag} $B$r:F5"E*$K8F$V(B.
  */
findNextTag(int tagc, char *tagv[],int tagc2,char *tagv2[]) {
  int i;
  int c,d;
  extern char *BeginTag0;
  extern char *BeginTag1;
  do {
    for (i=0; i<tagc; i++) {
      /* fprintf(stderr,"\nChecking %s : ",tagv[i]); */
      if (wcmp(tagv[i]) == 0) {
	LevelState1++;
	/* fprintf(stderr," : matched."); */
	wgetc(strlen(tagv[i])+1);
	if (OutputNoTaggedSegment == 1 && BeginVerbatim == 1) {
	  BeginVerbatim = 0;
	  if (!Plain) printf("\\end{verbatim\x07d}\n");
	}
	OutputtingTaggedSegment = 1;
	return(i);  /* Now, state is 1. */
      }
    }
    /*&jp {\tt / *\&} $B$@$1$I$I$N%?%0$K$b0lCW$7$J$$(B */
    if (wcmp(BeginTag0) == 1) {
      LevelState2++;
      wgetc(strlen(BeginTag0));
      while ((d=wgetc(1)) > ' ') ;
      /* We are in the state 2. */
      skipToEndTag(tagc2,tagv2,0);  
      /* We are in the state 0. */
      return(findNextTag(tagc,tagv,tagc2,tagv2));
    }else if (wcmp(BeginTag1) == 1) {
      LevelState2++;
      wgetc(strlen(BeginTag1));
      while ((d=wgetc(1)) > ' ') ;
      /* We are in the state 2. */
      skipToEndTag(tagc2,tagv2,1);  
      /* We are in the state 0. */
      return(findNextTag(tagc,tagv,tagc2,tagv2));
    }
    /* We are in the state 0 */
    c = wgetc(1);
    if (OutputNoTaggedSegment) {
      if (c != EOF) putchar(c);
    }
  }while( c!= EOF);
  if (BeginVerbatim == 1) {
    if (!Plain) printf("\n\\quad\n\\end{verbatim\x07d}\n");
  }
  exit(0);
}
      
/*&jp  \noindent {\tt findEndTag()} $B$O<!$N(B {\tt * /} $B$J$k%?%0$r$5$,$9(B.
       ( $B$3$l$O(B, EndComment0 $B$NCM$rJQ$($k$HJQ992DG=(B. )
      {\tt / /} $B$G;O$^$k>l9g$O(B, 0xa $B$,$*$o$j(B. 
*/
findEndTag(int tagc,char *tagv[],int rule) {
  int i;
  int c;
  /* We are in the state 1. */
  do {
    i = rule;
    if (wcmp(tagv[i]) == 0) {
      LevelState1--;
      /* printf("LevelState1=%d\n",LevelState1);*/
      if (LevelState1 > 0 && Recursive) {
	wgetc(strlen(tagv[i]));
	printf("%s",tagv[i]);
	return(findEndTag(tagc,tagv,rule));
      }else{
	wgetc(strlen(tagv[i]));
	if (strcmp(tagv[i],"\n")==0) putchar('\n');
	OutputtingTaggedSegment = 0;
	if (OutputNoTaggedSegment) {
	  if (!Plain) printf("\n{\\footnotesize \\begin{verbatim}\n");
	  BeginVerbatim = 1;
	}
	return;			/* Our state is 0. */
      }
    }
    /* Our state is 1. */
    if (wcmp("/*") >= 0 ) {
      LevelState1++;
      /* printf("LevelState1++=%d\n",LevelState1); */
    }
    c = wgetc(1);
    putchar(c);
  }while( c!= EOF);
  fprintf(stderr,"findEndTag: unexpected EOF.\n");
  irregularExit();
}

skipToEndTag(int tagc,char *tagv[],int rule) {
  int i;
  int c;
  extern char *EndComment0;
  extern char *EndComment1;
  /* our state is 2. */
  do {
    if (rule == 0) {
      if (wcmp(EndComment0) == 0) {
	LevelState2--;
	if (LevelState2 > 0 && Recursive) {
	  wgetc(strlen(EndComment0));
	  return(skipToEndTag(tagc,tagv,rule));
	}else{
	  wgetc(strlen(EndComment0));
	  return;  /* our state is 0. */
	}
      }
    }else if (rule == 1) {
      if (wcmp(EndComment1) == 0) {
	LevelState2--;
	if (LevelState2 > 0 && Recursive) {
	  wgetc(strlen(EndComment0));
	  return(skipToEndTag(tagc,tagv,rule));
	}else{
	  wgetc(strlen(EndComment1));
	  return;  /* our state is 0. */
	}
      }
    }else{
      for (i=0; i<tagc; i++) {
	if (wcmp(tagv[i]) == 0) {
	  LevelState2--;
	  if (LevelState2 > 0 && Recursive) {
	    wgetc(strlen(EndComment0));
	    return(skipToEndTag(tagc,tagv,rule));
	  }else{
	    wgetc(strlen(tagv[i]));
	    return;  /* our state is 0. */
	  }
	}
      }

    }
    /* our state is 2. */
    if (wcmp("/*") >= 0) LevelState2++;
    c = wgetc(1);
  }while( c!= EOF);
  fprintf(stderr,"findEndTag: unexpected EOF.\n");
  irregularExit();
}
  
/*&jp \noindent {\tt wcmp(s)} $B$OJ8;zNs(B {\tt s} $B$H(B {\tt Buf[Head]} $B$+$i(B
$B$O$8$^$kJ8;zNs$rHf3S$9$k(B.
{\tt Buf[Head+strlen(s) % BSIZE]} $B$,(B 0x20 $B0J2<$G$"$j(B, $B$"$H$N%P%$%H$,(B
$B0lCW$9$l$P(B 0 $B$rLa$9(B.
$B$"$H$N%P%$%H$,(B 0x20 $B0J2<$G$J$$$,(B, $B$=$NB>$N%P%$%H$,0lCW$9$k$H$-$O(B
1 $B$rLa$9(B.
$B0J>eFs$D$N>l9g$K9gCW$7$J$$>l9g$O(B -1 $B$rLa$9(B.
{\tt s} $B$,(B 0xa,0 $B$N$H$-$O(B, Buf[Head] $B$,(B 0xa $B$J$i(B, 0 $B$rLa$9(B.
$B$=$&$G$J$$$J$i(B, -1 $B$rLa$9(B.
*/    
wcmp(char *s) {
  int n;
  int i,j;
  wread();
  if (Debug == 2) fprintf(stderr,"[Checking %s]\n",s);
  if (strcmp(s,"\n") == 0) {
    if (s[0] == Buf[Head]) return(0);
    else return(-1);
  }
  n = strlen(s);
  j = Head;
  for (i=0; i<n; i++) {
    if (s[i] != Buf[j]) return(-1);
    j = inc(j);
  }
  if (Buf[j] <= ' ') {
    if (Debug == 2) fprintf(stderr,"[Matched %s]\n",s);
    return(0);
  } else return(1);
}

notEOF() {
  wread();
  if (Buf[Head] != -1) return(1);
  else return(0);
}

irregularExit() {
  if (BeginVerbatim == 1) {
    if (!Plain) printf("\\end{verbatim\x07d}\n");
  }
  exit(-1);
}


/*&jp
\end{document}
*/
/*&eg
\end{document}
*/




