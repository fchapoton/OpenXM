/* $OpenXM$ */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "lookup.h"
#include "matrix.h"
#include "gradedset.h"
#include "kclass.h"

static int Message = 1;
extern int KanGBmessage;
  
/** :kan, :ring */
struct object Kreduction(f,set)
     struct object f;
     struct object set;
{
  POLY r;
  struct gradedPolySet *grG;
  struct syz0 syz;
  struct object rob;
  int flag;
  extern int ReduceLowerTerms;
  
  if (f.tag != Spoly) errorKan1("%s\n","Kreduction(): the first argument must be a polynomial.");

  if (ectag(set) == CLASSNAME_GradedPolySet) {
    grG = KopGradedPolySet(set);
    flag = 1;
  }else{
    if (set.tag != Sarray) errorKan1("%s\n","Kreduction(): the second argument must be a set of polynomials.");
    grG = arrayToGradedPolySet(set);
    flag = 0;
  }
  if (ReduceLowerTerms) {
    r = (*reductionCdr)(f.lc.poly,grG,1,&syz);
  }else{
    r = (*reduction)(f.lc.poly,grG,1,&syz);
  }
  if (flag) {
    rob = newObjectArray(3);
    putoa(rob,0,KpoPOLY(r));
    putoa(rob,1,KpoPOLY(syz.cf));
    putoa(rob,2,syzPolyToArray(countGradedPolySet(grG),syz.syz,grG));
  }else {
    rob = newObjectArray(4);
    putoa(rob,0,KpoPOLY(r));
    putoa(rob,1,KpoPOLY(syz.cf));
    putoa(rob,2,syzPolyToArray(getoaSize(set),syz.syz,grG));
    putoa(rob,3,gradedPolySetToArray(grG,1));
  }
  return(rob);
}

struct object Kgroebner(ob)
     struct object ob;
{
  int needSyz = 0;
  int needBack = 0;
  int needInput = 0;
  int countDown = 0;
  int cdflag = 0;
  struct object ob1,ob2,ob2c;
  int i;
  struct gradedPolySet *grG;
  struct pair *grP;
  struct arrayOfPOLY *a;
  struct object rob;
  struct gradedPolySet *grBases;
  struct matrixOfPOLY *mp;
  struct matrixOfPOLY *backwardMat;
  struct object ob1New;
  extern char *F_groebner;
  extern int CheckHomogenization;
  extern int StopDegree;
  int sdflag = 0;
  int forceReduction = 0;

  int ob1Size, ob2Size, noZeroEntry;
  int *ob1ToOb2;
  int *ob1ZeroPos;
  int method;
  int j,k;
  struct object rob2;
  struct object rob3;
  struct object rob4;
  struct ring *myring;
  POLY f;
  struct object orgB;
  struct object newB;
  struct object orgC;
  struct object newC;
  static struct object paddingVector(struct object ob, int table[], int m);
  static struct object unitVector(int pos, int size,struct ring *r);
  extern struct ring *CurrentRingp;
  
  StopDegree = 0x7fff;

  if (ob.tag != Sarray) errorKan1("%s\n","Kgroebner(): The argument must be an array.");
  switch(getoaSize(ob)) {
  case 1:
    needBack = 0; needSyz = 0; needInput = 0;
    ob1 = getoa(ob,0);
    break;
  case 2:
    ob1 = getoa(ob,0);
    ob2 = getoa(ob,1);
    if (ob2.tag != Sarray) {
      errorKan1("%s\n","Kgroebner(): The options must be given by an array.");
    }
    for (i=0; i<getoaSize(ob2); i++) {
      ob2c = getoa(ob2,i);
      if (ob2c.tag == Sdollar) {
        if (strcmp(ob2c.lc.str,"needBack")==0) {
          needBack = 1;
        }else if (strcmp(ob2c.lc.str,"needSyz")==0) {
          if (!needBack) {
            /* warningKan("Kgroebner(): needBack is automatically set."); */
          }
          needSyz = needBack = 1;
        }else if (strcmp(ob2c.lc.str,"forceReduction")==0) {
          forceReduction = 1;
        }else if (strcmp(ob2c.lc.str,"countDown")==0) {
          countDown = 1; cdflag = 1;
          if (needSyz) {
            warningKan("Kgroebner(): needSyz is automatically turned off.");
            needSyz = 0;
          }
        }else if (strcmp(ob2c.lc.str,"StopDegree")==0) {
          StopDegree = 0; sdflag = 1;
          if (needSyz) {
            warningKan("Kgroebner(): needSyz is automatically turned off.");
            needSyz = 0;
          }
        }else {
          warningKan("Unknown keyword for options.");
        }
      }else if (ob2c.tag == Sinteger) {
        if (cdflag) {
          cdflag = 0;
          countDown = KopInteger(ob2c);
        }else if (sdflag) {
          sdflag = 0;
          StopDegree = KopInteger(ob2c);
        }
      }
    }
    break;
  default:
    errorKan1("%s\n","Kgroebner(): [ [polynomials] ] or [[polynomials] [options]].");
  }
     
