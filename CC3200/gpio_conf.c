/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "interrupt.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"

// OS includes
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED)
#include <stdlib.h>
#include "osi.h"
#endif

// Common interface include
#include "gpio_if.h"

#include "gpio_conf.h"

static unsigned long ulReg[] =
    {
        GPIOA0_BASE,
        GPIOA1_BASE,
        GPIOA2_BASE,
        GPIOA3_BASE,
        GPIOA4_BASE};

// Port and Pin for decoder control
unsigned int g_uiAPort = 0, g_uiBPort = 0, g_uiCPort = 0;
unsigned char g_ucAPin, g_ucBPin, g_ucCPin;

// Port and Pin for LEDs
unsigned int g_uiLED0Port = 0, g_uiLED1Port = 0;
unsigned char g_ucLED0Pin, g_ucLED1Pin;

// Port and Pin for RF switches
unsigned int g_uiD1Port = 0, g_uiD2Port = 0, g_uiD3Port = 0, g_uiD4Port = 0, g_uiStorePort = 0;
unsigned char g_ucD1Pin, g_ucD2Pin, g_ucD3Pin, g_ucD4Pin, g_ucStorePin;

// Port and Pin for baseband switch
unsigned int g_uiS0Port = 0, g_uiS1Port = 0, g_uiS2Port = 0, g_uiS3Port = 0;
unsigned char g_ucS0Pin, g_ucS1Pin, g_ucS2Pin, g_ucS3Pin;

// TX pulse
unsigned int g_uiP0Port = 0;
unsigned char g_ucP0Pin;

//****************************************************************************
//
//! Get the port and pin of a given GPIO
//!
//! \param ucPin is the pin to be set-up as a GPIO (0:39)
//! \param puiGPIOPort is the pointer to store GPIO port address return value
//! \param pucGPIOPin is the pointer to store GPIO pin return value
//!
//! This function
//!    1. Return the GPIO port address and pin for a given external pin number
//!
//! \return None.
//
//****************************************************************************
void GPIO_GetPortNPin(unsigned char ucPin,
                      unsigned int *puiGPIOPort,
                      unsigned char *pucGPIOPin)
{
    // Get the GPIO pin from the external Pin number
    *pucGPIOPin = 1 << (ucPin % 8);

    // Get the GPIO port from the external Pin number
    *puiGPIOPort = (ucPin / 8);
    *puiGPIOPort = ulReg[*puiGPIOPort];
}

//****************************************************************************
//
//! Set a value to the specified GPIO pin
//!
//! \param ucPin is the GPIO pin to be set (0:39)
//! \param uiGPIOPort is the GPIO port address
//! \param ucGPIOPin is the GPIO pin of the specified port
//! \param ucGPIOValue is the value to be set
//!
//! This function
//!    1. Sets a value to the specified GPIO pin
//!
//! \return None.
//
//****************************************************************************
void GPIO_Set(unsigned char ucPin, unsigned int uiGPIOPort, unsigned char ucGPIOPin, unsigned char ucGPIOValue)
{
    // Set the corresponding bit in the bitmask
    ucGPIOValue = ucGPIOValue << (ucPin % 8);

    // Invoke the API to set the value
    MAP_GPIOPinWrite(uiGPIOPort, ucGPIOPin, ucGPIOValue);
}

//****************************************************************************
//
//! Set a value to the specified GPIO pin
//!
//! \param ucPin is the GPIO pin to be set (0:39)
//! \param uiGPIOPort is the GPIO port address
//! \param ucGPIOPin is the GPIO pin of the specified port
//!
//! This function
//!    1. Gets a value of the specified GPIO pin
//!
//! \return value of the GPIO pin
//
//****************************************************************************
unsigned char GPIO_Get(unsigned char ucPin, unsigned int uiGPIOPort, unsigned char ucGPIOPin)
{
    unsigned char ucGPIOValue;
    long lGPIOStatus;

    // Invoke the API to Get the value
    lGPIOStatus = MAP_GPIOPinRead(uiGPIOPort, ucGPIOPin);

    // Set the corresponding bit in the bitmask
    ucGPIOValue = lGPIOStatus >> (ucPin % 8);
    return ucGPIOValue;
}

