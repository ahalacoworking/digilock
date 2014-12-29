//
//  led.h
//  digilock
//
//  Created by Olivier on 29/12/2014.
//  Copyright (c) 2014 etincelle. All rights reserved.
//

#ifndef __digilock__led__
#define __digilock__led__

#include <stdio.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef enum {
    ELEDPinEntryOK = 11,
    ELEDPinEntryNOK,
    ELEDPinEntryWait,
    ELEDPinExitOK,
    ELEDPinExitNOK,
    ELEDPinExitWait,
    ELEDPinLast,
} ELEDPin;

void led_show(int aLed, bool aEnable);
void led_show(int aLed, bool aEnable, bool aDisableOthers);
void led_show(int aLed, bool aEnable, bool aBlink, int aDurationMS, bool aDisableOthers);



//#ifdef __cplusplus
//} /* extern "C" */
//#endif

#endif /* defined(__digilock__led__) */