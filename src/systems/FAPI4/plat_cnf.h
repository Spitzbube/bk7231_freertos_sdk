/*
*******************************************************************************
**
** \file        ./subsystems/bus/musb/include/plat_cnf.h
**
** \version     1.0 
**
** \brief       This header file provides board specific parameter of the 
**              controller.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. FUJITSU MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS.
**
** (C) Copyright 2002-2007 by Fujitsu Microelectronics Europe GmbH
** (C) Copyright 2010 by Fujitsu Semiconductor Europe GmbH
** 
*******************************************************************************
*/

#ifndef __MUSB_FAPI4_PLATFORM_CONFIG_H__
#define __MUSB_FAPI4_PLATFORM_CONFIG_H__



#include <stdlib.h>
#include "mu_dsi.h"
/*
 * Board-specific information about a controller
 */
#define	MUSB_TS_SESSREQ	         8	/* SRP, WAIT_A_BCON, WAIT_B_ACON */
#define	MUSB_MAX_TRANSITION_TM   250	/* 250 msec for transition coalesce */
#define MUSB_TB_SRP_FAIL	 5

extern uint32_t FAPI_USB_KeepVbusFlag;

#endif	/* multiple inclusion protection */
