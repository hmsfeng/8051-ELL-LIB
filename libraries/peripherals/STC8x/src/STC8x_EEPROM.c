/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : STC8x_EEPROM.c
  - Author        : zeweni
  - Update date   : 2020.02.06
  -	Copyright(C)  : 2020-2021 zeweni. All rights reserved.
-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
|                               INCLUDES                                |
-----------------------------------------------------------------------*/
#include "STC8x_EEPROM.h"
/*-----------------------------------------------------------------------
|                                 DATA                                  |
-----------------------------------------------------------------------*/
/* None */
/*-----------------------------------------------------------------------
|                               FUNCTION                                |
-----------------------------------------------------------------------*/

/**
  * @name    nop
  * @brief   working wait nop  function
  * @param   None
  * @return  None 
***/
static void nop(void)
{
	uint8_t i = 255;
	while(i--);
}

/**
  * @name    EEPROM_WT_Time
  * @brief   EEPROM working wait time function
  * @param   None
  * @return  None 
***/
static void EEPROM_WT_Time(void)
{	
	extern uint32 Get_Sysclk_FRE(void);
    uint32 sysClk_FRE;
	/* Get system clock frequency */
	sysClk_FRE = Get_Sysclk_FRE();
    #if (PER_LIB_MCU_MUODEL == STC8Ax || PER_LIB_MCU_MUODEL == STC8Cx || PER_LIB_MCU_MUODEL == STC8Fx)
        IAP_CONTR &= 0xF8;
        if (sysClk_FRE >= 30000000UL) IAP_CONTR |= 0x00;	
        else if ((sysClk_FRE >= 24000000UL) && (sysClk_FRE < 30000000UL)) IAP_CONTR |= 0x01;
        else if ((sysClk_FRE >= 20000000UL) && (sysClk_FRE < 24000000UL)) IAP_CONTR |= 0x02;
        else if ((sysClk_FRE >= 12000000UL) && (sysClk_FRE < 20000000UL)) IAP_CONTR |= 0x03;
        else if ((sysClk_FRE >=  6000000UL) && (sysClk_FRE < 12000000UL)) IAP_CONTR |= 0x04;
        else if ((sysClk_FRE >=  3000000UL) && (sysClk_FRE <  6000000UL)) IAP_CONTR |= 0x05;
        else if ((sysClk_FRE >=  2000000UL) && (sysClk_FRE <  3000000UL)) IAP_CONTR |= 0x06;
        else if ((sysClk_FRE >=  1000000UL) && (sysClk_FRE <  2000000UL)) IAP_CONTR |= 0x07;
    #elif (PER_LIB_MCU_MUODEL == STC8Hx || PER_LIB_MCU_MUODEL == STC8Gx)
        IAP_TPS = (0xC0 & IAP_TPS) | (sysClk_FRE/1000000UL);
    #endif
}

/**
  * @name    EEPROM_Ctrl
  * @brief   EEPROM control function
  * @param   run  ENABLE | DISABLE
  * @return  FSC_SUCCESS(1) / FSC_FAIL(0) 
***/
FSCSTATE EEPROM_Ctrl(FUNSTATE run)
{
	IAP_CONTR = (IAP_CONTR & 0x7F) | (run << 7);
    EEPROM_WT_Time();
	return FSC_SUCCESS;
}

/**
  * @name    EEPROM_Read_Byte
  * @brief   EEPROM read byte function  
  * @param   addr   falsh address (uint16_t)
  * @return  data of byte (uint8_t) 
***/
uint8_t EEPROM_Read_Byte(uint16_t addr)
{
	uint8_t dat;
	IAP_CMD = 1;  //Set IAP read command
	IAP_ADDRH = addr >> 8; 	//Set IAP high address
	IAP_ADDRL = addr; 		//Set IAP low address
	IAP_TRIG  = 0x5A; 		//Write trigger command (0x5a)
	IAP_TRIG  = 0xA5; 		//Write trigger command (0xa5)
	dat = IAP_DATA;
	nop();
	return dat;
}

/**
  * @name    EEPROM_Read_arry
  * @brief   EEPROM read byte function  
  * @param   addr   falsh address (uint16_t)
  * @param   *arry  data of arry (uint8_t)
  * @param   len    arry length  (uint8_t) 
  * @return  FSC_SUCCESS(1) / FSC_FAIL(0) 
***/
FSCSTATE EEPROM_Read_arry(uint16_t addr,uint8_t *arry,uint8_t len)
{
	while(len--)
	{
		*arry++ = EEPROM_Read_Byte(addr++);
	}
	return FSC_SUCCESS;
}

/**
  * @name    EEPROM_Write_Byte
  * @brief   EEPROM write byte function  
  * @param   addr  falsh address (uint16_t)
  * @param   byte  data of byte (uint8_t)
  * @return  FSC_SUCCESS(1) / FSC_FAIL(0) 
***/
FSCSTATE EEPROM_Write_Byte(uint16_t addr,uint8_t byte)
{
	IAP_CMD = 2;  //Set IAP write command
	IAP_ADDRH = addr >> 8; 	//Set IAP high address
	IAP_ADDRL = addr; 		//Set IAP low address
	IAP_DATA =  byte; 		//Read IAP data	
    IAP_TRIG  = 0x5A; 		//Write trigger command (0x5a)
	IAP_TRIG  = 0xA5; 		//Write trigger command (0xa5)
	nop(); 
	return FSC_SUCCESS;
}

/**
  * @name    EEPROM_Write_str
  * @brief   EEPROM write string function 
  * @param   addr    falsh address (uint16_t)
  * @param   *str    data of string (uint8_t)
  * @return  FSC_SUCCESS(1) / FSC_FAIL(0) 
***/
FSCSTATE EEPROM_Write_str(uint16_t addr,const uint8_t *str)
{
	while(*str)
	{
		EEPROM_Write_Byte(addr++,*str++);
	}
	return FSC_SUCCESS;
}

/**
  * @name    EEPROM_Write_arry
  * @brief   EEPROM write byte function  
  * @param   addr    falsh address (uint16_t)
  * @param   *arry   data of arry (uint8_t)
  * @param   len     arry length  (uint8_t) 
  * @return  FSC_SUCCESS(1) / FSC_FAIL(0) 
***/
FSCSTATE EEPROM_Write_arry(uint16_t addr,const uint8_t *arry,uint8_t len)
{
	while(len--)
	{
		EEPROM_Write_Byte(addr++,*arry++);
	}
	return FSC_SUCCESS;
}

/**
  * @name    EEPROM_Erase_Page
  * @brief   EEPROM erase page function
  * @param   addr    falsh address (uint16_t)
  * @return  FSC_SUCCESS(1) / FSC_FAIL(0) 
***/
FSCSTATE EEPROM_Erase_Page(uint16_t addr)
{
	IAP_CMD = 3;            //Set IAP write command
	IAP_ADDRH = addr >> 8; 	//Set IAP high address
	IAP_ADDRL = addr; 		//Set IAP low address
	IAP_TRIG = 0x5A; 		//Write trigger command (0x5a)
	IAP_TRIG = 0xA5; 		//Write trigger command (0x5a)	
	nop(); 
	return FSC_SUCCESS;
}

/*-----------------------------------------------------------------------
|                   END OF FLIE.  (C) COPYRIGHT zeweni                  | 
-----------------------------------------------------------------------*/



