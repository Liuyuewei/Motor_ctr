#include "common.h"
#include "type_def.h"
#include "user_mb_app.h"

//将浮点数保存到两个保持寄存器中
void hole_regist_write_float(float data,int location)
{
	u8_t * p;
    u16_t data_h,data_l;
    rt_uint32_t level;
    
    p = (u8_t *)&data;
    
    data_h = (*p << 8) | *(p + 1);
    data_l = (*(p + 2) << 8) | *(p + 3);

	usSRegHoldBuf[location]		= data_h;
	usSRegHoldBuf[location + 1]	= data_l;	
}
