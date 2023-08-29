

//#pragma thumb

#include "ucos_ii.h"
#include "mu_arch.h"
#include "mu_cdi.h"
#include "mu_descs.h"
#include "mu_diag.h"
#include "mu_mem.h"
#include "mu_stdio.h"
#include "mu_asix.h"
#include "mii.h"


static MUSB_AsixDriverContext asixDriverContext;

MUSB_DeviceDriver MGC_AsixDriver =
{
		&asixDriverContext,
		0,
		NULL, //aTimerResolution,
		MGC_AsixConnect,
		MGC_AsixDisconnect,
		NULL, //pfBusSuspended
		NULL, //pfBusResumed
};

static uint8_t bInitialized = 0;


static void MGC_AsixDriverInit()
{
	console_send_string("MGC_AsixDriverInit (todo.c): TODO\r\n");

	asixDriverContext.bNumOfDevActive = 0;

	for (uint8_t bIndex = 0; bIndex < MUSB_ASIX_MAX_DEV_SUPPORTED; bIndex++)
	{
		asixDriverContext.asixDeviceList[bIndex].bState = 0;
	}

	MGC_AsixDriver.pPrivateData = &asixDriverContext;

	bInitialized = 1;
}


static uint8_t MGC_AsixConnect(void *pPrivateData,
        MUSB_BusHandle     hBus,
        MUSB_Device       *pUsbDevice,
        const uint8_t     *pPeripheralList)
{
	console_send_string("MGC_AsixConnect (todo.c): TODO\r\n");

	MUSB_AsixDevice* pAsixDevice;
	MUSB_ConfigurationDescriptor* pCfgDscr;
	const MUSB_InterfaceDescriptor* pInfDscr;
	const MUSB_EndpointDescriptor* pEndpDscr;
	MUSB_AsixDriverContext* pDriverContext;
	int dwStatus;
	uint8_t bDevIndex;
	uint8_t bEndIndex;

	if (bInitialized == 0)
	{
		MGC_AsixDriverInit();
	}

	pDriverContext = (MUSB_AsixDriverContext*) pPrivateData;

	dwStatus = MGC_AsixFindFreeDeviceIndex(pDriverContext, &bDevIndex);
	if (dwStatus == -1)
	{
		console_send_string("AsixError: Free pAsixDevice not available");
		return 0;
	}

	pDriverContext->bNumOfDevActive++;

	pAsixDevice = &pDriverContext->asixDeviceList[bDevIndex];
	pAsixDevice->bIndexAtAsixDeviceList = bDevIndex;

	pCfgDscr = pUsbDevice->apConfigDescriptors[0];

	pInfDscr = MUSB_FindInterfaceDescriptor(pCfgDscr, 0, 0);

	if ((pInfDscr == 0) || (pInfDscr->bNumEndpoints == 0))
	{
		console_send_string("AsixError: no endpoints");
		return 0;
	}

	for (bEndIndex = 0; bEndIndex < pInfDscr->bNumEndpoints; bEndIndex++)
	{
		pEndpDscr = MUSB_FindEndpointDescriptor(pCfgDscr, pInfDscr, bEndIndex);

		switch (pEndpDscr->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK)
		{
		case MUSB_ENDPOINT_XFER_INT:
			if ((pEndpDscr->bEndpointAddress & MUSB_ENDPOINT_DIR_MASK) == MUSB_DIR_IN)
			{
				pAsixDevice->pIntEndpDscr = pEndpDscr;
			}
			break;

		case MUSB_ENDPOINT_XFER_BULK:
			if ((pEndpDscr->bEndpointAddress & MUSB_ENDPOINT_DIR_MASK) == MUSB_DIR_IN)
			{
				pAsixDevice->pRxEndpDscr = pEndpDscr;
			}
			else
			{
				pAsixDevice->pTxEndpDscr = pEndpDscr;
			}
			break;
		}
	}

	if ((pAsixDevice->pRxEndpDscr == NULL) || (pAsixDevice->pTxEndpDscr == NULL))
	{
		console_send_string("AsixError: Necessary endpoint is not available");
		return 0;
	}

	pAsixDevice->pInfDscr = pInfDscr;
	pUsbDevice->pDriverPrivateData = pPrivateData;
	pAsixDevice->busHandle = hBus;
	pAsixDevice->pUsbDevice = pUsbDevice;
	pAsixDevice->bState = 1;

	/* ---------------- Create Pipes --------------- */

	if (pAsixDevice->pIntEndpDscr != NULL)
	{
//		MGC_AsixStartInterruptTransfer(pAsixDevice);
	}

	pAsixDevice->send = MGC_AsixStartTxBulkTransfer;
	MGC_AsixPrepareBulkTxTransfer(pAsixDevice);
	MGC_AsixStartRxBulkTransfer(pAsixDevice);

	/* ---------------- Start Thread --------------- */

	static uint32_t asixThreadStack[THREAD_STACK_SIZE_MGC_ASIX];

	pAsixDevice->threadSema = OSSemCreate(0);
	pAsixDevice->pMBox = (void*) OSMboxCreate(0);

	OSTaskCreateExt(MGC_AsixThread, pAsixDevice, &asixThreadStack[THREAD_STACK_SIZE_MGC_ASIX-1],
			THREAD_PRIO_MGC_ASIX, THREAD_PRIO_MGC_ASIX,
			asixThreadStack, THREAD_STACK_SIZE_MGC_ASIX, "MGC_ASIX", 0x03);

	return 1;
}


