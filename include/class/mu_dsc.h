/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW DSC (Digital Still Camera) definitions.
 * $Revision: 5874 $
 */

#ifndef __MUSB_DSC_H__
#define __MUSB_DSC_H__

#include "mu_tools.h"

/* subclass/protocol (class=MUSB_CLASS_IMAGE): */
#define MGC_DSC_SUBCLASS  0x01
#define MGC_DSC_PROTOCOL  0x01

/* request types (bmRequestType values) for OUTs and INs: */
#define MGC_DSC_REQTYPE_OUT       (MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
#define MGC_DSC_REQTYPE_IN        (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)

/* request (bRequest) values: */
/** cancel (wLength=6, OUT data 0x4001, 4B TransID) */
#define MGC_DSC_REQ_CANCEL         0x64
/** get extended event data (IN data 2B event code, 4B TransID, 2B num params, (2B param size, param)... */
#define MGC_DSC_REQ_GET_EVENT      0x65
/** reset device (no data) */
#define MGC_DSC_REQ_RESET          0x66
/** get device status (IN data - 2B length, 2B event code, params 1-N) */
#define MGC_DSC_REQ_GET_STATUS     0x67

/**
 * MGC_DscContainer.
 * Commands, data, responses and events use this header.
 * For commands, responses and events (on interrupt pipe), 
 * this is followed by 0 or more 4-byte parameters.
 * Otherwise the payload is data.
 * @field dwLength container length
 * @field wType MGC_DSC_BOP_CONTAINER_TYPE_*
 * @field wCode PTP operation, response or event code
 * @field dwTransactionId transaction ID generated by host
 */
#include "mu_pkon.h"
typedef struct
{
    uint32_t dwLength;
    uint16_t wType;
    uint16_t wCode;
    uint32_t dwTransactionId;
} MGC_DscContainer;
#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
