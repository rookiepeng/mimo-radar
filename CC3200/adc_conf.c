/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#include <stddef.h>

#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_adc.h"
#include "hw_udma.h"

#include "rom_map.h"
#include "prcm.h"

#include "adc.h"
#include "udma.h"
#include "gpio.h"

#include "udma_if.h"
#include "gpio_if.h"

#include "adc_conf.h"
#include "app_conf.h"
#include "tcpudp.h"

unsigned long g_ucADCCH0BuffPRI[ADC_BUFF_SIZE];
unsigned long g_ucADCCH0BuffALT[ADC_BUFF_SIZE];

unsigned long g_ucADCCH1BuffPRI[ADC_BUFF_SIZE];
unsigned long g_ucADCCH1BuffALT[ADC_BUFF_SIZE];

unsigned long g_ucADCCH2BuffPRI[ADC_BUFF_SIZE];
unsigned long g_ucADCCH2BuffALT[ADC_BUFF_SIZE];

unsigned long g_ucADCCH3BuffPRI[ADC_BUFF_SIZE];
unsigned long g_ucADCCH3BuffALT[ADC_BUFF_SIZE];
char ADCSeg3[ADC_BUFF_SIZE], ADCSeg2[ADC_BUFF_SIZE], ADCSeg1[ADC_BUFF_SIZE], ADCSeg0[ADC_BUFF_SIZE];

char CH0PRI[]="CH0PRI";
char CH1PRI[]="CH1PRI";
char CH2PRI[]="CH2PRI";
char CH3PRI[]="CH3PRI";

char CH0ALT[]="CH0ALT";
char CH1ALT[]="CH1ALT";
char CH2ALT[]="CH2ALT";
char CH3ALT[]="CH3ALT";

void InitAdcDma()
{
    unsigned short Status;
    PRCMPeripheralClkDisable(PRCM_ADC, PRCM_RUN_MODE_CLK);

    UDMAInit();

    // CH0
    MAP_uDMAChannelAssign(UDMA_CH14_ADC_CH0);
    UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH0BuffPRI), UDMA_DST_INC_32);

    UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH0BuffALT), UDMA_DST_INC_32);

    //MAP_ADCDMAEnable(ADC_BASE, ADC_CH_0);
    //MAP_ADCIntRegister(ADC_BASE, ADC_CH_0, ADCIntHandler);
    //Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_0);
    //MAP_ADCIntClear(ADC_BASE, ADC_CH_0, Status | ADC_DMA_DONE);
    //MAP_ADCIntEnable(ADC_BASE, ADC_CH_0, ADC_DMA_DONE);
    //MAP_ADCChannelEnable(ADC_BASE, ADC_CH_0);

    // CH1
    MAP_uDMAChannelAssign(UDMA_CH15_ADC_CH1);
    UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH1BuffPRI), UDMA_DST_INC_32);

    UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH1BuffALT), UDMA_DST_INC_32);

    //MAP_ADCDMAEnable(ADC_BASE, ADC_CH_1);
    //MAP_ADCIntRegister(ADC_BASE, ADC_CH_1, ADCIntHandler);
    //Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_1);
    //MAP_ADCIntClear(ADC_BASE, ADC_CH_1, Status | ADC_DMA_DONE);
    //MAP_ADCIntEnable(ADC_BASE, ADC_CH_1, ADC_DMA_DONE);
    //MAP_ADCChannelEnable(ADC_BASE, ADC_CH_1);

    // CH2
    MAP_uDMAChannelAssign(UDMA_CH16_ADC_CH2);
    UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH2BuffPRI), UDMA_DST_INC_32);

    UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      //(void *)(0x4402E874 + ADC_CH_2), UDMA_SRC_INC_NONE,
                      (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH2BuffALT), UDMA_DST_INC_32);

    //MAP_ADCDMAEnable(ADC_BASE, ADC_CH_2);
    //MAP_ADCIntRegister(ADC_BASE, ADC_CH_2, ADCIntHandler);
    //Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_2);
    //MAP_ADCIntClear(ADC_BASE, ADC_CH_2, Status | ADC_DMA_DONE);
    //MAP_ADCIntEnable(ADC_BASE, ADC_CH_2, ADC_DMA_DONE);
    //MAP_ADCChannelEnable(ADC_BASE, ADC_CH_2);

    // CH3
    MAP_uDMAChannelAssign(UDMA_CH17_ADC_CH3);
    UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH3BuffPRI), UDMA_DST_INC_32);

    UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_32, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH3BuffALT), UDMA_DST_INC_32);

    ///////////////////////////////////////////////////////////////////////////////
    // Configure ADC timer which is used to timestamp the ADC data samples
    MAP_ADCTimerConfig(ADC_BASE, 2 ^ 16);

    // Enable ADC timer which is used to timestamp the ADC data samples
    MAP_ADCTimerEnable(ADC_BASE);

    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_0);
    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_1);
    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_2);
    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_3);
    MAP_ADCIntRegister(ADC_BASE, ADC_CH_3, ADCIntHandler);
    Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_3);
    MAP_ADCIntClear(ADC_BASE, ADC_CH_3, Status | ADC_DMA_DONE);
    MAP_ADCIntEnable(ADC_BASE, ADC_CH_3, ADC_DMA_DONE);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_0);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_1);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_2);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_3);

    PRCMPeripheralClkEnable(PRCM_ADC, PRCM_RUN_MODE_CLK);

    MAP_ADCEnable(ADC_BASE);
}

