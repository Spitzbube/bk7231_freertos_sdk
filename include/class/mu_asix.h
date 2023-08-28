
#define AX_CMD_SET_SW_MII           0x06
#define AX_CMD_READ_MII_REG         0x07
#define AX_CMD_WRITE_MII_REG        0x08
#define AX_CMD_STATMNGSTS_REG		0x09
#define AX_CMD_SET_HW_MII           0x0a
#define AX_CMD_READ_RX_CTL          0x0f
#define AX_CMD_WRITE_RX_CTL         0x10
#define AX_CMD_WRITE_IPG0           0x12
#define AX_CMD_READ_NODE_ID         0x13
#define AX_CMD_READ_PHY_ID          0x19
#define AX_CMD_READ_MEDIUM_STATUS   0x1a
#define AX_CMD_WRITE_MEDIUM_MODE    0x1b
#define AX_CMD_WRITE_GPIOS          0x1f
#define AX_CMD_SW_RESET             0x20
#define AX_CMD_SW_PHY_SELECT		0x22

#define AX_EMBD_PHY_ADDR	0x10
#define AX_CHIPCODE_MASK		0x70
#define AX_AX88772_CHIPCODE		0x00
#define AX_AX88772A_CHIPCODE		0x10
#define AX_AX88772B_CHIPCODE		0x20

#define AX_GPIO_GPO2EN		0x10	/* GPIO2 Output enable */
#define AX_GPIO_GPO_2		0x20	/* GPIO2 Output value */
#define AX_GPIO_RSE		0x80	/* Reload serial EEPROM */

#define AX_SWRESET_CLEAR		0x00
#define AX_SWRESET_IPRL			0x20
#define AX_SWRESET_IPPD			0x40

#define AX_PHYSEL_SSEN			0x10

#define AX88772_IPG0_DEFAULT		0x15
#define AX88772_IPG1_DEFAULT		0x0c
#define AX88772_IPG2_DEFAULT		0x12

/* AX88772 & AX88178 Medium Mode Register */
#define AX_MEDIUM_PF		0x0080
#define AX_MEDIUM_JFE		0x0040
#define AX_MEDIUM_TFC		0x0020
#define AX_MEDIUM_RFC		0x0010
#define AX_MEDIUM_ENCK		0x0008
#define AX_MEDIUM_AC		0x0004
#define AX_MEDIUM_FD		0x0002
#define AX_MEDIUM_GM		0x0001
#define AX_MEDIUM_SM		0x1000
#define AX_MEDIUM_SBP		0x0800
#define AX_MEDIUM_PS		0x0200
#define AX_MEDIUM_RE		0x0100

#define AX88178_MEDIUM_DEFAULT	\
	(AX_MEDIUM_PS | AX_MEDIUM_FD | AX_MEDIUM_AC | \
	 AX_MEDIUM_RFC | AX_MEDIUM_TFC | AX_MEDIUM_JFE | \
	 AX_MEDIUM_RE)

#define AX88772_MEDIUM_DEFAULT	\
	(AX_MEDIUM_FD | AX_MEDIUM_PS | \
	 AX_MEDIUM_AC | AX_MEDIUM_RE)

/* AX88772 & AX88178 RX_CTL values */
#define AX_RX_CTL_SO		0x0080
#define AX_RX_CTL_AP		0x0020
#define AX_RX_CTL_AM		0x0010
#define AX_RX_CTL_AB		0x0008
#define AX_RX_CTL_SEP		0x0004
#define AX_RX_CTL_AMALL		0x0002
#define AX_RX_CTL_PRO		0x0001
#define AX_RX_CTL_MFB_2048	0x0000
#define AX_RX_CTL_MFB_4096	0x0100
#define AX_RX_CTL_MFB_8192	0x0200
#define AX_RX_CTL_MFB_16384	0x0300

#define AX_DEFAULT_RX_CTL	(AX_RX_CTL_SO | AX_RX_CTL_AB)

#define AX_RX_IRB_SIZE					2048
#define AX_TX_IRB_SIZE					2048

#define MUSB_ASIX_MAX_DEV_SUPPORTED		1

#define THREAD_PRIO_MGC_ASIX			30
#define THREAD_STACK_SIZE_MGC_ASIX 		0x400

