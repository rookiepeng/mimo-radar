/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#include "rom_map.h"
#include "utils.h"

#include "gpio_if.h"

#include "spi_conf.h"
#include "gpio_conf.h"
#include "radar.h"

// Port and Pin for decoder control
extern unsigned int g_uiAPort, g_uiBPort, g_uiCPort;
extern unsigned char g_ucAPin, g_ucBPin, g_ucCPin;

// Port and Pin for LEDs
extern unsigned int g_uiLED0Port, g_uiLED1Port;
extern unsigned char g_ucLED0Pin, g_ucLED1Pin;

// Port and Pin for RF switches
extern unsigned int g_uiD1Port, g_uiD2Port, g_uiD3Port, g_uiD4Port, g_uiStorePort;
extern unsigned char g_ucD1Pin, g_ucD2Pin, g_ucD3Pin, g_ucD4Pin, g_ucStorePin;

// Port and Pin for baseband switch
extern unsigned int g_uiS0Port, g_uiS1Port, g_uiS2Port, g_uiS3Port;
extern unsigned char g_ucS0Pin, g_ucS1Pin, g_ucS2Pin, g_ucS3Pin;

// TX pulse
extern unsigned int g_uiP0Port;
extern unsigned char g_ucP0Pin;

static unsigned char l_ucTxChannel = 0;
static unsigned char l_ucRxChannel = 0;

//*****************************************************************************
//
//!  \brief Decoder control
//!
//! \param[in] ucSelect               Input code
//!
//! \return none
//!
//
//*****************************************************************************
void CS_Decoder(char ucSelect)
{
    switch (ucSelect)
    {
    case '0':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 0);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 0);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 0);
        break;
    case '1':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 1);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 0);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 0);
        break;
    case '2':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 0);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 1);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 0);
        break;
    case '3':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 1);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 1);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 0);
        break;
    case '4':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 0);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 0);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 1);
        break;
    case '5':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 1);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 0);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 1);
        break;
    case '6':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 0);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 1);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 1);
        break;
    case '7':
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 1);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 1);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 1);
        break;
    default:
        GPIO_Set(GPIO_A, g_uiAPort, g_ucAPin, 0);
        GPIO_Set(GPIO_B, g_uiBPort, g_ucBPin, 0);
        GPIO_Set(GPIO_C, g_uiCPort, g_ucCPin, 0);
        break;
    }
}

//*****************************************************************************
//
//!  \brief RF switch control
//!
//! \param[in] ucSelect               Input code
//!
//! \return none
//!
//
//*****************************************************************************
void RF_Switch(char ucSelect)
{
    switch (ucSelect)
    {
    case 0:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 1:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 2:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 3:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 4:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 5:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 6:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 7:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 0);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 8:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 9:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 10:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 11:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 0);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 12:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 13:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 0);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 14:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 0);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    case 15:
        GPIO_Set(GPIO_D4, g_uiD4Port, g_ucD4Pin, 1);
        GPIO_Set(GPIO_D3, g_uiD3Port, g_ucD3Pin, 1);
        GPIO_Set(GPIO_D2, g_uiD2Port, g_ucD2Pin, 1);
        GPIO_Set(GPIO_D1, g_uiD1Port, g_ucD1Pin, 1);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 1);
        MAP_UtilsDelay(8000);
        GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);
        break;
    default:
        break;
    }
}

//*****************************************************************************
//
//! \brief Initialize VCO
//!
//! \param[in] vcoNum VCO number, 0, 1
//!
//! \return none
//!
//
//*****************************************************************************
void VCO_Init(unsigned char vcoNum)
{
    GPIO_LedOn(LED1);
    if (vcoNum == 0)
    {
        CS_Decoder('0');
    }
    else if (vcoNum == 1)
    {
        CS_Decoder('2');
    }
    MAP_UtilsDelay(8000);
    Master_Send(0x02000007);
    Master_Send(0x0000002B);
    Master_Send(0x0000000B);
    Master_Send(0x1D32A64A);
    Master_Send(0x2A20B929);
    Master_Send(0x40003E88);
    Master_Send(0x809FE520);
    MAP_UtilsDelay(8000);
    Master_Send(0x011F4027);
    Master_Send(0x00000006);
    Master_Send(0x025B2005);
    Master_Send(0x00200004);
    Master_Send(0x01890803);
    Master_Send(0x00020642);
    Master_Send(0xFFF7FFE1);
    Master_Send(0x809FE720);
    MAP_UtilsDelay(8000);
    Master_Send(0x809FE560);
    Master_Send(0x809FED60);
    MAP_UtilsDelay(8000);
    Master_Send(0x809FE5A0);
    Master_Send(0x809FF5A0);
    MAP_UtilsDelay(8000);
    Master_Send(0x2800B929);
    Master_Send(0x809F2500);
    //Master_Send_Word(0x809F25A0);
    GPIO_LedOff(LED1);
}

void PLL_Init(unsigned char pllNum)
{
    GPIO_LedOn(LED1);
    if (pllNum == 0)
    {
        CS_Decoder('1');
    }
    else if (pllNum == 1)
    {
        CS_Decoder('3');
    }
    //Master_Send(0x00000007);
    //Master_Send(0x00001F46);
    //Master_Send(0x00380835);
    //Master_Send(0x00601904);
    //Master_Send(0x00820443);
    //Master_Send(0x07408052);
    //Master_Send(0x00000009);
    //Master_Send(0x00960000);
    Master_Send(0x00100007);
    Master_Send(0x0000270E);
    Master_Send(0x003812C5);
    Master_Send(0x00781004);
    Master_Send(0x00820C43);
    Master_Send(0x07408052);
    Master_Send(0x00000009);
    Master_Send(0x78960000);
    GPIO_LedOff(LED1);
    Tx_Pulse();
}

