/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#ifndef RADAR_H_
#define RADAR_H_

void CS_Decoder(char ucSelect);
void RF_Switch(char ucSelect);
void VCO_Init(unsigned char vcoNum);
void PLL_Init(unsigned char pllNum);
void RX_Init_Off();
void TX_Off();
void TX(unsigned char txNum);
void RX(unsigned char rxNum);
void Radar_Init();

#endif /* RADAR_H_ */