typedef struct MUSB_AsixDevice
{
    MUSB_BusHandle   busHandle;
    MUSB_Device*     pUsbDevice;

    const MUSB_InterfaceDescriptor* pInfDscr;
    const MUSB_EndpointDescriptor*  pTxEndpDscr;
    const MUSB_EndpointDescriptor*  pRxEndpDscr;
    const MUSB_EndpointDescriptor*  pIntEndpDscr;

    MUSB_Irp        intrInIrp;
    MUSB_Pipe       intrInPipe;

    MUSB_Irp		bulkInIrp;
    MUSB_Pipe       bulkInPipe;

    MUSB_Irp		bulkOutIrp;
    MUSB_Pipe       bulkOutPipe;

    void*			threadSema;
    void*			pMBox;

    struct netif*   pNetif;
    int 			(*send)(struct MUSB_AsixDevice* pAsixDevice, uint8_t* buf_ptr, uint32_t packet_len);

    MUSB_ControlIrp                 ctrlIrp;
    MUSB_DeviceRequest              setupPacket;
    uint8_t                         aRawBuf[16];
    uint8_t          aInterruptDataBuffer[8];
    uint8_t          aRxDataBuffer[AX_RX_IRB_SIZE];
    uint8_t          aTxDataBuffer[AX_TX_IRB_SIZE];
    uint8_t          abMacAddress[6];
    uint16_t         phy_addr;
    uint8_t          embd_phy;
    uint8_t          chipcode;
    uint8_t          bState;
    uint8_t          bIndexAtAsixDeviceList;

} MUSB_AsixDevice;

typedef struct
{
	MUSB_AsixDevice  asixDeviceList[MUSB_ASIX_MAX_DEV_SUPPORTED];
    uint8_t          bNumOfDevActive;

} MUSB_AsixDriverContext;


static void MGC_AsixDriverInit();
static uint8_t MGC_AsixConnect(void* pPrivateData, MUSB_BusHandle hBus, MUSB_Device* pUsbDevice, const uint8_t* pPeripheralList);
static void MGC_AsixDisconnect();
static void MGC_AsixThread(void*);
static uint32_t MGC_AsixFindFreeDeviceIndex(MUSB_AsixDriverContext* pDriverContext, uint8_t* pIndex);
static int MGC_AsixStartInterruptTransfer(MUSB_AsixDevice* pAsixDevice);
static int MGC_AsixStartRxBulkTransfer(MUSB_AsixDevice* pAsixDevice);
static int MGC_AsixPrepareBulkTxTransfer(MUSB_AsixDevice* pAsixDevice);
static int MGC_AsixStartTxBulkTransfer(MUSB_AsixDevice* pAsixDevice, uint8_t* buf_ptr, uint32_t packet_len);
static void MGC_AsixIntrIrpCallback (void* pCtxt, MUSB_Irp* pIntrIrp);
static void MGC_AsixRxIrpCallback(void* pCtxt, MUSB_Irp* pBulkIrp);
static void MGC_AsixTxIrpCallback(void* pCtxt, MUSB_Irp* pBulkIrp);
static void MUSB_AsixPrepareSetupPkt(MUSB_DeviceRequest *pSetup,
		uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength);
static uint32_t MGC_AsixReadCmd(MUSB_AsixDevice* pAsixDevice,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, void* pData);
static uint32_t MGC_AsixWriteCmd(MUSB_AsixDevice* pAsixDevice,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, void* pData);
static uint32_t MGC_AsixWriteGpio(MUSB_AsixDevice* pAsixDevice, uint16_t value, int sleepy);
static int MGC_AsixSwReset(MUSB_AsixDevice* pAsixDevice, uint8_t flags);
static uint16_t MGC_AsixReadRxCtl(MUSB_AsixDevice* pAsixDevice);
static int MGC_AsixWriteRxCtl(MUSB_AsixDevice* pAsixDevice, uint16_t mode);
static int MGC_AsixWriteMediumMode(MUSB_AsixDevice* pAsixDevice, uint16_t mode);
static uint16_t MGC_AsixReadMediumStatus(MUSB_AsixDevice* pAsixDevice);
static int MGC_AsixGetPhyAddr(MUSB_AsixDevice* pAsixDevice);
static void MGC_AsixMdioWrite(MUSB_AsixDevice* pAsixDevice, int phy_id, int loc, int val);
static void MGC_AsixSetSwMii(MUSB_AsixDevice* pAsixDevice);
static void MGC_AsixSetHwMii(MUSB_AsixDevice* pAsixDevice);
static int mii_nway_restart(MUSB_AsixDevice* pAsixDevice);


