/*
$OpenXM$
License: LGPL
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define SMAX 4096
#define inci(i) { i++; if (i >= argc) { fprintf(stderr,"Option argument is not given.\n"); return(-1); }}
extern char *MH_Gfname;
extern char *MH_Dfname;

/* global variables. They are set in setParam() */
int MH_RANK;
int MH_M;

int MH_Mg;  /* m */
double *MH_Beta; /* beta[0], ..., beta[m-1] */
double *MH_Ng;   /* freedom n.  c=(m+1)/2+n/2; Note that it is a pointer */
double MH_X0g;   /* initial point */
static double *Iv;   /* Initial values of mhg sorted by mhbase() in rd.rr at beta*x0 */
static double Ef;   /* exponential factor at beta*x0 */
extern double MH_Hg;   /* step size of rk defined in rk.c */
int MH_Dp;      /* Data sampling period */
static double Xng=0.0;   /* the last point */
int MH_RawName = 0;
static int Testrank=0;
extern int MH_Verbose;

extern int MH_P95;  /* 95 % points */
int mh_gopen_file(void);
double mh_rkmain(double x0,double y0[],double xn);
static int setParamTest(void);
static int setParamDefault(void);
static int setParam(char *fname);
static int showParam(void);

/* #define DEBUG */
#ifdef DEBUG
char *MH_Dfname; char *MH_Gfname; double MH_Hg;
int mh_gopen_file(void) { }
double mh_rkmain(double x0,double y0[],double xn) { }
#endif

void *mh_malloc(int s) {
  void *p;
  p = (void*)malloc(s);
  if (p == NULL) {
	fprintf(stderr,"No memory.\n"); mh_exit(-1);
  }
  return(p);
}
mh_free(void *p) {
  free(p);
  return(0);
}
static mypower(int x,int n) {
  int a,i;
  a = 1;
  for (i=0; i<n; i++) a = a*x;
  return(a);
}
#ifdef STANDALONE
main(int argc,char *argv[]) {
  return mh_main(argc,char *argv[]);
}
#endif
mh_main(int argc,char *argv[]) {
  double *y0;
  double x0,xn;
  double ef;
  int i,rank;
  setParam(NULL); MH_Gfname = MH_Dfname = NULL; MH_Verbose=1;
  for (i=1; i<argc; i++) {
	if (strcmp(argv[i],"--idata")==0) {
	  inci(i);
	  setParam(argv[i]); MH_Verbose=0;
	}else if (strcmp(argv[i],"--gnuplotf")==0) {
	  inci(i);
	  MH_Gfname = (char *)mh_malloc(SMAX);
	  strncpy(MH_Gfname,argv[i],SMAX-1);
	}else if (strcmp(argv[i],"--dataf")==0) {
	  inci(i);
	  MH_Dfname = (char *)mh_malloc(SMAX);
	  strncpy(MH_Dfname,argv[i],SMAX-1);
	}else if (strcmp(argv[i],"--xmax")==0) {
      inci(i);
	  sscanf(argv[i],"%lf",&Xng);
	}else if (strcmp(argv[i],"--step")==0) {
      inci(i);
	  sscanf(argv[i],"%lg",&MH_Hg);
	}else if (strcmp(argv[i],"--help")==0) {
	  mh_usage(); return(0);
	}else if (strcmp(argv[i],"--raw")==0) {
	  MH_RawName = 1;
	}else if (strcmp(argv[i],"--test")==0) {
	  inci(i);
	  sscanf(argv[i],"%d",&Testrank);
	  setParamTest();
	}else if (strcmp(argv[i],"--95")==0) {
	  MH_P95=1;
	}else if (strcmp(argv[i],"--verbose")==0) {
	  MH_Verbose=1;
	}else {
	  fprintf(stderr,"Unknown option %s\n",argv[i]);
	  mh_usage();
	  return(-1);
	}
  }
  if (MH_Verbose) showParam();
  x0 = MH_X0g;
  xn = Xng;
  ef = Ef;
  rank = mypower(2,MH_Mg);
  y0 = (double *) mh_malloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) y0[i] = ef*Iv[i];
  mh_gopen_file();
  if (MH_Verbose) {for (i=0; i<rank; i++) printf("%lf\n",y0[i]); }
  mh_rkmain(x0,y0,xn);
}

mh_usage() {
  fprintf(stderr,"Usages:\n");
  fprintf(stderr,"w-m [--idata input_data_file --gnuplotf gnuplot_file_name\n");
  fprintf(stderr," --dataf output_data_file --raw --xmax xmax --test m --step h]\n");
  fprintf(stderr,"[ --95 --verbose] \n");
  fprintf(stderr,"--raw does not add data parameters to the output_data_file.\n");
  fprintf(stderr,"\nThe command w-m [options] evaluates Pr({y | y<xmax}), which is the cumulative distribution function of the largest root of the m by m Wishart matrix with n degrees of freedom and the covariantce matrix sigma.\n");
  fprintf(stderr,"All the eigenvalues of sigma must be simple.\n");
  fprintf(stderr,"Parameters are specified by the input_data_file.\n");
  fprintf(stderr,"The format of the input_data_file.\n");
  fprintf(stderr," MH_Mg: m, MH_Beta: beta=sigma^(-1)/2 (diagonized), MH_Ng: n, Iv: initial values at MH_X0g*MH_Beta (see our paper how to order them), \n");
  fprintf(stderr," Ef: a scalar factor to the initial value. It may set to 1.\n");
  fprintf(stderr," MH_Hg: h (step size), MH_X0g: starting value of x, Xng: terminating value of x\n");
  fprintf(stderr," MH_Dp: output data is stored in every MH_Dp steps when output_data_file is specified.\n");
  fprintf(stderr," --95: output the 95% point. --verbose: verbose mode.\n");
  fprintf(stderr," The line started with %% is a comment line.\n");
  fprintf(stderr," An example format of the input_data_file can be obtained by executing w-2 with no option.\n");
  fprintf(stderr,"\nExamples:\n");
  fprintf(stderr,"[1] ./w-2 \n");
  fprintf(stderr,"[2] ./w-2 --xmax 20\n");
  fprintf(stderr,"[3] ./w-6 --test 6\n");
  fprintf(stderr,"   A test run of w-6.\n");
  fprintf(stderr,"   The number m and mm must agree for  ./w-m --test mm.\n");
  fprintf(stderr,"[4] ./w-5 --idata tmp-inm-m5* --gnuplot tmp-graph\n");
  fprintf(stderr,"  tmp-inm-m5* is a sample input data distributed with this file.\n");
  fprintf(stderr,"  tmp-graph-gp.txt is an input file of the gnuplot\n");
  fprintf(stderr,"  It can be executed with the load command in the gnuplot.\n");
  fprintf(stderr,"  tmp-graph is the table of x and the values of Pr({y | y<x}).\n");
}

