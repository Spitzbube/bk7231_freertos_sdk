/*
*******************************************************************************
**
** \file        ./subsystems/bus/musb/include/plat_arc.h
**
** \version     1.0 
**
** \brief       This header file provides mappings to Fujitsu's FAPI register 
**              access functions and will be included by mu_arc.h
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

#ifndef __MUSB_FAPI_PLATFORM_ARCH_H__
#define __MUSB_FAPI_PLATFORM_ARCH_H__

#include "stdint.h"

/**
 * Read an 8-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 8-bit datum
 */
#undef MGC_Read8
#define MGC_Read8(_pBase, _offset) MGC_FAPI_ReadReg8(_pBase, _offset)

/**
 * Read a 16-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 16-bit datum
 */
#undef MGC_Read16
#define MGC_Read16(_pBase, _offset) MGC_FAPI_ReadReg16(_pBase, _offset)
           

/**
 * Read a 32-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 32-bit datum
 */
#undef MGC_Read32 
#define MGC_Read32(_pBase, _offset) MGC_FAPI_ReadReg32(_pBase, _offset)
            
/**
 * Write an 8-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 8-bit datum
 */
#undef MGC_Write8
#define MGC_Write8(_pBase, _offset, _data) MGC_FAPI_WriteReg8( _pBase, _offset, _data )

/**
 * Write a 16-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 16-bit datum
 */
#undef MGC_Write16
#define MGC_Write16(_pBase, _offset, _data) MGC_FAPI_WriteReg16( _pBase, _offset, _data )

/**
 * Write a 32-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 32-bit datum
 */
#undef MGC_Write32
#define MGC_Write32(_pBase, _offset, _data) MGC_FAPI_WriteReg32( _pBase, _offset, _data )

extern uint8_t  MGC_FAPI_ReadReg8  (uint8_t* baseAddr, uint16_t offset);
extern uint16_t MGC_FAPI_ReadReg16 (uint8_t* baseAddr, uint16_t offset);
extern uint32_t MGC_FAPI_ReadReg32 (uint8_t* baseAddr, uint16_t offset);
extern void MGC_FAPI_WriteReg8  (uint8_t* baseAddr, uint16_t offset, uint8_t data);
extern void MGC_FAPI_WriteReg16 (uint8_t* baseAddr, uint16_t offset, uint16_t data);
extern void MGC_FAPI_WriteReg32 (uint8_t* baseAddr, uint16_t offset, uint32_t data);

#endif	/* multiple inclusion protection */
