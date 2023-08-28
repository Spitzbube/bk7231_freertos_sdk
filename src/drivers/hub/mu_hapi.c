
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


#define PRINT_MSG(...) 								\
		{ 											\
			extern char debug_string[]; 			\
			sprintf(debug_string, ##__VA_ARGS__); 	\
			console_send_string(debug_string); 		\
			console_send_string("\r\n"); 			\
		}
#define MUSB_PrintLine(x) PRINT_MSG x

uint8_t MGC_bDiagLevel = 2;
uint8_t MGC_DiagMsg[0x100];
uint8_t MGC_DiagNumber[0x22]; //size???


static uint32_t aTimerResolution[] =
{
		10, 10, 10, 10, 10, 10
};

static uint8_t MGC_HubConnect(void *pPrivateData,
        MUSB_BusHandle     hBus,
        MUSB_Device       *pUsbDevice,
        const uint8_t     *pPeripheralList);
static void MGC_HubDisconnect();

static MUSB_HubDriverContext hubDriverContext;

MUSB_DeviceDriver MGC_HubClassDriver =
{
		&hubDriverContext,
		6,
		aTimerResolution,
		MGC_HubConnect,
		MGC_HubDisconnect,
		NULL, //pfBusSuspended
		NULL, //pfBusResumed
};

static uint8_t bInitialized = 0;


void MGC_HubDriverInit() //116
{
#if 0
	console_send_string("MGC_HubDriverInit (todo.c): TODO\r\n");
#endif

	hubDriverContext.bNumOfHubActive = 0;
	hubDriverContext.pDriver = NULL;

	for (uint8_t bIndex = 0; bIndex < MUSB_HUB_MAX_HUB_SUPPORTED; bIndex++)
	{
		hubDriverContext.hubDeviceList[bIndex].bState = 0;
	}

	MGC_HubClassDriver.pPrivateData = &hubDriverContext;

	bInitialized = 1;
}


static uint8_t MGC_HubConnect(void* pPrivateData/*r4*/,
        MUSB_BusHandle     hBus/*r5*/,
        MUSB_Device       *pUsbDevice/*r6*/,
        const uint8_t     *pPeripheralList) //147
{
#if 0
	console_send_string("MGC_HubConnect (todo.c): TODO\r\n");
#endif

	MUSB_Hub* pHubDevice; //r7;
	MUSB_ConfigurationDescriptor* pCfgDscr; //sp_0x28;
	const MUSB_InterfaceDescriptor* pInfDscr; //r8;
	const MUSB_EndpointDescriptor* pEndpDscr; //sb;
	MUSB_HubDriverContext* pDriverContext; //sp_0x24;
	//pHubParent
	MUSB_Device* pWorkDevice; //sp_0x1c;
	int dwStatus; //sl
	uint8_t bHubIndex; //sp_0x18;
	//bIndex
	uint8_t bEndIndex; //sp_0x14
	uint8_t bDepthInHubChain = 1; //sp_0x10
	uint8_t bEndpDir = MUSB_DIR_IN; //sp_0xc
	uint8_t bEndpType = MUSB_ENDPOINT_XFER_INT; //sp8

	if (bInitialized == 0)
	{
		MGC_HubDriverInit();
	}
	//loc_13c
#if 0
	MUSB_DIAG_STRING(2, ("MUSB_HubConnect() >> "));
#else
	if (MGC_bDiagLevel >= 2)
	{
		MUSB_PrintLine(("MUSB_HubConnect() >> "));
	}
#endif
	//loc_154
	pWorkDevice = pUsbDevice;
	//->loc_178
	while (pWorkDevice->pParentUsbDevice != 0)
	{
		//loc_15c
		pWorkDevice = pWorkDevice->pParentUsbDevice;
		bDepthInHubChain++;
	}
	//0x00000188
	MGC_DiagMsg[0] = 0;
	MUSB_StringConcat(MGC_DiagMsg, 0x100, "Depth of connected hub: ");
	MUSB_Stringize(MGC_DiagNumber, 0x21, bDepthInHubChain, 16, 0);
	MUSB_StringConcat(MGC_DiagMsg, 0x100, MGC_DiagNumber);
	if (MGC_bDiagLevel >= 2)
	{
		MUSB_PrintLine((MGC_DiagMsg));
	}
	//loc_1e8
	if (bDepthInHubChain > 6)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Hub tree is too deep"));
		}
		//loc_20c
		return 0;
	}
	//loc_218
	pDriverContext = (MUSB_HubDriverContext*) pPrivateData;

	dwStatus = MGC_HubFindFreeHubDeviceIndex(pDriverContext, &bHubIndex);
	if (dwStatus == -1)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Free pHubDevice not available"));
		}
		//loc_24c
		return 0;
	}
	//loc_254
	pDriverContext->bNumOfHubActive++;

	pHubDevice = &pDriverContext->hubDeviceList[bHubIndex];
	pHubDevice->bIndexAtHubDeviceList = bHubIndex;

	if (pUsbDevice->pParentUsbDevice != NULL)
	{
		MUSB_Hub* sp_0x20;
		dwStatus = MGC_HubFindHubDevice(pUsbDevice->pParentUsbDevice, &sp_0x20);
		if (dwStatus != 0)
		{
			if (MGC_bDiagLevel >= 1)
			{
				MUSB_PrintLine(("HubError: This Is Not A Hub Device/NULL"));
			}
			//loc_2c8
			return 0;
		}
		//loc_2d0
		if (sp_0x20->bSelfPower/*TODO!!!*/ == 0)
		{
			if ((pUsbDevice->apConfigDescriptors[0]->bmAttributes & 0x40) == 0)
			{
				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine(("HubError: Bus-powered hub cannot drive another bus-powered hub"));
				}
				//loc_30c
				if (MGC_bDiagLevel >= 1)
				{
					MUSB_PrintLine(("HubError: Please disconnect hub"));
				}
				//loc_328
				return 0;
			}
			//loc_330
		}
		//loc_330
	}
	//loc_330
	pCfgDscr = pUsbDevice->apConfigDescriptors[0];

	pInfDscr = MUSB_FindInterfaceDescriptor(pCfgDscr, 0, 0);

	if ((pInfDscr == 0) || (pInfDscr->bNumEndpoints == 0))
	{
		//loc_364
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: no endpoints; please disconnect hub"));
		}
		//loc_37c
		return 0;
	}
	//loc_49c
	//->loc_4fc: Search for the Interrupt-In endpoint
	for (bEndIndex = 0; bEndIndex < pInfDscr->bNumEndpoints; bEndIndex++)
	{
		//loc_4a8
		pEndpDscr = MUSB_FindEndpointDescriptor(pCfgDscr, pInfDscr, bEndIndex);
		if (((pEndpDscr->bEndpointAddress & MUSB_ENDPOINT_DIR_MASK) == bEndpDir) &&
				((pEndpDscr->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK) == bEndpType))
		{
			//->loc_50c
			break;
		}
		//loc_4e8
		pEndpDscr = NULL;
	}
	//loc_50c
	if (pEndpDscr == NULL)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Interrupt-In endpoint is not available; please disconnect hub"));
		}
		//loc_530
		return 0;
	}
	//loc_538
	pHubDevice->pInfDscr = pInfDscr;
	pHubDevice->pEndpDscr = pEndpDscr;

	if (bHubIndex == 0)
	{
		if (MGC_bDiagLevel >= 3)
		{
			MUSB_PrintLine(("Root hub is connected"));
		}
		//loc_564
		pUsbDevice->pParentUsbDevice = NULL;
	}
	//loc_56c
	pUsbDevice->pDriverPrivateData = pPrivateData;
	pHubDevice->busHandle = hBus;
	pHubDevice->pUsbDevice = pUsbDevice;
	pHubDevice->bDepthInHubChain = bDepthInHubChain;
	pHubDevice->bState = MUSB_HUB_STATE_CONNECT; //1;
	pHubDevice->bNextState = MUSB_HUB_STATE_CONFIGURED; //2;
	pHubDevice->bPortState = 0;
	pHubDevice->bPortNextState = 0;
	pHubDevice->bActivePortNum = 0;
	pHubDevice->bIntrIrpExecutionErrorCount = 0;

	pHubDevice->debounceParam.dwWaitingTime = 100;
	pHubDevice->debounceParam.bErrorCount = 0;

	pHubDevice->resetParam.dwWaitingTime = 10;
	pHubDevice->resetParam.dwNumOfTry = 0;
	//->loc_5e0
	for (uint8_t fp = 0; fp < MUSB_HUB_MAX_PORTS; fp++)
	{
		//loc_5cc
		pHubDevice->pChild[fp] = 0;
	}
	//0x000005e8
	dwStatus = MGC_HubConnectHandler(pHubDevice, pUsbDevice, pUsbDevice->bBusAddress, pUsbDevice->pPort);
	if (dwStatus == -1)
	{
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: MUSB_HubConnect() <<"));
		}
		//loc_620
		if (MGC_bDiagLevel >= 1)
		{
			MUSB_PrintLine(("HubError: Please disconnect hub"));
		}
		//loc_63c
		return 0;
	}
	//loc_644
	if (MGC_bDiagLevel >= 3)
	{
		MUSB_PrintLine(("HubSuccess: MUSB_HubConnect() <<"));
	}
	//loc_65c
	return 1;
}


static void MGC_HubDisconnect()
{
	console_send_string("MGC_HubDisconnect (todo.c): TODO\r\n");

}