static int MGC_AsixStartInterruptTransfer(MUSB_AsixDevice* pAsixDevice)
{
	MUSB_DeviceEndpoint endPoint;
	uint32_t dwStatus;
	uint16_t wPacketSize;

	wPacketSize = pAsixDevice->pIntEndpDscr->wMaxPacketSize;

	if (wPacketSize > sizeof(pAsixDevice->aInterruptDataBuffer))
	{
		console_send_string("AsixError: MGC_AsixStartInterruptTransfer, packet size=%d\r\n", wPacketSize);
		return 0;
	}

	endPoint.pDevice = pAsixDevice->pUsbDevice;
	endPoint.wNakLimit = 0xffff;
	MUSB_MemCopy(&endPoint.UsbDescriptor, pAsixDevice->pIntEndpDscr, sizeof(MUSB_EndpointDescriptor));

	pAsixDevice->intrInPipe = MUSB_OpenPipe(pAsixDevice->busHandle, &endPoint, NULL);

	if (pAsixDevice->intrInPipe == NULL)
	{
		console_send_string("AsixError: MGC_AsixStartInterruptTransfer, Pipe is NULL\r\n");
		return 0;
	}

	MUSB_Irp* pIrp = &pAsixDevice->intrInIrp;
	pIrp->hPipe = pAsixDevice->intrInPipe;
	pIrp->bAllowShortTransfer = 0;
	pIrp->pBuffer = pAsixDevice->aInterruptDataBuffer;
	pIrp->dwLength = wPacketSize;
	pIrp->pfIrpComplete = MGC_AsixIntrIrpCallback;
	pIrp->pCompleteParam = pAsixDevice;

	dwStatus = MUSB_StartTransfer(pIrp);
	if (dwStatus != 0)
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixStartInterruptTransfer: dwStatus=%d\r\n",
					dwStatus);
			console_send_string(debug_string);
		}
	}
#endif

	return 1;
}


static int MGC_AsixPrepareBulkTxTransfer(MUSB_AsixDevice* pAsixDevice)
{
	MUSB_DeviceEndpoint endPoint;
	uint16_t wPacketSize;

	wPacketSize = pAsixDevice->pTxEndpDscr->wMaxPacketSize;

	if (wPacketSize > sizeof(pAsixDevice->aTxDataBuffer))
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "AsixError: MGC_AsixPrepareBulkTxTransfer, Tx packet size=%d\r\n",
					wPacketSize);
			console_send_string(debug_string);
		}
#endif
		return 0;
	}

	endPoint.pDevice = pAsixDevice->pUsbDevice;
	endPoint.wNakLimit = 0xffff;
	MUSB_MemCopy(&endPoint.UsbDescriptor, pAsixDevice->pTxEndpDscr, sizeof(MUSB_EndpointDescriptor));

	pAsixDevice->bulkOutPipe = MUSB_OpenPipe(pAsixDevice->busHandle, &endPoint, NULL);

	if (pAsixDevice->bulkOutPipe == NULL)
	{
		console_send_string("AsixError: MGC_AsixPrepareBulkTxTransfer, Out-Pipe is NULL\r\n");
		return 0;
	}

	MUSB_Irp* pIrp = &pAsixDevice->bulkOutIrp;
	pIrp->hPipe = pAsixDevice->bulkOutPipe;
	pIrp->bAllowShortTransfer = 0;
	pIrp->pBuffer = pAsixDevice->aTxDataBuffer;
	pIrp->dwLength = AX_TX_IRB_SIZE;
	pIrp->pfIrpComplete = MGC_AsixTxIrpCallback;
	pIrp->pCompleteParam = pAsixDevice;

	return 1;
}


