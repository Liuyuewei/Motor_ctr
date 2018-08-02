
#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#include <rtthread.h>

//=====宏定义===============
#define RET_ERR 0
#define RET_OK  1

#define NULL		0

#define FALSE		0
#define TRUE 	  1

//变量类型定义
typedef unsigned char u8_t;
typedef char s8_t;
typedef unsigned char bool_t;
typedef unsigned char ref_t;
typedef unsigned short int u16_t;
typedef signed short int s16_t;
typedef unsigned long int u32_t;
typedef long int s32_t;
typedef u32_t  addr_t;
typedef signed long long  s64_t;
typedef unsigned long long  u64_t;
typedef float  FLOAT;
typedef double DOUBLE;
typedef unsigned short int R_STATUS;


typedef volatile unsigned char vu8_t;
typedef volatile char 				 vs8_t;
typedef volatile unsigned char vbool_t;
typedef volatile unsigned char vref_t;
typedef volatile unsigned short int vu16_t;
typedef volatile signed short int vs16_t;
typedef volatile unsigned long int vu32_t;
typedef volatile long int vs32_t;
typedef volatile u32_t  vaddr_t;
typedef volatile signed long long  vs64_t;
typedef volatile unsigned long long  vu64_t;
typedef volatile float  vFLOAT;
typedef volatile double vDOUBLE;
typedef volatile unsigned short int vR_STATUS;


typedef unsigned int size_t;

#define HEAP_MALLOC pvPortMalloc
#define HEAP_FREE		vPortFree

extern void *pvPortMalloc( size_t xWantedSize );
extern void vPortFree( void *pv );



#endif

