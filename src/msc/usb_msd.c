/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/
//#include "include.h"

#pragma thumb

#define MUSB_PRT

#include "mu_cdi.h"
#include "mu_mem.h"
//#include "mu_impl.h"
#include "mu_stdio.h"
#include "mu_strng.h"
#include "mu_hfi.h"
#include "mu_spi.h"
#include "class/mu_msd.h"
//#include "mu_mapi.h"
//#include "mu_none.h"
#include "usb_msd.h"
//#include "rtos_pub.h"
#include "ucos_ii.h"

typedef void (*FUNCPTR)();

void sub_2343a17a (const uint16_t* r0);


#if 1 //CFG_SUPPORT_MSD
extern MUSB_FunctionClient MGC_xxxFunctionClient;
static void MGC_MsdNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State);
static void MGC_MsdOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus);

/**************************** GLOBALS *****************************/
static MUSB_Port *MGC_pCdiPort = NULL;
static MUSB_BusHandle MGC_hCdiBus = NULL;
static uint8_t MGC_bDesireHostRole = TRUE;
static uint8_t MGC_aMsdPeripheralList[256];
static MUSB_DeviceDriver MGC_aDeviceDriver[2];

#if 0

static MUSB_HostClient MGC_MsdHostClient =
{
    MGC_aMsdPeripheralList,		/* peripheral list */
    0,			    /* filled in main */
    MGC_aDeviceDriver,
    0					/* filled in main */
};

static MUSB_OtgClient MGC_MsdOtgClient =
{
    NULL,	/* no instance data; we are singleton */
    &MGC_bDesireHostRole,
    MGC_MsdNewOtgState,
    MGC_MsdOtgError
};

#endif

#if 0
static MUSB_HfiDevice *MGC_pHfiDevice = NULL;
static const MUSB_HfiMediumInfo *MGC_pHfiMediumInfo = NULL;
static uint8_t MediaIsOk = FALSE;
FUNCPTR trx_callback_ptr = NULL;
#endif

typedef struct
{
	MUSB_HfiVolumeHandle Data_0; //0
	const MUSB_HfiDeviceInfo* Data_4; //4
	MUSB_HfiDevice* Data_8; //8
	void* Data_0xc; //12
	void* Data_0x10; //16
	uint16_t wData_0x14; //20
	//0x18 = 24
} Struct_235faa2c;

Struct_235faa2c Data_235faa2c[4]; //235faa2c

//23492c60
void (*Data_23492c60)() = 0; //23492c60 +0



#if 0

#ifdef CFG_ENABLE_SYC_OP
beken_semaphore_t msd_rd_sema = NULL;
beken_semaphore_t msd_wr_sema = NULL;
#endif

/*************************** FUNCTIONS ****************************/
/* OTG client */
static void MGC_MsdNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State)
{
    char aAnswer[4];

    switch(State)
    {
    case MUSB_AB_IDLE:
        MUSB_PrintLine("S - Start Session");
        MUSB_GetLine(aAnswer, 4);
        if(('s' == aAnswer[0]) || ('S' == aAnswer[0]))
        {
            MUSB_RequestBus(MGC_hCdiBus);
        }
        break;

    case MUSB_A_SUSPEND:
        MUSB_PrintLine("R - Resume bus");
        MUSB_GetLine(aAnswer, 4);
        if(('r' == aAnswer[0]) || ('R' == aAnswer[0]))
        {
            MUSB_ResumeBus(MGC_hCdiBus);
        }
        break;

    default:
        break;
    }
}

static void MGC_MsdOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus)
{
    switch(dwStatus)
    {
    case MUSB_STATUS_UNSUPPORTED_DEVICE:
        MUSB_PRT("Device not supported\r\n");
        break;

    case MUSB_STATUS_UNSUPPORTED_HUB:
        MUSB_PRT("Hubs not supported\r\n");
        break;

    case MUSB_STATUS_OTG_VBUS_INVALID:
        MUSB_PRT("Vbus error\r\n");
        break;

    case MUSB_STATUS_OTG_NO_RESPONSE:
        MUSB_PRT("Device not responding\r\n");
        break;

    case MUSB_STATUS_OTG_SRP_FAILED:
        MUSB_PRT("Device not responding (SRP failed)\r\n");
        break;

    default:
        break;
    }
}