static int MGC_AsixStartRxBulkTransfer(MUSB_AsixDevice* pAsixDevice)
{
	MUSB_DeviceEndpoint endPoint;
	uint32_t dwStatus;
	uint16_t wPacketSize;

	wPacketSize = pAsixDevice->pRxEndpDscr->wMaxPacketSize;

	if (wPacketSize > sizeof(pAsixDevice->aRxDataBuffer))
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "AsixError: MGC_AsixStartRxBulkTransfer, Rx packet size=%d\r\n",
					wPacketSize);
			console_send_string(debug_string);
		}
#endif
		return 0;
	}

	endPoint.pDevice = pAsixDevice->pUsbDevice;
	endPoint.wNakLimit = 0xffff;
	MUSB_MemCopy(&endPoint.UsbDescriptor, pAsixDevice->pRxEndpDscr, sizeof(MUSB_EndpointDescriptor));

	pAsixDevice->bulkInPipe = MUSB_OpenPipe(pAsixDevice->busHandle, &endPoint, NULL);

	if (pAsixDevice->bulkInPipe == NULL)
	{
		console_send_string("AsixError: MGC_AsixStartRxBulkTransfer, In-Pipe is NULL\r\n");
		return 0;
	}

	MUSB_Irp* pIrp = &pAsixDevice->bulkInIrp;
	pIrp->hPipe = pAsixDevice->bulkInPipe;
	pIrp->bAllowShortTransfer = 0;
	pIrp->pBuffer = pAsixDevice->aRxDataBuffer;
	pIrp->dwLength = AX_RX_IRB_SIZE;
	pIrp->pfIrpComplete = MGC_AsixRxIrpCallback;
	pIrp->pCompleteParam = pAsixDevice;

	dwStatus = MUSB_StartTransfer(pIrp);
	if (dwStatus != 0)
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixStartRxBulkTransfer: dwStatus=%d\r\n",
					dwStatus);
			console_send_string(debug_string);
		}
	}
#endif

	return 1;
}


static void MGC_AsixIntrIrpCallback(void* pCtxt, MUSB_Irp* pIntrIrp)
{
#if 0
	console_send_string("MGC_AsixIntrIrpCallback\r\n");
#endif

	if (pIntrIrp->dwStatus != 0)
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixIntrIrpCallback: dwStatus=%d\r\n", pIntrIrp->dwStatus);
			console_send_string(debug_string);
		}
#endif
	}
	else
	{
#if 0
		for (int i = 0; i < pIntrIrp->dwLength; i++)
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixIntrIrpCallback: buf[%d]=0x%02x\r\n",
					i, pIntrIrp->pBuffer[i]);
			console_send_string(debug_string);
		}
#endif

#if 0
		//    uint32_t len = transfer->length - ((transfer->qtd.token >> 16) & 0x7FFF);
		    const uint8_t *p = (const uint8_t *)transfer->buffer;
		    PHYSpeed = (p[2] & 0x10) ? 1 : 0;
		    if(((p[2] & 0x1) ? 1 : 0) == 1 && pending_control == 255) {
		        pending_control = 48;
		        mk_setup(setup, 0x40, 6, 0x0000, 0, 0);
		        queue_Control_Transfer(device, &setup, NULL, this);
		        control_queued = true;
		        pending_control = 49;
		    }
		    else if(((p[2] & 0x1) ? 1 : 0) == 0 && pending_control == 254) {
		        pending_control = 255;
		        connected = false;
		    }
		//    println("interrupt_data(asix): ", len, DEC);
		//    print_hexbytes((uint8_t*)transfer->buffer, (len < 32)? len : 32 );
#endif

	}
}


