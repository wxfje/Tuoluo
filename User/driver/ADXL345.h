#ifndef  _ADXL345_H
#define  _ADXL345_H

#include <stdint.h>

extern void Init_ADXL345( void );
//void Multiple_Read_ADXL345(void);
extern void Read_ADXL345(int16_t *px,int16_t *py,int16_t *pz);
#endif