static setParamTest() {
  int rank;
  int i;
  extern int Testrank;
  extern int MH_Verbose;
  MH_Verbose=1;
  MH_M= MH_Mg = Testrank ;
  MH_RANK = rank = mypower(2,MH_Mg);
  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  for (i=0; i<MH_Mg; i++) MH_Beta[i] = 1.0+0.1*i;
  MH_Ng = (double *)mh_malloc(sizeof(double)); *MH_Ng = 3.0;
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) Iv[i] = 0;
  Iv[0] = 0.001;
  Ef = 1;
  MH_X0g = 0.3;
  MH_Hg = 0.001;
  MH_Dp = 1;
  Xng = 10.0;
}
static setParamDefault() {
  int rank;
  MH_M=MH_Mg = 2 ;
  MH_RANK=rank = mypower(2,MH_Mg);
  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  MH_Beta[0] = 1.0; MH_Beta[1] = 2.0;
  MH_Ng = (double *)mh_malloc(sizeof(double)); *MH_Ng = 3.0;
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  Iv[0] = 1.58693;
  Iv[1] = 0.811369;
  Iv[2] = 0.846874;
  Iv[3] = 0.413438;
  Ef = 0.01034957388338225707;
  MH_X0g = 0.3;
  MH_Hg = 0.001;
  MH_Dp = 1;
  Xng = 10.0;
}

next(FILE *fp,char *s,char *msg) {
  s[0] = '%';
  while (s[0] == '%') {
	if (!fgets(s,SMAX,fp)) {
	  fprintf(stderr,"Data format error at %s\n",msg);
	  mh_exit(-1);
	}
	if (s[0] != '%') return(0);
  }
}
static setParam(char *fname) {
  int rank;
  char s[SMAX];
  FILE *fp;
  int i;
  if (fname == NULL) return(setParamDefault());

  if ((fp=fopen(fname,"r")) == NULL) {
	fprintf(stderr,"File %s is not found.\n",fname);
	mh_exit(-1);
  }
  next(fp,s,"MH_Mg(m)");
  sscanf(s,"%d",&MH_Mg); MH_M=MH_Mg;
  MH_RANK=rank = mypower(2,MH_Mg);

  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  for (i=0; i<MH_Mg; i++) {
    next(fp,s,"MH_Beta");
	sscanf(s,"%lf",&(MH_Beta[i]));
  }

  MH_Ng = (double *)mh_malloc(sizeof(double));
  next(fp,s,"MH_Ng(freedom parameter n)");
  sscanf(s,"%lf",MH_Ng);
  
  next(fp,s,"MH_X0g(initial point)");
  sscanf(s,"%lf",&MH_X0g);
  
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) {
	next(fp,s,"Iv(initial values)");
	sscanf(s,"%lg",&(Iv[i]));
  }

  next(fp,s,"Ef(exponential factor)");
  sscanf(s,"%lg",&Ef);

  next(fp,s,"MH_Hg (step size of rk)");
  sscanf(s,"%lg",&MH_Hg);

  next(fp,s,"MH_Dp (data sampling period)");
  sscanf(s,"%d",&MH_Dp);

  next(fp,s,"Xng (the last point, cf. --xmax)");
  sscanf(s,"%lf",&Xng);
  fclose(fp);
}

showParam() {
  int rank,i;
  rank = mypower(2,MH_Mg);
  printf("%%MH_Mg=\n%d\n",MH_Mg);
  for (i=0; i<MH_Mg; i++) {
	printf("%%MH_Beta[%d]=\n%lf\n",i,MH_Beta[i]);
  }
  printf("%%MH_Ng=\n%lf\n",*MH_Ng);
  printf("%%MH_X0g=\n%lf\n",MH_X0g);
  for (i=0; i<rank; i++) {
	printf("%%Iv[%d]=\n%lg\n",i,Iv[i]);
  }
  printf("%%Ef=\n%lf\n",Ef);
  printf("%%MH_Hg=\n%lf\n",MH_Hg);
  printf("%%MH_Dp=\n%d\n",MH_Dp);
  printf("%%Xng=\n%lf\n",Xng);
}

mh_exit(int n) {
#ifdef STANDALONE
  exit(n);
#else
  fprintf(stderr,"Fatal error mh_exit(%d) in mh-w-n.\n",n);
#endif  
}