void Chirp_Enable()
{
    CS_Decoder('1');
    Master_Send(0xF8960000);
    CS_Decoder('3');
    Master_Send(0xF8960000);
}

void Chirp_Disable()
{
    CS_Decoder('1');
    Master_Send(0x78960000);
    CS_Decoder('3');
    Master_Send(0x78960000);
}

void RX_Init_Off()
{
    GPIO_LedOn(LED1);
    CS_Decoder('4');
    Master_Send(0x00000003);
    Master_Send(0x00020006);
    Master_Send(0xE0001499);
    Master_Send(0x800000A0);
    CS_Decoder('5');
    Master_Send(0x00000003);
    Master_Send(0x00020006);
    Master_Send(0xE0001499);
    Master_Send(0x800000A0);
    CS_Decoder('6');
    Master_Send(0x00000003);
    Master_Send(0x00020006);
    Master_Send(0xE0001499);
    Master_Send(0x800000A0);
    CS_Decoder('7');
    Master_Send(0x00000003);
    Master_Send(0x00020006);
    Master_Send(0xE0001499);
    Master_Send(0x800000A0);
    GPIO_LedOff(LED1);
}

void TX(unsigned char txNum)
{
    l_ucTxChannel = txNum;
    GPIO_LedOn(LED1);
    RF_Switch(txNum);
    if (txNum < 4)
    {
        if (l_ucRxChannel < 8)
        {
            CS_Decoder('0');
            Master_Send(0x809F25A0);
            CS_Decoder('2');
            Master_Send(0x809F2500);
        }
        else
        {
            CS_Decoder('0');
            Master_Send(0x809F2580);
            CS_Decoder('2');
            Master_Send(0x809F2520);
        }
    }
    else if (txNum >= 4 && txNum < 8)
    {
        if (l_ucRxChannel < 8)
        {
            CS_Decoder('0');
            Master_Send(0x809F2560);
            CS_Decoder('2');
            Master_Send(0x809F2500);
        }
        else
        {
            CS_Decoder('0');
            Master_Send(0x809F2540);
            CS_Decoder('2');
            Master_Send(0x809F2520);
        }
    }
    else if (txNum >= 8 && txNum < 12)
    {
        if (l_ucRxChannel < 8)
        {
            CS_Decoder('0');
            Master_Send(0x809F2520);
            CS_Decoder('2');
            Master_Send(0x809F2540);
        }
        else
        {
            CS_Decoder('0');
            Master_Send(0x809F2500);
            CS_Decoder('2');
            Master_Send(0x809F2560);
        }
    }
    else if (txNum >= 12 && txNum < 16)
    {
        if (l_ucRxChannel < 8)
        {
            CS_Decoder('0');
            Master_Send(0x809F2520);
            CS_Decoder('2');
            Master_Send(0x809F2580);
        }
        else
        {
            CS_Decoder('0');
            Master_Send(0x809F2500);
            CS_Decoder('2');
            Master_Send(0x809F25A0);
        }
    }
    GPIO_LedOff(LED1);
}

void TX_Off()
{
    GPIO_LedOn(LED1);

    CS_Decoder('0');
    Master_Send(0x809F2500);
    CS_Decoder('2');
    Master_Send(0x809F2500);

    GPIO_LedOff(LED1);
}

void RX(unsigned char rxNum)
{
    l_ucRxChannel = rxNum;
    GPIO_LedOn(LED1);
    RX_Init_Off();
    switch (rxNum)
    {
    case 0:
        CS_Decoder('4');
        Master_Send(0x80000CA0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 1:
        CS_Decoder('4');
        Master_Send(0x800014A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 2:
        CS_Decoder('4');
        Master_Send(0x800044A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 3:
        CS_Decoder('4');
        Master_Send(0x800024A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 4:
        CS_Decoder('5');
        Master_Send(0x80000CA0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 5:
        CS_Decoder('5');
        Master_Send(0x800014A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 6:
        CS_Decoder('5');
        Master_Send(0x800044A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 7:
        CS_Decoder('5');
        Master_Send(0x800024A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 1);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 8:
        CS_Decoder('6');
        Master_Send(0x800024A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 9:
        CS_Decoder('6');
        Master_Send(0x800044A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 10:
        CS_Decoder('6');
        Master_Send(0x800014A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 11:
        CS_Decoder('6');
        Master_Send(0x80000CA0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 1);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 12:
        CS_Decoder('7');
        Master_Send(0x800024A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 13:
        CS_Decoder('7');
        Master_Send(0x800044A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    case 14:
        CS_Decoder('7');
        Master_Send(0x800014A0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 1);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 1);
        break;
    case 15:
        CS_Decoder('7');
        Master_Send(0x80000CA0);
        GPIO_Set(GPIO_S3, g_uiS3Port, g_ucS3Pin, 0);
        GPIO_Set(GPIO_S2, g_uiS2Port, g_ucS2Pin, 0);
        GPIO_Set(GPIO_S1, g_uiS1Port, g_ucS1Pin, 0);
        GPIO_Set(GPIO_S0, g_uiS0Port, g_ucS0Pin, 0);
        break;
    }
    TX(l_ucTxChannel);
    GPIO_LedOff(LED1);
}

void Tx_Pulse()
{
    GPIO_Set(GPIO_P0, g_uiP0Port, g_ucP0Pin, 1);
    GPIO_Set(GPIO_P0, g_uiP0Port, g_ucP0Pin, 0);
}

void Radar_Init()
{
    VCO_Init(0);
    //VCO_Init(1);
    PLL_Init(0);
    //PLL_Init(1);
    RF_Switch(0);
    RX_Init_Off();
}
