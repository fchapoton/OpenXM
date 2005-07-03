/* $OpenXM: OpenXM/src/kan96xx/Kan/order.c,v 1.14 2005/06/16 05:07:23 takayama Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

/* The format of order.
   Example:   graded lexicographic order
   x_{N-1}  x_{N-2}  ...  x_0  D_{N-1}  ....  D_{0}
    1        1             1    1              1
    1        0             0    0              0
    0        1             0    0              0
    ..............................................

   (ringp->order)[i][j] should be (ringp->order)[i*2*N+j].
   All order matrix is generated by functions in smacro.sm1
*/

static void warningOrder(char *s);
static void errorOrder(char *s);

void setOrderByMatrix(order,n,c,l,omsize)
     int order[];
     int n,c,l,omsize;
{
  int i,j;
  int *Order;
  extern struct ring *CurrentRingp;

  switch_mmLarger("default");
  /* q-case */
  if ( l-c > 0) {
    switch_mmLarger("qmatrix");
  }

  Order = (int *)sGC_malloc(sizeof(int)*(2*n)*(omsize));
  if (Order == (int *)NULL) errorOrder("No memory.");
  CurrentRingp->order = Order;
  CurrentRingp->orderMatrixSize = omsize;
  for (i=0; i<omsize; i++) {
    for (j=0; j<2*n; j++) {
      Order[i*2*n+j] = order[i*2*n+j];
    }
  }
}
  
void showRing(level,ringp) 
     int level;
     struct ring *ringp;
{
  int i,j;
  FILE *fp;
  char tmp[100];
  int N,M,L,C,NN,MM,LL,CC;
  char **TransX,**TransD;
  int *Order;
  int P;
  char *mtype;
  extern char *F_isSameComponent;
  POLY f;
  POLY fx;
  POLY fd;
  POLY rf;
  fp = stdout;

  N=ringp->n; M = ringp->m; L = ringp->l; C = ringp->c;
  NN=ringp->nn; MM = ringp->mm; LL = ringp->ll; CC = ringp->cc;
  TransX = ringp->x; TransD = ringp->D;
  Order = ringp->order;
  P = ringp->p;
  

  fprintf(fp,"\n----------  the current ring ---- name: %s------\n",ringp->name);
  fprintf(fp,"Characteristic is %d. ",P);
  fprintf(fp,"N0=%d N=%d NN=%d M=%d MM=%d L=%d LL=%d C=%d CC=%d omsize=%d\n",N0,N,NN,M,MM,L,LL,C,CC,ringp->orderMatrixSize);
  fprintf(fp,"\n");

  /* print identifier names */
  if (N-M >0) {
    fprintf(fp,"Differential variables: ");
    for (i=M; i<N; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=M; i<N; i++) fprintf(fp," %4s ",TransD[i]);
    fprintf(fp,"\n");
    fprintf(fp,"where ");
    for (i=M; i<N; i++) {
      fx = cxx(1,i,1,ringp); fd = cdd(1,i,1,ringp);
	  rf = ppSub(ppMult(fd,fx),ppMult(fx,fd));
      fprintf(fp," %s %s - %s %s = %s, ",TransD[i],TransX[i],
              TransX[i],TransD[i],POLYToString(rf,'*',0));
    }
    fprintf(fp,"\n\n");
  }
  if (M-L >0) {
    fprintf(fp,"Difference  variables: ");
    for (i=L; i<M; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=L; i<M; i++) fprintf(fp," %4s ",TransD[i]);
    fprintf(fp,"\n");
    fprintf(fp,"where ");
    for (i=L; i<M; i++) {
      fprintf(fp," %s %s - %s %s = ",TransD[i],TransX[i],
              TransX[i],TransD[i]);
      f=ppSub(ppMult(cdd(1,i,1,ringp),cxx(1,i,1,ringp)),
              ppMult(cxx(1,i,1,ringp),cdd(1,i,1,ringp)));
      fprintf(fp," %s, ",POLYToString(f,'*',0));
    }
    fprintf(fp,"\n\n");
  }
  if (L-C >0) {
    fprintf(fp,"q-Difference  variables: ");
    for (i=C; i<L; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=C; i<L; i++) fprintf(fp," %4s ",TransD[i]);
    fprintf(fp,"\n");
    fprintf(fp,"where ");
    for (i=C; i<L; i++) {
      fprintf(fp," %s %s = %s %s %s, ",TransD[i],TransX[i],
              TransX[0],
              TransX[i],TransD[i]);
    }
    fprintf(fp,"\n\n");
  }
  if (C>0) {
    fprintf(fp,"Commutative  variables: ");
    for (i=0; i<C; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=0; i<C; i++) fprintf(fp," %4s ",TransD[i]);
    fprintf(fp,"\n\n");
  }

