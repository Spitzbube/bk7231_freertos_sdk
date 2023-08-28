/*!
*******************************************************************************
**
** \file        ./subsystems/bus/musb/include/fapi_dump.h
** 
** \brief       Dump Functions.
** 
** This header file contains the declaration of public macros and function(s) 
** to dump messages into a SDRAM buffer.
** Assuming such a buffer exists all register accesses from MUSB-Stack will be
** recorded accordingly. With the defined macros the application can furthermore 
** add additional messages when needed. 
** Note the application has full control whether and what is dumped and there is
** no performance drop for the USB traffic as long as no dump buffer is 
** initialized. 
** This module has been designed to support enhanced USB software debugging.
**
** \attention To activate this module the define MUSB_FAPI_DUMP must
**            exist when compiling the related source codes.  
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. FUJITSU MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS.
**
** (C) Copyright 2002-2007 by Fujitsu Microelectronics Europe GmbH
** (C) Copyright 2010 by Fujitsu Semiconductor Europe GmbH
** 
*******************************************************************************
*/
#ifndef __MUSB_FAPI_DUMP_H__
    #define __MUSB_FAPI_DUMP_H__
    
   
    /*
    *********************************************************************
    *********************************************************************
    ** Include all required header files 
    *********************************************************************
    *********************************************************************
    */   
    #include "mu_stdio.h"
    #include "mu_strng.h"
    
    /*
    *********************************************************************
    *********************************************************************
    ** Data types 
    *********************************************************************
    *********************************************************************
    */
    #ifdef MUSB_FAPI_DUMP
    
        #ifndef MGC_FAPI_DIAG_MSG_SIZE
            #define MGC_FAPI_DIAG_MSG_SIZE   256
        #endif
        extern char MGC_FAPI_DiagMsg[MGC_FAPI_DIAG_MSG_SIZE];
        
        #ifndef MGC_FAPI_DIAG_NUM_SIZE
            #define MGC_FAPI_DIAG_NUM_SIZE   33
        #endif
        extern uint32_t  MGC_FAPI_DumpBufSize; // DON't touch this!!!
        extern char MGC_FAPI_DiagNumber[MGC_FAPI_DIAG_NUM_SIZE];
        
        /*!
        *********************************************************************
        ** Initialize the dump buffer.
        **
        ** This function allocates a dump buffer of the given size. If a dump
        ** buffer already exists and the given size is 0 it will be deleted, 
        ** otherwise the macro fails.
        ** 
        ** \param bufferSize The desired size of the dump buffer in bytes or 0
        **                   if an existing buffer should be deleted. 
        **                
        ** \return 1 on success, otherwise 0. 
        *********************************************************************
        */
        #define MUSB_FAPI_DUMP_INIT(bufferSize)                       \
             MUSB_FAPI_DumpInit(bufferSize)
        
        
        /*!
        *********************************************************************
        ** Writes a message to the dump buffer.
        **
        ** This function writes a message string to the dump buffer and wraps
        ** it arround if necessary. If no dump buffer exist nothing will be
        ** written.
        **
        ** \param msgPtr Pointer to the message string to print.
        **
        *********************************************************************
        */
        #define MUSB_FAPI_DUMP_DIAG_STRING(msgPtr)                       \
            if ( MGC_FAPI_DumpBufSize ) {                                \
                 MUSB_FAPI_DumpString(msgPtr);                           \
            }
        
        /*!
        *********************************************************************
        ** Print a diagnostic with a string and number to the dump buffer.
        **
        ** This macro behaves like MUSB_DIAG1 but it's independent on a 
        ** diagnostic level. If no dump buffer exist nothing will be
        ** written.
        **
        ** \param _pText The text portion of the message; printed first.
        ** \param _dwNum The numeric portion of the message; appended to the 
        **               text portion.
        ** \param _bBase Numeric base (2, 8, 10 or 16).
        ** \param _bJust The justification (0 for none); this is for 
        **               leading-zero padding.
        **
        ** \note Lint rule 534 disabled because we dont
        ** need the return value of the internal MUSB_StringConcat function.
        *********************************************************************
        */
        #define MUSB_FAPI_DUMP_DIAG1(_pText, _dwNum, _bBase, _bJust) {  \
            if ( MGC_FAPI_DumpBufSize ) {                               \
                MGC_FAPI_DiagMsg[0] = 0;                                \
                MGC_FAPI_DiagNumber[0] = 0;                             \
                /*lint -save -e534 */                                   \
                MUSB_StringConcat(MGC_FAPI_DiagMsg,                     \
                                  MGC_FAPI_DIAG_MSG_SIZE,               \
                                  _pText);                              \
                MUSB_Stringize(MGC_FAPI_DiagNumber,                     \
                               MGC_FAPI_DIAG_NUM_SIZE,                  \
                               _dwNum,                                  \
                               _bBase,                                  \
                               _bJust);                                 \
                MUSB_StringConcat(MGC_FAPI_DiagMsg,                     \
                                  MGC_FAPI_DIAG_MSG_SIZE,               \
                                  MGC_FAPI_DiagNumber);                 \
                MUSB_FAPI_DumpString(MGC_FAPI_DiagMsg);                 \
                /*lint -restore */                                      \
            }                                                           \
        }
        
        /*!
        *********************************************************************
        ** Print a diagnostic with two strings and two numbers to the dump 
        ** buffer.
        ** 
        ** Print a diagnostic with a string followed by a number, followed 
        ** by another set. This macro behaves like MUSB_DIAG2 but it's 
        ** independent on a diagnostic level. If no dump buffer exist nothing 
        ** will be written.
        **
        ** \param _pText1 The first text portion of the message; printed first.
        ** \param _dwNum1 The first numeric portion of the message; appended 
        **                to the above.
        ** \param _pText2 The second text portion of the message; appended 
        **                to the above.
        ** \param _dwNum2 the second numeric portion of the message; appended 
        **                to the above.
        ** \param _bBase  Numeric base for both numbers (2, 8, 10 or 16).
        ** \param _bJust  The justification (0 for none); this is for 
        **                leading-zero padding.
        **
        ** \note Lint rule 534 disabled because we dont
        ** need the return value of the internal MUSB_StringConcat function.
        *********************************************************************
        */
        #define MUSB_FAPI_DUMP_DIAG2(_pText1, _dwNum1, _pText2, _dwNum2, _bBase, _bJust) { \
            if ( MGC_FAPI_DumpBufSize ) {                                \
                MGC_FAPI_DiagMsg[0] = 0;                                 \
                MGC_FAPI_DiagNumber[0] = 0;                              \
                /*lint -save -e534 */                                    \
                MUSB_StringConcat(MGC_FAPI_DiagMsg,                      \
                                  MGC_FAPI_DIAG_MSG_SIZE,                \
                                  _pText1);                              \
                MUSB_Stringize(MGC_FAPI_DiagNumber,                      \
                               MGC_FAPI_DIAG_NUM_SIZE,                   \
                               _dwNum1,                                  \
                               _bBase,                                   \
                               _bJust);                                  \
                MUSB_StringConcat(MGC_FAPI_DiagMsg,                      \
                                  MGC_FAPI_DIAG_MSG_SIZE,                \
                                  MGC_FAPI_DiagNumber);                  \
                MUSB_StringConcat(MGC_FAPI_DiagMsg,                      \
                                  MGC_FAPI_DIAG_MSG_SIZE,                \
                                  _pText2);                              \
                MGC_FAPI_DiagNumber[0] = 0;                              \
                MUSB_Stringize(MGC_FAPI_DiagNumber,                      \
                               MGC_FAPI_DIAG_NUM_SIZE,                   \
                               _dwNum2,                                  \
                               _bBase,                                   \
                               _bJust);                                  \
                MUSB_StringConcat(MGC_FAPI_DiagMsg,                      \
                                  MGC_FAPI_DIAG_MSG_SIZE,                \
                                  MGC_FAPI_DiagNumber);                  \
                MUSB_FAPI_DumpString(MGC_FAPI_DiagMsg);                  \
                /*lint -restore */                                       \
            }                                                            \
        }
        
        /*!
        *********************************************************************
        ** Prints the dump buffer to the terminal.
        **
        ** This function prints the content of the dump buffer to the terminal
        ** window. 
        **
        *********************************************************************
        */
        #define MUSB_FAPI_DUMP_PRINT                                   \
            MUSB_FAPI_DumpPrint();
            
    #else
        #define MUSB_FAPI_DUMP_INIT(bufferSize) (1)
        #define MUSB_FAPI_DUMP_DIAG_STRING(msgPtr)
        #define MUSB_FAPI_DUMP_DIAG1(_pText, _dwNum, _bBase, _bJust)
        #define MUSB_FAPI_DUMP_DIAG2(_pText1, _dwNum1, _pText2, _dwNum2, _bBase, _bJust)
        #define MUSB_FAPI_DUMP_PRINT
    #endif            
        
    /*
    *********************************************************************
    *********************************************************************
    ** DUMP functions 
    *********************************************************************
    *********************************************************************
    */ 
    
    #ifdef MUSB_FAPI_DUMP
    
        #ifdef __cplusplus
        extern "C" {
        #endif
        uint32_t MUSB_FAPI_DumpInit(uint32_t bufferSize);
        void MUSB_FAPI_DumpString(char* diagMsgPtr);
        void MUSB_FAPI_DumpPrint(void);
        #ifdef __cplusplus
        }
        #endif
    
    #endif
    
#endif
