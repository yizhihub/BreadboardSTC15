#ifndef _bkey_h_
#define _bkey_h_

#include "common.h"

sbit INDEPKEY1 = P2^5; 
sbit INDEPKEY2 = P2^4;
sbit INDEPKEY3 = P2^3;
sbit INDEPKEY4 = P2^2;


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