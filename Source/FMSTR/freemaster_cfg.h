/***************************************************************************//*!
*
* @file   freemaster_cfg.h.example
*
* @brief  FreeMASTER Serial Communication Driver configuration file
*
*******************************************************************************/

#ifndef __FREEMASTER_CFG_H
#define __FREEMASTER_CFG_H

/******************************************************************************
* Select interrupt or poll-driven serial communication
******************************************************************************/

#define FMSTR_LONG_INTR        0    /* complete message processing in interrupt */
#define FMSTR_SHORT_INTR       0    /* SCI FIFO-queuing done in interrupt */
#define FMSTR_POLL_DRIVEN      1    /* no interrupt needed, polling only */

/*****************************************************************************
* Select communication interface (SCI or CAN)
******************************************************************************/

#define FMSTR_SCI_BASE         0x98 /* SCI1 register base on DZ60 */
// #define FMSTR_SCI_RX_INTERRUPT 17   /* HC08-only: SCI RX and SCI TX interrupt numbers */
// #define FMSTR_SCI_TX_INTERRUPT 18   /*         (or use VECTOR statements in PRM file) */
#include "STC15F2K60S2.h"

#define FMSTR_CAN_BASE         0x1880 /* MSCAN register base on DZ60 */
#define FMSTR_CAN_RX_INTERRUPT 28   /* HC08-only: MSCAN RX and MSCAN TX interrupt numbers */
#define FMSTR_CAN_TX_INTERRUPT 29   /*            (or use VECTOR statements in PRM file) */

#define FMSTR_DISABLE          0    /* To disable all FreeMASTER functionalities */
#define FMSTR_USE_SCI          1    /* To select SCI communication interface */
#define FMSTR_USE_MSCAN        0    /* To selecet MSCAN communication interface */
#define FMSTR_USE_USB_CDC      0    /* To select USB CDC communication interface */
#define FMSTR_USE_PDBDM        0    /* To select Packet Driven BDM communication interface (optional) */

/******************************************************************************
* Input/output communication buffer size
******************************************************************************/

#define FMSTR_COMM_BUFFER_SIZE 0    /* set to 0 for "automatic" */

/******************************************************************************
* Receive FIFO queue size (use with FMSTR_SHORT_INTR only)
******************************************************************************/

#define FMSTR_COMM_RQUEUE_SIZE 32   /* set to 0 for "default" */

/*****************************************************************************
* Support for Application Commands 
******************************************************************************/

#define FMSTR_USE_APPCMD       0    /* enable/disable App.Commands support */
#define FMSTR_APPCMD_BUFF_SIZE 32   /* App.Command data buffer size */
#define FMSTR_MAX_APPCMD_CALLS 0    /* how many app.cmd callbacks? (0=disable) */

/*****************************************************************************
* Oscilloscope support
******************************************************************************/

#define FMSTR_USE_SCOPE        1    /* enable/disable scope support */
#define FMSTR_MAX_SCOPE_VARS   4    /* max. number of scope variables (2..8) */
#define FMSTR_USE_THROTTLE     1

/*****************************************************************************
* Recorder support
******************************************************************************/

#define FMSTR_USE_RECORDER     0   /* enable/disable recorder support */
#define FMSTR_MAX_REC_VARS     8    /* max. number of recorder variables (2..8) */
#define FMSTR_REC_OWNBUFF      0    /* use user-allocated rec. buffer (1=yes) */

/* built-in recorder buffer (use when FMSTR_REC_OWNBUFF is 0) */
#define FMSTR_REC_BUFF_SIZE    32 /* built-in buffer size */

/* recorder time base, specifies how often the recorder is called in the user app. */
#define FMSTR_REC_TIMEBASE     FMSTR_REC_BASE_MILLISEC(0) /* 0 = "unknown" */

#define FMSTR_REC_FLOAT_TRIG   0    /* enable/disable floating point triggering */

/*****************************************************************************
* Target-side address translation (TSA)
******************************************************************************/

#define FMSTR_USE_TSA          0    /* enable TSA functionality */
#define FMSTR_USE_TSA_SAFETY   1    /* enable access to TSA variables only */
#define FMSTR_USE_TSA_INROM    0    /* TSA tables declared as const (put to ROM) */

/*****************************************************************************
* Enable/Disable read/write memory commands
******************************************************************************/

#define FMSTR_USE_READMEM      1    /* enable read memory commands */
#define FMSTR_USE_WRITEMEM     1    /* enable write memory commands */
#define FMSTR_USE_WRITEMEMMASK 1    /* enable write memory bits commands */

/*****************************************************************************
* Enable/Disable read/write variable commands (a bit faster than Read Mem)
******************************************************************************/

#define FMSTR_USE_READVAR      1    /* enable read variable fast commands */
#define FMSTR_USE_WRITEVAR     1    /* enable write variable fast commands */
#define FMSTR_USE_WRITEVARMASK 1    /* enable write variable bits fast commands */


#endif /* __FREEMASTER_CFG_H */