  if (strcmp(F_isSameComponent,"x") == 0) {
    fprintf(fp,"Integral or summation or graduation variables are : ");
    for (i=CC; i<C; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=LL; i<L; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=MM; i<M; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=NN; i<N; i++) fprintf(fp," %4s ",TransX[i]);
    fprintf(fp,"\n");
  }else if (strcmp(F_isSameComponent,"xd") == 0) {
    fprintf(fp,"Graduation variables are : ");
    for (i=CC; i<C; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=LL; i<L; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=MM; i<M; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=NN; i<N; i++) fprintf(fp," %4s ",TransX[i]);
    for (i=CC; i<C; i++) fprintf(fp," %4s ",TransD[i]);
    for (i=LL; i<L; i++) fprintf(fp," %4s ",TransD[i]);
    for (i=MM; i<M; i++) fprintf(fp," %4s ",TransD[i]);
    for (i=NN; i<N; i++) fprintf(fp," %4s ",TransD[i]);
    fprintf(fp,"\n");
  }else {
    fprintf(fp,"Unknown graduation variable specification.\n\n");
  }
  fprintf(fp,"The homogenization variable is : ");
  fprintf(fp," %4s ",TransD[0]);
  fprintf(fp,"\n");



  fprintf(fp,"-------------------------------------------\n");
  fprintf(fp,"Output order : ");
  for (i=0; i<2*N; i++) {
    if (ringp->outputOrder[i] < N) {
      fprintf(fp,"%s ",TransX[ringp->outputOrder[i]]);
    }else{
      fprintf(fp,"%s ",TransD[(ringp->outputOrder[i])-N]);
    }
  }
  fprintf(fp,"\n");

  if (ringp->multiplication == mpMult_poly) {
    mtype = "poly";
  }else if  (ringp->multiplication == mpMult_diff) {
    mtype = "diff";
  }else if  (ringp->multiplication == mpMult_difference) {
    mtype = "difference";
  }else {
    mtype = "unknown";
  }
  fprintf(fp,"Multiplication function --%s(%xH).\n",
          mtype,(unsigned int) ringp->multiplication);
  if (ringp->schreyer) {
    fprintf(fp,"schreyer=1, gbListTower=");
    printObjectList((struct object *)(ringp->gbListTower));
    fprintf(fp,"\n");
  }
  if (ringp->degreeShiftSize) {
    fprintf(fp,"degreeShift vector (N=%d,Size=%d)= \n[\n",ringp->degreeShiftN,ringp->degreeShiftSize);
    {
      int i,j;
      for (i=0; i<ringp->degreeShiftN; i++) {
        fprintf(fp," [");
        for (j=0; j< ringp->degreeShiftSize; j++) {
          fprintf(fp," %d ",ringp->degreeShift[i*(ringp->degreeShiftSize)+j]);
        }
        fprintf(fp,"]\n");
      }
    }
    fprintf(fp,"]\n");
  }
  fprintf(fp,"---  weight vectors ---\n");
  if (level) printOrder(ringp);

  if (ringp->partialEcart) {
    fprintf(fp,"---  partialEcartGlobalVarX ---\n");
    for (i=0; i<ringp->partialEcart; i++) {
      fprintf(fp," %4s ",TransX[ringp->partialEcartGlobalVarX[i]]);
    }
    fprintf(fp,"\n");
  }
  
  if (ringp->next != (struct ring *)NULL) {
    fprintf(fp,"\n\n-------- The next ring is .... --------------\n");
    showRing(level,ringp->next);
  }
}

/***************************************************************
   functions related to order
******************************************************************/
#define xtoi(k) ((N-1)-(k))
#define dtoi(k) ((2*N-1)-(k))
#define itox(k) ((N-1)-(k))
#define itod(k) ((2*N-1)-(k))
#define isX(i) (i<N? 1: 0)
#define isD(i) (i<N? 0: 1)
/****************************************************
i : 0       1         N-1       N           2N-1
x :x_{N-1} x_{N-2}   x_0  
d :                          D_{N-1}        D_{0}
if (isX(i))  x_{itox(i)}
if (isD(i))  D_{itod(i)}
******************************************************/
/* xtoi(0):N-1   xtoi(1):N-2  ....
   dtoi(0):2N-1  dtoi(1):2N-2 ...
   itod(N):N-1   dtoi(N-1):N ...
*/