static int MGC_AsixStartTxBulkTransfer(MUSB_AsixDevice* pAsixDevice, uint8_t* buf_ptr, uint32_t length)
{
#if 0
	console_send_string("MGC_AsixStartTxBulkTransfer\r\n");
#endif

	uint32_t dwStatus;
	uint32_t packet_len;

	packet_len = (((length) ^ 0x0000ffff) << 16) + (length);

	memcpy(pAsixDevice->aTxDataBuffer, &packet_len, sizeof(packet_len));
	memcpy(pAsixDevice->aTxDataBuffer + sizeof(packet_len), buf_ptr, length);

	pAsixDevice->bulkOutIrp.dwLength = length + sizeof(packet_len);

	dwStatus = MUSB_StartTransfer(&pAsixDevice->bulkOutIrp);
	if (dwStatus != 0)
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixStartTxBulkTransfer: dwStatus=%d\r\n",
					dwStatus);
			console_send_string(debug_string);
		}
	}
#endif

	return dwStatus;
}


static void MGC_AsixRxIrpCallback(void* pCtxt, MUSB_Irp* pBulkIrp)
{
#if 0
	console_send_string("MGC_AsixRxIrpCallback\r\n");
#endif

	MUSB_AsixDevice* pAsixDevice = (MUSB_AsixDevice*) pCtxt;
	uint32_t dwStatus;

	if (pBulkIrp->dwStatus != 0)
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixRxIrpCallback: dwStatus=%d\r\n", pBulkIrp->dwStatus);
			console_send_string(debug_string);
		}
#endif
	}
	else
	{
#if 0
		for (int i = 0; i < pBulkIrp->dwActualLength; i++)
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixRxIrpCallback: buf[%d]=0x%02x\r\n",
					i, pBulkIrp->pBuffer[i]);
			console_send_string(debug_string);
		}
#endif
		uint32_t packet_len;
		int actual_len = pBulkIrp->dwActualLength;
		uint8_t* buf_ptr = pBulkIrp->pBuffer;
#if 0
		hex_dump("MGC_AsixRxIrpCallback", buf_ptr, actual_len);
#endif

		while (actual_len > 0)
		{
			/*
			 * 1st 4 bytes contain the length of the actual data as two
			 * complementary 16-bit words. Extract the length of the data.
			 */
			if (actual_len < sizeof(packet_len))
			{
				console_send_string("MGC_AsixRxIrpCallback: incomplete packet length\r\n");
				break;
			}

			memcpy(&packet_len, buf_ptr, sizeof(packet_len));
			if (((~packet_len >> 16) & 0x7ff) != (packet_len & 0x7ff))
			{
#if 1
				{
					extern char debug_string[];
					sprintf(debug_string, "MGC_AsixRxIrpCallback: malformed packet length: %#x (%#x:%#x)\r\n",
							packet_len, (~packet_len >> 16) & 0x7ff, packet_len & 0x7ff);
					console_send_string(debug_string);
				}
#endif
				break;
			}

			packet_len = packet_len & 0x7ff;
			if (packet_len > (actual_len - sizeof(packet_len)))
			{
#if 1
				{
					extern char debug_string[];
					sprintf(debug_string, "MGC_AsixRxIrpCallback: too large packet: %d\r\n",
							packet_len);
					console_send_string(debug_string);
				}
#endif
				break;
			}

			/* Notify net stack */
			ethif_asixusb_input(pAsixDevice, buf_ptr + sizeof(packet_len), packet_len);

			/* Adjust for next iteration. Packets are padded to 16-bits */
			if (packet_len & 1)
				packet_len++;
			actual_len -= sizeof(packet_len) + packet_len;
			buf_ptr += sizeof(packet_len) + packet_len;

		} //while (actual_len > 0)

		/* Re-schedule the Bulk-In transfer */
		dwStatus = MUSB_StartTransfer(&pAsixDevice->bulkInIrp);
		if (dwStatus != 0)
		{
#if 1
			{
				extern char debug_string[];
				sprintf(debug_string, "MGC_AsixRxIrpCallback: MUSB_StartTransfer, dwStatus=%d\r\n",
						dwStatus);
				console_send_string(debug_string);
			}
		}
#endif
	}
}


