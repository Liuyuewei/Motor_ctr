#include "common.h"
#include "mb.h"


//д���ּ���
void hold_regist_write(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{	
	eMBRegHoldingCB(pucRegBuffer,usAddress + 1,usNRegs,eMode);
}