  if (ob1.tag != Sarray) errorKan1("%s\n","Kgroebner(): The argument must be an array. Example: [ [$x-1$ . $x y -2$ .] [$needBack$ $needSyz$ $needInput$]] ");
  ob1New = newObjectArray(getoaSize(ob1));
  for (i=0; i< getoaSize(ob1); i++) {
    if (getoa(ob1,i).tag == Spoly) {
      putoa(ob1New,i,getoa(ob1,i));
    }else if (getoa(ob1,i).tag == Sarray) {
      /* If the generater is given as an array, flatten it. */
      putoa(ob1New,i,KpoPOLY( arrayToPOLY(getoa(ob1,i))));
    }else{
      errorKan1("%s\n","Kgroebner(): The elements must be polynomials or array of polynomials.");
    }
    /* getoa(ob1,i) is poly, now check the homogenization. */
    if (CheckHomogenization) {
      if ((strcmp(F_groebner,"standard")==0) &&
          !isHomogenized(KopPOLY(getoa(ob1New,i)))) {
        fprintf(stderr,"\n%s",KPOLYToString(KopPOLY(getoa(ob1New,i))));
        errorKan1("%s\n","Kgroebner(): The above polynomial is not homogenized. cf. homogenize.");
      }
    }
  }
  ob1 = ob1New;

  /* To handle the input with zero entries. For debug, debug/gr.sm1*/
  ob1Size = getoaSize(ob1);
  ob2Size = 0; myring = CurrentRingp;
  for (i=0; i<ob1Size; i++) {
    if (KopPOLY(getoa(ob1,i)) != POLYNULL) ob2Size++;
  }
  if (ob2Size == ob1Size) noZeroEntry = 1;
  else noZeroEntry = 0;
  if (ob1Size == 0)  {
    if (needBack && needSyz) {
      rob = newObjectArray(3);
      putoa(rob,0,newObjectArray(0));
      putoa(rob,1,newObjectArray(0));
      putoa(rob,2,newObjectArray(0));
    }else if (needBack) {
      rob = newObjectArray(2);
      putoa(rob,0,newObjectArray(0));
      putoa(rob,1,newObjectArray(0));
    }else {
      rob = newObjectArray(1);
      putoa(rob,0,newObjectArray(0));
    }
    return(rob);
  }
  /* Assume ob1size > 0 */
  if (ob2Size == 0) {
    rob2 = newObjectArray(1); putoa(rob2,0,KpoPOLY(POLYNULL));
    if (needBack && needSyz) {
      rob = newObjectArray(3);
      putoa(rob,0,rob2);
      rob3 = newObjectArray(1);
      putoa(rob3,0,unitVector(-1,ob1Size,(struct ring *)NULL));
      putoa(rob,1,rob3);
      rob4 = newObjectArray(ob1Size);
      for (i=0; i<ob1Size; i++) {
        putoa(rob4,i,unitVector(i,ob1Size,myring));
      }
      putoa(rob,2,rob4);
    }else if (needBack) {
      rob = newObjectArray(2);
      putoa(rob,0,rob2);
      rob3 = newObjectArray(1);
      putoa(rob3,0,unitVector(-1,ob1Size,(struct ring *)NULL));
      putoa(rob,1,rob3);
    }else {
      rob = newObjectArray(1);
      putoa(rob,0,rob2);
    }
    return(rob);
  }
  /* Assume ob1Size , ob2Size > 0 */
  ob2 = newObjectArray(ob2Size);
  ob1ToOb2 =   (int *)GC_malloc(sizeof(int)*ob1Size);
  ob1ZeroPos = (int *)GC_malloc(sizeof(int)*(ob1Size-ob2Size+1));
  if (ob1ToOb2 == NULL || ob1ZeroPos == NULL) errorKan1("%s\n","No more memory.");
  j = 0; k = 0;
  for (i=0; i<ob1Size; i++) {
    f = KopPOLY(getoa(ob1,i));
    if (f != POLYNULL) {
      myring = f->m->ringp;
      putoa(ob2,j,KpoPOLY(f));
      ob1ToOb2[i] = j; j++;
    }else{
      ob1ToOb2[i] = -1;
      ob1ZeroPos[k] = i; k++;
    }
  }