static void MGC_AsixTxIrpCallback(void* pCtxt, MUSB_Irp* pBulkIrp)
{
#if 0
	console_send_string("MGC_AsixTxIrpCallback\r\n");
#endif

	MUSB_AsixDevice* pAsixDevice = (MUSB_AsixDevice*) pCtxt;

	if (pBulkIrp->dwStatus != 0)
	{
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixTxIrpCallback: dwStatus=%d\r\n", pBulkIrp->dwStatus);
			console_send_string(debug_string);
		}
#endif
	}
	else
	{
		int actual_len = pBulkIrp->dwActualLength;
		uint8_t* buf_ptr = pBulkIrp->pBuffer;
#if 0
		hex_dump("MGC_AsixTxIrpCallback", buf_ptr, actual_len);
#endif
	}
}


static uint32_t MGC_AsixReadCmd(MUSB_AsixDevice* pAsixDevice,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, void* pData)
{
	MUSB_Device* pUsbDevice = pAsixDevice->pUsbDevice;
	MUSB_ControlIrp* pControlIrp = &pAsixDevice->ctrlIrp;
	uint32_t dwStatus;

	MUSB_HubPrepareSetupPkt(&pAsixDevice->setupPacket,
			(MUSB_DIR_IN | MUSB_TYPE_VENDOR | MUSB_RECIP_DEVICE),
			bRequest, wValue, wIndex, wLength);

	pControlIrp->pOutBuffer = (const uint8_t*) &pAsixDevice->setupPacket;
	pControlIrp->dwOutLength = 8;
	pControlIrp->pInBuffer = pData;
	pControlIrp->dwInLength = wLength;

	dwStatus = MUSB_StartControlTransfer(pUsbDevice->pPort, pControlIrp);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixReadCmd(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	void* pMsg = (void*) OSMboxPend(pAsixDevice->pMBox, 0, &dwStatus);

	return dwStatus;
}


static uint32_t MGC_AsixWriteCmd(MUSB_AsixDevice* pAsixDevice,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, void* pData)
{
	MUSB_Device* pUsbDevice = pAsixDevice->pUsbDevice;
	MUSB_ControlIrp* pControlIrp = &pAsixDevice->ctrlIrp;
	uint32_t dwStatus;

	MUSB_HubPrepareSetupPkt(&pAsixDevice->setupPacket,
			(MUSB_TYPE_VENDOR | MUSB_RECIP_DEVICE),
			bRequest, wValue, wIndex, wLength);

	memcpy(pAsixDevice->aRawBuf, &pAsixDevice->setupPacket, 8);
	memcpy(pAsixDevice->aRawBuf + 8, pData, wLength);
	pControlIrp->pOutBuffer = pAsixDevice->aRawBuf;
	pControlIrp->dwOutLength = 8 + wLength;
	pControlIrp->pInBuffer = NULL;
	pControlIrp->dwInLength = 0;

	dwStatus = MUSB_StartControlTransfer(pUsbDevice->pPort, pControlIrp);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixWriteCmd(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	void* pMsg = (void*) OSMboxPend(pAsixDevice->pMBox, 0, &dwStatus);

	return dwStatus;
}


static uint32_t MGC_AsixWriteGpio(MUSB_AsixDevice* pAsixDevice, uint16_t value, int sleepy)
{
    uint32_t dwStatus;

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixWriteGpio: value=0x%04x\r\n", value);
		console_send_string(debug_string);
	}
#endif

    dwStatus = MGC_AsixWriteCmd(pAsixDevice, AX_CMD_WRITE_GPIOS, value, 0, 0, NULL);
    if (dwStatus != 0)
    {
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixWriteGpio(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
    }
    if (sleepy)
    {
    	rtos_task_wait(sleepy);
    }

    return dwStatus;
}


static int MGC_AsixSwReset(MUSB_AsixDevice* pAsixDevice, uint8_t flags)
{
    uint32_t dwStatus;

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixSwReset: flags=0x%02x\r\n", flags);
		console_send_string(debug_string);
	}
#endif

	dwStatus = MGC_AsixWriteCmd(pAsixDevice, AX_CMD_SW_RESET, flags, 0, 0, NULL);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixSwReset(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	return dwStatus;
}


static uint16_t MGC_AsixReadRxCtl(MUSB_AsixDevice* pAsixDevice)
{
    uint32_t dwStatus;
    uint16_t wValue;

	dwStatus = MGC_AsixReadCmd(pAsixDevice, AX_CMD_READ_RX_CTL, 0, 0, 2, &wValue);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixWriteRxCtl(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	return wValue;
}


static int MGC_AsixWriteRxCtl(MUSB_AsixDevice* pAsixDevice, uint16_t mode)
{
    uint32_t dwStatus;

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixWriteRxCtl: mode=0x%04x\r\n", mode);
		console_send_string(debug_string);
	}
#endif

	dwStatus = MGC_AsixWriteCmd(pAsixDevice, AX_CMD_WRITE_RX_CTL, mode, 0, 0, NULL);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixWriteRxCtl(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	return dwStatus;
}


static int MGC_AsixWriteMediumMode(MUSB_AsixDevice* pAsixDevice, uint16_t mode)
{
    uint32_t dwStatus;

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixWriteMediumMode: mode=0x%04x\r\n", mode);
		console_send_string(debug_string);
	}
#endif

	dwStatus = MGC_AsixWriteCmd(pAsixDevice, AX_CMD_WRITE_MEDIUM_MODE, mode, 0, 0, NULL);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixWriteMediumMode(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	return dwStatus;
}


static uint16_t MGC_AsixReadMediumStatus(MUSB_AsixDevice* pAsixDevice)
{
    uint32_t dwStatus;
    uint16_t wValue;

	dwStatus = MGC_AsixReadCmd(pAsixDevice, AX_CMD_READ_MEDIUM_STATUS, 0, 0, 2, &wValue);
	if (dwStatus != 0)
	{
#if 1
		extern char debug_string[];
		sprintf(debug_string, "AsixError: MGC_AsixReadMediumStatus(): dwStatus=%d\r\n",
				dwStatus);
		console_send_string(debug_string);
#endif
	}

	return wValue;
}


static void MGC_AsixDisconnect()
{
	console_send_string("MGC_AsixDisconnect (todo.c): TODO\r\n");

}


static uint32_t MGC_AsixFindFreeDeviceIndex(MUSB_AsixDriverContext* pDriverContext, uint8_t* pIndex)
{
#if 0
	console_send_string("MGC_AsixFindFreeDeviceIndex (todo.c): TODO\r\n");
#endif

	for (uint8_t index = 0; index < MUSB_ASIX_MAX_DEV_SUPPORTED; index++)
	{
		if (pDriverContext->asixDeviceList[index].bState == 0)
		{
			*pIndex = index;
			return 0;
		}
	}

	return -1;
}


static void MUSB_AsixPrepareSetupPkt(MUSB_DeviceRequest *pSetup,
		 uint8_t bmRequestType,
		 uint8_t bRequest, uint16_t wValue,
		 uint16_t wIndex, uint16_t wLength)
{
#if 0
	console_send_string("MUSB_AsixPrepareSetupPkt (todo.c): TODO\r\n");
#endif

	pSetup->bmRequestType = bmRequestType;
	pSetup->bRequest = bRequest;
	pSetup->wValue = wValue;
	pSetup->wIndex = wIndex;
	pSetup->wLength = wLength;
}


static void MGC_AsixControlTransferCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
	MUSB_AsixDevice* pAsixDevice = pContext;
	int msg;

	if (pControlIrp->dwStatus != 0)
	{
	#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixControlTransferCallback(): pControlIrp->dwStatus=%d\r\n",
					pControlIrp->dwStatus);
			console_send_string(debug_string);
		}
	#endif
	}

//	OSSemPost(pAsixDevice->threadSema);
	OSMboxPost(pAsixDevice->pMBox, &msg);

}


