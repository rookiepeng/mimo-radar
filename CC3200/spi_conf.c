/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_mcspi.h"

#include "rom.h"
#include "rom_map.h"

#include "prcm.h"
#include "spi.h"

#include "spi_conf.h"

unsigned long ulDummy;

void SPI_Init()
{
    // Reset SPI
    MAP_SPIReset(SPI_Interface);

    // Configure SPI interface
    MAP_SPIConfigSetExpClk(SPI_Interface, MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                           SPI_IF_BIT_RATE, SPI_MODE_MASTER, SPI_SUB_MODE_0,
                           (SPI_HW_CTRL_CS |
                            SPI_4PIN_MODE |
                            SPI_TURBO_OFF |
                            SPI_CS_ACTIVELOW |
                            SPI_WL_32));

    // Enable SPI for communication
    MAP_SPIEnable(SPI_Interface);
}

void Master_Send(unsigned long txData)
{
    // Push the character over SPI
    MAP_SPIDataPut(SPI_Interface, txData);

    // Clean up the receive register into a dummy variable
    MAP_SPIDataGet(SPI_Interface, &ulDummy);
}
