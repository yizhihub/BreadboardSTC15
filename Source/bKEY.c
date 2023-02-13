#include "bkey.h"
//#include "adc.h"

 









KEYn_e ADKey_Scan(void)
{
  KEYn_e i=KEY_NONE;
//  ad=Get_ADC10bitResult(0x03); 
//  if(ad>660)                i=KEY_UP;
//  else if(ad>370 && ad<490) i=KEY_DOWN;
//  else if(ad>500 && ad<650) i=KEY_LEFT;
//  else if(ad>240 && ad<350) i=KEY_RIGHT;
//  else if(ad>120 && ad<200) i=KEY_ENTER;
  if      (INDEPKEY1 == 0)    i=KEY_UP;
  else if (INDEPKEY2 == 0)    i=KEY_DOWN;
  else if (INDEPKEY3 == 0)    i=KEY_LEFT;
  else if (INDEPKEY4 == 0)    i=KEY_RIGHT;
  else                        i=KEY_NONE;
  return i;
}


// 加松手检测 
KEYn_e ADKey_Check(void)
{
    static KEYn_e last_key;
    static uchar key_cnt,key_cnt_flag;
    KEYn_e tmp_key,key=KEY_NONE; 

    tmp_key=ADKey_Scan();
    if(tmp_key!=last_key && tmp_key!=KEY_NONE) 
    { 
        key_cnt_flag=1;  // push the key for the first time 
        key_cnt=0;
    }
    else if(key_cnt_flag==1 && tmp_key==last_key)
    {
        key_cnt++;
        if(key_cnt>15)    // dt=5ms  20 dt=20ms 5;
        {
            key=last_key; // get the fatall value 
            key_cnt=0;
            key_cnt_flag=0;
        }
    }
    else 
    {
        key=KEY_NONE;
    }
    last_key=tmp_key;
    return key;
}
// 不卡死式  可连续加减 
KEYn_e ADKey_Check2(void)
{
   static KEYn_e last_key;
   static uchar key_cnt;
   KEYn_e tmp_key,key=KEY_NONE; 
   
//   tmp_key=ADKey_Scan();
   if(tmp_key!=KEY_NONE && tmp_key==last_key) 
   { 
     key_cnt++;
		 if(key_cnt>20)
		 {
			 key=last_key;
			 key_cnt=0;
		 }
   }
   else 
   {
     key_cnt=0;  
   }
   last_key=tmp_key;
   return key;
}