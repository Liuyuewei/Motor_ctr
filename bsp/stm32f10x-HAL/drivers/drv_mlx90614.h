#ifndef DRV_MLX90614_H__
#define DRV_MLX90614_H__

#include <rtdevice.h>
#include "type_def.h"

#define MLX90614_READ_STATUS  0x01    //¶Á×´Ì¬ÃüÁî
#define MLX90614_GET_DATA     0x02    //¶ÁÊý¾Ý


typedef struct Mlx90614Device
{
    struct rt_device device;
    struct rt_i2c_bus_device *iicBus;
}Mlx90614Device_t;

void mlx90614_Init(const char *name,const char * bus);

#endif

