

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



uint32_t MUSB_HubGetStatus (MUSB_Hub *pHubDevice/*r4*/,
                                   MUSB_pfControlIrpComplete complete/*r5*/,
                                   uint8_t bPortNum/*r6*/, uint8_t bmRequest/*r7*/) //35
{
#if 0
	console_send_string("MUSB_HubGetStatus (todo.c): TODO\r\n");
#endif

	MUSB_ControlIrp* pCtrlIrp = &pHubDevice->ctrlIrp;
	int dwStatus = -1;
	int wLength = 4;

	if (MGC_bDiagLevel >= 3)
	{
		MUSB_PrintLine(("MUSB_HubGetStatus() >>"));
	}
	//loc_38
	MUSB_HubPrepareSetupPkt(&pHubDevice->setupPacket,
			bmRequest, 0/*bRequest*/, 0/*wFeatureSelector*/, bPortNum, wLength);

	pCtrlIrp->pInBuffer = pHubDevice->aHubPortStatus;
	pCtrlIrp->dwInLength = MUSB_HUB_PORT_STATUS_LENGTH; //4;
	pCtrlIrp->dwOutLength = 8;
	pCtrlIrp->pfIrpComplete = complete;

	dwStatus = MUSB_StartControlTransfer(pHubDevice->pUsbDevice->pPort, pCtrlIrp);

	if (dwStatus)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: MUSB_HubGetStatus() <<"));
		}
		//loc_a0
		return -1;
	}
	//loc_a8
	if (MGC_bDiagLevel >= 3) //1)
	{
		MUSB_PrintLine(("HubSuccess: MUSB_HubGetStatus() <<"));
	}
	//loc_c0
	return 0;
}


uint32_t MUSB_HubFeatureRequest (MUSB_Hub *pHubDevice/*r4*/,
                                        uint16_t wFeatureSelector/*r5*/,
                                        MUSB_pfControlIrpComplete complete/*r6*/,
                                        uint8_t bPortNum/*r7*/,
                                        uint8_t bmRequest/*r8*/,
                                        uint8_t bRequest/*sb*/) //76
{
#if 0
	console_send_string("MUSB_HubFeatureRequest (todo.c): TODO\r\n");
#endif

	MUSB_ControlIrp* pCtrlIrp = &pHubDevice->ctrlIrp;
	int wLength = 0;
	int dwStatus;

	if (MGC_bDiagLevel >= 3)
	{
		MUSB_PrintLine(("MUSB_HubFeatureRequest() >>"));
	}
	//loc_104
	MUSB_HubPrepareSetupPkt(&pHubDevice->setupPacket,
			bmRequest, bRequest, wFeatureSelector, bPortNum, wLength);

	pCtrlIrp->pInBuffer = NULL;
	pCtrlIrp->dwInLength = 0;
	pCtrlIrp->dwOutLength = 8;
	pCtrlIrp->pfIrpComplete = complete;

	dwStatus = MUSB_StartControlTransfer(pHubDevice->pUsbDevice->pPort, pCtrlIrp);

	if (dwStatus != 0)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: MUSB_HubFeatureRequest() <<"));
		}
		//loc_170
		return -1;
	}
	//loc_178
	if (MGC_bDiagLevel >= 3)
	{
		MUSB_PrintLine(("HubSuccess: MUSB_HubFeatureRequest(): <<"));
	}
	//loc_190
	return 0;
}








