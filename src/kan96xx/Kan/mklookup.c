/* $OpenXM: OpenXM/src/kan96xx/Kan/mklookup.c,v 1.2 2000/01/16 07:55:39 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"

/* Refer to global.c. You need to keep the orders of names and values */
main() {
  int i,j;
  char *names[]={"Snull","Sinteger","Sstring","SexecutableArray","Soperator",
                 "Sdollar","Sarray","SleftBraceTag","SrightBraceTag","Spoly",
                 "SarrayOfPOLY","SmatrixOfPOLY","Slist","Sfile","Sring",
                 "SuniversalNumber","SrationalFunction","Sclass","Sdouble"};
  int values[]={Snull,Sinteger,Sstring,SexecutableArray,Soperator,
                Sdollar,Sarray,SleftBraceTag,SrightBraceTag,Spoly,
                SarrayOfPOLY,SmatrixOfPOLY,Slist,Sfile,Sring,
                SuniversalNumber,SrationalFunction,Sclass,Sdouble};
  for (i=0; i<TYPES; i++) {
    if (values[i] != i) {
      fprintf(stderr,"Errors in names and values\n");
      exit(100);
    }
    for (j=0; j<TYPES; j++) {
      printf("#define %s%s %d\n",names[i],names[j],values[i]*TYPES+values[j]);
    }
  }
  exit(0);
}

           
