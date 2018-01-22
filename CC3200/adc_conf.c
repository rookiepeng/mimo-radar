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

unsigned short g_ucADCCH0BuffPRI[ADC_BUFF_SIZE];
unsigned short g_ucADCCH0BuffALT[ADC_BUFF_SIZE];

unsigned short g_ucADCCH1BuffPRI[ADC_BUFF_SIZE];
unsigned short g_ucADCCH1BuffALT[ADC_BUFF_SIZE];

unsigned short g_ucADCCH2BuffPRI[ADC_BUFF_SIZE];
unsigned short g_ucADCCH2BuffALT[ADC_BUFF_SIZE];

unsigned short g_ucADCCH3BuffPRI[ADC_BUFF_SIZE];
unsigned short g_ucADCCH3BuffALT[ADC_BUFF_SIZE];
char ADCSeg1[ADC_BUFF_SIZE], ADCSeg0[ADC_BUFF_SIZE];

void InitAdcDma()
{
    unsigned short Status;
    UDMAInit();

    // CH0
    MAP_uDMAChannelAssign(UDMA_CH14_ADC_CH0);
    UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH0BuffPRI), UDMA_DST_INC_16);

    UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH0BuffALT), UDMA_DST_INC_16);

    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_0);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_0);
    MAP_ADCIntRegister(ADC_BASE, ADC_CH_0, ADCIntHandlerCH0);
    Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_0);
    MAP_ADCIntClear(ADC_BASE, ADC_CH_0, Status | ADC_DMA_DONE);
    MAP_ADCIntEnable(ADC_BASE, ADC_CH_0, ADC_DMA_DONE);

    // CH1
    MAP_uDMAChannelAssign(UDMA_CH15_ADC_CH1);
    UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH1BuffPRI), UDMA_DST_INC_16);

    UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH1BuffALT), UDMA_DST_INC_16);

    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_1);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_1);
    MAP_ADCIntRegister(ADC_BASE, ADC_CH_1, ADCIntHandlerCH1);
    Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_1);
    MAP_ADCIntClear(ADC_BASE, ADC_CH_1, Status | ADC_DMA_DONE);
    MAP_ADCIntEnable(ADC_BASE, ADC_CH_1, ADC_DMA_DONE);

    // CH2
    MAP_uDMAChannelAssign(UDMA_CH16_ADC_CH2);
    UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH2BuffPRI), UDMA_DST_INC_16);

    UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH2BuffALT), UDMA_DST_INC_16);

    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_2);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_2);
    MAP_ADCIntRegister(ADC_BASE, ADC_CH_2, ADCIntHandlerCH2);
    Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_2);
    MAP_ADCIntClear(ADC_BASE, ADC_CH_2, Status | ADC_DMA_DONE);
    MAP_ADCIntEnable(ADC_BASE, ADC_CH_2, ADC_DMA_DONE);

    // CH3
    MAP_uDMAChannelAssign(UDMA_CH17_ADC_CH3);
    UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH3BuffPRI), UDMA_DST_INC_16);

    UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                      ADC_BUFF_SIZE,
                      UDMA_SIZE_16, UDMA_ARB_1,
                      (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                      (void *)&(g_ucADCCH3BuffALT), UDMA_DST_INC_16);

    MAP_ADCDMAEnable(ADC_BASE, ADC_CH_3);
    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_3);
    MAP_ADCIntRegister(ADC_BASE, ADC_CH_3, ADCIntHandlerCH3);
    Status = MAP_ADCIntStatus(ADC_BASE, ADC_CH_3);
    MAP_ADCIntClear(ADC_BASE, ADC_CH_3, Status | ADC_DMA_DONE);
    MAP_ADCIntEnable(ADC_BASE, ADC_CH_3, ADC_DMA_DONE);

    ///////////////////////////////////////////////////////////////////////////////
    // Configure ADC timer which is used to timestamp the ADC data samples
    //MAP_ADCTimerConfig(ADC_BASE, 2 ^ 16);

    // Enable ADC timer which is used to timestamp the ADC data samples
    //MAP_ADCTimerEnable(ADC_BASE);

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