void printOrder(ringp)
     struct ring *ringp;
{
  int i,j;
  FILE *fp;
  char tmp[100];
  int N,M,L,C,NN,MM,LL,CC;
  char **TransX,**TransD;
  int *Order;
  int P;
  int omsize;
  extern char *F_isSameComponent;

  N=ringp->n; M = ringp->m; L = ringp->l; C = ringp->c;
  NN=ringp->nn; MM = ringp->mm; LL = ringp->ll; CC = ringp->cc;
  TransX = ringp->x; TransD = ringp->D;
  Order = ringp->order;
  P = ringp->p;
  omsize = ringp->orderMatrixSize;

  fp = stdout;

  
  for (i=0; i<2*N; i++) printf("%4d",i);
  fprintf(fp,"\n");
  
  /* print variables names */
  for (i=0; i<N; i++) {
    sprintf(tmp,"x%d",N-1-i);
    fprintf(fp,"%4s",tmp);
  }
  for (i=0; i<N; i++) {
    sprintf(tmp,"D%d",N-1-i);
    fprintf(fp,"%4s",tmp);
  }
  fprintf(fp,"\n");

  /* print identifier names */
  for (i=0; i<N; i++) fprintf(fp,"%4s",TransX[itox(i)]);
  for (i=N; i<2*N; i++) fprintf(fp,"%4s",TransD[itod(i)]);
  fprintf(fp,"\n");

  /* print D: differential     DE: differential, should be eliminated
     E: difference
     Q: q-difference
     C: commutative
  */
  if (strcmp(F_isSameComponent,"x")== 0 || strcmp(F_isSameComponent,"xd")==0) {
    for (i=0; i<N; i++) {
      if ((NN<=itox(i)) && (itox(i)<N)) fprintf(fp,"%4s","DE");
      if ((M<=itox(i)) && (itox(i)<NN)) fprintf(fp,"%4s","D");
      if ((MM<=itox(i)) && (itox(i)<M)) fprintf(fp,"%4s","EE");
      if ((L<=itox(i)) && (itox(i)<MM)) fprintf(fp,"%4s","E");
      if ((LL<=itox(i)) && (itox(i)<L)) fprintf(fp,"%4s","QE");
      if ((C<=itox(i)) && (itox(i)<LL)) fprintf(fp,"%4s","Q");
      if ((CC<=itox(i)) && (itox(i)<C)) fprintf(fp,"%4s","CE");
      if ((0<=itox(i)) && (itox(i)<CC)) fprintf(fp,"%4s","C");
    }
  }
  if (strcmp(F_isSameComponent,"x")==0) {
    for (i=N; i<2*N; i++) {
      if ((M<=itod(i)) && (itod(i)<N)) fprintf(fp,"%4s","D");
      if ((L<=itod(i)) && (itod(i)<M)) fprintf(fp,"%4s","E");
      if ((C<=itod(i)) && (itod(i)<L)) fprintf(fp,"%4s","Q");
      if ((0<=itod(i)) && (itod(i)<C)) fprintf(fp,"%4s","C");
    }
  }else if (strcmp(F_isSameComponent,"xd")==0) {
    for (i=N; i<2*N; i++) {
      if ((NN<=itod(i)) && (itod(i)<N)) fprintf(fp,"%4s","DE");
      if ((M<=itod(i)) && (itod(i)<NN)) fprintf(fp,"%4s","D");
      if ((MM<=itod(i)) && (itod(i)<M)) fprintf(fp,"%4s","EE");
      if ((L<=itod(i)) && (itod(i)<MM)) fprintf(fp,"%4s","E");
      if ((LL<=itod(i)) && (itod(i)<L)) fprintf(fp,"%4s","QE");
      if ((C<=itod(i)) && (itod(i)<LL)) fprintf(fp,"%4s","Q");
      if ((CC<=itod(i)) && (itod(i)<C)) fprintf(fp,"%4s","CE");
      if ((0<=itod(i)) && (itod(i)<CC)) fprintf(fp,"%4s","C");
    }
  } else {
    fprintf(fp,"Unknown graduation variable type.\n");
  }
  fprintf(fp,"\n");