  a = arrayToArrayOfPOLY(ob2);
  grG = (*groebner)(a,needBack,needSyz,&grP,countDown,forceReduction);

  if (strcmp(F_groebner,"gm") == 0 && (needBack || needSyz)) {
    warningKan("The options needBack and needSyz are ignored.");
    needBack = needSyz = 0;
  }
  
  /*return(gradedPolySetToGradedArray(grG,0));*/
  if (needBack && needSyz) {
    rob = newObjectArray(3);
    if (Message && KanGBmessage) {
      printf("Computing the backward transformation   ");
      fflush(stdout);
    }
    getBackwardTransformation(grG); /* mark and syz is modified. */
    if (KanGBmessage) printf("Done.\n");

    /* Computing the syzygies. */
    if (Message && KanGBmessage) {
      printf("Computing the syzygies    ");
      fflush(stdout);
    }
    mp = getSyzygy(grG,grP->next,&grBases,&backwardMat);
    if (KanGBmessage) printf("Done.\n");

    putoa(rob,0,gradedPolySetToArray(grG,0));
    putoa(rob,1,matrixOfPOLYToArray(backwardMat));
    putoa(rob,2,matrixOfPOLYToArray(mp));
  }else if (needBack) {
    rob = newObjectArray(2);
    if (Message && KanGBmessage) {
      printf("Computing the backward transformation.....");
      fflush(stdout);
    }
    getBackwardTransformation(grG); /* mark and syz is modified. */
    if (KanGBmessage) printf("Done.\n");
    putoa(rob,0,gradedPolySetToArray(grG,0));
    putoa(rob,1,getBackwardArray(grG));
  }else { 
    rob = newObjectArray(1);
    putoa(rob,0,gradedPolySetToArray(grG,0));
  }

  /* To handle zero entries in the input. */
  if (noZeroEntry) {
    return(rob);
  }
  method = getoaSize(rob);
  switch(method) {
  case 1:
    return(rob);
    break;
  case 2:
    orgB = getoa(rob,1); /* backward transformation. */
    newB = newObjectArray(getoaSize(orgB));
    for (i=0; i<getoaSize(orgB); i++) {
      putoa(newB,i,paddingVector(getoa(orgB,i),ob1ToOb2,ob1Size));
    }
    rob2 = newObjectArray(2);
    putoa(rob2,0,getoa(rob,0));
    putoa(rob2,1,newB);
    return(rob2);
    break;
  case 3:
    orgB = getoa(rob,1); /* backward transformation. */
    newB = newObjectArray(getoaSize(orgB));
    for (i=0; i<getoaSize(orgB); i++) {
      putoa(newB,i,paddingVector(getoa(orgB,i),ob1ToOb2,ob1Size));
    }
    orgC = getoa(rob,2);
    newC = newObjectArray(getoaSize(orgC)+ob1Size-ob2Size);
    for (i=0; i<getoaSize(orgC); i++) {
      putoa(newC, i, paddingVector(getoa(orgC,i),ob1ToOb2,ob1Size));
    }
    for (i = getoaSize(orgC), j = 0; i<getoaSize(orgC)+ob1Size-ob2Size; i++,j++) {
      putoa(newC,i,unitVector(ob1ZeroPos[j],ob1Size,myring));
    }
    rob2 = newObjectArray(3);
    putoa(rob2,0,getoa(rob,0));
    putoa(rob2,1,newB);
    putoa(rob2,2,newC);
    return(rob2);
    break;
  default:
    errorKan1("%s","Kgroebner: unknown method.");
  }
}

static struct object paddingVector(struct object ob, int table[], int m)
{
  struct object rob;
  int i;
  rob = newObjectArray(m);
  for (i=0; i<m; i++) {
    if (table[i] != -1) {
      putoa(rob,i,getoa(ob,table[i]));
    }else{
      putoa(rob,i,KpoPOLY(POLYNULL));
    }
  }
  return(rob);
}
  
static struct object unitVector(int pos, int size,struct ring *r)
{
  struct object rob;
  int i;
  POLY one;
  rob = newObjectArray(size);
  for (i=0; i<size; i++) {
    putoa(rob,i,KpoPOLY(POLYNULL));
  }
  if ((0 <= pos) && (pos < size)) {
    putoa(rob,pos, KpoPOLY(cxx(1,0,0,r)));
  }
  return(rob);
}
  
  
  
