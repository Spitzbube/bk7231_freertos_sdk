
#include <stdint.h>
#include <stddef.h>

#pragma thumb


void dma_SetUsbMode_LengthInput(int);


int Data_234927cc/*dma_UsbModeMirror*/; //234927cc


/* 23467b64 - complete */
uint8_t MGC_FAPI_ReadReg8(uint8_t* baseAddr, uint16_t offset)
{
	uint32_t cpu_sr;
	uint8_t reg_data;

	baseAddr += 4 * offset;

	cpu_sr = FAMOS_EnterCriticalSection();

	dma_SetUsbMode_LengthInput(0);

	reg_data = ((volatile uint32_t*)(baseAddr))[0];

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_FAPI_ReadReg8: offset=0x%x, data=0x%x\r\n", offset, reg_data);
		console_send_string(debug_string);
	}
#endif

	FAMOS_LeaveCriticalSection(cpu_sr);

	return reg_data;
}


/* 23467b84 / - complete */
uint16_t MGC_FAPI_ReadReg16 (uint8_t* baseAddr, uint16_t offset)
{
	uint32_t cpu_sr;
	uint16_t reg_data;

	baseAddr += 4 * offset;

	cpu_sr = FAMOS_EnterCriticalSection();

	dma_SetUsbMode_LengthInput(1);

	reg_data = ((volatile uint32_t*)(baseAddr))[0];

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_FAPI_ReadReg16: offset=0x%x, data=0x%x\r\n", offset, reg_data);
		console_send_string(debug_string);
	}
#endif

	FAMOS_LeaveCriticalSection(cpu_sr);

	return reg_data;
}


/* 23467ba4 - complete */
uint32_t MGC_FAPI_ReadReg32 (uint8_t* baseAddr, uint16_t offset)
{
#if 0
	console_send_string("MGC_FAPI_ReadReg32 (todo.c): TODO\r\n");
#endif

	uint32_t cpu_sr;
	uint32_t reg_data;

	baseAddr += 4 * offset;

	cpu_sr = FAMOS_EnterCriticalSection();

	dma_SetUsbMode_LengthInput(2);

	reg_data = ((volatile uint32_t*)(baseAddr))[0];

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_FAPI_ReadReg32: offset=0x%x, data=0x%x\r\n", offset, reg_data);
		console_send_string(debug_string);
	}
#endif

	FAMOS_LeaveCriticalSection(cpu_sr);

	return reg_data;
}


/* 23467bc2 - complete */
void MGC_FAPI_WriteReg8(uint8_t* baseAddr, uint16_t offset, uint8_t data)
{
	uint32_t cpu_sr;

	baseAddr += 4 * offset;

	cpu_sr = FAMOS_EnterCriticalSection();

	dma_SetUsbMode_LengthInput(0);

	((volatile uint32_t*)(baseAddr))[0] = data;

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_FAPI_WriteReg8: offset=0x%x, data=0x%x\r\n", offset, data);
		console_send_string(debug_string);
	}
#endif

	FAMOS_LeaveCriticalSection(cpu_sr);
}


/* 23467be0 / - complete */
void MGC_FAPI_WriteReg16 (uint8_t* baseAddr, uint16_t offset, uint16_t data)
{
	uint32_t cpu_sr;

	baseAddr += 4 * offset;

	cpu_sr = FAMOS_EnterCriticalSection();

	dma_SetUsbMode_LengthInput(1);

	((volatile uint32_t*)(baseAddr))[0] = data;

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_FAPI_WriteReg16: offset=0x%x, data=0x%x\r\n", offset, data);
		console_send_string(debug_string);
	}
#endif

	FAMOS_LeaveCriticalSection(cpu_sr);
}


/* 23467bfe - complete */
void MGC_FAPI_WriteReg32 (uint8_t* baseAddr, uint16_t offset, uint32_t data)
{
#if 0
	console_send_string("MGC_FAPI_WriteReg32 (todo.c): TODO\r\n");
#endif

	uint32_t cpu_sr;

	baseAddr += 4 * offset;

	cpu_sr = FAMOS_EnterCriticalSection();

	dma_SetUsbMode_LengthInput(2);

	((volatile uint32_t*)(baseAddr))[0] = data;

#if 0
	{
		extern char debug_string[];
		sprintf(debug_string, "MGC_FAPI_WriteReg32: offset=0x%x, data=0x%x\r\n", offset, data);
		console_send_string(debug_string);
	}
#endif

	FAMOS_LeaveCriticalSection(cpu_sr);
}


/* 23467c24 / - complete */
void dma_SetUsbMode_LengthInput(int a)
{
	int mask = 0x06;
	a = (a << 1) & mask;

	Data_234927cc = (Data_234927cc & ~mask) | a;

	((volatile uint32_t*)(0xc7000014))[0] = Data_234927cc;
}