  for (i=0; i< omsize; i++) {
    for (j=0; j<2*N; j++) {
      fprintf(fp,"%4d", Order[i*2*N+j]);
    }
    fprintf(fp,"\n");
  }
  fprintf(fp,"\n");

}

struct object oGetOrderMatrix(struct ring *ringp)
{
  struct object rob = OINIT;
  struct object ob2 = OINIT;
  int n,i,j,m;
  int *om;
  n = ringp->n;
  m = ringp->orderMatrixSize;
  om = ringp->order;
  if (m<=0) m = 1;
  rob = newObjectArray(m);
  for (i=0; i<m; i++) {
    ob2 = newObjectArray(2*n);
    for (j=0; j<2*n; j++) {
      putoa(ob2,j,KpoInteger(om[2*n*i+j]));
    }
    putoa(rob,i,ob2);
  }
  return(rob);
}


int mmLarger_matrix(ff,gg)
     POLY ff; POLY gg;
{
  int exp[2*N0]; /* exponents */
  int i,k;
  int sum,flag;
  int *Order;
  int N;
  MONOMIAL f,g;
  struct ring *rp;
  int in2;
  int *from, *to;
  int omsize;
  int dssize;
  int dsn;
  int *degreeShiftVector;
  
  if (ff == POLYNULL ) {
    if (gg == POLYNULL) return( 2 );
    else return( 0 );
  }
  if (gg == POLYNULL) {
    if (ff == POLYNULL) return( 2 );
    else return( 1 );
  }
  f = ff->m; g=gg->m;

  rp = f->ringp;
  Order = rp->order;
  N = rp->n;
  from = rp->from;
  to = rp->to;
  omsize = rp->orderMatrixSize;
  if (dssize = rp->degreeShiftSize) {
	degreeShiftVector = rp->degreeShift;  /* Note. 2003.06.26 */
	dsn = rp->degreeShiftN;
  }
 
  flag = 1;
  for (i=N-1,k=0; i>=0; i--,k++) {
    exp[k] = (f->e[i].x) - (g->e[i].x);
    exp[k+N] = (f->e[i].D) - (g->e[i].D);
    if ((exp[k] != 0) || (exp[k+N] != 0)) flag =0;
  }
  if (flag==1) return(2);
  /* exp > 0   <--->  f>g
     exp = 0   <--->  f=g
     exp < 0   <--->  f<g
  */
  for (i=0; i< omsize; i++) {
    sum = 0; in2 = i*2*N;
    /* for (k=0; k<2*N; k++) sum += exp[k]*Order[in2+k]; */
    for (k=from[i]; k<to[i]; k++) sum += exp[k]*Order[in2+k];
    if (dssize && ( i < dsn)) { /* Note, 2003.06.26 */
      if ((f->e[N-1].x < dssize) && (f->e[N-1].x >= 0) &&
          (g->e[N-1].x < dssize) && (g->e[N-1].x >= 0)) {
        sum += degreeShiftVector[i*dssize+ (f->e[N-1].x)]
              -degreeShiftVector[i*dssize+ (g->e[N-1].x)];
      }else{
        /*warningOrder("Size mismatch in the degree shift vector. It is ignored.");*/
      }
    }
    if (sum > 0) return(1);
    if (sum < 0) return(0);
  }
  return(2);
}

/* This should be used in case of q */
int mmLarger_qmatrix(ff,gg)
     POLY ff; POLY gg;
{
  int exp[2*N0]; /* exponents */
  int i,k;
  int sum,flag;
  int *Order;
  int N;
  MONOMIAL f,g;
  int omsize;
  
  if (ff == POLYNULL ) {
    if (gg == POLYNULL) return( 2 );
    else return( 0 );
  }
  if (gg == POLYNULL) {
    if (ff == POLYNULL) return( 2 );
    else return( 1 );
  }
  f = ff->m; g = gg->m;
  Order = f->ringp->order;
  N = f->ringp->n;
  omsize = f->ringp->orderMatrixSize;
  
  flag = 1;
  for (i=N-1,k=0; i>=0; i--,k++) {
    exp[k] = (f->e[i].x) - (g->e[i].x);
    exp[k+N] = (f->e[i].D) - (g->e[i].D);
    if ((exp[k] != 0) || (exp[k+N] != 0)) flag =0;
  }
  if (flag==1) return(2);
  /* exp > 0   <--->  f>g
     exp = 0   <--->  f=g
     exp < 0   <--->  f<g
  */
  for (i=0; i< omsize; i++) {
    sum = 0;
    /* In case of q, you should do as follows */
    for (k=0; k<N-1; k++) sum += exp[k]*Order[i*2*N+k]; /* skip k= N-1 -->q */
    for (k=N; k<2*N-1; k++) sum += exp[k]*Order[i*2*N+k]; /* SKip k= 2*N-1 */
    if (sum > 0) return(1);
    else if (sum < 0) return(0);
  }
  if (exp[N-1] > 0) return(1);
  else if (exp[N-1] < 0) return(0);
  else return(2);
}

