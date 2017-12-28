/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#ifndef SPI_CONF_H_
#define SPI_CONF_H_

#define SPI_Interface GSPI_BASE       // SPI interface
#define SPI_IF_BIT_RATE 20000000 / 20 // * must be an integer divisor of 20000000 *

void SPI_Init();
void Master_Send(unsigned long txData);

#endif /* SPI_CONF_H_ */