static void MGC_AsixThread(void* pContext)
{
	MUSB_AsixDevice* pAsixDevice = pContext;
	MUSB_Device* pUsbDevice = pAsixDevice->pUsbDevice;
	MUSB_ControlIrp* pCtrlIrp = &pAsixDevice->ctrlIrp;
	MUSB_DeviceRequest* pSetupPacket = &pAsixDevice->setupPacket;
	int dwStatus;

	/* ---------------- MUSB_REQ_SET_CONFIGURATION --------------- */

	MUSB_AsixPrepareSetupPkt(pSetupPacket,
			(MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE),
			MUSB_REQ_SET_CONFIGURATION,
			pUsbDevice->apConfigDescriptors[0]->bConfigurationValue, 0, 0);

	pCtrlIrp->pDevice = pUsbDevice;
	pCtrlIrp->pOutBuffer = (const uint8_t*) pSetupPacket;
	pCtrlIrp->dwOutLength = 8;
	pCtrlIrp->pInBuffer = NULL;
	pCtrlIrp->dwInLength = 0;
	pCtrlIrp->pfIrpComplete = MGC_AsixControlTransferCallback;
	pCtrlIrp->pCompleteParam = pAsixDevice;

	dwStatus = MUSB_StartControlTransfer(pUsbDevice->pPort, pCtrlIrp);

//	OSSemPend(pAsixDevice->threadSema, 0, &dwStatus);
	void* pMsg = (void*) OSMboxPend(pAsixDevice->pMBox, 0, &dwStatus);

	console_send_string("MGC_AsixThread: Callback for MUSB_REQ_SET_CONFIGURATION\r\n");

	/* ---------------- Get the MAC address --------------- */

	dwStatus = MGC_AsixReadCmd(pAsixDevice,
			AX_CMD_READ_NODE_ID, 0, 0,
			sizeof(pAsixDevice->abMacAddress), pAsixDevice->abMacAddress);

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixThread: abMacAddress=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
				pAsixDevice->abMacAddress[0], pAsixDevice->abMacAddress[1], pAsixDevice->abMacAddress[2],
				pAsixDevice->abMacAddress[3], pAsixDevice->abMacAddress[4], pAsixDevice->abMacAddress[5]);
		console_send_string(debug_string);
	}
