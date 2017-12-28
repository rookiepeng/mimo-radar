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
#include <strings.h>

// simplelink includes
#include "simplelink.h"
#include "wlan.h"

// driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"

#include "rom.h"
#include "rom_map.h"

#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
//#include "uart.h"

// common interface includes
#include "udma_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "gpio_if.h"

// ti-rtos includes
#include "osi.h"

#include "app_conf.h"
#include "tcpudp.h"
#include "spi_conf.h"
#include "gpio_conf.h"
#include "radar.h"
#include "adc_conf.h"

unsigned long g_ulDestinationIp = IP_ADDR; // Client IP address
//volatile unsigned long g_ulPacketCount = PACKET_COUNT;
char g_cBsdBuf[BUF_SIZE];

extern unsigned char g_ulStatus; //SimpleLink Status

int iLocalAddrSize;
SlSockAddrIn_t sClientTCPAddr;
SlSockAddrIn_t sClientUDPAddr;
int iClientAddrSize;
int iClientTCPSockID;
int iClientUDPSockID;

//****************************************************************************
//
//! \brief Opening a TCP server side socket and receiving data
//!
//! This function opens a TCP socket in Listen mode and waits for an incoming
//!    TCP connection.
//! If a socket connection is established then the function will try to read
//!    1000 TCP packets from the connected client.
//!
//! \param[in] port number on which the server will be listening on
//!
//! \return     0 on success, -1 on error.
//!
//! \note   This function will wait for an incoming connection till
//!                     one is established
//
//****************************************************************************
void BsdTcpServer(unsigned short usPort)
{
    SlSockAddrIn_t sLocalAddr;
    int iSockID;
    int iStatus;
    long lNonBlocking = 0; // 1 non blocking, 0 blocking
    //int iBufLen;

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)usPort);
    sLocalAddr.sin_addr.s_addr = 0;

    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
    if (iSockID < 0)
    {
// error
#ifndef NOTERM
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
#endif
    }

    iLocalAddrSize = sizeof(SlSockAddrIn_t);

    // binding the TCP socket to the TCP server address
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, iLocalAddrSize);
    if (iStatus < 0)
    {
        // error
        sl_Close(iSockID);
#ifndef NOTERM
        ASSERT_ON_ERROR(BIND_ERROR);
#endif
    }

    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, 0);
    if (iStatus < 0)
    {
        sl_Close(iSockID);
#ifndef NOTERM
        ASSERT_ON_ERROR(LISTEN_ERROR);
#endif
    }

    // setting socket option to make the socket as blocking
    iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
                            &lNonBlocking, sizeof(lNonBlocking));
    if (iStatus < 0)
    {
        sl_Close(iSockID);
#ifndef NOTERM
        ASSERT_ON_ERROR(SOCKET_OPT_ERROR);
#endif
    }

    // waiting for an incoming TCP connection
    while (1)
    {
        iClientTCPSockID = SL_EAGAIN;
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
        iClientTCPSockID = sl_Accept(iSockID, (struct SlSockAddr_t *)&sClientTCPAddr,
                               (SlSocklen_t *)&iClientAddrSize);
        if (iClientTCPSockID < 0)
        {
            // error
            sl_Close(iClientTCPSockID);
            sl_Close(iSockID);
#ifndef NOTERM
            ASSERT_ON_ERROR(ACCEPT_ERROR);
#endif
        }

        BsdUdpClient(1234);

        GPIO_LedOn(LED0);

        while (iClientTCPSockID > 0)
        {
            iStatus = sl_Recv(iClientTCPSockID, g_cBsdBuf, BUF_SIZE, 0);
            if (iStatus <= 0)
            {
                // error
                sl_Close(iClientTCPSockID);
                iClientTCPSockID = SL_EAGAIN;
//sl_Close(iSockID);
#ifndef NOTERM
                ASSERT_ON_ERROR(RECV_ERROR);
#endif
            }

            CommandProcess();
        }
        GPIO_LedOff(LED0);
    }
}

void UpdateUDPClientIP(unsigned long ip_addr, unsigned short port)
{
    sClientUDPAddr.sin_family = SL_AF_INET;
    sClientUDPAddr.sin_port = sl_Htons((unsigned short)port);
    sClientUDPAddr.sin_addr.s_addr = htonl(ip_addr);
}