//*****************************************************************************
//
//! \brief ADC Interrupt handler
//!
//! \param None
//!
//! \return None
//!
//*****************************************************************************
void ADCIntHandler()
{
    unsigned long ulMode;
    unsigned long ulStatus;

    ulStatus = ADCIntStatus(ADC_BASE, ADC_CH_3);
    ADCIntClear(ADC_BASE, ADC_CH_3, ulStatus | ADC_DMA_DONE);
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH17_ADC_CH3 | UDMA_PRI_SELECT);
    if (ulMode == UDMA_MODE_STOP)
    {
        UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_32, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH0BuffPRI), UDMA_DST_INC_32);

        UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_32, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH1BuffPRI), UDMA_DST_INC_32);

        UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_32, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH2BuffPRI), UDMA_DST_INC_32);

        UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_32, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH3BuffPRI), UDMA_DST_INC_32);
/*
        UDP_send(CH0PRI, 6);
        TCP_send(g_ucADCCH0BuffPRI, ADC_BUFF_SIZE);
        UDP_send(CH1PRI, 6);
        TCP_send(g_ucADCCH1BuffPRI, ADC_BUFF_SIZE);
        UDP_send(CH2PRI, 6);
        TCP_send(g_ucADCCH2BuffPRI, ADC_BUFF_SIZE);
        UDP_send(CH3PRI, 6);
        TCP_send(g_ucADCCH3BuffPRI, ADC_BUFF_SIZE);*/
        //ADCDMADisable(ADC_BASE, ADC_CH_2);
        //ADCIntDisable(ADC_BASE, ADC_CH_2, ADC_DMA_DONE);
        //ADCDisable(ADC_BASE);
    }
    else
    {
        ulMode = MAP_uDMAChannelModeGet(UDMA_CH17_ADC_CH3 | UDMA_ALT_SELECT);
        if (ulMode == UDMA_MODE_STOP)
        {

            UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_32, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH0BuffALT), UDMA_DST_INC_32);

            UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_32, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH1BuffALT), UDMA_DST_INC_32);

            UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_32, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH2BuffALT), UDMA_DST_INC_32);

            UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_32, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH3BuffALT), UDMA_DST_INC_32);
/*
            UDP_send(CH0ALT, 6);
            TCP_send(g_ucADCCH0BuffALT, ADC_BUFF_SIZE);
            UDP_send(CH1ALT, 6);
            TCP_send(g_ucADCCH1BuffALT, ADC_BUFF_SIZE);
            UDP_send(CH2ALT, 6);
            TCP_send(g_ucADCCH2BuffALT, ADC_BUFF_SIZE);
            UDP_send(CH3ALT, 6);
            TCP_send(g_ucADCCH3BuffALT, ADC_BUFF_SIZE);*/

            ADCDMADisable(ADC_BASE, ADC_CH_0);
            ADCDMADisable(ADC_BASE, ADC_CH_1);
            ADCDMADisable(ADC_BASE, ADC_CH_2);
            ADCDMADisable(ADC_BASE, ADC_CH_3);
            ADCIntDisable(ADC_BASE, ADC_CH_3, ADC_DMA_DONE);
            ADCDisable(ADC_BASE);
            UDMADeInit();

            UDP_send("ADCDATA", 7);
            ADCLong2Char(g_ucADCCH0BuffPRI);
            ADCLong2Char(g_ucADCCH1BuffPRI);
            ADCLong2Char(g_ucADCCH2BuffPRI);
            ADCLong2Char(g_ucADCCH3BuffPRI);
            ADCLong2Char(g_ucADCCH0BuffALT);
            ADCLong2Char(g_ucADCCH1BuffALT);
            ADCLong2Char(g_ucADCCH2BuffALT);
            ADCLong2Char(g_ucADCCH3BuffALT);
            UDP_send("ADCSTOP", 7);
        }
    }
}

void ADCLong2Char(unsigned long *adcBuffer)
{
    unsigned int i;
    for (i=0; i<ADC_BUFF_SIZE; i++)
    {
        ADCSeg0[i] = (char) (adcBuffer[i]);
        ADCSeg1[i] = (char) (adcBuffer[i]>>8);
        ADCSeg2[i] = (char) (adcBuffer[i]>>16);
        ADCSeg3[i] = (char) (adcBuffer[i]>>24);
    }

    //TCP_send(ADCSeg3, ADC_BUFF_SIZE);
    //TCP_send(ADCSeg2, ADC_BUFF_SIZE);
    //TCP_send(ADCSeg1, ADC_BUFF_SIZE);
    //TCP_send(ADCSeg0, ADC_BUFF_SIZE);

    UDP_send(ADCSeg3, ADC_BUFF_SIZE);
    UDP_send(ADCSeg2, ADC_BUFF_SIZE);
    UDP_send(ADCSeg1, ADC_BUFF_SIZE);
    UDP_send(ADCSeg0, ADC_BUFF_SIZE);
    //UART_PRINT("\n\rbits[13:2] : ADC sample\n\r");
    //UART_PRINT("\n\rbits[31:14]: Time stamp of ADC sample \n\r");
}