/* x(N-1)>x(N-2)>....>D(N-1)>....>D(0) */
mmLarger_pureLexicographic(f,g)
     POLY f;
     POLY g;
{
  int i,r;
  int n;
  MONOMIAL fm,gm;
  /* Note that this function ignores the order matrix of the given
     ring. */
  if (f == POLYNULL ) {
    if (g == POLYNULL) return( 2 );
    else return( 0 );
  }
  if (g == POLYNULL) {
    if (f == POLYNULL) return( 2 );
    else return( 1 );
  }


  fm = f->m; gm = g->m;
  n = fm->ringp->n;
  for (i=n-1; i>=0; i--) {
    r = (fm->e[i].x) - (gm->e[i].x);
    if (r > 0) return(1);
    else if (r < 0) return(0);
    else ;
  }

  for (i=n-1; i>=0; i--) {
    r = (fm->e[i].D) - (gm->e[i].D);
    if (r > 0) return(1);
    else if (r < 0) return(0);
    else ;
  }

  return(2);

}


void setFromTo(ringp)
     struct ring *ringp;
{
  int n;
  int i,j,oasize;
  if (ringp->order == (int *)NULL) errorOrder("setFromTo(); no order matrix.");
  n = (ringp->n)*2;
  oasize = ringp->orderMatrixSize;
  ringp->from = (int *)sGC_malloc(sizeof(int)*oasize);
  ringp->to = (int *)sGC_malloc(sizeof(int)*oasize);
  if (ringp->from == (int *)NULL  || ringp->to == (int *)NULL) {
    errorOrder("setFromTo(): No memory.");
  }
  for (i=0; i<oasize; i++) {
    ringp->from[i] = 0; ringp->to[i] = n;
    for (j=0; j<n; j++) {
      if (ringp->order[i*n+j] != 0) {
        ringp->from[i] = j;
        break;
      }
    }
    for (j=n-1; j>=0; j--) {
      if (ringp->order[i*n+j] != 0) {
        ringp->to[i] = j+1;
        break;
      }
    }
  }
}

/* It ignores h and should be used with mmLarger_tower */
/* cf. mmLarger_matrix.  h always must be checked at last. */
static int mmLarger_matrix_schreyer(ff,gg)
     POLY ff; POLY gg;
{
  int exp[2*N0]; /* exponents */
  int i,k;
  int sum,flag;
  int *Order;
  int N;
  MONOMIAL f,g;
  struct ring *rp;
  int in2;
  int *from, *to;
  int omsize;
  
  if (ff == POLYNULL ) {
    if (gg == POLYNULL) return( 2 );
    else return( 0 );
  }
  if (gg == POLYNULL) {
    if (ff == POLYNULL) return( 2 );
    else return( 1 );
  }
  f = ff->m; g=gg->m;

  rp = f->ringp;
  Order = rp->order;
  N = rp->n;
  from = rp->from;
  to = rp->to;
  omsize = rp->orderMatrixSize;
 
  flag = 1;
  for (i=N-1,k=0; i>0; i--,k++) {
    exp[k] = (f->e[i].x) - (g->e[i].x);
    exp[k+N] = (f->e[i].D) - (g->e[i].D);
    if ((exp[k] != 0) || (exp[k+N] != 0)) flag =0;
  }
  exp[N-1] = (f->e[0].x) - (g->e[0].x); 
  exp[2*N-1] = 0;  /* f->e[0].D - g->e[0].D.  Ignore h! */
  if ((exp[N-1] != 0) || (exp[2*N-1] != 0)) flag =0;

  if (flag==1) return(2);
  /* exp > 0   <--->  f>g
     exp = 0   <--->  f=g
     exp < 0   <--->  f<g
  */
  for (i=0; i< omsize; i++) {
    sum = 0; in2 = i*2*N;
    /* for (k=0; k<2*N; k++) sum += exp[k]*Order[in2+k]; */
    for (k=from[i]; k<to[i]; k++) sum += exp[k]*Order[in2+k];
    if (sum > 0) return(1);
    if (sum < 0) return(0);
  }
  return(2);
}

