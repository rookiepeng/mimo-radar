/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#include <stdlib.h>
#include <string.h>

// driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "spi.h"
#include "adc.h"
#include "hw_adc.h"
#include "hw_udma.h"
#include "udma.h"
#include "udma_if.h"

// ti-rtos includes
#include "osi.h"

// Simplelink includes
#include "simplelink.h"

// common interface includes
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "pinmux.h"
#include "gpio_if.h"

#include "gpio_conf.h"
#include "app_conf.h"
#include "wifi.h"
#include "simplelink_conf.h"
#include "tcpudp.h"
#include "spi_conf.h"
#include "adc_conf.h"
#include "radar.h"

unsigned char g_ulStatus = 0;
unsigned long g_ulStaIp = 0;
unsigned long g_ulPingPacketsRecv = 0;
unsigned long g_uiGatewayIP = 0;

//*****************************************************************************
//                            MAIN FUNCTION
//*****************************************************************************
void main()
{
    OsiTaskHandle g_WifiTask = NULL;
    long lRetVal = -1;

    BoardInit();    // Board Initialization
    PinMuxConfig(); // Configure the pinmux settings

    GPIO_PinConfigure(); // Configure IO pins

    SPI_Init(); // SPI Initialization
    Radar_Init();

#ifndef NOTERM
    InitTerm(); // Configuring UART
#endif

    // Start the SimpleLink Host
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if (lRetVal < 0)
    {
#ifndef NOTERM
        ERR_PRINT(lRetVal);
#endif
        LOOP_FOREVER();
    }

    // Start the WlanAPMode task
    lRetVal = osi_TaskCreate(WlanAPMode,
                             (const signed char *)"wireless LAN in AP mode",
                             OSI_STACK_SIZE, NULL, 1, &g_WifiTask);
    if (lRetVal < 0)
    {
#ifndef NOTERM
        ERR_PRINT(lRetVal);
#endif
        LOOP_FOREVER();
    }

    osi_start(); // Start the task scheduler
}
