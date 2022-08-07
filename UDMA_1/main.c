//****************************************************************************************
// Author: Obed Oyandut
// Date  : 07.08.2022
// Version: 1
//****************************************************************************************
// This Program worlks on TIVA TM4C1294XL Evaluation Board.
// The program performs a memory to memory transfer using udma.
//****************************************************************************************
//                     ! IMPORTANT !
// This program runs endless. Stop with the "Red Square Button"
// in Debug Mode (Terminate = CTRL + F2)
//****************************************************************************************
// Include the Header File for controller tm4c1294ncpdt

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//************************************************************************************************************************
// Length of control table, source buffer and destination buffer
//************************************************************************************************************************

#define BASE 256
#define LEN 144

//************************************************************************************************************************
// The content of locationOne will be transfered into locationTwo using UDMA
//************************************************************************************************************************

volatile char locationOne[] = "This program intend to transfer this text in locationOne array into locationTwo array using UDMA";
volatile char locationTwo[LEN];

//************************************************************************************************************************
// The control table is declared as unsigned int. This means the length is 256. Channel 30 is used. The offset of the pri-
//mary control structure of channel 30 is 120.
//************************************************************************************************************************

unsigned int utable[BASE];

//************************************************************************************************************************
// Interrupt is executed after udma transfer is done.
//************************************************************************************************************************

void udmaInterrupt(void) {

    locationTwo[96] = '\0';
    printf("LocationTwo: %s\n", locationTwo);
}

//************************************************************************************************************************
// Prints a notification if udma encounters error.
//************************************************************************************************************************

void udmaInterruptError(void) {

    UDMA_ERRCLR_R |= 0x01;
    printf("Udma encounters an error...\n");
}

//*************************************************************************************************************************
// Configure and enable channel 30
//*************************************************************************************************************************

void dmaconfig(void) {
    SYSCTL_RCGCDMA_R |= 0x01;
    while(!(SYSCTL_PRDMA_R & 0x01));
    UDMA_CFG_R |= 0x01;
    UDMA_PRIOSET_R |= (0x01<<30);
    UDMA_ALTCLR_R |= (0x01<<30);
    UDMA_USEBURSTCLR_R |= 0x40000000;
    UDMA_REQMASKCLR_R |= 0x40000000;
    UDMA_CTLBASE_R |= (unsigned int)utable;
    UDMA_ENASET_R |= 0x40000000;
    UDMA_SWREQ_R |= 0x40000000;
}

//**************************************************************************************************************************
// Enable udma interrupts in NVIC
//**************************************************************************************************************************

void enableNVIC(void) {

    NVIC_EN1_R |= (0x03<<44-32);

}

//**************************************************************************************************************************
// Make a transfer
// Input parameters include source end pointer, destination end pointer and and primary control structure.
// All 96 characters of locationOne is copied into locationTwo using udma. This is performed in 6 transfers. Each tranfer
// copies a half word i.e. 16 bits into the destination.

// ! Description of the control word !
// >Destination address increment: 8 bits or a byte
// >Destination data size: 8 bits
// >Source address increment: 8 bits or a byte
// >Source data size: 8 bits
// >Arbitration size: 16 transfers
// >Total number of transfer: 96
// >Transfer mode is auto-request
//**************************************************************************************************************************

void udmaTransfer(unsigned int src, unsigned int dest, unsigned int controlWord) {

    utable[120] = src;
    utable[121] = dest;
    utable[122] = controlWord;

    //***********************************************************************************************************************
    // configures the channel and enable interrupt lines in NVIC
    //***********************************************************************************************************************

    enableNVIC();
    dmaconfig();
}

void main(void) {


    udmaTransfer((unsigned int)&locationOne[95], (unsigned int)&locationTwo[95], 0x00080602);

    while(1) {

    }
}