//****************************************************************************
//
//! \brief Opening a UDP client side socket and sending data
//!
//! This function opens a UDP socket and tries to connect to a Server IP_ADDR
//!    waiting on port PORT_NUM.
//!    Then the function will send 1000 UDP packets to the server.
//!
//! \param[in]  port number on which the server will be listening on
//!
//! \return    0 on success, -1 on Error.
//
//****************************************************************************
int BsdUdpClient(unsigned short usPort)
{
    // creating a UDP socket
    iClientUDPSockID = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, 0);
    if (iClientUDPSockID < 0)
    {
// error
#ifndef NOTERM
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
#endif
    }

    return SUCCESS;
}

int UDP_send(char *g_ulBuffer, short sBufLen)
{
    int iStatus;
    // sending packet
    iStatus = sl_SendTo(iClientUDPSockID, g_ulBuffer, sBufLen, 0,
                        (SlSockAddr_t *)&sClientUDPAddr, iClientAddrSize);
    if (iStatus <= 0)
    {
        // error
        sl_Close(iClientUDPSockID);
#ifndef NOTERM
            ASSERT_ON_ERROR(SEND_ERROR);
#endif
    }

    //closing the socket after sending 1000 packets
    //sl_Close(iClientUDPSockID);

    return SUCCESS;
}

int TCP_send(char *g_ulBuffer, short sBufLen)
{
    int iStatus;
    iStatus = sl_Send(iClientTCPSockID, g_ulBuffer, sBufLen, 0);
    if (iStatus <= 0)
    {
        // error
        sl_Close(iClientTCPSockID);
        //sl_Close(iSockID);
        iClientTCPSockID = SL_EAGAIN;
#ifndef NOTERM
        ASSERT_ON_ERROR(RECV_ERROR);
#endif
    }

    return SUCCESS;
}

void CommandProcess()
{
    unsigned int iCounter;
    // Port and Pin for RF switches
    extern unsigned int g_uiD1Port, g_uiD2Port, g_uiD3Port, g_uiD4Port, g_uiStorePort;
    extern unsigned char g_ucD1Pin, g_ucD2Pin, g_ucD3Pin, g_ucD4Pin, g_ucStorePin;
    if (strcasecmp(g_cBsdBuf, "LED0 ON") == 0)
    {
        GPIO_LedOn(LED0);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "LED0 OFF") == 0)
    {
        GPIO_LedOff(LED0);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "LED1 ON") == 0)
    {
        GPIO_LedOn(LED1);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "LED1 OFF") == 0)
    {
        GPIO_LedOff(LED1);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strncasecmp(g_cBsdBuf, "SPI", 3) == 0)
    {
        CS_Decoder(g_cBsdBuf[3]);
        Master_Send(strtoul(&g_cBsdBuf[5], NULL, 16));
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "VCO0 INIT") == 0)
    {
        VCO_Init(0);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "VCO1 INIT") == 0)
    {
        VCO_Init(1);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "PLL0 INIT") == 0)
    {
        PLL_Init(0);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "PLL1 INIT") == 0)
    {
        PLL_Init(1);
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strncasecmp(g_cBsdBuf, "TXSW", 4) == 0)
    {
        RF_Switch(strtoul(&g_cBsdBuf[5], NULL, 16));
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "TX OFF") == 0)
    {
        TX_Off();
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strncasecmp(g_cBsdBuf, "TX", 2) == 0)
    {
        TX(strtoul(&g_cBsdBuf[3], NULL, 16));
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "RX INIT") == 0)
    {
        RX_Init_Off();
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "RX OFF") == 0)
    {
        RX_Init_Off();
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strncasecmp(g_cBsdBuf, "RX", 2) == 0)
    {
        RX(strtoul(&g_cBsdBuf[3], NULL, 16));
        TCP_send(g_cBsdBuf, strlen(g_cBsdBuf));
    }
    else if (strcasecmp(g_cBsdBuf, "UDP") == 0)
    {
        char temp[]="abcdefghijklmnopqrstuvwxyz";
        UDP_send(temp, 26);
    }
    else if (strcasecmp(g_cBsdBuf, "ADC START") == 0)
    {
        InitAdcDma();
    }

    for (iCounter = 0; iCounter < BUF_SIZE; iCounter++)
    {
        g_cBsdBuf[iCounter] = 0;
    }
}