int mmLarger_tower(POLY f,POLY g) {
  struct object *gbList;
  int r;
  if (f == POLYNULL) {
    if (g == POLYNULL)  return(2);
    else return(0);
  }
  if (g == POLYNULL) {
    if (f == POLYNULL) return(2);
    else return(1);
  }
  if (!(f->m->ringp->schreyer) || !(g->m->ringp->schreyer))
    return(mmLarger_matrix(f,g));
  /* modifiable: mmLarger_qmatrix */
  gbList = (struct object *)(g->m->ringp->gbListTower);
  if (gbList == NULL) return(mmLarger_matrix(f,g));
  /* modifiable: mmLarger_qmatrix */
  if (gbList->tag != Slist) {
    warningOrder("mmLarger_tower(): gbList must be in Slist.\n");
    return(1);
  }
  if (klength(gbList) ==0) return(mmLarger_matrix(f,g));
  /* modifiable: mmLarger_qmatrix */

  r = mmLarger_tower3(f,g,gbList);
  /* printf("mmLarger_tower3(%s,%s) -->  %d\n",POLYToString(head(f),'*',1),POLYToString(head(g),'*',1),r); */
  if (r == 2) { /* Now, compare by h */
    if (f->m->e[0].D > g->m->e[0].D) return(1);
    else if (f->m->e[0].D < g->m->e[0].D) return(0);
    else return(2);
  }else{
    return(r);
  }
}

int mmLarger_tower3(POLY f,POLY g,struct object *gbList)
{ /* gbList is assumed to be Slist */
  int n,fv,gv,t,r,nn;
  POLY fm;
  POLY gm;
  struct object gb = OINIT;

  if (f == POLYNULL) {
    if (g == POLYNULL)  return(2);
    else return(0);
  }
  if (g == POLYNULL) {
    if (f == POLYNULL) return(2);
    else return(1);   /* It assumes the zero is the minimum element!! */
  }
  n = f->m->ringp->n;
  nn = f->m->ringp->nn;
  /* critical and modifiable */  /* m e_u > m e_v <==> m g_u > m g_v */
  /*                  or equal and u < v */  
  fv = f->m->e[nn].x ; /* extract component (vector) number of f! */
  gv = g->m->e[nn].x ;
  if (fv == gv) { /* They have the same component number. */
    return(mmLarger_matrix_schreyer(f,g));
  }

  if (gbList == NULL) return(mmLarger_matrix_schreyer(f,g));
  /* modifiable: mmLarger_qmatrix */
  if (gbList->tag != Slist) {
    warningOrder("mmLarger_tower(): gbList must be in Slist.\n");
    return(1);
  }
  if (klength(gbList) ==0) return(mmLarger_matrix(f,g));
  /* modifiable: mmLarger_qmatrix */
  gb = car(gbList);  /* each entry must be monomials */
  if (gb.tag != Sarray) {
    warningOrder("mmLarger_tower3(): car(gbList) must be an array.\n");
    return(1);
  }
  t = getoaSize(gb);
  if (t == 0) return(mmLarger_tower3(f,g,cdr(gbList)));

  fm = pmCopy(head(f)); fm->m->e[nn].x = 0; /* f is not modified. */
  gm = pmCopy(head(g)); gm->m->e[nn].x = 0;
  if (fv >= t || gv >= t) {
    warningOrder("mmLarger_tower3(): incompatible input and gbList.\n");
    printf("Length of gb is %d, f is %s, g is %s\n",t,KPOLYToString(f),
           KPOLYToString(g));
    KSexecuteString(" show_ring ");
    return(1);
  }
  /* mpMult_poly is too expensive to call. @@@*/
  r = mmLarger_tower3(mpMult_poly(fm,KopPOLY(getoa(gb,fv))),
                      mpMult_poly(gm,KopPOLY(getoa(gb,gv))),
                      cdr(gbList));
  if (r != 2) return(r);
  else if (fv == gv) return(2);
  else if (fv > gv) return(0); /* modifiable */
  else if (fv < gv) return(1); /* modifiable */
}

