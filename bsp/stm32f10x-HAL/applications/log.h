#ifndef	__LOG_H__
#define	__LOG_H__

#include "rtthread.h"
#define	BXYY_LOG		1
#ifdef BXYY_LOG
#define	LOG(debug,message) do {if(debug) rt_kprintf message;}while(0)
#else
#define LOG(debug,message)
#endif

#endif 

