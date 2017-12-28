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

#include "simplelink_conf.h"
#include "tcpudp.h"
#include "wifi.h"
#include "app_conf.h"

extern unsigned long g_uiIpAddress;                        //Device IP address
extern unsigned char g_ucConnectionSSID[SSID_LEN_MAX + 1]; //Connection SSID
extern unsigned char g_ucConnectionBSSID[BSSID_LEN_MAX];   //Connection BSSID

extern unsigned char g_ulStatus; //SimpleLink Status
extern unsigned long g_ulStaIp;
extern unsigned long g_ulPingPacketsRecv;
extern unsigned long g_uiGatewayIP;

extern OsiTaskHandle g_WifiTask;

long lRetVal = -1;

#define OSI_STACK_SIZE 2048

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if (pWlanEvent == NULL)
    {
#ifndef NOTERM
        UART_PRINT("Null pointer\n\r");
#endif
        LOOP_FOREVER();
    }
    switch (pWlanEvent->Event)
    {
    case SL_WLAN_CONNECT_EVENT:
    {
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

        //
        // Information about the connected AP (like name, MAC etc) will be
        // available in 'slWlanConnectAsyncResponse_t'
        // Applications can use it if required
        //
        //  slWlanConnectAsyncResponse_t *pEventData = NULL;
        // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
        //

        // Copy new connection SSID and BSSID to global parameters
        memcpy(g_ucConnectionSSID, pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_name,
               pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
        memcpy(g_ucConnectionBSSID,
               pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
               SL_BSSID_LENGTH);

#ifndef NOTERM
        UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s , "
                   "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                   g_ucConnectionSSID, g_ucConnectionBSSID[0],
                   g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
                   g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
                   g_ucConnectionBSSID[5]);
#endif
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
        slWlanConnectAsyncResponse_t *pEventData = NULL;

        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        // If the user has initiated 'Disconnect' request,
        //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION
        if (SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
        {
#ifndef NOTERM
            UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s, "
                       "BSSID: %x:%x:%x:%x:%x:%x on application's "
                       "request \n\r",
                       g_ucConnectionSSID, g_ucConnectionBSSID[0],
                       g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
                       g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
                       g_ucConnectionBSSID[5]);
#endif
        }
        else
        {
#ifndef NOTERM
            UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s, "
                       "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                       g_ucConnectionSSID, g_ucConnectionBSSID[0],
                       g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
                       g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
                       g_ucConnectionBSSID[5]);
#endif
        }
        memset(g_ucConnectionSSID, 0, sizeof(g_ucConnectionSSID));
        memset(g_ucConnectionBSSID, 0, sizeof(g_ucConnectionBSSID));
    }
    break;

    case SL_WLAN_STA_CONNECTED_EVENT:
    {
        // when device is in AP mode and any client connects to device cc3xxx
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

#ifndef NOTERM
        UART_PRINT("[AP EVENT] Device connected \n\r");
#endif

        //
        // Information about the connected client (like SSID, MAC etc) will be
        // available in 'slPeerInfoAsyncResponse_t' - Applications
        // can use it if required
        //
        // slPeerInfoAsyncResponse_t *pEventData = NULL;
        // pEventData = &pSlWlanEvent->EventData.APModeStaConnected;
        //
    }
    break;

    case SL_WLAN_STA_DISCONNECTED_EVENT:
    {
        // when client disconnects from device (AP)
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

#ifndef NOTERM
        UART_PRINT("[AP EVENT] Device disconnected \n\r");
#endif

        BoardReboot();

        //
        // Information about the connected client (like SSID, MAC etc) will
        // be available in 'slPeerInfoAsyncResponse_t' - Applications
        // can use it if required
        //
        // slPeerInfoAsyncResponse_t *pEventData = NULL;
        // pEventData = &pSlWlanEvent->EventData.APModestaDisconnected;
        //
    }
    break;

    default:
    {
#ifndef NOTERM
        UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                   pWlanEvent->Event);
