
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



uint32_t MGC_HubFindFreeHubDeviceIndex(MUSB_HubDriverContext* pDriverContext/*r4*/, uint8_t* pIndex/*r6*/)
{
#if 0
	console_send_string("MGC_HubFindFreeHubDeviceIndex (todo.c): TODO\r\n");
#endif

	for (uint8_t index = 0; index < 6; index++)
	{
		//loc_1a0c
		if (pDriverContext->hubDeviceList[index].bState == 0)
		{
			if (MGC_bDiagLevel >= 3)
			{
				MUSB_PrintLine(("HubSuccess: FreeHubDeviceIsAvailable"));
			}
			//loc_1a40
			*pIndex = index;
			return 0;
		}
		//loc_1a4c
	}

	if (MGC_bDiagLevel >= 1)
	{
		MUSB_PrintLine(("HubSuccess: FreeHubDeviceIsNotAvailable"));
	}

	return -1;
}


uint32_t MGC_HubFindHubDevice(MUSB_Device* pUsbDevice/*r2*/, MUSB_Hub** pHubDevice/*r1*/)
{
#if 0
	console_send_string("MGC_HubFindHubDevice (todo.c): TODO\r\n");
#endif

	MUSB_HubDriverContext* ip = pUsbDevice->pDriverPrivateData;
	MUSB_Hub* lr = ip->hubDeviceList;

	for (uint8_t r3 = 0; r3 < 6; r3++)
	{
		//loc_514
		if (lr[r3].pUsbDevice == pUsbDevice)
		{
			*pHubDevice = &lr[r3];
			return 0;
		}
		//loc_544
	}

	return -1;
}


uint32_t MGC_HubValidateUpdateTtInfo(MUSB_Hub* pHubDevice/*r4*/)
{
#if 0
	console_send_string("MGC_HubValidateUpdateTtInfo (todo.c): TODO\r\n");
#endif

	MUSB_Device* r8 = pHubDevice->pUsbDevice;

	const MUSB_InterfaceDescriptor* r5 =
			MUSB_FindInterfaceDescriptor(r8->pCurrentConfiguration, 0, pHubDevice->bAlternateSetting);

	if (r5 == NULL)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: UpdateTTInfo-FindInfDscrFail"));
		}
		//loc_890
		return -1;
	}
	//loc_898
	int r6 = r8->DeviceDescriptor.bDeviceProtocol;
	int r7 = r5->bInterfaceProtocol;

	if ((r6 == 0) && (r7 == 0))
	{
		pHubDevice->bTtType = 2;

		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("Hub:Operating At Full-/Low-Speed-NoTT"));
		}
		//loc_b28
	}
	//loc_8d4
	else if ((r6 == 2) && (r7 == 1))
	{
		pHubDevice->bTtType = 0x10;
		//->loc_b28
	}
	//loc_8f0
	else if ((r6 == 2) && (r7 == 2))
	{
		pHubDevice->bTtType = 0x11;
		//->loc_b28
	}
	//loc_90c
	else if ((r6 == 1) && (r7 == 0))
	{
		pHubDevice->bTtType = 0;
		//->loc_b28
	}
	else
	{
		//loc_928
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError:UpdateTTInfo-UndefinedDescriptor:"));
		}
		//loc_940
		MGC_DiagMsg[0] = 0;
		MUSB_StringConcat(MGC_DiagMsg, 0x100, "bDeviceProtocol:");
		MUSB_Stringize(MGC_DiagNumber, 0x21, r6, 16, 0);
		MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine((MGC_DiagMsg));
		}
		//loc_9a0
		MGC_DiagMsg[0] = 0;
		MUSB_StringConcat(MGC_DiagMsg, 0x100, "bInfProtocol:");
		MUSB_Stringize(MGC_DiagNumber, 0x21, r7, 16, 0);
		MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine((MGC_DiagMsg));
		}
		//loc_a00
		pHubDevice->bTtType = 0xff;

		return -1;
	}
	//loc_b28
	return 0;
}


