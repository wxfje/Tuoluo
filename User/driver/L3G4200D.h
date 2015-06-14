#ifndef L3G4200D_H
#define L3G4200D_H

#include <stdint.h>

//extern void L3G4_data(void);
extern void Init_L3G4200D(void);
extern void READ_L3G4(int16_t *px,int16_t *py,int16_t *pz);
#endif

