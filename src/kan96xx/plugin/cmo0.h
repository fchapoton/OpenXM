/* $OpenXM: OpenXM/src/kan96xx/plugin/cmo0.h,v 1.2 2000/01/16 07:55:46 takayama Exp $ */
/*  cmo0.h */

typedef enum {CMOINIT,CMOPUT,CMOGET,CMOFLUSH,
	      CMOINITSTREAM,CMOGETBYTE,CMOERROR,CMOERROR2} cmoAction;
typedef unsigned int cmoint;

struct  cmoBuffer {
  int size;
  int pos;
  int rpos;
  int isStream;
  int errorno;
  FILE2 *fp;
  void *buf;
};

/* ------------------------------------------- */
struct object cmoObjectToCmo(struct object ob);
void cmoPrintCmo(struct object ob);
struct object cmoCmoToObject(struct object ob);

/* ----------------------- */
int errorCmo(char *s);
struct cmoBuffer *cmoOutputToBuf(cmoAction a,void *data, int size);
int dumpCmoBuf(struct cmoBuffer *cb);
int cmoOutInt32(int k);
int cmoOutString(char *s,int size);
int cmoOutInt32Coeff(int k);
int cmoGetIntFromBuf(cmoAction a,struct cmoBuffer *cb);
int cmoOutMonomial32(POLY cell);
int cmoOutDMS() ;
int cmoOutPolynomial(POLY f) ;
int cmoOutPolynomial2(POLY f) ;
int cmoOutRingDefinition(struct ring * rp,int option);
int cmoOutRingDefinition2(struct ring * rp,int option);

void *cmoGetString(struct cmoBuffer *cb, int size);
struct coeff * cmoGetInt32Coeff(struct cmoBuffer *cb);
POLY cmoGetMonomial32(struct cmoBuffer *cb);
POLY cmoListToPOLY(struct object ob); /* Assumes [CMO_ADD, ....] */
struct object streamToCmo(struct object of);
int cmoToStream(struct object cmoObj, struct object of);
struct object cmoListToPoly(struct object ob);
struct object cmoListToPoly2(struct object ob);
struct object cmoObjectToStream(struct object ob, struct object obStream);
struct object cmoObjectFromStream(struct object obStream);
struct object cmoObjectFromStream2(FILE2 *fp2);
struct object cmoObjectToStream2(struct object ob,FILE2 *fp2);

int Kan_PushBinary(int size,void *data);
void *Kan_PopBinary(int *sizep);
int Kan_pushCMOFromStream(FILE2 *fp);
int Kan_popCMOToStream(FILE2 *fp,int serial);


int cmoOutRawInt(int k);
void *KSmathCapByStruct(void) ;
struct object KSmathCap(void);

struct object cmoTranslateVariable_outGoing(struct object ob);
struct object cmoTranslateVariable_inComming(struct object ob);
/* ---------------- end of cmo.h ---------------------*/











