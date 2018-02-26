/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#ifndef __WIFI_H__
#define __WIFI_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#define SSID "MIMO-RADAR"

//*****************************************************************************
//
// Define Packet Size, Rx and Tx Buffer
//
//*****************************************************************************
#define PACKET_SIZE 1024
#define PLAY_WATERMARK 30 * 1024
#define RECORD_BUFFER_SIZE 10 * PACKET_SIZE
#define PLAY_BUFFER_SIZE 70 * PACKET_SIZE

//
// Values for below macros shall be modified for setting the 'Ping' properties
//
//#define PING_INTERVAL 1000 /* In msecs */
//#define PING_TIMEOUT 3000  /* In msecs */
//#define PING_PKT_SIZE 20   /* In bytes */
//#define NO_OF_ATTEMPTS 3
//#define PING_FLAG 0

//*****************************************************************************
//
// UDP Socket Structure
//
//*****************************************************************************
typedef struct UDPSocket
{
    int iSockDesc;             //Socket FD
    struct sockaddr_in Server; //Address to Bind - INADDR_ANY for Local IP
    struct sockaddr_in Client; //Client Address or Multicast Group
    int iServerLength;
    int iClientLength;
} tUDPSocket;

//*****************************************************************************
//void SimpleLinkPingReport(SlPingReport_t *pPingReport);

static long ConfigureSimpleLinkToDefaultState();

static int ConfigureMode(int iMode);

void WlanAPMode(void *pvParameters);

static void InitializeAppVariables();

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //__WIFI_H__