static struct object auxPruneZeroRow(struct object ob) {
  int i,m,size;
  struct object obt = OINIT;
  struct object rob = OINIT;
  m = getoaSize(ob);
  size=0;
  for (i=0; i<m; i++) {
	obt = getoa(ob,i);
	if (getoaSize(obt) != 0) size++;
  }
  if (size == m) return ob;
  rob = newObjectArray(size);
  for (i=0, size=0; i<m; i++) {
	obt = getoa(ob,i);
	if (getoaSize(obt) != 0) {
	  putoa(rob,size,obt); size++;
	}
  }
  return rob;
}  
static struct object oRingToOXringStructure_long(struct ring *ringp)
{
  struct object rob = OINIT;
  struct object ob2 = OINIT;
  struct object obMat = OINIT;
  struct object obV = OINIT;
  struct object obShift = OINIT;
  struct object obt = OINIT;
  char **TransX; char **TransD;
  int n,i,j,m,p,nonzero;
  int *om;
  n = ringp->n;
  m = ringp->orderMatrixSize;
  om = ringp->order;
  TransX = ringp->x; TransD = ringp->D;
  if (m<=0) m = 1;
  /*test: (1). getRing /rr set rr (oxRingStructure) dc  */
  obMat = newObjectArray(m);
  for (i=0; i<m; i++) {
    nonzero = 0;
    for (j=0; j<2*n; j++) {
      if (om[2*n*i+j] != 0) nonzero++;
    }
    ob2 = newObjectArray(nonzero*2);
    nonzero=0;
    for (j=0; j<2*n; j++) {
      /* fprintf(stderr,"%d, ",nonzero); */
      if (om[2*n*i+j] != 0) {
        if (j < n) {
          putoa(ob2,nonzero,KpoString(TransX[n-1-j])); nonzero++;
        }else{
          putoa(ob2,nonzero,KpoString(TransD[n-1-(j-n)])); nonzero++;
        }
        putoa(ob2,nonzero,KpoUniversalNumber(newUniversalNumber(om[2*n*i+j]))); nonzero++;
      }
    }
    /* printObject(ob2,0,stderr); fprintf(stderr,".\n"); */
    putoa(obMat,i,ob2);
  }
  obMat = auxPruneZeroRow(obMat);
  /* printObject(obMat,0,stderr); */

  obV = newObjectArray(2*n);
  for (i=0; i<n; i++) putoa(obV,i,KpoString(TransX[n-1-i]));
  for (i=0; i<n; i++) putoa(obV,i+n,KpoString(TransD[n-1-i]));
  /* printObject(obV,0,stderr); */

  if (ringp->degreeShiftSize) {
    /*test:
    [(x) ring_of_differential_operators [[(x)]] weight_vector 0
      [(weightedHomogenization) 1 (degreeShift) [[1 2 1]]] ] define_ring ;
     (1). getRing /rr set rr (oxRingStructure) dc message
    */
    obShift = newObjectArray(ringp->degreeShiftN);
    for (i=0; i<ringp->degreeShiftN; i++) {
      obt = newObjectArray(ringp->degreeShiftSize);
      for (j=0; j< ringp->degreeShiftSize; j++) {
        putoa(obt,j,KpoUniversalNumber(newUniversalNumber(ringp->degreeShift[i*(ringp->degreeShiftSize)+j])));
      }
      putoa(obShift,i,obt);
    }
    /* printObject(obShift,0,stderr); */
  }

  p = 0;
  if (ringp->degreeShiftSize) {
    rob = newObjectArray(3);
    obt = newObjectArray(2);
    putoa(obt,0,KpoString("degreeShift"));
    putoa(obt,1,obShift);
    putoa(rob,p, obt); p++;
  }else {
    rob = newObjectArray(2);
  }

  obt = newObjectArray(2);
  putoa(obt,0,KpoString("v"));
  putoa(obt,1,obV);
  putoa(rob,p, obt); p++;

  obt = newObjectArray(2);
  putoa(obt,0,KpoString("order"));
  putoa(obt,1,obMat);
  putoa(rob,p, obt); p++;

  return(rob);
}
static int auxEffectiveVar(int idx,int n) {
  int x;
  if (idx < n) x=1; else x=0;
  if (x) {
	if ((idx >= 1) && (idx < n-1)) return 1;
	else return 0;
  }else{
	if ( 1 <= idx-n )  return 1;
	else return 0;
  }
}
/*test: 
   [(x,y) ring_of_differential_operators [[(Dx) 1 (Dy)  1]]
    weight_vector 0] define_ring
    (x). getRing (oxRingStructure) dc ::
 */