#endif


/* 23439e84 - todo */
void sub_23439e84(int a, int b)
{
	Struct_235faa2c* r4 = 0;

	for (uint8_t i = 0; i < 4; i++)
	{
		//loc_23439e8c
		if (Data_235faa2c[i].Data_0 == 0)
		{
			r4 = &Data_235faa2c[i];
			//->loc_23439ea6
			break;
		}
		//loc_23439e9e
	}
	//loc_23439ea6
	if (r4 != 0)
	{
		r4->wData_0x14 = b;

		OSSemPost(r4->Data_0x10);
	}
	//loc_23439eb2
}


/* 23439eb4 - complete */
MUSB_HfiStatus MUSB_HfiAddDevice(MUSB_HfiVolumeHandle *phVolume/*r6*/,
                                 const MUSB_HfiDeviceInfo *pInfo/*r5*/,
                                 MUSB_HfiDevice *pDevice/*r7*/)
{
	uint32_t i;

#if 0
    MGC_pHfiDevice = pDevice;
    MediaIsOk = TRUE;
#endif

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MUSB_HfiAddDevice\r\n");
		console_send_string(debug_string);
	}
#endif

	sub_2343a17a(pInfo->awDiskVendor);
	sub_2343a17a(pInfo->awSerialNumber);
	sub_2343a17a(pInfo->awDiskProduct);

	for (i = 0; i < 4; i++)
	{
		//loc_23439ed8
		if (Data_235faa2c[i].Data_0 == 0)
		{
			Data_235faa2c[i].Data_0 = *phVolume;
			Data_235faa2c[i].Data_8 = pDevice;
			Data_235faa2c[i].Data_4 = pInfo;
			Data_235faa2c[i].Data_0xc = OSSemCreate(1);
			Data_235faa2c[i].Data_0x10 = OSSemCreate(0);
			//->loc_23439f08
			break;
		}
		//loc_23439f02
	}
	//loc_23439f08
	if (Data_23492c60 != 0)
	{
		(Data_23492c60)(1, i);
	}

    return MUSB_HFI_SUCCESS;

}


/* 23439f66 - todo */
void MUSB_HfiMediumInserted(MUSB_HfiVolumeHandle 	 hVolume,
                            const MUSB_HfiMediumInfo *pMediumInfo)
{
#if 0
    MGC_pHfiMediumInfo = pMediumInfo;
#endif

#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MUSB_HfiMediumInserted\r\n");
		console_send_string(debug_string);
	}
#endif
}

/* 23439f68 - todo */
void MUSB_HfiMediumRemoved(MUSB_HfiVolumeHandle hVolume)
{
#if 0
    MGC_pHfiMediumInfo = NULL;
#endif

}


/* 23439f1a - todo */
void MUSB_HfiDeviceRemoved(MUSB_HfiVolumeHandle hVolume)
{
	uint8_t err;

	for (uint32_t i = 0; i < 4; i++)
	{
		//loc_23439f20
		if (Data_235faa2c[i].Data_0 == hVolume)
		{
			Data_235faa2c[i].Data_0 = 0;
			Data_235faa2c[i].Data_8 = 0;
			Data_235faa2c[i].Data_0xc = (void*) OSSemDel(Data_235faa2c[i].Data_0xc, 1, &err);
			Data_235faa2c[i].Data_0x10 = (void*) OSSemDel(Data_235faa2c[i].Data_0x10, 1, &err);

			if (Data_23492c60 != 0)
			{
				(Data_23492c60)(1, i);
			}

			break;
		}
		//loc_23439f5e
	}
}


/* 23439f6a - todo */
void sub_23439f6a()
{
#if 1
	console_send_string("sub_23439f6a (todo.c): TODO\r\n");
#endif


}


/* 23439fc4 - todo */
void sub_23439fc4()
{
#if 1
	console_send_string("sub_23439fc4 (todo.c): TODO\r\n");
#endif


}