uint8_t MUSB_HubResetTt(
    MUSB_Device   *pUsbDevice/*r4*/,
    MUSB_pfControlIrpComplete ControlIrpComplete/*r5*/,
    void          *pCompleteParam/*r6*/,
    uint16_t       wPortNumber/*r7*/)
{
#if 0
	console_send_string("MUSB_HubResetTt (todo.c): TODO\r\n");
#endif

	MUSB_Hub* sp8;

	int sl = MGC_HubFindHubDevice(pUsbDevice, &sp8);

	if (sl != 0)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: This Is Not A Hub Device/NULL"));
		}
		//loc_740
		return 0;
	}
	//loc_748
	MUSB_ControlIrp* pCtrlIrp = &sp8->ctrlIrp;
	MUSB_DeviceRequest* pSetupPacket = &sp8->setupPacket;

	int fp;

	if ((sp8->bTtType & 1) == 1)
	{
		fp = wPortNumber;
		//->loc_7b0
	}
	//loc_774
	else if ((sp8->bTtType & 1) == 0)
	{
		fp = 1;
		//->loc_7b0
	}
	else
	{
		//loc_78c
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Invalid TT type"));
		}
		//loc_7a8
		return 0;
	}
	//loc_7b0
	MUSB_HubPrepareSetupPkt(pSetupPacket,
			(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
			9, 0, fp, 0);

#if 0
	pCtrlIrp->pOutBuffer = (const uint8_t*) pSetupPacket;
	pCtrlIrp->dwOutLength = 8;
	pCtrlIrp->pInBuffer = NULL;
	pCtrlIrp->dwInLength = 0;
	pCtrlIrp->pfIrpComplete = ControlIrpComplete;
	pCtrlIrp->pCompleteParam = pCompleteParam;
#else
	MUSB_FILL_CTRL_IRP(pCtrlIrp, 0, pSetupPacket, 8, NULL, 0, ControlIrpComplete, pCompleteParam);
#endif

	sl = MUSB_StartControlTransfer(pUsbDevice->pPort, pCtrlIrp);

	if (sl != 0)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Reset TT control request failed"));
		}
		//loc_81c
		return 0;
	}
	//loc_824
	return 1;
}


uint32_t MGC_HubPowerValidateInit(MUSB_Hub* pHubDevice/*r4*/)
{
#if 0
	console_send_string("MGC_HubPowerValidateInit (todo.c): TODO\r\n");
#endif

	MUSB_Hub* sp;
	int res = 0;
	int r7 = pHubDevice->hubStatus.wStatus & 1;
	MUSB_ConfigurationDescriptor* r8 = pHubDevice->pUsbDevice->pCurrentConfiguration;
	int r5 = r8->bmAttributes & 0x40;
	int r6 = r8->bMaxPower;

	if (pHubDevice->pUsbDevice->pParentUsbDevice != NULL)
	{
		res = MGC_HubFindHubDevice(pHubDevice->pUsbDevice->pParentUsbDevice, &sp);

		if (res != 0)
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Parent is not a Hub"));
			}
			//loc_5c8
			return -1;
		}
		//loc_6dc
		if ((sp->bSelfPower == 0) && (r5 == 0))
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Parent and child both are bus-powered so they cannot work"));
			}
			//loc_70c
			pHubDevice->bAllocatedPower = 5;
			//->loc_720
		}
		else
		{
			//loc_718
			pHubDevice->bAllocatedPower = 0;
		}
	}
	//loc_720
	pHubDevice->bAllocatedPower = 0;

	if (r5 != 0)
	{
		if ((r7 == 0) && (r6 == 0))
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Self-powered hub status local power & max power=0"));
			}
			//loc_758
			res = -1;
			//->loc_7ec
		}
		//loc_760
		else if ((r7 != 0) && (r6 == 0))
		{
			pHubDevice->bLocalPower = pHubDevice->bSelfPower = 1;
			//->loc_7ec
		}
		//loc_780
		else if ((r7 == 0) && (r6 != 0))
		{
			pHubDevice->bSelfPower = 2;
			pHubDevice->bLocalPower = 0;
			//->loc_7ec
		}
		else
		{
			//loc_7a4
			pHubDevice->bSelfPower = 2;
			pHubDevice->bLocalPower = 1;
			//->loc_7ec
		}
	}
	//loc_7b8
	else if (r6 == 0)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Bus-powered max power=0"));
		}
		//loc_7d8
		res = -1;
		//->loc_7ec
	}
	else
	{
		//loc_7e0
		pHubDevice->bLocalPower = pHubDevice->bSelfPower = 0;
	}

	return res;
}