#endif

	/* ---------------- Get the PHY address --------------- */

	pAsixDevice->phy_addr = MGC_AsixGetPhyAddr(pAsixDevice);
	pAsixDevice->embd_phy = ((pAsixDevice->phy_addr & 0x1f) == AX_EMBD_PHY_ADDR);

	/* ---------------- Read ChipCode --------------- */

	dwStatus = MGC_AsixReadCmd(pAsixDevice,
				AX_CMD_STATMNGSTS_REG, 0, 0, 1, &pAsixDevice->chipcode);

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixThread: ChipCode=0x%02x\r\n", pAsixDevice->chipcode);
		console_send_string(debug_string);
	}
#endif

	if ((pAsixDevice->chipcode & AX_CHIPCODE_MASK) == AX_AX88772_CHIPCODE)
	{
		//ax88772_hw_reset

		//TODO!!!
	}
	else
	{
		//ax88772a_hw_reset

		dwStatus = MGC_AsixWriteGpio(pAsixDevice, AX_GPIO_RSE, 5);

		dwStatus = MGC_AsixWriteCmd(pAsixDevice, AX_CMD_SW_PHY_SELECT,
				pAsixDevice->embd_phy | AX_PHYSEL_SSEN, 0, 0, NULL);

    	rtos_task_wait(20);

		dwStatus = MGC_AsixSwReset(pAsixDevice, AX_SWRESET_IPPD | AX_SWRESET_IPRL);

    	rtos_task_wait(20);

		dwStatus = MGC_AsixSwReset(pAsixDevice, AX_SWRESET_IPRL);
		dwStatus = MGC_AsixSwReset(pAsixDevice, AX_SWRESET_CLEAR);
		dwStatus = MGC_AsixSwReset(pAsixDevice, AX_SWRESET_IPRL);

    	rtos_task_wait(200);

		/* Check if the PHY registers have default settings */

    	//TODO!!!

   	    dwStatus = MGC_AsixWriteCmd(pAsixDevice, AX_CMD_WRITE_IPG0,
	    		AX88772_IPG0_DEFAULT | AX88772_IPG1_DEFAULT,
				AX88772_IPG2_DEFAULT, 0, NULL);

		/* Set RX_CTL to default values with 2k buffer, and enable cactus */
		dwStatus = MGC_AsixWriteRxCtl(pAsixDevice, AX_DEFAULT_RX_CTL);

		dwStatus = MGC_AsixWriteMediumMode(pAsixDevice, AX88772_MEDIUM_DEFAULT);

		uint16_t wRxCtl = MGC_AsixReadRxCtl(pAsixDevice);
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixThread: wRxCtl=0x%04x\r\n", wRxCtl);
			console_send_string(debug_string);
		}
#endif

		uint16_t wMediumStatus = MGC_AsixReadMediumStatus(pAsixDevice);
#if 1
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_AsixThread: wMediumStatus=0x%04x\r\n", wMediumStatus);
			console_send_string(debug_string);
		}