/* 2343a02e - todo */
void sub_2343a02e()
{
#if 1
	console_send_string("sub_2343a02e (todo.c): TODO\r\n");
#endif


}


/* 2343a11c - todo */
void sub_2343a11c()
{
#if 1
	console_send_string("sub_2343a11c (todo.c): TODO\r\n");
#endif


}


/* 2343a136 - todo */
void sub_2343a136()
{
#if 1
	console_send_string("sub_2343a136 (todo.c): TODO\r\n");
#endif


}


#if 0

uint8_t MGC_MsdGetMediumstatus(void)
{
    return MediaIsOk;
}

#endif

void MGC_StartupAppEvent(void)
{
#if 1
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_StartupAppEvent\r\n");
		console_send_string(debug_string);
	}
#endif

#if 0
    if(trx_callback_ptr)
    {
        (*trx_callback_ptr)();
    }
#endif
}

#if 0

void MGC_RegisterCBTransferComplete(FUNCPTR func)
{
    trx_callback_ptr = func;
}

uint32_t MGC_MsdTransferComplete(MUSB_HfiVolumeHandle hVolume,
                                 uint16_t wActualBlocks)

{
    if(trx_callback_ptr)
    {
        (*trx_callback_ptr)();
    }
    MUSB_PRT("MGC_MsdTransferComplete:0x%x\r\n", trx_callback_ptr);

    return 2;
}

uint32_t get_HfiMedium_size(void)
{
    if(MGC_pHfiMediumInfo)
        return MGC_pHfiMediumInfo->dwBlockCountLo;
    else
        return 0;
}
uint32_t get_HfiMedium_blksize(void)
{
    if(MGC_pHfiMediumInfo)
        return MGC_pHfiMediumInfo->dwBlockSize;
    else
        return 0;
}

uint32_t MUSB_HfiRead( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice && MGC_pHfiDevice->pfReadDevice)
    {
        RetValue = MGC_pHfiDevice->pfReadDevice(MGC_pHfiDevice->pPrivateData,
                                                first_block, 0, block_num, dest,
                                                MGC_MsdTransferComplete, TRUE);
    }

    return RetValue;
}

uint32_t MUSB_HfiWrite( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice)
    {
        MUSB_PRT("pfWriteDevice0\r\n");
        RetValue = MGC_pHfiDevice->pfWriteDevice(MGC_pHfiDevice->pPrivateData,
                   first_block, 0, block_num, dest,
                   0, MGC_MsdTransferComplete, FALSE);
        MUSB_PRT("pfWriteDevice1\r\n");
    }

    return RetValue;
}

#ifdef CFG_ENABLE_SYC_OP
uint32_t MGC_MsdReadSyncInit(void)
{
    OSStatus ret = kNoErr;

    if(NULL == msd_rd_sema)
    {
        ret = rtos_init_semaphore(&msd_rd_sema, 1);
        if (kNoErr != ret)
        {
            MUSB_PRT("MGC_MsdReadSyncInit failed\r\n");
        }
    }

    return ret;
}

uint32_t MGC_MsdWriteSyncInit(void)
{
    OSStatus ret = kNoErr;

    if(NULL == msd_wr_sema)
    {
        ret = rtos_init_semaphore(&msd_wr_sema, 1);
        if (kNoErr != ret)
        {
            MUSB_PRT("MGC_MsdReadSyncInit failed\r\n");
        }
    }

    return ret;
}

OSStatus MGC_MsdReadSyncWaiting(uint32_t timeout)
{
    return rtos_get_semaphore(&msd_rd_sema, timeout);
}

OSStatus MGC_MsdWriteSyncWaiting(uint32_t timeout)
{
    return rtos_get_semaphore(&msd_wr_sema, timeout);
}


uint32_t MGC_MsdReadComplete(MUSB_HfiVolumeHandle hVolume,
                             uint16_t wActualBlocks)

{
    MUSB_PRT("MGC_MsdReadComplete\r\n");
    rtos_set_semaphore(&msd_rd_sema);

    return 0;
}

