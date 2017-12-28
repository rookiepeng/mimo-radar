/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

//driverlib includes
#include "hw_types.h"

#include "rom.h"
#include "rom_map.h"

#include "utils.h"

#include "gpio_if.h"
#ifndef NOTERM
#include "uart_if.h"
#endif

// ti-rtos includes
#include "osi.h"

//common interface includes
#include "common.h"

//App Includes
#include "simplelink_conf.h"
#include "app_conf.h"
#include "wifi.h"
#include "tcpudp.h"

unsigned long g_uiIpAddress = 0;                    //Device IP address
unsigned char g_ucConnectionSSID[SSID_LEN_MAX + 1]; //Connection SSID
unsigned char g_ucConnectionBSSID[BSSID_LEN_MAX];   //Connection BSSID

extern unsigned char g_ulStatus; //SimpleLink Status
extern unsigned long g_ulStaIp;
extern unsigned long g_ulPingPacketsRecv;
extern unsigned long g_uiGatewayIP;

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull ver = {0};
    _WlanRxFilterOperationCommandBuff_t RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
#ifndef NOTERM
    ASSERT_ON_ERROR(lMode);
#endif

    // If the device is not in station-mode, try configuring it in station-mode
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while (!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
                _SlNonOsMainLoopTask();
#endif
            }
        }

        // Switch to STA role and restart
        lRetVal = sl_WlanSetMode(ROLE_STA);
#ifndef NOTERM
        ASSERT_ON_ERROR(lRetVal);
#endif

        lRetVal = sl_Stop(0xFF);
#ifndef NOTERM
        ASSERT_ON_ERROR(lRetVal);
#endif

        lRetVal = sl_Start(0, 0, 0);
#ifndef NOTERM
        ASSERT_ON_ERROR(lRetVal);
#endif

        // Check if the device is in station again
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
                        &ucConfigLen, (unsigned char *)(&ver));
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT("Host Driver Version: %s\n\r", SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
               ver.NwpVersion[0], ver.NwpVersion[1], ver.NwpVersion[2], ver.NwpVersion[3],
               ver.ChipFwAndPhyVersion.FwVersion[0], ver.ChipFwAndPhyVersion.FwVersion[1],
               ver.ChipFwAndPhyVersion.FwVersion[2], ver.ChipFwAndPhyVersion.FwVersion[3],
               ver.ChipFwAndPhyVersion.PhyVersion[0], ver.ChipFwAndPhyVersion.PhyVersion[1],
               ver.ChipFwAndPhyVersion.PhyVersion[2], ver.ChipFwAndPhyVersion.PhyVersion[3]);
#endif

    // Set connection policy to Auto + SmartConfig
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
                               SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
    // other return-codes
    lRetVal = sl_WlanDisconnect();
    if (0 == lRetVal)
    {
        // Wait
        while (IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
            _SlNonOsMainLoopTask();
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &ucVal);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN, ucConfigOpt, NULL, 0);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
                         WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                                 sizeof(_WlanRxFilterOperationCommandBuff_t));
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
#ifndef NOTERM
    ASSERT_ON_ERROR(lRetVal);
#endif

    InitializeAppVariables();

    return lRetVal; // Success
}

//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables()
{
    g_ulStatus = 0;
    g_uiIpAddress = 0;
    memset(g_ucConnectionSSID, 0, sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID, 0, sizeof(g_ucConnectionBSSID));

    g_ulStaIp = 0;
    g_ulPingPacketsRecv = 0;
    g_uiGatewayIP = 0;
}

#ifdef MULTICAST
//*****************************************************************************
//
//! Add to Multicast Group to receive Audio Stream
//!
//! \param none
//!
//! \return  0 - Success
//!            -1 - Failure
//
//*****************************************************************************
long ReceiveMulticastPacket()
{
    long lRetVal = -1;
    SlSockIpMreq mreq;
    memset(&mreq, 0, sizeof(SlSockIpMreq));

    // set interface
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    mreq.imr_multiaddr.s_addr = ADDR_MULTICAST_GROUP;

    // do membership call
    lRetVal = setsockopt(g_UdpSock.iSockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         &mreq, sizeof(SlSockIpMreq));
    ASSERT_ON_ERROR(lRetVal);

    return SUCCESS;
}
#endif