//*****************************************************************************
//
//! GPIO Enable & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void GPIO_PinConfigure()
{
    GPIO_GetPortNPin(GPIO_A,
                     &g_uiAPort,
                     &g_ucAPin);
    GPIO_GetPortNPin(GPIO_B,
                     &g_uiBPort,
                     &g_ucBPin);
    GPIO_GetPortNPin(GPIO_C,
                     &g_uiCPort,
                     &g_ucCPin);

    GPIO_GetPortNPin(LED0,
                     &g_uiLED0Port,
                     &g_ucLED0Pin);
    GPIO_GetPortNPin(LED1,
                     &g_uiLED1Port,
                     &g_ucLED1Pin);

    GPIO_GetPortNPin(GPIO_D1,
                     &g_uiD1Port,
                     &g_ucD1Pin);
    GPIO_GetPortNPin(GPIO_D2,
                     &g_uiD2Port,
                     &g_ucD2Pin);
    GPIO_GetPortNPin(GPIO_D3,
                     &g_uiD3Port,
                     &g_ucD3Pin);
    GPIO_GetPortNPin(GPIO_D4,
                     &g_uiD4Port,
                     &g_ucD4Pin);
    GPIO_GetPortNPin(GPIO_STORE,
                     &g_uiStorePort,
                     &g_ucStorePin);

    GPIO_Set(GPIO_STORE, g_uiStorePort, g_ucStorePin, 0);

    GPIO_GetPortNPin(GPIO_S0,
                     &g_uiS0Port,
                     &g_ucS0Pin);
    GPIO_GetPortNPin(GPIO_S1,
                     &g_uiS1Port,
                     &g_ucS1Pin);
    GPIO_GetPortNPin(GPIO_S2,
                     &g_uiS2Port,
                     &g_ucS2Pin);
    GPIO_GetPortNPin(GPIO_S3,
                     &g_uiS3Port,
                     &g_ucS3Pin);

    GPIO_GetPortNPin(GPIO_P0,
                     &g_uiP0Port,
                     &g_ucP0Pin);

    GPIO_Set(GPIO_P0, g_uiP0Port, g_ucP0Pin, 0);

}

//*****************************************************************************
//
//! Get Peripheral Interrupt Number
//!
//! \param  uiGPIOPort is the GPIO port number
//!
//! \return Peripheral number
//
//*****************************************************************************
static unsigned char GetPeripheralIntNum(unsigned int uiGPIOPort)
{
    switch (uiGPIOPort)
    {
    case GPIOA0_BASE:
        return INT_GPIOA0;
    case GPIOA1_BASE:
        return INT_GPIOA1;
    case GPIOA2_BASE:
        return INT_GPIOA2;
    case GPIOA3_BASE:
        return INT_GPIOA3;
    default:
        return INT_GPIOA0;
    }
}

//*****************************************************************************
//
//! Initialize GPIO interrupt
//!
//! \param  interrupt handlers
//!
//! \return none
//
//*****************************************************************************
void GPIO_IF_Init(void (*SW2IntHandler)(void), void (*SW3IntHandler)(void))
{
    // SW3
    // Set Interrupt Type for GPIO
    MAP_GPIOIntTypeSet(GPIOA1_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);

// Register Interrupt handler
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED)
    // USE_TIRTOS: if app uses TI-RTOS (either networking/non-networking)
    // USE_FREERTOS: if app uses Free-RTOS (either networking/non-networking)
    // SL_PLATFORM_MULTI_THREADED: if app uses any OS + networking(simplelink)
    osi_InterruptRegister(INT_GPIOA1, (P_OSI_INTR_ENTRY)SW3IntHandler,
                          INT_PRIORITY_LVL_1);
#else
    MAP_IntPrioritySet(INT_GPIOA1, INT_PRIORITY_LVL_1);
    MAP_GPIOIntRegister(GPIOA1_BASE, SW3IntHandler);
#endif

    // Enable Interrupt
    MAP_GPIOIntClear(GPIOA1_BASE, GPIO_PIN_5);
    MAP_GPIOIntEnable(GPIOA1_BASE, GPIO_INT_PIN_5);

    // SW2
    // Set Interrupt Type for GPIO
    MAP_GPIOIntTypeSet(GPIOA2_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);