uint32_t  MGC_HubFillAndSubmitIntrIrp (MUSB_Hub *pHubDevice/*r4*/, uint8_t bBusAddress/*sl*/)
{
#if 0
	console_send_string("MGC_HubFillAndSubmitIntrIrp (todo.c): TODO\r\n");
#endif

	MUSB_DeviceEndpoint sp8;
	int r8 = 0xffff;
	int sb = -1;
	const MUSB_EndpointDescriptor* r5 = pHubDevice->pEndpDscr;
	MUSB_Irp* r6 = &pHubDevice->intrInIrp;

	if (MGC_bDiagLevel >= 3)
	{
		MUSB_PrintLine(("MGC_HubFillAndSubmitIntrIrp() >>"));
	}
	//loc_1c0
	int r7 = 1 + (pHubDevice->hubDscr.bNbrPorts >> 3);

	if ((r5->bmAttributes & 3) == 3)
	{
		if ((r5->bEndpointAddress & 0x80) == 0x80)
		{
			MUSB_MemCopy(&sp8.UsbDescriptor, r5, 7);

			sp8.wNakLimit = r8;
			sp8.pDevice = pHubDevice->pUsbDevice;

			pHubDevice->intrInPipe = MUSB_OpenPipe(pHubDevice->busHandle, &sp8, NULL);

			if (pHubDevice->intrInPipe == 0)
			{
				MGC_DiagMsg[0] = 0;
				MUSB_StringConcat(MGC_DiagMsg, 0x100, "HubError: OpenPipe status=");
				MUSB_Stringize(MGC_DiagNumber, 0x21, sb, 16, 0);
				MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine((MGC_DiagMsg));
				}
				//loc_288
				return sb;
			}
			//loc_3b4
			r6->hPipe = pHubDevice->intrInPipe;
			r6->bAllowShortTransfer = 0;
			r6->pBuffer = pHubDevice->aHubPortStatusChangeBuffer;
			r6->dwLength = r7;
			r6->pfIrpComplete = MGC_HubIntrIrpCallback;
			r6->pCompleteParam = pHubDevice;

			if (MGC_bDiagLevel >= 2)
			{
				MUSB_PrintLine(("Hub: Submitting interrupt IRP"));
			}
			//loc_3f4
			sb = MUSB_StartTransfer(r6);

			if (sb != 0)
			{
				MGC_DiagMsg[0] = 0;
				MUSB_StringConcat(MGC_DiagMsg, 0x100, "HubError: MUSB_StartTransfer() status=");
				MUSB_Stringize(MGC_DiagNumber, 0x21, sb, 16, 0);
				MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

				if (MGC_bDiagLevel >= 3)
				{
					MUSB_PrintLine((MGC_DiagMsg));
				}
				//loc_468
				return sb;
			}
			//loc_470
			if (MGC_bDiagLevel >= 3)
			{
				MUSB_PrintLine(("HubSuccess: MGC_HubFillAndSubmitIntrIrp() <<"));
			}
			//loc_488
			sb = 0;
			//->loc_4f4
		} //if ((r5->bEndpointAddress & 0x80) == 0x80)
		else
		{
			//loc_490
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: EndpDirOutButExpectedIn"));
			}
			//loc_4a8
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: MGC_HubFillAndSubmitIntrIrp() <<"));
			}
			//loc_4f4
		}
	} //if ((r5->bmAttributes & 3) == 3)
	else
	{
		//loc_4c4
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: InterruptEndpNotAvailable"));
		}
		//loc_4dc
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: MGC_HubFillAndSubmitIntrIrp() <<"));
		}
		//loc_4f4
	}

	return sb;
}