/* :misc */

#define INITGRADE 3
#define INITSIZE 0

struct gradedPolySet *arrayToGradedPolySet(ob)
     struct object ob;
{
  int n,i,grd,ind;
  POLY f;
  struct gradedPolySet *grG;
  int serial;
  extern int Sugar;
  
  if (ob.tag != Sarray) errorKan1("%s\n","arrayToGradedPolySet(): the argument must be array.");
  n = getoaSize(ob);
  for (i=0; i<n; i++) {
    if (getoa(ob,i).tag != Spoly)
      errorKan1("%s\n","arrayToGradedPolySet(): the elements must be polynomials.");
  }
  grG = newGradedPolySet(INITGRADE);

  for (i=0; i<grG->lim; i++) {
    grG->polys[i] = newPolySet(INITSIZE);
  }
  for (i=0; i<n; i++) {
    f = KopPOLY(getoa(ob,i));
    grd = -1; whereInG(grG,f,&grd,&ind,Sugar);
    serial = i;
    grG = putPolyInG(grG,f,grd,ind,(struct syz0 *)NULL,1,serial);
  }
  return(grG);
}


struct object polySetToArray(ps,keepRedundant)
     struct polySet *ps;
     int keepRedundant;
{
  int n,i,j;
  struct object ob;
  if (ps == (struct polySet *)NULL) return(newObjectArray(0));
  n = 0;
  if (keepRedundant) {
    n = ps->size;
  }else{
    for (i=0; i<ps->size; i++) {
      if (ps->del[i] == 0) ++n;
    }
  }
  ob = newObjectArray(n);
  j = 0;
  for (i=0; i<ps->size; i++) {
    if (keepRedundant || (ps->del[i] == 0)) {
      putoa(ob,j,KpoPOLY(ps->g[i]));
      j++;
    }
  }
  return(ob);
}


struct object gradedPolySetToGradedArray(gps,keepRedundant)
     struct gradedPolySet *gps;
     int keepRedundant;
{
  struct object ob,vec;
  int i;
  if (gps == (struct gradedPolySet *)NULL) return(NullObject);
  ob = newObjectArray(gps->maxGrade +1);
  vec = newObjectArray(gps->maxGrade);
  for (i=0; i<gps->maxGrade; i++) {
    putoa(vec,i,KpoInteger(i));
    putoa(ob,i+1,polySetToArray(gps->polys[i],keepRedundant));
  }
  putoa(ob,0,vec);
  return(ob);
}

  
struct object gradedPolySetToArray(gps,keepRedundant)
     struct gradedPolySet *gps;
     int keepRedundant;
{
  struct object ob,vec;
  struct polySet *ps;
  int k;
  int i,j;
  int size;
  if (gps == (struct gradedPolySet *)NULL) return(NullObject);
  size = 0;
  for (i=0; i<gps->maxGrade; i++) {
    ps = gps->polys[i];
    if (keepRedundant) {
      size += ps->size;
    }else{
      for (j=0; j<ps->size; j++) {
        if (ps->del[j] == 0) ++size;
      }
    }
  }
    
  ob = newObjectArray(size);
  k = 0;
  for (i=0; i<gps->maxGrade; i++) {
    ps = gps->polys[i];
    for (j=0; j<ps->size; j++) {
      if (keepRedundant || (ps->del[j] == 0)) {
        putoa(ob,k,KpoPOLY(ps->g[j]));
        k++;
      }
    }
  }
  return(ob);
}
  

/* serial == -1  :  It's not in the marix input. */
struct object syzPolyToArray(size,f,grG)
     int size;
     POLY f;
     struct gradedPolySet *grG;
{
  struct object ob;
  int i,g0,i0,serial;

  ob = newObjectArray(size);
  for (i=0; i<size; i++) {
    putoa(ob,i,KpoPOLY(ZERO));
  }

  while (f != POLYNULL) {
    g0 = srGrade(f);
    i0 = srIndex(f);
    serial = grG->polys[g0]->serial[i0];
    if (serial < 0) {
      errorKan1("%s\n","syzPolyToArray(): invalid serial[i] of grG.");
    }
    if (KopPOLY(getoa(ob,serial)) != ZERO) {
      errorKan1("%s\n","syzPolyToArray(): syzygy polynomial is broken.");
    }
    putoa(ob,serial,KpoPOLY(f->coeffp->val.f));
    f = f->next;
  }
  return(ob);
}

