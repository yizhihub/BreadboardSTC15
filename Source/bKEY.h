#ifndef _bkey_h_
#define _bkey_h_

#include "common.h"

#if	defined(IAP15W413ASDIP28)
sbit INDEPKEY1 = P2^5;
sbit INDEPKEY2 = P2^4;
sbit INDEPKEY3 = P2^3;
sbit INDEPKEY4 = P2^2;
#elif (defined(IAP15W413ASSOP20) || defined(STC15W408ASDIP16))
sbit INDEPKEY1 = P1^1;
sbit INDEPKEY2 = P1^0;
sbit INDEPKEY3 = P3^7;
sbit INDEPKEY4 = P3^6;
#endif

enum keyn_e
{
    KEY_NONE=0, 
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_CANCEL,
    KEY_MAX,
};
typedef enum keyn_e KEYn_e;


//typedef enum
//{
//KEY_NONE,
//KEY_UP,
//KEY_DOWN,
//KEY_BACK,
//KEY_IN,
//KEY_SHIFT,
//KEY_MAX,
//} KEYn_e; 

KEYn_e ADKey_Scan(void);
KEYn_e ADKey_Check(void);
KEYn_e ADKey_Check2(void);

#endif 