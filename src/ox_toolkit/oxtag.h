/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */

/* A part of ox_kan.h */
/* Do not edit this file. */

/*  cmotag.h */
/* #define     CMO   1024  It is defined in stackm.h. */
#define LARGEID   0x7f000000
#define CMO_PRIVATE 0x7fff0000
#define CMO_ERROR  (LARGEID+1)
#define CMO_ERROR2 (LARGEID+2)
#define CMO_NULL   1
#define CMO_INT32  2
#define CMO_DATUM  3
#define CMO_STRING 4
#define CMO_MATHCAP 5

#define CMO_START_SIGNATURE    0x7fabcd03     
#define CMO_LOCAL_OBJECT       0x7fcdef03
#define CMO_LOCAL_OBJECT_ASIR  (CMO_LOCAL_OBJECT+0)
#define CMO_LOCAL_OBJECT_SM1   (CMO_LOCAL_OBJECT+1)
/* for OpenMathematica */
#define MLO_FUNCTION           (CMO_LOCAL_OBJECT+2)

#define OX_LOCAL_OBJECT       0x7fcdef30
#define OX_LOCAL_OBJECT_ASIR   (OX_LOCAL_OBJECT+0)
#define OX_LOCAL_OBJECT_SM1    (OX_LOCAL_OBJECT+1)
#define OX_LOCAL_OBJECT_MATH   (OX_LOCAL_OBJECT+2)

#define     CMO_ARRAY  16
#define     CMO_LIST 17
#define     CMO_ATOM 18
#define     CMO_MONOMIAL32  19
#define     CMO_ZZ          20
#define     CMO_QQ          21
#define     CMO_ZERO        22
/* #define     CMO_DMS          23   /* Distributed monomial system */
#define     CMO_DMS_GENERIC     24
#define     CMO_DMS_OF_N_VARIABLES 25
#define     CMO_RING_BY_NAME  26
#define     CMO_RECURSIVE_POLYNOMIAL 27
#define     CMO_LIST_R   28

#define     CMO_INT32COEFF  30
#define     CMO_DISTRIBUTED_POLYNOMIAL 31
/* #define     CMO_ZZ_OLD   32  */
#define     CMO_POLYNOMIAL_IN_ONE_VARIABLE 33
#define     CMO_RATIONAL 34

#define     CMO_64BIT_MACHINE_DOUBLE   40
#define     CMO_ARRAY_OF_64BIT_MACHINE_DOUBLE  41
#define     CMO_128BIT_MACHINE_DOUBLE   42
#define     CMO_ARRAY_OF_128BIT_MACHINE_DOUBLE  43

#define     CMO_BIGFLOAT    50
#define     CMO_IEEE_DOUBLE_FLOAT 51

#define     CMO_INDETERMINATE  60
#define     CMO_TREE           61
#define     CMO_LAMBDA         62    /* for function definition */

/* end of cmotag.h */

/* ox_message_tag.h */
#define OX_COMMAND                   513    
#define OX_DATA                      514
#define OX_SYNC_BALL                 515      /* ball to interrupt */

/* level 2 stack machine with programming facility. */
#define OX_START_OF_FUNCTION_BLOCK    518
#define OX_END_OF_FUNCTION_BLOCK      519
#define OX_ADDRESS                    520

#define OX_DATA_WITH_SIZE              521
#define OX_DATA_ASIR_BINARY_EXPRESSION 522  /* This number should be changed*/
#define   OX_DATA_OPENMATH_XML         523
#define   OX_DATA_OPENMATH_BINARY      524
#define   OX_DATA_MP                   525

/* OX BYTE command */
#define OX_BYTE_NETWORK_BYTE_ORDER    0
#define OX_BYTE_LITTLE_ENDIAN         1
#define OX_BYTE_BIG_ENDIAN         0xff

/* ox_function_id.h */
#define SM_popSerializedLocalObject 258
#define SM_popCMO 262
#define SM_popString 263    /* result ==> string and send the string by CMO */

#define SM_mathcap 264
#define SM_pops 265
#define SM_setName 266
#define SM_evalName 267 
#define SM_executeStringByLocalParser 268 
#define SM_executeFunction 269
#define SM_beginBlock  270
#define SM_endBlock    271
#define SM_shutdown    272
#define SM_setMathCap  273
#define SM_executeStringByLocalParserInBatchMode 274
#define SM_getsp   275
#define SM_dupErrors 276

#define SM_DUMMY_sendcmo  280
#define SM_sync_ball 281

#define SM_control_kill 1024
#define SM_control_reset_connection  1030
#define SM_control_to_debug_mode 1025
#define SM_control_exit_debug_mode 1026
#define SM_control_ping  1027
#define SM_control_start_watch_thread 1028
#define SM_control_stop_watch_thread 1029