struct object getBackwardArray(grG)
     struct gradedPolySet *grG;
{
  /* use serial, del.  cf. getBackwardTransformation(). */
  int inputSize,outputSize;
  int i,j,k;
  struct object ob;
  struct polySet *ps;
  
  inputSize = 0; outputSize = 0;
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->serial[j] >= 0) ++inputSize;
      if (ps->del[j] == 0) ++outputSize;
    }
  }

  ob = newObjectArray(outputSize);
  k = 0;
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->del[j] == 0) {
        putoa(ob,k,syzPolyToArray(inputSize,ps->syz[j]->syz,grG));
        k++;
      }
    }
  }
  return(ob);
}


POLY arrayToPOLY(ob)
     struct object ob;
{
  int size,i;
  struct object f;
  POLY r;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  POLY ff,ee;
  MONOMIAL tf;

  if (ob.tag != Sarray) errorKan1("%s\n","arrayToPOLY(): The argument must be an array.");
  size = getoaSize(ob);
  r = ZERO;
  for (i=0; i<size; i++) {
    f = getoa(ob,i);
    if (f.tag != Spoly) errorKan1("%s\n","arrayToPOLY(): The elements must be polynomials.");
    ff = KopPOLY(f);
    if (ff != ZERO) {
      tf = ff->m;
      if (tf->ringp != cr) {
        n = tf->ringp->n;
        m = tf->ringp->m;
        l = tf->ringp->l;
        c = tf->ringp->c;
        nn = tf->ringp->nn;
        mm = tf->ringp->mm;
        ll = tf->ringp->ll;
        cc = tf->ringp->cc;
        cr = tf->ringp;
      }
      if (n-nn >0) ee = cxx(1,n-1,i,tf->ringp);
      else if (m-mm >0) ee = cxx(1,m-1,i,tf->ringp);
      else if (l-ll >0) ee = cxx(1,l-1,i,tf->ringp);
      else if (c-cc >0) ee = cxx(1,c-1,i,tf->ringp);
      else ee = ZERO;
      r = ppAddv(r,ppMult(ee,ff));
    }
  }
  return(r);
}

struct object POLYToArray(ff)
     POLY ff;
{
  
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  POLY ee;
  MONOMIAL tf;
  int k,i,matn,size;
  struct matrixOfPOLY *mat;
  POLY ex,sizep;
  struct object ob;

  if (ff != ZERO) {
    tf = ff->m;
    if (tf->ringp != cr) {
      n = tf->ringp->n;
      m = tf->ringp->m;
      l = tf->ringp->l;
      c = tf->ringp->c;
      nn = tf->ringp->nn;
      mm = tf->ringp->mm;
      ll = tf->ringp->ll;
      cc = tf->ringp->cc;
      cr = tf->ringp;
    }
    if (n-nn >0) ee = cxx(1,n-1,1,tf->ringp);
    else if (m-mm >0) ee = cxx(1,m-1,1,tf->ringp);
    else if (l-ll >0) ee = cxx(1,l-1,1,tf->ringp);
    else if (c-cc >0) ee = cxx(1,c-1,1,tf->ringp);
    else ee = ZERO;
  }else{
    ob = newObjectArray(1);
    getoa(ob,0) = KpoPOLY(ZERO);
    return(ob);
  }
  mat = parts(ff,ee);
  matn = mat->n;
  sizep = getMatrixOfPOLY(mat,0,0);
  if (sizep == ZERO) size = 1;
  else size = coeffToInt(sizep->coeffp)+1;
  ob = newObjectArray(size);
  for (i=0; i<size; i++) getoa(ob,i) = KpoPOLY(ZERO);
  for (i=0; i<matn; i++) {
    ex = getMatrixOfPOLY(mat,0,i);
    if (ex == ZERO) k = 0;
    else {
      k = coeffToInt(ex->coeffp);
    }
    getoa(ob,k) = KpoPOLY(getMatrixOfPOLY(mat,1,i));
  }
  return(ob);
}

static int isThereh(f)
     POLY f;
{
  POLY t;
  if (f == 0) return(0);
  t = f;
  while (t != POLYNULL) {
    if (t->m->e[0].D) return(1);
    t = t->next;
  }
  return(0);
}

