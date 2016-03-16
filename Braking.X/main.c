/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define _XTAL_FREQ 32000000
#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "can.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

bool FORW = false;
/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    //InitApp();
    initializePorts();
    /* TODO <INSERT USER APPLICATION CODE HERE> */
    ecan_init();
    //Interrupts
    RCONbits.IPEN = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE3bits.RXB0IE = 1;
    
    setAnalogIn();
    while(1)
    {
        //Code to read from CANBUS if there is a brake signal
        selectAN0();
        int sensorZero = readADC();
        selectAN1();
        int sensorOne = readADC();
        if (FORW || sensorZero < 60 || sensorOne < 60) {
            LATDbits.LATD0 = 0; //01 is extend
            LATDbits.LATD1 = 1;
        }
        else {
            LATDbits.LATD0 = 1;
            LATDbits.LATD1 = 0;
        } 
   }
}

void interrupt ISR(void)
{
    if(PIR3bits.RXB0IF)
    {
        LATDbits.LATD2 ^= 1;
        Message newMessage;
        ecan_receive_rxb0(&newMessage);
        if(newMessage.data[0] == 0x00)
        {
            FORW = false;
            //BACK = true;
            
        }
        if (newMessage.data[0] == 0x01) 
        {
            FORW = true;
            //BACK = false;
            
        }
        ecan_rxb0_clear();
        PIR3bits.RXB0IF = 0;
    }
}