// Register Interrupt handler
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED)
    // USE_TIRTOS: if app uses TI-RTOS (either networking/non-networking)
    // USE_FREERTOS: if app uses Free-RTOS (either networking/non-networking)
    // SL_PLATFORM_MULTI_THREADED: if app uses any OS + networking(simplelink)
    osi_InterruptRegister(INT_GPIOA2, (P_OSI_INTR_ENTRY)SW2IntHandler,
                          INT_PRIORITY_LVL_1);
#else
    MAP_IntPrioritySet(INT_GPIOA2, INT_PRIORITY_LVL_1);
    MAP_GPIOIntRegister(GPIOA2_BASE, SW2IntHandler);
#endif

    // Enable Interrupt
    MAP_GPIOIntClear(GPIOA2_BASE, GPIO_PIN_6);
    MAP_GPIOIntEnable(GPIOA2_BASE, GPIO_INT_PIN_6);
}

//****************************************************************************
//
//! Configures the GPIO selected as input to generate interrupt on activity
//!
//! \param uiGPIOPort is the GPIO port address
//! \param ucGPIOPin is the GPIO pin of the specified port
//! \param uiIntType is the type of the interrupt (refer gpio.h)
//! \param pfnIntHandler is the interrupt handler to register
//!
//! This function
//!    1. Sets GPIO interrupt type
//!    2. Registers Interrupt handler
//!    3. Enables Interrupt
//!
//! \return None
//
//****************************************************************************
void GPIO_IF_ConfigureNIntEnable(unsigned int uiGPIOPort,
                                 unsigned char ucGPIOPin,
                                 unsigned int uiIntType,
                                 void (*pfnIntHandler)(void))
{
    // Set GPIO interrupt type
    MAP_GPIOIntTypeSet(uiGPIOPort, ucGPIOPin, uiIntType);

// Register Interrupt handler
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED)
    // USE_TIRTOS: if app uses TI-RTOS (either networking/non-networking)
    // USE_FREERTOS: if app uses Free-RTOS (either networking/non-networking)
    // SL_PLATFORM_MULTI_THREADED: if app uses any OS + networking(simplelink)
    osi_InterruptRegister(GetPeripheralIntNum(uiGPIOPort),
                          pfnIntHandler, INT_PRIORITY_LVL_1);

#else
    MAP_IntPrioritySet(GetPeripheralIntNum(uiGPIOPort), INT_PRIORITY_LVL_1);
    MAP_GPIOIntRegister(uiGPIOPort, pfnIntHandler);
#endif

    // Enable Interrupt
    MAP_GPIOIntClear(uiGPIOPort, ucGPIOPin);
    MAP_GPIOIntEnable(uiGPIOPort, ucGPIOPin);
}

//*****************************************************************************
//
//!  \brief Enables Push Button GPIO Interrupt
//!
//! \param[in] ucSwitch               Push Button Swich Enum - SW2,SW3
//!
//! \return none
//!
//
//*****************************************************************************
/*void GPIO_IF_EnableInterrupt(unsigned char ucSwitch)
{
    if (ucSwitch & SW2)
    {
        //Enable GPIO Interrupt
        MAP_GPIOIntClear(GPIOA2_BASE, GPIO_PIN_6);
        MAP_IntPendClear(INT_GPIOA2);
        MAP_IntEnable(INT_GPIOA2);
        MAP_GPIOIntEnable(GPIOA2_BASE, GPIO_PIN_6);
    }

    if (ucSwitch & SW3)
    {
        //Enable GPIO Interrupt
        MAP_GPIOIntClear(GPIOA1_BASE, GPIO_PIN_5);
        MAP_IntPendClear(INT_GPIOA1);
        MAP_IntEnable(INT_GPIOA1);
        MAP_GPIOIntEnable(GPIOA1_BASE, GPIO_PIN_5);
    }
}*/