struct object homogenizeObject(ob,gradep)
     struct object ob;
     int *gradep;
{
  struct object rob,ob1;
  int maxg;
  int gr,flag,i,d,size;
  struct ring *rp;
  POLY f;
  extern struct ring *CurrentRingp;
  extern int Homogenize_vec;

  if (!Homogenize_vec) return(homogenizeObject_vec(ob,gradep));
  
  switch(ob.tag) {
  case Spoly:
    if (isThereh(KopPOLY(ob))) {
      fprintf(stderr,"\n%s\n",KPOLYToString(KopPOLY(ob)));
      errorKan1("%s\n","homogenizeObject(): The above polynomial has already had a homogenization variable.\nPut the homogenization variable 1 before homogenization.\ncf. replace.");
    }
    f = homogenize( KopPOLY(ob) );
    *gradep = (*grade)(f);
    return(KpoPOLY(f));
    break;
  case Sarray:
    size = getoaSize(ob);
    if (size == 0) {
      errorKan1("%s\n","homogenizeObject() is called for the empty array.");
    }
    rob = newObjectArray(size);
    flag = 0;
    ob1 = getoa(ob,0);
	if (ob1.tag == Sdollar) return(homogenizeObject_go(ob,gradep));
    ob1 = homogenizeObject(ob1,&gr);
    maxg = gr;
    getoa(rob,0) = ob1;
    for (i=1; i<size; i++) {
      ob1 = getoa(ob,i);
      ob1 = homogenizeObject(ob1,&gr);
      if (gr > maxg) {
        maxg = gr;
      }
      getoa(rob,i) = ob1;
    }
    maxg = maxg+size-1;
    if (1) {
      rp = oRingp(rob);
      if (rp == (struct ring *)NULL) rp = CurrentRingp;
      for (i=0; i<size; i++) {
        gr = oGrade(getoa(rob,i));
        /**printf("maxg=%d, gr=%d(i=%d) ",maxg,gr,i); fflush(stdout);**/
        if (maxg > gr) {
          f = cdd(1,0,maxg-gr-i,rp); /* h^{maxg-gr-i} */
          getoa(rob,i) = KooMult(KpoPOLY(f),getoa(rob,i));
        }
      }
    }
    *gradep = maxg;
    return(rob);
    break;
  default:
    errorKan1("%s\n","homogenizeObject(): Invalid argument data type.");
    break;
  }
}

struct object homogenizeObject_vec(ob,gradep)
     struct object ob;
     int *gradep;
{
  struct object rob,ob1;
  int maxg;
  int gr,i,size;
  POLY f;
  extern struct ring *CurrentRingp;
  
  switch(ob.tag) {
  case Spoly:
    if (isThereh(KopPOLY(ob))) {
      fprintf(stderr,"\n%s\n",KPOLYToString(KopPOLY(ob)));
      errorKan1("%s\n","homogenizeObject_vec(): The above polynomial has already had a homogenization variable.\nPut the homogenization variable 1 before homogenization.\ncf. replace.");
    }
    if (containVectorVariable(KopPOLY(ob))) {
      errorKan1("%s\n","homogenizedObject_vec(): The given polynomial contains a variable to express a vector component.");
    }
    f = homogenize( KopPOLY(ob) );
    *gradep = (*grade)(f);
    return(KpoPOLY(f));
    break;
  case Sarray:
    size = getoaSize(ob);
    if (size == 0) {
      errorKan1("%s\n","homogenizeObject_vec() is called for the empty array.");
    }
	if (getoa(ob,0).tag == Sdollar) return(homogenizeObject_go(ob,gradep));
    rob = newObjectArray(size);
    for (i=0; i<size; i++) {
      ob1 = getoa(ob,i);
      ob1 = homogenizeObject_vec(ob1,&gr);
      if (i==0) maxg = gr;
      else {
        maxg = (maxg > gr? maxg: gr);
      }
      putoa(rob,i,ob1);
    }
    *gradep = maxg;
    return(rob);
    break;
  default:
    errorKan1("%s\n","homogenizeObject_vec(): Invalid argument data type.");
    break;
  }
}

struct object homogenizeObject_go(struct object ob,int *gradep) {
  int size,i,dssize,j;
  struct object ob0;
  struct object ob1;
  struct object ob2;
  struct object rob;
  struct object tob;
  struct object ob1t;
  int *ds;
  POLY f;
  rob = NullObject;
  if (ob.tag != Sarray) errorKan1("%s\n","homogenizeObject_go(): Invalid argument data type.");