uint32_t MGC_MsdWriteComplete(MUSB_HfiVolumeHandle hVolume,
                              uint16_t wActualBlocks)

{
    MUSB_PRT("MGC_MsdWriteComplete\r\n");
    rtos_set_semaphore(&msd_wr_sema);

    return 0;
}

uint32_t MUSB_HfiRead_sync( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice && MGC_pHfiDevice->pfReadDevice)
    {
        RetValue = MGC_pHfiDevice->pfReadDevice(MGC_pHfiDevice->pPrivateData,
                                                first_block, 0, block_num, dest,
                                                MGC_MsdReadComplete, TRUE);
        MGC_MsdReadSyncWaiting(BEKEN_WAIT_FOREVER);
    }

    return RetValue;
}

uint32_t MUSB_HfiWrite_sync( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice)
    {
        RetValue = MGC_pHfiDevice->pfWriteDevice(MGC_pHfiDevice->pPrivateData,
                   first_block, 0, block_num, dest,
                   0, MGC_MsdWriteComplete, FALSE);
        MGC_MsdWriteSyncWaiting(BEKEN_WAIT_FOREVER);
    }

    return RetValue;
}
#endif // CFG_ENABLE_SYC_OP

int usb_sw_init(void)
{
    uint8_t *pList;
    uint16_t ret;
    uint8_t bDriver = 0;
    uint16_t wCount = 0;
    uint16_t wSize = 0;
    uint16_t wRemain;
    MUSB_DeviceDriver *pDriver;

#ifdef CFG_ENABLE_SYC_OP
    ret = MGC_MsdReadSyncInit();
    ASSERT(0 == ret);

    ret = MGC_MsdWriteSyncInit();
    ASSERT(0 == ret);
#endif

    wRemain = (uint16_t)sizeof(MGC_aMsdPeripheralList);
    pList = MGC_aMsdPeripheralList;

    wSize = MUSB_FillMsdPeripheralList(bDriver, pList, wRemain);
    if(wSize < wRemain)
    {
        pDriver = MUSB_GetStorageClassDriver();
        if(pDriver)
        {
            MUSB_MemCopy(&(MGC_MsdHostClient.aDeviceDriverList[bDriver]),
                         pDriver,
                         sizeof(MUSB_DeviceDriver));

            pList += wSize;
            wCount += wSize;
            wRemain -= wSize;

            bDriver++;
        }
    }

    MGC_MsdHostClient.wPeripheralListLength = wCount;
    MGC_MsdHostClient.bDeviceDriverListLength = bDriver;

    if(!MUSB_InitSystem(5))
    {
        MUSB_PRT("[MGC] InitSystem failed\r\n");
        return -1;
    }

    /* find first CDI port */
    MGC_pCdiPort = MUSB_GetPort(0);

    /* start session */
    MGC_hCdiBus = MUSB_RegisterOtgClient(MGC_pCdiPort,
                                         &MGC_xxxFunctionClient,
                                         &MGC_MsdHostClient,
                                         &MGC_MsdOtgClient);

    MUSB_NoneRunBackground();

    return 0;
}

int usb_sw_uninit(void)
{
    return 0;
}

#endif


/* 2343a150 - todo */
MUSB_HfiDevice* sub_2343a150(int a)
{
#if 0
	console_send_string("sub_2343a150 (todo.c): TODO\r\n");
#endif

	if (Data_235faa2c[a].Data_8 == 0)
	{
		return 0;
	}

	return Data_235faa2c[a].Data_8;
}


/* 2343a162 - complete */
void sub_2343a162(void (*a)())
{
#if 0
	console_send_string("sub_2343a162 (todo.c): TODO\r\n");
#endif

	Data_23492c60 = a;
}


/* 2343a17a - todo */
void sub_2343a17a (const uint16_t* r0)
{
#if 0
	console_send_string("sub_2343a17a (todo.c): TODO\r\n");
#endif

	uint8_t sp = 0;

	while (*r0 != 0)
	{
		sp = *r0++;
#if 0
		console_send_string(&sp);
#endif
	}
}


#endif // CFG_SUPPORT_MSD

// eof