#endif
    }
    break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if (pNetAppEvent == NULL)
    {
#ifndef NOTERM
        UART_PRINT("Null pointer\n\r");
#endif
        LOOP_FOREVER();
    }

    switch (pNetAppEvent->Event)
    {
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
    {
        SlIpV4AcquiredAsync_t *pEventData = NULL;

        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

        //Ip Acquired Event Data
        pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

        //Gateway IP address
        g_uiIpAddress = pEventData->ip;

#ifndef NOTERM
        UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                   "Gateway=%d.%d.%d.%d\n\r",
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 3),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 2),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 1),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 0),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 3),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 2),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 1),
                   SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 0));
#endif
    }
    break;

    case SL_NETAPP_IPV6_IPACQUIRED_EVENT:
    {
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);
    }
    break;

    case SL_NETAPP_IP_LEASED_EVENT:
    {
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

        g_ulStaIp = (pNetAppEvent)->EventData.ipLeased.ip_address;

#ifndef NOTERM
        UART_PRINT("[NETAPP EVENT] IP Leased to Client: IP=%d.%d.%d.%d , ",
                   SL_IPV4_BYTE(g_ulStaIp, 3), SL_IPV4_BYTE(g_ulStaIp, 2),
                   SL_IPV4_BYTE(g_ulStaIp, 1), SL_IPV4_BYTE(g_ulStaIp, 0));
#endif

        UpdateUDPClientIP(g_ulStaIp,1234);

        //Speaker Detected - Add Client
        //        g_UdpSock.Client.sin_family = AF_INET;
        //        g_UdpSock.Client.sin_addr.s_addr = htonl(g_ulStaIp);
        //        g_UdpSock.Client.sin_port = htons(AUDIO_PORT);
        //        g_UdpSock.iClientLength = sizeof(g_UdpSock.Client);
    }
    break;

    case SL_NETAPP_IP_RELEASED_EVENT:
    {
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

#ifndef NOTERM
        UART_PRINT("[NETAPP EVENT] IP Released for Client: IP=%d.%d.%d.%d , ",
                   SL_IPV4_BYTE(g_ulStaIp, 3), SL_IPV4_BYTE(g_ulStaIp, 2),
                   SL_IPV4_BYTE(g_ulStaIp, 1), SL_IPV4_BYTE(g_ulStaIp, 0));
#endif
    }
    break;

    default:
    {
#ifndef NOTERM
        UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                   pNetAppEvent->Event);
#endif
    }
    break;
    }
}

//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    if (pDevEvent == NULL)
    {
#ifndef NOTERM
        UART_PRINT("Null pointer\n\r");
#endif
        LOOP_FOREVER();
    }

//
// Most of the general errors are not FATAL are are to be handled
// appropriately by the application
#ifndef NOTERM
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
#endif
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if (pSock == NULL)
    {
        return;
    }

    switch (pSock->Event)
    {
    case SL_SOCKET_TX_FAILED_EVENT:
        switch (pSock->socketAsyncEvent.SockTxFailData.status)
        {
        case SL_ECLOSE:
#ifndef NOTERM
            UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                       "failed to transmit all queued packets\n\n",
                       pSock->socketAsyncEvent.SockTxFailData.sd);
#endif
            break;
        default:
#ifndef NOTERM
            UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                       "(%d) \n\n",
                       pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
#endif
            break;
        }
        break;

    default:
#ifndef NOTERM
        UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n", pSock->Event);
#endif
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles ping report events
//!
//! \param[in]     pPingReport - Ping report statistics
//!
//! \return None
//
//****************************************************************************
void SimpleLinkPingReport(SlPingReport_t *pPingReport)
{
    SET_STATUS_BIT(g_ulStatus, STATUS_BIT_PING_DONE);
    g_ulPingPacketsRecv = pPingReport->PacketsReceived;
}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************