  size = getoaSize(ob);
  if (size == 0) errorKan1("%s\n","homogenizeObject_go(): the first argument must be a string.");
  ob0 = getoa(ob,0);
  if (ob0.tag != Sdollar) {
    errorKan1("%s\n","homogenizeObject_go(): the first argument must be a string.");
  }
  if (strcmp(KopString(ob0),"degreeShift") == 0) {
	if (size != 3)
	  errorKan1("%s\n","homogenizeObject_go(): [(degreeShift) shift-vector obj]");
	ob1 = getoa(ob,1); ob2 = getoa(ob,2);
	dssize = getoaSize(ob1);
	ds = (int *)sGC_malloc(sizeof(int)*(dssize>0?dssize:1));
	for (i=0; i<dssize; i++) {
	  ds[i] = objToInteger(getoa(ob1,i));
	}
	if (ob2.tag == Spoly) {
	  f = goHomogenize11(KopPOLY(ob2),ds,dssize,-1);
	  rob = KpoPOLY(f);
	}else if (ob2.tag == SuniversalNumber) {
	  rob = ob2;
	}else if (ob2.tag == Sarray) {
	  rob = newObjectArray(getoaSize(ob2));
	  for (i=0; i<getoaSize(ob2); i++) {
		tob = newObjectArray(3);
		ob1t = newObjectArray(dssize);
		if (getoa(ob2,i).tag == Spoly) {
		  for (j=0; j<dssize; j++) getoa(ob1t,j) = KpoInteger(0);
		  for (j=0; j<dssize-i; j++) getoa(ob1t,j) = getoa(ob1,j+i);
		}else{
		  ob1t = ob1;
		}
		getoa(tob,0) = ob0; getoa(tob,1) = ob1t; getoa(tob,2) = getoa(ob2,i);
		getoa(rob,i) = homogenizeObject_go(tob,gradep);
	  }
	}else{
	  errorKan1("%s\n","homogenizeObject_go(): invalid object for the third element.");
	}
  }else{
	  errorKan1("%s\n","homogenizeObject_go(): unknown key word.");
  }
  return( rob );	
}


struct ring *oRingp(ob)
     struct object ob;
{
  struct ring *rp,*rptmp;
  int i,size;
  POLY f;
  switch(ob.tag) {
  case Spoly:
    f = KopPOLY(ob);
    if (f == ZERO) return((struct ring *)NULL);
    return( f->m->ringp);
    break;
  case Sarray:
    size = getoaSize(ob);
    rp = (struct ring *)NULL;
    for (i=0; i<size; i++) {
      rptmp = oRingp(getoa(ob,i));
      if (rptmp != (struct ring *)NULL) rp = rptmp;
      return(rp);
    }
    break;
  default:
    errorKan1("%s\n","oRingp(): Invalid argument data type.");
    break;
  }
}

int oGrade(ob)
     struct object ob;
{
  int i,size;
  POLY f;
  int maxg,tmpg;
  switch(ob.tag) {
  case Spoly:
    f = KopPOLY(ob);
    return( (*grade)(f) );
    break;
  case Sarray:
    size = getoaSize(ob);
    if (size == 0) return(0);
    maxg = oGrade(getoa(ob,0));
    for (i=1; i<size; i++) {
      tmpg = oGrade(getoa(ob,i));
      if (tmpg > maxg) maxg = tmpg;
    }
    return(maxg);
    break;
  default:
    errorKan1("%s\n","oGrade(): Invalid data type for the argument.");
    break;
  }
}


struct object oPrincipalPart(ob)
     struct object ob;
{
  POLY f;
  struct object rob;

  switch(ob.tag) {
  case Spoly:
    f = KopPOLY(ob);
    return( KpoPOLY(POLYToPrincipalPart(f)));
    break;
  default:
    errorKan1("%s\n","oPrincipalPart(): Invalid data type for the argument.");
    break;
  }
}
struct object oInitW(ob,oWeight)
     struct object ob;
     struct object oWeight;
{
  POLY f;
  struct object rob;
  int w[2*N0];
  int n,i;
  struct object ow;

  if (oWeight.tag != Sarray) {
    errorKan1("%s\n","oInitW(): the second argument must be array.");
  }
  n = getoaSize(oWeight);
  if (n >= 2*N0) errorKan1("%s\n","oInitW(): the size of the second argument is invalid.");
  for (i=0; i<n; i++) {
    ow = getoa(oWeight,i);
    if (ow.tag != Sinteger) {
      errorKan1("%s\n","oInitW(): the entries of the second argument must be integers.");
    }
    w[i] = KopInteger(ow);
  }
  switch(ob.tag) {
  case Spoly:
    f = KopPOLY(ob);
    return( KpoPOLY(POLYToInitW(f,w)));
    break;
  default:
    errorKan1("%s\n","oInitW(): Argument must be polynomial.");
    break;
  }
}

int KpolyLength(POLY f) {
  int size;
  if (f == POLYNULL) return(1);
  size = 0;
  while (f != POLYNULL) {
    f = f->next;
    size++;
  }
  return(size);
}

