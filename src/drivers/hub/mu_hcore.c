
//#pragma thumb

#include "mu_arch.h"
#include "mu_cdi.h"
#include "mu_descs.h"
#include "mu_diag.h"
#include "mu_mem.h"
#include "mu_stdio.h"
#include "class/mu_hub.h"
#include "mu_hdef.h"
#include "mu_hapi.h"
#include "mu_hcore.h"
#include "mu_hcreq.h"

#define PRINT_MSG(...) 								\
		{ 											\
			extern char debug_string[]; 			\
			sprintf(debug_string, ##__VA_ARGS__); 	\
			console_send_string(debug_string); 		\
			console_send_string("\r\n"); 			\
		}
#define MUSB_PrintLine(x) PRINT_MSG x

extern uint8_t MGC_bDiagLevel;
extern uint8_t MGC_DiagMsg[];
extern uint8_t MGC_DiagNumber[];



void MUSB_HubPrepareSetupPkt(MUSB_DeviceRequest *pSetup,
		 uint8_t bmRequestType,
		 uint8_t bRequest, uint16_t wValue,
		 uint16_t wIndex, uint16_t wLength)
{
#if 0
	console_send_string("MUSB_HubPrepareSetupPkt (todo.c): TODO\r\n");
#endif

	pSetup->bmRequestType = bmRequestType;
	pSetup->bRequest = bRequest;
	pSetup->wValue = wValue;
	pSetup->wIndex = wIndex;
	pSetup->wLength = wLength;
}


void MGC_HubStoreStatus (uint8_t* pStatusData/*r5*/, MUSB_HubPortStatus* pHubStatus/*r4*/)
{
#if 0
	console_send_string("MGC_HubStoreStatus (todo.c): TODO\r\n");
#endif

	uint32_t sp;

	MUSB_BitsGet(pStatusData, 0, 16, &sp);

	pHubStatus->wStatus = sp;

	MUSB_BitsGet(pStatusData + 2, 0, 16, &sp);

	pHubStatus->wStatusChange = sp;
}


void MGC_HubPortStatusChange(MUSB_Hub *pHubDevice)
{
#if 0
	console_send_string("MGC_HubPortStatusChange (todo.c): TODO\r\n");
#endif

#if 0
	if (pHubDevice->bActivePortNum > pHubDevice->hubDscr.bNbrPorts)
	{
		return;
	}
#endif

//	uint32_t lr = pHubDevice->dwHubPortStatusChange;
//	int ip = 1;

	while (pHubDevice->bActivePortNum <= pHubDevice->hubDscr.bNbrPorts)
	{
		//loc_ab0
		if ((pHubDevice->dwHubPortStatusChange & (1 << pHubDevice->bActivePortNum)) == 0)
		{
			pHubDevice->bActivePortNum++;
			//->loc_adc
		}
		else
		{
			pHubDevice->bPortNextState = MUSB_HUB_PORT_STATE_GOT_STATUS; //1

#if 0
			MUSB_HubGetStatus(pHubDevice,
					MGC_HubGetPortStatusCallback,
					pHubDevice->bActivePortNum,
					(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0xa3*/);
#else
			MUSB_HubGetPortStatus(pHubDevice, MGC_HubGetPortStatusCallback, pHubDevice->bActivePortNum);
#endif

			break;
		}
	}
}


void MGC_HubGetPortStatusCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp)
{
#if 0
	console_send_string("MGC_HubGetPortStatusCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r4 = pCtxt;
	MUSB_HubPortStatus* r6 = &r4->portStatus;

	if (pCtrlIrp->dwStatus != 0)
	{
		//loc_81c
		return;
	}

	uint8_t* r5 = &r4->bPortNextState;
	//int r7 = 0;
	//int r8 = 2;

	r4->bPortState = r4->bPortNextState;

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_HubGetPortStatusCallback: r4->bPortState=%d\r\n",
				r4->bPortState);
		console_send_string(debug_string);
	}
#endif

	if (r4->bPortState == MUSB_HUB_PORT_STATE_GOT_STATUS) //1)
	{
		MGC_HubStoreStatus(r4->aHubPortStatus, r6);

		if ((r6->wStatusChange & 1) == 0)
		{
			r4->bPortState = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2; //r8; //2
			//->loc_868
		}
		else if (r6->wStatus & 1)
		{
			r4->debounceParam.dwWaitingTime = 100;
			r4->debounceParam.bErrorCount = 0; //r7

			if (r4->bAllocatedPower < 5)
			{
				*r5 = MUSB_HUB_PORT_STATE_WAIT_FOR_DEBOUNCE; //9;
			}
			else
			{
				*r5 = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2; //r8
			}

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_CONNECTION_FEATURE, //0x10,
					MGC_HubPortDebounceCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_CONNECTION_FEATURE,
					MGC_HubPortDebounceCallback, r4->bActivePortNum);
#endif
			//loc_81c
			return;
		}
		else
		{
			//loc_824
			MGC_HubUpdatePower(r4, 1);

			*r5 = MUSB_HUB_PORT_STATE_DISCONNECT; //14;

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_CONNECTION_FEATURE, //0x10,
					MGC_HubGetPortStatusCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_CONNECTION_FEATURE,
					MGC_HubGetPortStatusCallback, r4->bActivePortNum);
#endif

			return;
		}
	} //if (r4->bPortState == 1)
	//loc_860
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE) //2
	{
		//loc_868
		if ((r6->wStatusChange & 2) == 0)
		{
			r4->bPortState = MUSB_HUB_PORT_STATE_C_ENABLE_COMPLETE; //3;
			//->loc_8b4
		}
		else
		{
			*r5 = MUSB_HUB_PORT_STATE_C_ENABLE_COMPLETE; //3

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_ENABLE_FEATURE, //0x11,
					MGC_HubGetPortStatusCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_ENABLE_FEATURE,
					MGC_HubGetPortStatusCallback, r4->bActivePortNum);
#endif

			return;
		}
	}
	//loc_8ac
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_ENABLE_COMPLETE) //3
	{
		//loc_8b4
		//r8 = 4;
		if ((r6->wStatusChange & 4) == 0)
		{
			r4->bPortState = MUSB_HUB_PORT_STATE_C_SUSPEND_COMPLETE; //4;
			//->loc_928
		}
		else
		{
			if (r6->wStatus & 4)
			{
				MGC_HubUpdatePower(r4, 2);
				//->loc_8f0
			}
			else
			{
				//loc_8e4
				MGC_HubUpdatePower(r4, 3);
			}
			//loc_8f0
			*r5 = MUSB_HUB_PORT_STATE_C_SUSPEND_COMPLETE; //4;

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_SUSPEND_FEATURE, //0x12,
					MGC_HubGetPortStatusCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_SUSPEND_FEATURE,
					MGC_HubGetPortStatusCallback, r4->bActivePortNum);
#endif

			return;
		}
	}
	//loc_920
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_SUSPEND_COMPLETE) //4
	{
		//loc_928
		//r8 = 8;
		if ((r6->wStatusChange & 8) == 0)
		{
			r4->bPortState = MUSB_HUB_PORT_STATE_C_OVERCURRENT_COMPLETE; //8
			//->loc_9e4
		}
		else
		{
			if (r6->wStatus & 8)
			{
				*r5 = MUSB_HUB_PORT_STATE_C_OVERCURRENT; //6;
			}
			else
			{
				*r5 = MUSB_HUB_PORT_STATE_C_OVERCURRENT_COMPLETE; //8;
			}

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_OVER_CURRENT_FEATURE, //0x13,
					MGC_HubGetPortStatusCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_OVER_CURRENT_FEATURE,
					MGC_HubGetPortStatusCallback, r4->bActivePortNum);
#endif

			return;
		}
	}
	//loc_974
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_OVERCURRENT) //6
	{
		//TODO!!!
		console_send_string("MGC_HubGetPortStatusCallback: MUSB_HUB_PORT_STATE_C_OVERCURRENT (todo.c): TODO\r\n");
	}
	//loc_9ac
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_OVERCURRENT_CONTINUE) //7)
	{
		//TODO!!!
		console_send_string("MGC_HubGetPortStatusCallback: MUSB_HUB_PORT_STATE_C_OVERCURRENT_CONTINUE (todo.c): TODO\r\n");
	}
	//loc_9dc
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_OVERCURRENT_COMPLETE) //8)
	{
		//loc_9e4
		//r0, #0x5
		if ((r6->wStatusChange & 0x10) == 0)
		{
			r4->bPortState = MUSB_HUB_PORT_STATE_C_RESET_COMPLETE; //5;
			//->loc_a2c
		}
		else
		{
			*r5 = MUSB_HUB_PORT_STATE_C_RESET_COMPLETE; //5;

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_RESET_FEATURE, //0x14
					MGC_HubGetPortStatusCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_RESET_FEATURE,
					MGC_HubGetPortStatusCallback, r4->bActivePortNum);
#endif

			return;
		}
	}
	//loc_a24
	if (r4->bPortState == MUSB_HUB_PORT_STATE_C_RESET_COMPLETE) //5)
	{
		//loc_a2c
		r4->bPortState = MUSB_HUB_PORT_STATE_DEFAULT; //0; //r7;
		*r5 = MUSB_HUB_PORT_STATE_DEFAULT; //0; //r7

		r4->bActivePortNum++;

		MGC_HubPortStatusChange(r4);

		return;
	}
	//loc_a50
	if (r4->bPortState == MUSB_HUB_PORT_STATE_DISCONNECT) //14)
	{
		//a58

		//TODO!!!
		console_send_string("MGC_HubGetPortStatusCallback: MUSB_HUB_PORT_STATE_DISCONNECT (todo.c): TODO\r\n");
	}
	//->loc_81c
	return;
}