//****************************************************************************
//
//! Confgiures the mode in which the device will work
//!
//! \param iMode is the current mode of the device
//!
//! This function
//!    1. prompt user for desired configuration and accordingly configure the
//!          networking mode(STA or AP).
//!       2. also give the user the option to configure the ssid name in case of
//!       AP mode.
//!
//! \return sl_start return value(int).
//
//****************************************************************************
static int ConfigureMode(int iMode)
{
#ifndef NOTERM
    long lRetVal = -1;

    lRetVal = sl_WlanSetMode(ROLE_AP);
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, strlen(SSID),
                         (unsigned char *)SSID);
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT("Device is configured in AP mode\n\r");

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
#endif

#ifdef NOTERM
    sl_WlanSetMode(ROLE_AP);
    sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, strlen(SSID),
               (unsigned char *)SSID);
    // Restart Network processor
    sl_Stop(SL_STOP_TIMEOUT);
#endif

    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);

    return sl_Start(NULL, NULL, NULL);
}

//****************************************************************************
//
//! Get Ssid name form the user over UART
//!
//! \param pcSsidName is a pointer to the array which will contain the ssid name
//!
//! This function
//!    1. gets the ssid name string over uart
//!
//! \return iRetVal is the length of the ssid(user input).
//
//****************************************************************************
/*static int GetSsidName(char *pcSsidName, unsigned int uiMaxLen)
{
    char ucRecvdAPDetails = 0;
    int iRetVal = 0;
    char acCmdStore[128];
    do
    {
        ucRecvdAPDetails = 0;

        //
        // Get the AP name to connect over the UART
        //
        iRetVal = GetCmd(acCmdStore, sizeof(acCmdStore));
        if (iRetVal > 0)
        {
            // remove start/end spaces if any
            iRetVal = TrimSpace(acCmdStore);

            //
            // Parse the AP name
            //
            strncpy(pcSsidName, acCmdStore, iRetVal);
            if (pcSsidName != NULL)
            {
                ucRecvdAPDetails = 1;
                pcSsidName[iRetVal] = '\0';
            }
        }
    } while (ucRecvdAPDetails == 0);

    return (iRetVal);
}*/

//****************************************************************************
//
//!    \brief start simplelink, wait for the sta to connect to the device and
//!        run the ping test for that sta
//!
//!    \param  pvparameters is the pointer to the list of parameters that can be
//!         passed to the task while creating it
//!
//!    \return None
//
//****************************************************************************
void WlanAPMode(void *pvParameters)
{
    //int iTestResult = 0;
    unsigned char ucDHCP;
    long lRetVal = -1;

    InitializeAppVariables();

    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if (lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
        {
#ifndef NOTERM
            UART_PRINT("Failed to configure the device in its default state \n\r");
#endif
        }

        LOOP_FOREVER();
    }

#ifndef NOTERM
    UART_PRINT("Device is configured in default state \n\r");
#endif

    // Asumption is that the device is configured in station mode already
    // and it is in its default state
    lRetVal = sl_Start(NULL, NULL, NULL);

    if (lRetVal < 0)
    {
#ifndef NOTERM
        UART_PRINT("Failed to start the device \n\r");
#endif
        LOOP_FOREVER();
    }

#ifndef NOTERM
    UART_PRINT("Device started as STATION \n\r");
#endif

    // Configure the networking mode and ssid name(for AP mode)
    if (lRetVal != ROLE_AP)
    {
        if (ConfigureMode(lRetVal) != ROLE_AP)
        {
#ifndef NOTERM
            UART_PRINT("Unable to set AP mode, exiting Application...\n\r");
#endif
            sl_Stop(SL_STOP_TIMEOUT);
            LOOP_FOREVER();
        }
    }

    while (!IS_IP_ACQUIRED(g_ulStatus))
    {
        //looping till ip is acquired
    }

    unsigned char len = sizeof(SlNetCfgIpV4Args_t);
    SlNetCfgIpV4Args_t ipV4 = {0};

    // get network configuration
    lRetVal = sl_NetCfgGet(SL_IPV4_AP_P2P_GO_GET_INFO, &ucDHCP, &len,
                           (unsigned char *)&ipV4);
    if (lRetVal < 0)
    {
#ifndef NOTERM
        UART_PRINT("Failed to get network configuration \n\r");
#endif
        LOOP_FOREVER();
    }

#ifndef NOTERM
    UART_PRINT("Connect a client to Device\n\r");
#endif
    while (!IS_IP_LEASED(g_ulStatus))
    {
        //wating for the client to connect
    }
#ifndef NOTERM
    UART_PRINT("Client is connected to Device\n\r");
#endif

    BsdTcpServer(1234);
}