int validOutputOrder(int ord[],int n) {
  int i,j,flag;
  for (i=0; i<n; i++) {
    flag = 0;
    for (j=0; j<n; j++) {
      if (ord[j] == i) flag = 1;
    }
    if (flag == 0) return(0); /* invalid */
  }
  return(1);
}

struct object KsetOutputOrder(struct object ob, struct ring *rp)
{
  int n,i;
  struct object ox;
  struct object otmp;
  int *xxx;
  int *ddd;
  if (ob.tag  != Sarray) {
    errorKan1("%s\n","KsetOutputOrder(): the argument must be of the form [x y z ...]");
  }
  n = rp->n;
  ox = ob;
  if (getoaSize(ox) != 2*n) {
    errorKan1("%s\n","KsetOutputOrder(): the argument must be of the form [x y z ...] and the length of [x y z ...] must be equal to the number of x and D variables.");
  }
  xxx = (int *)sGC_malloc(sizeof(int)*n*2);
  if (xxx == NULL ) {
    errorKan1("%s\n","KsetOutputOrder(): no more memory.");
  }
  for (i=0; i<2*n; i++) {
    otmp = getoa(ox,i);
    if(otmp.tag != Sinteger) {
      errorKan1("%s\n","KsetOutputOrder(): elements must be integers.");
    }
    xxx[i] = KopInteger(otmp);
  }
  if (!validOutputOrder(xxx,2*n)) {
    errorKan1("%s\n","KsetOutputOrder(): Invalid output order for variables.");
  }
  rp->outputOrder = xxx;
  return(ob);
}

struct object KschreyerSkelton(struct object g)
{
  struct object rob;
  struct object ij;
  struct object ab;
  struct object tt;
  struct arrayOfPOLY *ap;
  struct arrayOfMonomialSyz ans;
  int k;
  rob.tag = Snull;
  if (g.tag != Sarray) {
    errorKan1("%s\n","KschreyerSkelton(): argument must be an array of polynomials.");
  }

  ap = arrayToArrayOfPOLY(g);
  ans = schreyerSkelton(*ap);

  rob = newObjectArray(ans.size);
  for (k=0; k<ans.size; k++) {
    ij = newObjectArray(2);
    putoa(ij,0, KpoInteger(ans.p[k]->i));
    putoa(ij,1, KpoInteger(ans.p[k]->j));
    ab = newObjectArray(2);
    putoa(ab,0, KpoPOLY(ans.p[k]->a));
    putoa(ab,1, KpoPOLY(ans.p[k]->b));
    tt = newObjectArray(2);
    putoa(tt,0, ij);
    putoa(tt,1, ab);
    putoa(rob,k,tt);
  }
  return(rob);
}

struct object KisOrdered(struct object of)
{
  if (of.tag != Spoly) {
    errorKan1("%s\n","KisOrdered(): argument must be a polynomial.");
  }
  if (isOrdered(KopPOLY(of))) {
    return(KpoInteger(1));
  }else{
    return(KpoInteger(0));
  }
}

struct object KvectorToSchreyer_es(struct object obarray)
{
  int m,i;
  int nn;
  POLY f;
  POLY g;
  struct object ob;
  struct ring *rp;
  if (obarray.tag != Sarray) {
    errorKan1("%s\n","KvectorToSchreyer_es(): argument must be an array of polynomials.");
  }
  m = getoaSize(obarray);
  f = POLYNULL;
  for (i=0; i<m; i++) {
    ob = getoa(obarray,i);
    if (ob.tag != Spoly) {
      errorKan1("%s\n","KvectorToSchreyer_es(): each element of the array must be a polynomial.");
    }
    g = KopPOLY(ob);
    if (g != POLYNULL) {
      rp = g->m->ringp;
      nn = rp->nn;
      /*   g = es^i  g */
      g = mpMult_poly(cxx(1,nn,i,rp), g);
      if (!isOrdered(g)) {
        errorKan1("%s\n","KvectorToSchreyer_es(): given polynomial is not ordered properly by the given Schreyer order.");
      }
      f = ppAdd(f,g);
    }
  }
  return(KpoPOLY(f));
}

int objToInteger(struct object ob) {
  if (ob.tag == Sinteger) {
	return KopInteger(ob);
  }else if (ob.tag == SuniversalNumber) {
	return(coeffToInt(KopUniversalNumber(ob)));
  }else {
	errorKan1("%s\n","objToInteger(): invalid argument.");
  }
}