uint32_t MGC_HubInitAndResetTt(MUSB_Device  *pUsbDevice/*r4*/,
                                      MUSB_pfControlIrpComplete ControlIrpComplete/*r5*/,
                                      MUSB_Hub     *pHubDevice/*r6*/)
{
#if 0
	console_send_string("MGC_HubInitAndResetTt (todo.c): TODO\r\n");
#endif

	int r7 = -1;

	r7 = MUSB_HubResetTt(pUsbDevice, ControlIrpComplete, pHubDevice, 1);

	if (r7 != 1)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("Hub:ErrorResetTT"));
		}
		//loc_840
		return -1;
	}
	//loc_848
	return 0;
}


void MGC_HubInitializeCallback (void *pContext/*r4*/, MUSB_ControlIrp *pControlIrp/*r5*/) //98
{
#if 0
	console_send_string("MGC_HubInitializeCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* pHubDevice/*r6*/ = pContext;
	MUSB_Device* pUsbDevice/*r7*/ = pHubDevice->pUsbDevice;
	//pHubDscr
	//pIrpData
	int dwStatus; //sb
	//bNumOfPort
	//dwBytesForPorts
	//dwNumOfBytes

	if (pControlIrp->dwStatus != 0)
	{
		MGC_DiagMsg[0] = 0;
		MUSB_StringConcat(MGC_DiagMsg, 0x100, "HubError: MGC_HubInitializeCallback(): pControlIrp->dwStatus=");
		MUSB_Stringize(MGC_DiagNumber, 0x21, pControlIrp->dwStatus, 16, 0);
		MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine((MGC_DiagMsg));
		}
		//loc_bb4
		MGC_DiagMsg[0] = 0;
		MUSB_StringConcat(MGC_DiagMsg, 0x100, "HubState: ");
		MUSB_Stringize(MGC_DiagNumber, 0x21, pHubDevice->bState, 16, 0);
		MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine((MGC_DiagMsg));
		}
		//loc_c14
		return;
	}
	//loc_c18
	pUsbDevice->pCurrentConfiguration = pUsbDevice->apConfigDescriptors[0];
	pHubDevice->bState = pHubDevice->bNextState;
#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_HubInitializeCallback: pHubDevice->bState=%d\r\n",
				pHubDevice->bState);
		console_send_string(debug_string);
	}