//*****************************************************************************
//
//!  \brief Disables Push Button GPIO Interrupt
//!
//! \param[in] ucSwitch               Push Button Swich Enum - SW2,SW3
//!
//! \return none
//!
//
//*****************************************************************************
/*void GPIO_IF_DisableInterrupt(unsigned char ucSwitch)
{
    if (ucSwitch & SW2)
    {
        //Clear and Disable GPIO Interrupt
        MAP_GPIOIntDisable(GPIOA2_BASE, GPIO_PIN_6);
        MAP_GPIOIntClear(GPIOA2_BASE, GPIO_PIN_6);
        MAP_IntDisable(INT_GPIOA2);
    }

    if (ucSwitch & SW3)
    {
        //Clear and Disable GPIO Interrupt
        MAP_GPIOIntDisable(GPIOA1_BASE, GPIO_PIN_5);
        MAP_GPIOIntClear(GPIOA1_BASE, GPIO_PIN_5);
        MAP_IntDisable(INT_GPIOA1);
    }
}*/

//*****************************************************************************
//
//! \brief IO Interrupt handler
//!
//! \param None
//!
//! \return None
//!
//*****************************************************************************
/*void PushButtonHandlerSW2()
{
    unsigned long ulPinState = GPIOIntStatus(GPIOA2_BASE, 1);
    if (ulPinState & GPIO_PIN_6)
    {
        GPIO_IF_DisableInterrupt(SW2);
    }
    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    //Master_Send(g_ucRxBuff, g_ucTxBuff, 1024);
    GPIO_IF_EnableInterrupt(SW2);
}

void PushButtonHandlerSW3()
{
    unsigned long ulPinState = GPIOIntStatus(GPIOA1_BASE, 1);
    if (ulPinState & GPIO_PIN_5)
    {
        GPIO_IF_DisableInterrupt(SW3);
    }
    GPIO_IF_LedOn(MCU_ALL_LED_IND);
    //Master_Send(g_ucRxBuff, g_ucTxBuff, 1024);
    GPIO_IF_EnableInterrupt(SW3);
}*/

//*****************************************************************************
//
//! Turn LED On
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
//
//*****************************************************************************
void GPIO_LedOn(char ledNum)
{ /*
    switch (ledNum)
    {
    case LED0:
    {
        GPIO_Set(LED0, g_uiLED0Port, g_ucLED0Pin, 1);
        break;
    }
    case LED1:
    {
        GPIO_Set(LED1, g_uiLED1Port, g_ucLED1Pin, 1);
        break;
    }
    default:
        break;
    }*/
}

//*****************************************************************************
//
//! Turn LED Off
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
//
//*****************************************************************************
void GPIO_LedOff(char ledNum)
{ /*
    switch (ledNum)
    {
    case LED0:
    {
        // Switch OFF GREEN LED
        GPIO_Set(LED0, g_uiLED0Port, g_ucLED0Pin, 0);
        break;
    }
    case LED1:
    {
        // Switch OFF ORANGE LED
        GPIO_Set(LED1, g_uiLED1Port, g_ucLED1Pin, 0);
        break;
    }
    default:
        break;
    }*/
}

//*****************************************************************************
//
//!  \brief This function returns LED current Status
//!
//!  \param[in] ucGPIONum is the GPIO to which the LED is connected
//!                MCU_GREEN_LED_GPIO\MCU_ORANGE_LED_GPIO\MCU_RED_LED_GPIO
//!
//!
//!  \return 1: LED ON, 0: LED OFF
//
//*****************************************************************************
unsigned char GPIO_LedStatus(unsigned char ucGPIONum)
{
    unsigned char ucLEDStatus;
    switch (ucGPIONum)
    {
    case LED0:
    {
        ucLEDStatus = GPIO_Get(ucGPIONum, g_uiLED0Port, g_ucLED0Pin);
        break;
    }
    case LED1:
    {
        ucLEDStatus = GPIO_Get(ucGPIONum, g_uiLED1Port, g_ucLED1Pin);
        break;
    }
    default:
        ucLEDStatus = 0;
    }
    return ucLEDStatus;
}

//*****************************************************************************
//
//! Toggle the Led state
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Toggles a board LED
//
//*****************************************************************************
void GPIO_LedToggle(unsigned char ucLedNum)
{

    unsigned char ucLEDStatus = GPIO_LedStatus(ucLedNum);
    if (ucLEDStatus == 1)
    {
        GPIO_LedOff(ucLedNum);
    }
    else
    {
        GPIO_LedOn(ucLedNum);
    }
}
