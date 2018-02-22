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
#include "hw_gpio.h"

#include "rom.h"
#include "rom_map.h"

#include "pin.h"
#include "gpio.h"
#include "prcm.h"

#include "pinmux.h"

#include "app_conf.h"

//*****************************************************************************
void PinMuxConfig(void)
{
        // Set unused pins to PIN_MODE_0 with the exception of JTAG pins 16,17,19,20
        PinModeSet(PIN_21, PIN_MODE_0);
        PinModeSet(PIN_45, PIN_MODE_0);
        PinModeSet(PIN_52, PIN_MODE_0);
        PinModeSet(PIN_53, PIN_MODE_0);

// Enable Peripheral Clocks
#ifndef NOTERM
        PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
#endif
        PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);
        PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
        PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
        PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
        PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);
        PRCMPeripheralClkEnable(PRCM_ADC, PRCM_RUN_MODE_CLK);

#ifndef NOTERM
        // UART0
        // Configure PIN_55 for UART0 UART0_TX
        PinTypeUART(PIN_55, PIN_MODE_3);
        // Configure PIN_57 for UART0 UART0_RX
        PinTypeUART(PIN_57, PIN_MODE_3);
#endif

        // SPI
        // Configure PIN_05 for SPI0 GSPI_CLK
        PinTypeSPI(PIN_05, PIN_MODE_7);
        // Configure PIN_06 for SPI0 GSPI_MISO
        PinTypeSPI(PIN_06, PIN_MODE_7);
        // Configure PIN_07 for SPI0 GSPI_MOSI
        PinTypeSPI(PIN_07, PIN_MODE_7);
        // Configure PIN_08 for SPI0 GSPI_CS
        PinTypeSPI(PIN_08, PIN_MODE_7);

        // SPI CS Selection
        // Configure PIN_04 for GPIO Output, A
        PinTypeGPIO(PIN_04, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_OUT);
        // Configure PIN_03 for GPIO Output, B
        PinTypeGPIO(PIN_03, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA1_BASE, 0x10, GPIO_DIR_MODE_OUT);
        // Configure PIN_02 for GPIO Output, C
        PinTypeGPIO(PIN_02, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);

        // LEDs
        // Configure PIN_17 for GPIO Output, LED0
        //PinTypeGPIO(PIN_17, PIN_MODE_0, false);
        //GPIODirModeSet(GPIOA3_BASE, 0x1, GPIO_DIR_MODE_OUT);
        // Configure PIN_18 for GPIO Output, LED1
        //PinTypeGPIO(PIN_18, PIN_MODE_0, false);
        //GPIODirModeSet(GPIOA3_BASE, 0x10, GPIO_DIR_MODE_OUT);

// RF switches
#ifdef NOTERM
        // Configure PIN_55 for GPIO Output, Store
        PinTypeGPIO(PIN_55, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA0_BASE, 0x2, GPIO_DIR_MODE_OUT);
#endif
        // Configure PIN_61 for GPIO Output, D1
        PinTypeGPIO(PIN_61, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA0_BASE, 0x40, GPIO_DIR_MODE_OUT);
        // Configure PIN_62 for GPIO Output, D2
        PinTypeGPIO(PIN_62, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA0_BASE, 0x80, GPIO_DIR_MODE_OUT);
        // Configure PIN_63 for GPIO Output, D3
        PinTypeGPIO(PIN_63, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA1_BASE, 0x1, GPIO_DIR_MODE_OUT);
        // Configure PIN_50 for GPIO Output, D4
        PinTypeGPIO(PIN_50, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA0_BASE, 0x1, GPIO_DIR_MODE_OUT);

        // baseband switch
        // Configure PIN_15 for GPIO Output, S3
        PinTypeGPIO(PIN_15, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA2_BASE, 0x40, GPIO_DIR_MODE_OUT);
        // Configure PIN_16 for GPIO Output, S2
        PinTypeGPIO(PIN_16, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA2_BASE, 0x80, GPIO_DIR_MODE_OUT);
        // Configure PIN_17 for GPIO Output, S1
        PinTypeGPIO(PIN_17, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA3_BASE, 0x1, GPIO_DIR_MODE_OUT);
        // Configure PIN_18 for GPIO Output, S0
        PinTypeGPIO(PIN_18, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA3_BASE, 0x10, GPIO_DIR_MODE_OUT);

        // ADC
        // Configure PIN_57 for ADC0 ADC_CH0
        PinTypeADC(PIN_57, PIN_MODE_255);
        // Configure PIN_58 for ADC0 ADC_CH1
        PinTypeADC(PIN_58, PIN_MODE_255);
        // Configure PIN_59 for ADC0 ADC_CH2
        PinTypeADC(PIN_59, PIN_MODE_255);
        // Configure PIN_60 for ADC0 ADC_CH3
        PinTypeADC(PIN_60, PIN_MODE_255);

        // TX pulse
        // Configure PIN_01 for GPIO Output
        PinTypeGPIO(PIN_01, PIN_MODE_0, false);
        GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);
}