#endif
	switch (pHubDevice->bState)
	{
	case MUSB_HUB_STATE_CONFIGURED: //2:
		//C5C
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("Hub: MUSB_HUB_STATE_CONFIGURED"));
		}
		//c78
		pHubDevice->bActivePortNum = 1;
		dwStatus = MGC_HubValidateUpdateTtInfo(pHubDevice);
		if (dwStatus != 0)
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Descriptor parsing failed"));
			}
			//cac
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
			//cc8 -> loc_c14
			return;
		}
		//ccc
		int r0;
		if (pHubDevice->bTtType == 2)
		{
			r0 = 1;
			//->ce4
		}
		else
		{
			//ce0
			r0 = 0;
		}
		//ce4
		if (r0 & 1)
		{
			pHubDevice->bNextState = 4;

			MGC_HubInitializeCallback(pContext, pControlIrp);
			//d88 -> loc_174c
		}
		else
		{
			//d04
			if (pHubDevice->bTtType == 0)
			{
				r0 = 1;
				//->d1c
			}
			else
			{
				//d18
				r0 = 0;
			}
			//d1c
			if (r0 & 1)
			{
				//d24
				if (MGC_bDiagLevel >= 2)
				{
					MUSB_PrintLine(("Hub: SingleTT"));
				}
				//d3c
				pHubDevice->bNextState = 4;
				//->d50
			}
			else
			{
				//d48
				pHubDevice->bNextState = 3;
			}
			//d50
			dwStatus = MGC_HubInitAndResetTt(pUsbDevice, MGC_HubInitializeCallback, pHubDevice);
			if (dwStatus != 0)
			{
				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine(("HubError: InitAndResetTt() failed: please disconnect hub"));
				}
				//d84 -> loc_c14
			}
			//d88 -> loc_174c
		}

		break;

	case MUSB_HUB_STATE_RESET_TT: //3:
		//D8C
		{
			extern char debug_string[];
			sprintf(debug_string, "MGC_HubInitializeCallback: pHubDevice->bState=%d (MUSB_HUB_STATE_RESET_TT), TODO!!!\r\n",
					pHubDevice->bState);
			console_send_string(debug_string);
		}
		break;

	case MUSB_HUB_STATE_RESET_TT_COMPLETE: //4:
		//f44
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("Hub: MUSB_HUB_STATE_RESET_TT_COMPLETE"));
		}
		//f60
		MUSB_HubPrepareSetupPkt(&pHubDevice->setupPacket,
				(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_DEVICE)/*0xa0*/,
				6, 0x2900, 0, 15);

		pControlIrp->pInBuffer = pHubDevice->aHubRawDscr;
		pControlIrp->dwInLength = 15;

		pHubDevice->bNextState = 5;

		dwStatus = MUSB_StartControlTransfer(pUsbDevice->pPort, pControlIrp);

		if (dwStatus != 0)
		{
			//fb0
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("\x1C\x1E"));
			}
			//fc8
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
			//fe4
		}
		//fe4 -> loc_174c
		break;

	case MUSB_HUB_STATE_GOT_HUB_DSCR: //5:
		//fe8
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("Hub: MUSB_HUB_STATE_GOT_HUB_DSCR"));
		}
		//1004
		MUSB_HubDescriptor* r8 = &pHubDevice->hubDscr;
		uint8_t* sp_0xc = pControlIrp->pInBuffer;
		int sl = sp_0xc[2];
		int fp = 1 + (sl >> 3);
		int sp8 = fp + 7;
		MUSB_MemCopy(r8, sp_0xc, sp8);
		sp_0xc += 7;
		MUSB_MemCopy(r8 + 7, sp_0xc, fp);
		sp_0xc += 1;
		MUSB_MemCopy(r8 + 9, sp_0xc, fp);

		pHubDevice->hubDscr.wHubCharacteristics =
				pControlIrp->pInBuffer[3] | (pControlIrp->pInBuffer[4] << 8);

		if (((pHubDevice->hubDscr.wHubCharacteristics & MUSB_M_HUB_CHARACTERISTICS_COMPOUND) ==
				MUSB_M_HUB_CHARACTERISTICS_COMPOUND) &&
				(pHubDevice->bDepthInHubChain == 6))
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("Compound device; cannot handle at this level"));
			}
			//10cc
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
			//10e8
			pHubDevice->bNextState = MUSB_HUB_STATE_DISCONNECT_DEVICE; //0x10
			//->loc_c14
			return;
		}
		//10f4
		if (sl != 0)
		{
			MGC_DiagMsg[0] = 0;
			MUSB_StringConcat(MGC_DiagMsg, 0x100, "NumberOfPorts = ");
			MUSB_Stringize(MGC_DiagNumber, 0x21, sl, 10, 0);
			MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

			if (MGC_bDiagLevel >= 2)
			{
				MUSB_PrintLine((MGC_DiagMsg));
			}
			//115c
			pHubDevice->bActivePortNum = 1;

			if ((pHubDevice->hubDscr.wHubCharacteristics & MUSB_M_HUB_CHARACTERISTICS_POWER) ==
					MUSB_HUB_CHARACTERISTICS_POWER_GANGED)
			{
				if (MGC_bDiagLevel >= 2)
				{
					MUSB_PrintLine(("HubGangedPowerSwitched"));
				}
				//1190
				pHubDevice->bNextState = MUSB_HUB_STATE_POWER_ON_COMPLETE; //7;
				//->11d4
			}
			else
			{
				//119c
				if (MGC_bDiagLevel >= 2)
				{
					MUSB_PrintLine(("HubPerPortPowerSwitched"));
				}
				//11b4
				if (pHubDevice->hubDscr.bNbrPorts == 1)
				{
					pHubDevice->bNextState = MUSB_HUB_STATE_POWER_ON_COMPLETE; //7;
					//->11d4
				}
				else
				{
					//11cc
					pHubDevice->bNextState = MUSB_HUB_STATE_POWER_ON; //6
				}
			}
			//11d4
#if 0
			dwStatus = MUSB_HubFeatureRequest(pHubDevice, MUSB_HUB_PORT_POWER_FEATURE/*8*/,
					MGC_HubInitializeCallback,
					pHubDevice->bActivePortNum,
					(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
					MUSB_REQ_SET_FEATURE/*3*/);
#else
			dwStatus = MUSB_HubSetPortFeature(pHubDevice, MUSB_HUB_PORT_POWER_FEATURE,
					MGC_HubInitializeCallback, pHubDevice->bActivePortNum);
#endif
			if (dwStatus == 0)
			{
				if (pHubDevice->bNextState == MUSB_HUB_STATE_POWER_ON_COMPLETE) //7)
				{
					if (MGC_bDiagLevel >= 2)
					{
						MUSB_PrintLine(("bNextSatate POWER_ON_COMPLETE"));
					}
					//1224
					pHubDevice->bActivePortNum = 0;
					//->1370
				}
				//1370
			}
			else
			{
				//1318
				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine(("HubError: While Calling MUSB_HubSetPortFeature()"));
				}
				//1330
				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine(("HubError: Please disconnect hub"));
				}
				//1370
			}
		} //if (sl != 0)
		else
		{
			//1350
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: 0 ports; please disconnect the hub"));
			}
			//1368
			pHubDevice->bNextState = MUSB_HUB_STATE_DISCONNECT_DEVICE; //0x10
		}
		//1370 ->loc_c14
		break;

	case MUSB_HUB_STATE_POWER_ON: //6:
		//1374
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("Hub: MUSB_HUB_STATE_POWER_ON"));
		}
		//1390
		pHubDevice->bActivePortNum++;
		if (pHubDevice->bActivePortNum < pHubDevice->hubDscr.bNbrPorts)
		{
			pHubDevice->bNextState = MUSB_HUB_STATE_POWER_ON;
		}
		else
		{
			//13b8
			pHubDevice->bNextState = MUSB_HUB_STATE_POWER_ON_COMPLETE;
		}
		//13c0