static struct object oRingToOXringStructure_short(struct ring *ringp)
{
  struct object rob = OINIT;
  struct object ob2 = OINIT;
  struct object obMat = OINIT;
  struct object obV = OINIT;
  struct object obShift = OINIT;
  struct object obt = OINIT;
  char **TransX; char **TransD;
  int n,i,j,m,p,nonzero;
  int *om;
  n = ringp->n;
  m = ringp->orderMatrixSize;
  om = ringp->order;
  TransX = ringp->x; TransD = ringp->D;
  if (m<=0) m = 1;
  /*test: (1). getRing /rr set rr (oxRingStructure) dc  */
  obMat = newObjectArray(m);
  for (i=0; i<m; i++) {
    nonzero = 0;
    for (j=0; j<2*n; j++) {
      if ((om[2*n*i+j] != 0) && auxEffectiveVar(j,n)) nonzero++;
    }
    ob2 = newObjectArray(nonzero*2);
    nonzero=0;
    for (j=0; j<2*n; j++) {
      /* fprintf(stderr,"%d, ",nonzero); */
      if ((om[2*n*i+j] != 0) && auxEffectiveVar(j,n)) {
        if (j < n) {
          putoa(ob2,nonzero,KpoString(TransX[n-1-j])); nonzero++;
        }else{
          putoa(ob2,nonzero,KpoString(TransD[n-1-(j-n)])); nonzero++;
        }
        putoa(ob2,nonzero,KpoUniversalNumber(newUniversalNumber(om[2*n*i+j]))); nonzero++;
      }
    }
    /* printObject(ob2,0,stderr); fprintf(stderr,".\n"); */
    putoa(obMat,i,ob2);
  }
  obMat = auxPruneZeroRow(obMat);
  /* printObject(obMat,0,stderr); */

  obV = newObjectArray(2*n-3);
  for (i=0; i<n-2; i++) putoa(obV,i,KpoString(TransX[n-1-i-1]));
  for (i=0; i<n-1; i++) putoa(obV,i+n-2,KpoString(TransD[n-1-i-1]));
  /* printObject(obV,0,stderr); */

  if (ringp->degreeShiftSize) {
    /*test:
    [(x) ring_of_differential_operators [[(x)]] weight_vector 0
      [(weightedHomogenization) 1 (degreeShift) [[1 2 1]]] ] define_ring ;
     (1). getRing /rr set rr (oxRingStructure) dc message
    */
    obShift = newObjectArray(ringp->degreeShiftN);
    for (i=0; i<ringp->degreeShiftN; i++) {
      obt = newObjectArray(ringp->degreeShiftSize);
      for (j=0; j< ringp->degreeShiftSize; j++) {
        putoa(obt,j,KpoUniversalNumber(newUniversalNumber(ringp->degreeShift[i*(ringp->degreeShiftSize)+j])));
      }
      putoa(obShift,i,obt);
    }
    /* printObject(obShift,0,stderr); */
  }

  p = 0;
  if (ringp->degreeShiftSize) {
    rob = newObjectArray(3);
    obt = newObjectArray(2);
    putoa(obt,0,KpoString("degreeShift"));
    putoa(obt,1,obShift);
    putoa(rob,p, obt); p++;
  }else {
    rob = newObjectArray(2);
  }

  obt = newObjectArray(2);
  putoa(obt,0,KpoString("v"));
  putoa(obt,1,obV);
  putoa(rob,p, obt); p++;

  obt = newObjectArray(2);
  putoa(obt,0,KpoString("order"));
  putoa(obt,1,obMat);
  putoa(rob,p, obt); p++;

  return(rob);
}
struct object oRingToOXringStructure(struct ring *ringp)
{
  struct object rob = OINIT;
  struct object tob = OINIT;
  rob = newObjectArray(2);
  tob = oRingToOXringStructure_short(ringp);
  putoa(rob,0,tob);
  tob = oRingToOXringStructure_long(ringp);
  putoa(rob,1,tob);
  return(rob);
}

static void warningOrder(s)
     char *s;
{
  fprintf(stderr,"Warning in order.c: %s\n",s);
}

static void errorOrder(s)
     char *s;
{
  fprintf(stderr,"order.c: %s\n",s);
  exit(14);
}


