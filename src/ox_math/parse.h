/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

#ifndef _PARSE_H_

#include "ox.h"

#define _PARSE_H_

/* �ȡ��������� */
/* 256 ��꾮������Τϡ���ʸ�������Υȡ�����Ǥ��� */

#define T_INTEGER       257
#define T_STRING        258
#define T_MAGIC         2048
#define TOKEN(x)        (T_MAGIC + (x))

/* �ȡ�����μ����Ƚ�ꤹ�뤿��Υޥ��� */
#define MIN_T_CMO     TOKEN(0)
#define MAX_T_CMO     TOKEN(256)
#define MIN_T_SM      TOKEN(256)
#define MAX_T_SM      TOKEN(1100)
#define MIN_T_OX      TOKEN(512)
#define MAX_T_OX      TOKEN(600)

#define PFLAG_ADDREV   1
int setflag_parse(int flag);

cmo *parse();

int setgetc(int (*foo)());
int resetgetc();
int mygetc();
int setmode_mygetc(char *s, int len);

#endif /* _PARSE_H_ */