#if 0
		dwStatus = MUSB_HubFeatureRequest(pHubDevice, MUSB_HUB_PORT_POWER_FEATURE,
				MGC_HubInitializeCallback,
				pHubDevice->bActivePortNum,
				(MUSB_TYPE_CLASS | MUSB_RECIP_OTHER)/*0x23*/,
				MUSB_REQ_SET_FEATURE/*3*/);
#else
		dwStatus = MUSB_HubSetPortFeature(pHubDevice, MUSB_HUB_PORT_POWER_FEATURE,
				MGC_HubInitializeCallback, pHubDevice->bActivePortNum);
#endif
		if (dwStatus != 0)
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: while calling MUSB_HubSetPortFeature()"));
			}
			//1404
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
		}
		//1420
		break;

	case MUSB_HUB_STATE_POWER_ON_COMPLETE: //7:
		//1424
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("Hub: MUSB_HUB_STATE_POWER_ON_COMPLETE"));
		}
		//1440
		pHubDevice->bNextState = MUSB_HUB_STATE_GOT_HUB_STATUS; //8;

#if 0
		dwStatus = MUSB_HubGetStatus(pHubDevice, MGC_HubInitializeCallback, 0,
				(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_DEVICE)/*0xa0*/);
#else
		dwStatus = MUSB_HubGetHubStatus(pHubDevice, MGC_HubInitializeCallback);
