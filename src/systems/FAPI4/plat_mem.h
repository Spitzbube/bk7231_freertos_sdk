/*
*******************************************************************************
**
** \file        ./subsystems/bus/musb/include/plat_mem.h
**
** \brief       This header file provides mappings to Fujitsu's FAPI 
**              memory functions and will be included by mu_mem.h
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


#ifndef __MUSB_FAPI_PLATFORM_MEMORY_H__
#define __MUSB_FAPI_PLATFORM_MEMORY_H__

#include <stdlib.h>
#include <string.h>
#include <fapi/sys_services.h>

extern void* main_hMemBlk2;
extern void* memblk_alloc(void*, uint32_t);
extern int memblk_free(void*, void*);

#define MUSB_MemAlloc(iSize)    memblk_alloc(main_hMemBlk2, iSize)
#define MUSB_MemFree(p)         memblk_free(main_hMemBlk2, p)
#define MUSB_MemRealloc         MGC_FAPI_MemRealloc
#define MUSB_MemCopy            MGC_FAPI_MemCopy
#define MUSB_MemSet             MGC_FAPI_MemSet

extern void* MGC_FAPI_MemRealloc( void* blockPtr, size_t iSize );
extern void  MGC_FAPI_MemCopy( void* toPtr, const void* fromPtr, size_t iSize );
extern void  MGC_FAPI_MemSet( void* ptr, uint8_t c, size_t iSize );

#endif	/* multiple inclusion protection */
