#ifndef _MLO_H_

#define _MLO_H_

#include "ox.h"

/* Mathematica �ǤΤ߻��Ѥ���� Local Object ����� */
typedef struct {
    int tag;
    int length;
    cell head[1];
    char *function;
} mlo_function;

cmo *receive_mlo();


#endif