void MGC_HubPortDebounceCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp)
{
#if 0
	console_send_string("MGC_HubPortDebounceCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r4 = pCtxt;
	MUSB_PortDebounceParam* r5 = &r4->debounceParam;
	MUSB_HubPortStatus* r7 = &r4->debounceParam.debouncePortStatus;

	if (pCtrlIrp->dwStatus != 0)
	{
		//->loc_71c
		return;
	}

	//int r6 = 10;
	r4->bPortState = r4->bPortNextState;

	switch (r4->bPortState)
	{
	case MUSB_HUB_PORT_STATE_WAIT_FOR_DEBOUNCE: //9:
		//loc_6fc
		r4->bPortNextState = MUSB_HUB_PORT_STATE_DEBOUNCE_GOT_STATUS; //10; //r6;

		MUSB_ArmTimer(r4->busHandle, r4->pDriver, r4->bIndexAtHubDeviceList,
				r5->dwWaitingTime, MGC_HubPortDebounceTimerCallback, r4);
		break;

	case MUSB_HUB_PORT_STATE_DEBOUNCE_GOT_STATUS: //10:
		//620
		MGC_HubStoreStatus(r4->aHubPortStatus, r7);

		//int r1 = 0;
		if (r7->wStatus & 1)
		{
			if (r5->dwWaitingTime > 200)
			{
				r4->bPortNextState = MUSB_HUB_PORT_STATE_START_RESET; //11;

				r4->ctrlIrp.pfIrpComplete = MGC_HubPortResetCallback;
				r4->ctrlIrp.dwStatus = 0; //r1

				r4->setupPacket.bmRequestType = (MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/;
				r4->setupPacket.bRequest = 3;
				r4->setupPacket.wValue = 4;
				r4->setupPacket.wIndex = r4->bActivePortNum;
				r4->setupPacket.wLength = 0;

				r4->ctrlIrp.pInBuffer = NULL;
				r4->ctrlIrp.dwInLength = 0;

				MUSB_StartControlTransfer(r4->pUsbDevice->pPort, &r4->ctrlIrp);

				return;
			}
			//loc_6fc
		} //if (r7->wStatus & 1)
		else
		{
			//loc_6b8
			r5->dwWaitingTime = 100;
			r5->bErrorCount++;

			if (r5->bErrorCount > 3)
			{
				r4->bPortNextState = r4->bPortState = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2;
				r4->ctrlIrp.dwStatus = 0; //r1

				MGC_HubGetPortStatusCallback(r4, &r4->ctrlIrp);

				return;
			}
			//loc_6fc
		}
		//loc_6fc
		r4->bPortNextState = MUSB_HUB_PORT_STATE_DEBOUNCE_GOT_STATUS; //10; //r6;

		MUSB_ArmTimer(r4->busHandle, r4->pDriver, r4->bIndexAtHubDeviceList,
				r5->dwWaitingTime, MGC_HubPortDebounceTimerCallback, r4);
		break;

	default:
		//loc_71c
		break;
	}
}


void MGC_HubPortDebounceTimerCallback (void *pParam, MUSB_BusHandle hBus)
{
#if 0
	console_send_string("MGC_HubPortDebounceTimerCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r0 = pParam;

	r0->debounceParam.dwWaitingTime += MUSB_HUB_INCREMENT_DEBOUNCE_TIME; //50;

	r0->bPortNextState = MUSB_HUB_PORT_STATE_DEBOUNCE_GOT_STATUS; //10;

#if 0
	MUSB_HubGetStatus(r0, MGC_HubPortDebounceCallback, r0->bActivePortNum,
			(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0xa3*/);
#else
	MUSB_HubGetPortStatus(r0, MGC_HubPortDebounceCallback, r0->bActivePortNum);
#endif
}


void MGC_HubPortResetCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp)
{
#if 0
	console_send_string("MGC_HubPortResetCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r4 = pCtxt;
	MUSB_PortResetParam* r5 = &r4->resetParam;
	MUSB_HubPortStatus* r7 = &r4->resetParam.resetPortStatus;

	if (pCtrlIrp->dwStatus != 0)
	{
		//loc_564
		return;
	}

	//int r6 = 12;
	r4->bPortState = r4->bPortNextState;

	switch (r4->bPortState)
	{
	case MUSB_HUB_PORT_STATE_START_RESET: //11:
		//loc_594
		r4->bPortNextState = MUSB_HUB_PORT_STATE_RESET_GOT_STATUS; //12

		MUSB_ArmTimer(r4->busHandle, r4->pDriver, r4->bIndexAtHubDeviceList,
				r5->dwWaitingTime, MGC_HubPortResetTimerCallback, r4);

		break;

	case MUSB_HUB_PORT_STATE_RESET_GOT_STATUS: //12:
		//4dc
		MGC_HubStoreStatus(r4->aHubPortStatus, r7);

		//int r1 = 2;
		if ((r7->wStatus & 1) == 0)
		{
			r4->bPortNextState = r4->bPortState = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2; //r1

			MGC_HubGetPortStatusCallback(r4, &r4->ctrlIrp);

			return;
		}
		else if (r7->wStatus & 2)
		{
			if (r7->wStatus & 0x200)
			{
				r4->bSpeed = 1;
			}
			else if ((r7->wStatus & 0x400) == 0)
			{
				r4->bSpeed = 2; //r1
			}
			else
			{
				r4->bSpeed = 3;
			}
			//loc_53c
			r4->bPortNextState = MUSB_HUB_PORT_STATE_RESET_COMPLETE; //13;

#if 0
			MUSB_HubFeatureRequest(r4, MUSB_HUB_C_PORT_RESET_FEATURE, //0x14,
					MGC_HubPortConnectCallback,
					r4->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_CLEAR_FEATURE/*1*/);
#else
			MUSB_HubClearPortFeature(r4, MUSB_HUB_C_PORT_RESET_FEATURE,
					MGC_HubPortConnectCallback, r4->bActivePortNum);
#endif
			//loc_564
		}
		else
		{
			//loc_56c
			if (r5->dwNumOfTry < MUSB_HUB_MAX_RESET_TRIES) //5)
			{
				if (r5->dwWaitingTime > 100)
				{
					r5->dwNumOfTry++;
					r5->dwWaitingTime = 10;
				}
				//loc_594
				r4->bPortNextState = MUSB_HUB_PORT_STATE_RESET_GOT_STATUS; //12;

				MUSB_ArmTimer(r4->busHandle, r4->pDriver, r4->bIndexAtHubDeviceList,
						r5->dwWaitingTime, MGC_HubPortResetTimerCallback, r4);
			}
			else
			{
				//loc_5bc
				r4->bPortNextState = r4->bPortState = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2; //r1

				r4->ctrlIrp.dwStatus = 0;

				MGC_HubGetPortStatusCallback(r4, &r4->ctrlIrp);
			}
		}
		break;

	default:
		//loc_564
		break;
	}
}


void MGC_HubPortResetTimerCallback (void *pParam, MUSB_BusHandle hBus)
{
#if 0
	console_send_string("MGC_HubPortResetTimerCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r0 = pParam;

	r0->resetParam.dwWaitingTime += MUSB_HUB_INCREMENT_RESET_TIME; //20

	r0->bPortNextState = MUSB_HUB_PORT_STATE_RESET_GOT_STATUS; //12;

#if 0
	MUSB_HubGetStatus(r0, MGC_HubPortResetCallback, r0->bActivePortNum,
			(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0xa3*/);
#else
	MUSB_HubGetPortStatus(r0, MGC_HubPortResetCallback, r0->bActivePortNum);
#endif
}


void MGC_HubPortConnectCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp)
{
#if 0
	console_send_string("MGC_HubPortConnectCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r4 = pCtxt;

	if (pCtrlIrp->dwStatus != 0)
	{
		//->loc_458
		return;
	}

	r4->bPortState = r4->bPortNextState;

	if (r4->bPortState == MUSB_HUB_PORT_STATE_RESET_COMPLETE) //13
	{
		if (0 == MGC_HubGetChildIndexForGivenPort(r4, r4->bActivePortNum, &r4->bCurrentChildIndex))
		{
			if (r4->bDepthInHubChain < 6)
			{
				r4->bPortNextState = MUSB_HUB_PORT_WAIT_FOR_ENUMERATE; //15;

				MUSB_EnumerateDevice(r4->pUsbDevice, r4->bActivePortNum,
						r4->bSpeed, MUSB_HubDeviceEnumerateCallback);

				return;
			}
			//loc_434
		}
		//loc_434
		r4->bPortNextState = r4->bPortState = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2; //r1

		r4->ctrlIrp.dwStatus = 0;

		MGC_HubGetPortStatusCallback(r4, &r4->ctrlIrp);
	}
	//loc_458
}


uint32_t MGC_HubGetChildIndexForGivenPort (MUSB_Hub* pHubDevice, uint8_t bPortNum, uint8_t* pChildIndex)
{
#if 0
	console_send_string("MGC_HubGetChildIndexForGivenPort (todo.c): TODO\r\n");
#endif

	MUSB_HubChildPortMap* r4 = pHubDevice->aHubChildPortMap;

	int r3 = -1;

	for (uint8_t i = 0; i < MUSB_HUB_MAX_PORTS; i++)
	{
		//loc_98
		if (r4[i].bStatus == 1)
		{
			if (r4[i].bPortNum == bPortNum)
			{
				*pChildIndex = i;

				return 1;
			}
		}
		else
		{
			//loc_c0
			if ((r4[i].bStatus == 0) && (r3 == -1))
			{
				r3 = i;
			}
		}
	}

	if (r3 == -1)
	{
		return -1;
	}

	*pChildIndex = r3;

	return 0;
}


void MUSB_HubDeviceEnumerateCallback (MUSB_Device *pHubUsbDevice, MUSB_Device *pChildUsbDevice/*r4*/)
{
#if 0
	console_send_string("MUSB_HubDeviceEnumerateCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* sp;

	if (0 == MGC_HubFindHubDevice(pHubUsbDevice, &sp))
	{
		MGC_HubUpdatePower(sp, 0);

		pChildUsbDevice->pParentUsbDevice = sp->pUsbDevice;

		sp->pChild[sp->bCurrentChildIndex] = pChildUsbDevice;
		sp->aHubChildPortMap[sp->bCurrentChildIndex].bStatus = 1;
		sp->aHubChildPortMap[sp->bCurrentChildIndex].bPortNum = sp->bActivePortNum;
		sp->bPortNextState = sp->bPortState = MUSB_HUB_PORT_STATE_C_CONNECT_COMPLETE; //2;
		sp->ctrlIrp.dwStatus = 0;

		MGC_HubGetPortStatusCallback(sp, &sp->ctrlIrp);
	}
}