#endif

		if (dwStatus != 0)
		{
			MGC_DiagMsg[0] = 0;
			MUSB_StringConcat(MGC_DiagMsg, 0x100, "HubError: MUSB_HubGetHubStatus() returned ");
			MUSB_Stringize(MGC_DiagNumber, 0x21, dwStatus, 16, 0);
			MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine((MGC_DiagMsg));
			}
			//14c8
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
			//14e4
			pHubDevice->bNextState = MUSB_HUB_STATE_DISCONNECT_DEVICE; //0x10
		}
		//14ec -> loc_174c
		break;

	case MUSB_HUB_STATE_GOT_HUB_STATUS: //8:
		//160c
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("Hub: MUSB_HUB_STATE_GOT_HUB_STATUS"));
		}
		//1628
		MGC_HubStoreStatus(pHubDevice->aHubPortStatus, &pHubDevice->hubStatus);

		dwStatus = MGC_HubPowerValidateInit(pHubDevice);

		if (dwStatus != 0)
		{
			MGC_DiagMsg[0] = 0;
			MUSB_StringConcat(MGC_DiagMsg, 0x100, "HubError: MGC_HubPowerValidateInit() returned ");
			MUSB_Stringize(MGC_DiagNumber, 0x21, dwStatus, 16, 0);
			MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);

			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine((MGC_DiagMsg));
			}
			//16a8
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
			//10e8
			pHubDevice->bNextState = MUSB_HUB_STATE_DISCONNECT_DEVICE; //0x10
			//->loc_c14
			return;
		}
		//16d0
		dwStatus = MGC_HubFillAndSubmitIntrIrp(pHubDevice, pUsbDevice->bBusAddress);

		if (dwStatus != 0)
		{
			if (pHubDevice->intrInPipe != 0)
			{
				dwStatus = MUSB_ClosePipe(pHubDevice->intrInPipe);
			}
			//1700
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: While calling MGC_HubFillAndSubmitIntrIrp()"));
			}
			//1718
			pHubDevice->bNextState = MUSB_HUB_STATE_DISCONNECT_DEVICE; //0x10
			//->loc_c14
			return;
		}
		//1724
		if (MGC_bDiagLevel >= 3)
		{
			MUSB_PrintLine(("HubSuccess: Hub is operating"));
		}
		//173c
		pHubDevice->bNextState = pHubDevice->bState = MUSB_HUB_STATE_INIT; //11;
		break;

	default:
		//loc_174c
		break;
	}

	return;
}


uint32_t MGC_HubConnectHandler(MUSB_Hub* pHubDevice/*r4*/,
							   MUSB_Device* pUsbDevice/*r5*/,
							   uint8_t bBusAddress/*fp*/,
							   MUSB_Port* pPort/*r6*/) //41
{
#if 0
	console_send_string("MGC_HubConnectHandler (todo.c): TODO\r\n");
#endif

	MUSB_ControlIrp* pCtrlIrp = &pHubDevice->ctrlIrp;
	MUSB_DeviceRequest* pSetupPacket = &pHubDevice->setupPacket;
	uint16_t bConfigurationNumber; //sb
	int dwStatus;

	if (MGC_bDiagLevel >= 2)
	{
		MUSB_PrintLine(("MGC_HubConnectHandler() >>"));
	}
	//loc_1788
	bConfigurationNumber = pUsbDevice->apConfigDescriptors[0]->bConfigurationValue;

	MUSB_HubPrepareSetupPkt(pSetupPacket,
			(MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE)/*0*/,
			MUSB_REQ_SET_CONFIGURATION/*9*/,
			bConfigurationNumber, 0, 0);

#if 0
	pCtrlIrp->pOutBuffer = (const uint8_t*) pSetupPacket;
	pCtrlIrp->dwOutLength = 8;
	pCtrlIrp->pInBuffer = NULL;
	pCtrlIrp->dwInLength = 0;
	pCtrlIrp->pfIrpComplete = MGC_HubInitializeCallback;
	pCtrlIrp->pCompleteParam = pHubDevice;
#else
	MUSB_FILL_CTRL_IRP(pCtrlIrp, 0, pSetupPacket, 8, NULL, 0, MGC_HubInitializeCallback, pHubDevice);
#endif
	pCtrlIrp->pDevice = pUsbDevice;

	dwStatus = MUSB_StartControlTransfer(pPort, pCtrlIrp);
	if (dwStatus != 0)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: MGC_HubConnectHandler()"));
		}
		//loc_180c
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Please disconnect hub"));
		}
		//loc_1828
		return -1;
	}
	//loc_1830
	if (MGC_bDiagLevel >= 3)
	{
		MUSB_PrintLine(("HubSuccess: MGC_HubConnectHandler() <<"));
	}

	return 0;
}