//****************************************************************************
//
//! \brief Opening a UDP server side socket and receiving data
//!
//! This function opens a UDP socket in Listen mode and waits for an incoming
//! UDP connection.
//!    If a socket connection is established then the function will try to
//!    read 1000 UDP packets from the connected client.
//!
//! \param[in]          port number on which the server will be listening on
//!
//! \return             0 on success, Negative value on Error.
//
//****************************************************************************
/*int BsdUdpServer(unsigned short usPort)
{
    SlSockAddrIn_t sAddr;
    SlSockAddrIn_t sLocalAddr;
    int iCounter;
    int iAddrSize;
    int iSockID;
    int iStatus;
    //long lLoopCount = 0;
    short sTestBufLen;

    // filling the buffer
    for (iCounter = 0; iCounter < BUF_SIZE; iCounter++)
    {
        g_cBsdBuf[iCounter] = 'a';
    }

    sTestBufLen = BUF_SIZE;
    //filling the UDP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)usPort);
    sLocalAddr.sin_addr.s_addr = 0;

    sAddr.sin_family = SL_AF_INET;
    sAddr.sin_port = sl_Htons((unsigned short)usPort);
    sAddr.sin_addr.s_addr = sl_Htonl((unsigned int)g_ulDestinationIp);

    iAddrSize = sizeof(SlSockAddrIn_t);

    // creating a UDP socket
    iSockID = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, 0);
    if (iSockID < 0)
    {
// error
#ifndef NOTERM
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
#endif
    }

    // binding the UDP socket to the UDP server address
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, iAddrSize);
    if (iStatus < 0)
    {
        // error
        sl_Close(iSockID);
#ifndef NOTERM
        ASSERT_ON_ERROR(BIND_ERROR);
#endif
    }

    // no listen or accept is required as UDP is connectionless protocol
    /// waits for 1000 packets from a UDP client
    while (1)
    {
        iStatus = sl_RecvFrom(iSockID, g_cBsdBuf, sTestBufLen, 0,
                              (SlSockAddr_t *)&sAddr, (SlSocklen_t *)&iAddrSize);

        if (iStatus < 0)
        {
            // error
            sl_Close(iSockID);
#ifndef NOTERM
            ASSERT_ON_ERROR(RECV_ERROR);
#endif
        }

        //UART_PRINT("Recieved %d \n\r", g_cBsdBuf[0]);

        iStatus = sl_SendTo(iSockID, g_cBsdBuf, sTestBufLen, 0,
                            (SlSockAddr_t *)&sAddr, iAddrSize);

        if (iStatus < 0)
        {
            // error
            sl_Close(iSockID);
#ifndef NOTERM
            ASSERT_ON_ERROR(SEND_ERROR);
#endif
        }

        //lLoopCount++;
    }

    //closing the socket after receiving 1000 packets
    //sl_Close(iSockID);

    //return SUCCESS;
}*/


//****************************************************************************
//
//! \brief Opening a TCP client side socket and sending data
//!
//! This function opens a TCP socket and tries to connect to a Server IP_ADDR
//!    waiting on port PORT_NUM.
//!    If the socket connection is successful then the function will send 1000
//! TCP packets to the server.
//!
//! \param[in]      port number on which the server will be listening on
//!
//! \return    0 on success, -1 on Error.
//
//****************************************************************************
/*int BsdTcpClient(unsigned short usPort)
{
    int iCounter;
    short sTestBufLen;
    SlSockAddrIn_t sAddr;
    int iAddrSize;
    int iSockID;
    int iStatus;
    long lLoopCount = 0;

    // filling the buffer
    for (iCounter = 0; iCounter < BUF_SIZE; iCounter++)
    {
        g_cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    sTestBufLen = BUF_SIZE;

    //filling the TCP server socket address
    sAddr.sin_family = SL_AF_INET;
    sAddr.sin_port = sl_Htons((unsigned short)usPort);
    sAddr.sin_addr.s_addr = sl_Htonl((unsigned int)g_ulDestinationIp);

    iAddrSize = sizeof(SlSockAddrIn_t);

    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
    if (iSockID < 0)
    {
#ifndef NOTERM
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
#endif
    }

    // connecting to TCP server
    iStatus = sl_Connect(iSockID, (SlSockAddr_t *)&sAddr, iAddrSize);
    if (iStatus < 0)
    {
        // error
        sl_Close(iSockID);
#ifndef NOTERM
        ASSERT_ON_ERROR(CONNECT_ERROR);
#endif
    }

    // sending multiple packets to the TCP server
    while (lLoopCount < g_ulPacketCount)
    {
        // sending packet
        iStatus = sl_Send(iSockID, g_cBsdBuf, sTestBufLen, 0);
        if (iStatus < 0)
        {
            // error
            sl_Close(iSockID);
#ifndef NOTERM
            ASSERT_ON_ERROR(SEND_ERROR);
#endif
        }
        lLoopCount++;
    }

    //Report("Sent %u packets successfully\n\r", g_ulPacketCount);

    iStatus = sl_Close(iSockID);
//closing the socket after sending 1000 packets
#ifndef NOTERM
    ASSERT_ON_ERROR(iStatus);
#endif

    return SUCCESS;
}*/