#endif

		MGC_AsixMdioWrite(pAsixDevice, pAsixDevice->phy_addr, MII_BMCR, BMCR_RESET);
		MGC_AsixMdioWrite(pAsixDevice, pAsixDevice->phy_addr, MII_ADVERTISE,
				ADVERTISE_ALL | ADVERTISE_CSMA);

		mii_nway_restart(pAsixDevice);

	}

	/* ---------- Create LWIP network interface ---- */

	ethif_asixusb_init(pAsixDevice);

	//TODO!!!

	while (1)
	{
#if 0
		pMsg = (void*) OSMboxPend(pAsixDevice->pMBox, 0, &dwStatus);

		console_send_string("MGC_AsixThread: Continue\r\n");
#endif

#if 1
        int link_detected = MGC_AsixMdioRead(pAsixDevice, pAsixDevice->phy_addr, MII_BMSR) &
            BMSR_LSTATUS;
#endif

        if (link_detected)
        {
        	if (/*ERR_OK*/0 != dhcp_start(pAsixDevice->pNetif))
        	{
        		console_send_string("ethif_asixusb_init: dhcp_start failed!\r\n");
        	}

        	break;
        }

    	rtos_task_wait(100);
	}

	while (1)
	{
    	rtos_task_wait(100);
	}
}


static int MGC_AsixGetPhyAddr(MUSB_AsixDevice* pAsixDevice)
{
	uint8_t buf[2];

	uint32_t dwStatus = MGC_AsixReadCmd(pAsixDevice,
			AX_CMD_READ_PHY_ID, 0, 0, 2, buf);

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixGetPhyAddr() returning 0x%02x%02x\r\n",
				buf[0], buf[1]);
		console_send_string(debug_string);
	}
#endif

	dwStatus = buf[1];

	return dwStatus;
}


static int MGC_AsixMdioRead(MUSB_AsixDevice* pAsixDevice, int phy_id, int loc)
{
	uint16_t val;
    uint8_t res[2];

	MGC_AsixSetSwMii(pAsixDevice);
	MGC_AsixReadCmd(pAsixDevice, AX_CMD_READ_MII_REG, phy_id, loc, 2, res);
    MGC_AsixSetHwMii(pAsixDevice);

    val = res[0] | (res[1] << 8);

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixMdioRead() phy_id=0x%02x, loc=0x%02x, val=0x%04x\r\n",
				phy_id, loc, val);
		console_send_string(debug_string);
	}
#endif

    return val;
}


static void MGC_AsixMdioWrite(MUSB_AsixDevice* pAsixDevice, int phy_id, int loc, int val)
{
    uint8_t res[2];

    res[0] = val & 0xff;
    res[1] = (val >> 8) & 0xff;

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_AsixMdioWrite() phy_id=0x%02x, loc=0x%02x, val=0x%04x\r\n",
				phy_id, loc, val);
		console_send_string(debug_string);
	}
#endif

	MGC_AsixSetSwMii(pAsixDevice);
    MGC_AsixWriteCmd(pAsixDevice, AX_CMD_WRITE_MII_REG, phy_id, loc, 2, res);
    MGC_AsixSetHwMii(pAsixDevice);
}


static void MGC_AsixSetSwMii(MUSB_AsixDevice* pAsixDevice)
{
	MGC_AsixWriteCmd(pAsixDevice, AX_CMD_SET_SW_MII, 0x0000, 0, 0, NULL);
}


static void MGC_AsixSetHwMii(MUSB_AsixDevice* pAsixDevice)
{
	MGC_AsixWriteCmd(pAsixDevice, AX_CMD_SET_HW_MII, 0x0000, 0, 0, NULL);
}


static int mii_nway_restart(MUSB_AsixDevice* pAsixDevice)
{
	int bmcr;
	int r = -1;

	/* if autoneg is off, it's an error */
	bmcr = MGC_AsixMdioRead(pAsixDevice, pAsixDevice->phy_addr, MII_BMCR);

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "mii_nway_restart() bmcr=0x%04x\r\n", bmcr);
		console_send_string(debug_string);
	}
#endif

	if (bmcr & BMCR_ANENABLE)
	{
		bmcr |= BMCR_ANRESTART;
		MGC_AsixMdioWrite(pAsixDevice, pAsixDevice->phy_addr, MII_BMCR, bmcr);
		r = 0;
	}

	return r;
}