void ADCIntHandlerCH0()
{
    unsigned long ulMode;
    unsigned long ulStatus;

    ulStatus = ADCIntStatus(ADC_BASE, ADC_CH_0);
    ADCIntClear(ADC_BASE, ADC_CH_0, ulStatus | ADC_DMA_DONE);
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH14_ADC_CH0 | UDMA_PRI_SELECT);
    if (ulMode == UDMA_MODE_STOP)
    {
        UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_16, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH0BuffPRI), UDMA_DST_INC_16);
    }
    else
    {
        ulMode = MAP_uDMAChannelModeGet(UDMA_CH14_ADC_CH0 | UDMA_ALT_SELECT);
        if (ulMode == UDMA_MODE_STOP)
        {

            UDMASetupTransfer(UDMA_CH14_ADC_CH0 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_16, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel0FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH0BuffALT), UDMA_DST_INC_16);

            //UDP_send("CH0", 3);
        }
    }
}

void ADCIntHandlerCH1()
{
    unsigned long ulMode;
    unsigned long ulStatus;

    ulStatus = ADCIntStatus(ADC_BASE, ADC_CH_1);
    ADCIntClear(ADC_BASE, ADC_CH_1, ulStatus | ADC_DMA_DONE);
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH15_ADC_CH1 | UDMA_PRI_SELECT);
    if (ulMode == UDMA_MODE_STOP)
    {
        UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_16, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH1BuffPRI), UDMA_DST_INC_16);
    }
    else
    {
        ulMode = MAP_uDMAChannelModeGet(UDMA_CH15_ADC_CH1 | UDMA_ALT_SELECT);
        if (ulMode == UDMA_MODE_STOP)
        {
            UDMASetupTransfer(UDMA_CH15_ADC_CH1 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_16, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel2FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH1BuffALT), UDMA_DST_INC_16);
            //UDP_send("CH1", 3);
        }
    }
}

void ADCIntHandlerCH2()
{
    unsigned long ulMode;
    unsigned long ulStatus;

    ulStatus = ADCIntStatus(ADC_BASE, ADC_CH_2);
    ADCIntClear(ADC_BASE, ADC_CH_2, ulStatus | ADC_DMA_DONE);
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH16_ADC_CH2 | UDMA_PRI_SELECT);
    if (ulMode == UDMA_MODE_STOP)
    {
        UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_16, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH2BuffPRI), UDMA_DST_INC_16);
    }
    else
    {
        ulMode = MAP_uDMAChannelModeGet(UDMA_CH16_ADC_CH2 | UDMA_ALT_SELECT);
        if (ulMode == UDMA_MODE_STOP)
        {
            UDMASetupTransfer(UDMA_CH16_ADC_CH2 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_16, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel4FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH2BuffALT), UDMA_DST_INC_16);
            //UDP_send("CH2", 3);
        }
    }
}

void ADCIntHandlerCH3()
{
    unsigned long ulMode;
    unsigned long ulStatus;

    ulStatus = ADCIntStatus(ADC_BASE, ADC_CH_3);
    ADCIntClear(ADC_BASE, ADC_CH_3, ulStatus | ADC_DMA_DONE);
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH17_ADC_CH3 | UDMA_PRI_SELECT);
    if (ulMode == UDMA_MODE_STOP)
    {
        UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
                          ADC_BUFF_SIZE,
                          UDMA_SIZE_16, UDMA_ARB_1,
                          (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                          (void *)&(g_ucADCCH3BuffPRI), UDMA_DST_INC_16);
    }
    else
    {
        ulMode = MAP_uDMAChannelModeGet(UDMA_CH17_ADC_CH3 | UDMA_ALT_SELECT);
        if (ulMode == UDMA_MODE_STOP)
        {
            UDMASetupTransfer(UDMA_CH17_ADC_CH3 | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
                              ADC_BUFF_SIZE,
                              UDMA_SIZE_16, UDMA_ARB_1,
                              (void *)(ADC_BASE + ADC_O_channel6FIFODATA), UDMA_SRC_INC_NONE,
                              (void *)&(g_ucADCCH3BuffALT), UDMA_DST_INC_16);

            //UDP_send("CH3", 3);

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

void ADCLong2Char(unsigned short *adcBuffer)
{
    unsigned int i;
    for (i = 0; i < ADC_BUFF_SIZE; i++)
    {
        ADCSeg0[i] = (char)(adcBuffer[i]);
        ADCSeg1[i] = (char)(adcBuffer[i] >> 8);
    }

    //TCP_send(ADCSeg3, ADC_BUFF_SIZE);
    //TCP_send(ADCSeg2, ADC_BUFF_SIZE);
    //TCP_send(ADCSeg1, ADC_BUFF_SIZE);
    //TCP_send(ADCSeg0, ADC_BUFF_SIZE);

    UDP_send(ADCSeg1, ADC_BUFF_SIZE);
    UDP_send(ADCSeg0, ADC_BUFF_SIZE);
}
