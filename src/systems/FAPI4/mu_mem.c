
#include <stdint.h>
#include <stddef.h>
#include "plat_mem.h"

#pragma thumb


/* 2347c804 /  - complete */
void* MGC_FAPI_MemRealloc( void* blockPtr, size_t iSize )
{
	uint32_t i = 0;
	uint8_t* r6 = blockPtr;
	uint8_t* r5 = memblk_alloc(main_hMemBlk2, iSize);

	if (r5 != NULL)
	{
		for (i = 0; i < iSize; i++)
		{
			r5[i] = r6[i];
		}

		memblk_free(main_hMemBlk2, r6);
	}

	return r5;
}


/* 2347c832 / - todo */
void MGC_FAPI_MemCopy(void* toPtr, const void* fromPtr, size_t iSize)
{
   uint8_t* u8_toPtr = toPtr;
   const uint8_t* u8_fromPtr = fromPtr;

   if (((int)u8_toPtr & 0x03) || ((int)u8_fromPtr & 0x03))
   {
	  while (iSize--)
	  {
		  *u8_toPtr++ = *u8_fromPtr++;
	  }
   }
   else
   {
	  memcpy(toPtr, fromPtr, iSize);
   }
}


/* 2347c854 / - complete */
void MGC_FAPI_MemSet(void* ptr, uint8_t c, size_t iSize)
{
   uint8_t* u8_ptr = ptr;

   if ((int)u8_ptr & 0x03)
   {
      while (iSize--)
      {
          *u8_ptr++ = c;
      }
   }
   else
   {
      memset(ptr, c, iSize);
   }
}