void MGC_HubIntrIrpCallback ( void* pCtxt/*r4*/, MUSB_Irp* pCtrlIrp/*r5*/)
{
#if 0
	console_send_string("MGC_HubIntrIrpCallback (todo.c): TODO\r\n");
#endif

	MUSB_Hub* r6 = pCtxt;

	if (MGC_bDiagLevel >= 2)
	{
		MUSB_PrintLine(("MGC_HubIntrIrpCallback() >> "));
	}
	//loc_d4
	if (pCtrlIrp->dwStatus != 0)
	{
		if (r6->bIntrIrpExecutionErrorCount <= 5)
		{
			r6->bIntrIrpExecutionErrorCount++;

			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: pIntrIrp->dwStatus in MGC_HubIntrIrpCallback()"));
			}
			//loc_154
		}
		else
		{
			//loc_114
			r6->bNextState = r6->bState = MUSB_HUB_STATE_DISCONNECT_DEVICE;

			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: MGC_HubIntrIrpCallback () error count more then expected."));
			}
			//loc_138
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: Please disconnect hub"));
			}
			//loc_154
		}
	} //if (pCtrlIrp->dwStatus != 0)
	else
	{
		//loc_158
		r6->bIntrIrpExecutionErrorCount = 0;

		MGC_HubIntrIrpHandler(r6, pCtrlIrp);

		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine(("MGC_HubIntrIrpCallback() <<"));
		}
		//loc_184 -> loc_154
	}
}


void MGC_HubIntrIrpHandler(MUSB_Hub* pHubDevice/*r5*/, MUSB_Irp* pIntrIrp/*r4*/)
{
#if 0
	console_send_string("MGC_HubIntrIrpHandler (todo.c): TODO\r\n");
#endif

	if (MGC_bDiagLevel >= 2)
	{
		MUSB_PrintLine(("MGC_HubIntrIrpHandler() >>"));
	}
	//loc_24
	MUSB_BitsGet(pIntrIrp->pBuffer, 0, 32, &pHubDevice->dwHubPortStatusChange);

	if ((pHubDevice->dwHubPortStatusChange & 1) &&
			(pHubDevice->bNextState != MUSB_HUB_STATE_GOT_HUB_STATUS))
	{
		if (MGC_bDiagLevel >= 2)
		{
			MUSB_PrintLine((" Hub's Status Changed"));
		}
		//loc_68
		MUSB_HubStatusChange(pHubDevice);
		//->loc_90
	}
	//loc_74
	else if (pHubDevice->bPortNextState == 0)
	{
		pHubDevice->bActivePortNum = 1;

		MGC_HubPortStatusChange(pHubDevice);
	}
	//loc_90
	if (MGC_bDiagLevel >= 2)
	{
		MUSB_PrintLine(("MGC_HubIntrIrpHandler() <<"));
	}
	//loc_a8
}


void MUSB_HubStatusChange (MUSB_Hub *pHubDevice)
{
	console_send_string("MUSB_HubStatusChange (todo.c): TODO\r\n");

}


uint32_t MGC_HubUpdatePower(MUSB_Hub *pHubDevice/*r4*/, uint8_t bPowerUpdateType/*r5*/)
{
#if 0
	console_send_string("MGC_HubUpdatePower (todo.c): TODO\r\n");
#endif

	int r6 = 0;

	if ((pHubDevice->bSelfPower != 1) &&
			(pHubDevice->bLocalPower != 1))
	{
		switch (bPowerUpdateType)
		{
		case 0:
			//loc_1ac8
			if (pHubDevice->bAllocatedPower < 5)
			{
				pHubDevice->bAllocatedPower++;
				//->loc_1b04
			}
			else
			{
				//loc_1ae8
				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine(("Hub:WarCannotSupplyLoad"));
				}
				//loc_1b00
				r6 = -1;
			}
			//loc_1b04 -> loc_1b5c
			break;

		case 1:
			//loc_1b08
			if (pHubDevice->bAllocatedPower != 0)
			{
				pHubDevice->bAllocatedPower--;
			}
			//loc_1b24 -> loc_1b5c
			break;

		case 2:
			//loc_1b28 -> loc_1b5c
			break;

		case 3:
			//1ac4 -> loc_1b30 -> loc_1b5c
			break;

		default:
			//loc_1b38
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("Hub:ErrorUnknownPowerUpdateType"));
			}
			//loc_1b54
			r6 = -1;
			break;
		}
	}
	//loc_1b60
	return r6;
}






