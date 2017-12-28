/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#ifndef APP_CONF_H_
#define APP_CONF_H_

#if defined(gcc)
extern void (*const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

#define APP_NAME "PROJ-ACQR"
#define APPLICATION_VERSION "1.0.0"
#define OSI_STACK_SIZE 2048

#define NOTERM 1

// Application specific status/error codes
typedef enum {
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    SOCKET_CREATE_ERROR = -0x7D0,
    BIND_ERROR = SOCKET_CREATE_ERROR - 1,
    LISTEN_ERROR = BIND_ERROR - 1,
    SOCKET_OPT_ERROR = LISTEN_ERROR - 1,
    CONNECT_ERROR = SOCKET_OPT_ERROR - 1,
    ACCEPT_ERROR = CONNECT_ERROR - 1,
    SEND_ERROR = ACCEPT_ERROR - 1,
    RECV_ERROR = SEND_ERROR - 1,
    SOCKET_CLOSE = RECV_ERROR - 1,
    LAN_CONNECTION_FAILED = SOCKET_CLOSE - 1,
    CLIENT_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = SOCKET_CLOSE - 1,
    STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

void BoardInit(void);
void BoardReboot();

#endif /* APP_CONF_H_ */
