/*
//  Copyright (C) 2017, Zach (Zhengyu) Peng, https://zpeng.me - All Rights Reserved
//
//  Unauthorized copying of this file, via any medium is strictly prohibited
//  Proprietary and confidential
//
//  Written by Zach (Zhengyu) Peng <rookiepeng@gmail.com>, May 2017
*/

#ifndef ADC_CONF_H_
#define ADC_CONF_H_

#define ADC_BUFF_SIZE 1024

void InitAdcDma();
//void ADCIntHandler();
void ADCIntHandlerCH0();
void ADCIntHandlerCH1();
void ADCIntHandlerCH2();
void ADCIntHandlerCH3();
void ADCLong2Char(unsigned short *adcBuffer);

#endif /* ADC_CONF_H_ */
