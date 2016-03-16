/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "user.h"
#include <math.h>
#define _XTAL_FREQ 32000000
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */
// Calibration values 
/*
signed int ac1; 
signed int ac2; 
signed int ac3; 
int ac4; 
int ac5; 
int ac6; 
signed int b1; 
signed int b2; 
signed int mb; 
signed int mc; 
signed int md; 

// floating point cal factors 
float _c3; 
float _c4; 
float _b1; 
float _c5; 
float _c6; 
float _mc; 
float _md; 

// polynomomial constants 
float _x0; 
float _x1; 
float _x2; 
float _y0; 
float _y1; 
float _y2; 
float _p0; 
float _p1; 
float _p2; 

float _s;     // T-25, used in pressure calculation - must run temperature reading before pressure reading 
float _Temp;  // set after every temperature or temperature/pressure reading 
#define BMP085_ADDRESS 0xEE          // I2C address of BMP085 
#define P_CORRECTION   1.5           // in mBars - factor to adjust for elevation to match local weather station pressure 
                                     // this value for 14' above sea level (in Boca Raton, Florida 
const int OVS_S = 2; // Oversampling Setting (0,1,2,3 from ultra low power, to ultra hi-resolution) 

*/
//void InitApp(void)
//{
    /* TODO Initialize User Ports/Peripherals/Project here */

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */

    /* Enable interrupts */
   
//}


void initializePorts() {
    TRISDbits.TRISD0 = 0; //outputs
    TRISDbits.TRISD1 = 0;
    //TRISAbits.TRISA0 = 1; // input for ADC Distance Sensors
    //TRISAbits.TRISA1 = 1;
    TRISDbits.TRISD2 = 0;
    
}

// Select the AN0 pin to be used for ADC
void selectAN0()
{
    ADCON0bits.CHS = 0x00;
}

// Select the AN1 pin to be used for ADC
void selectAN1()
{
    ADCON0bits.CHS = 0x01;
}

// Select the AN2 pin to be used for ADC
void selectAN2()
{
    ADCON0bits.CHS = 0x02;
}

// Select the AN3 pin to be used for ADC
void selectAN3()
{
    ADCON0bits.CHS = 0x03;
}

// Select the AN4 pin to be used for ADC
void selectAN4()
{
    ADCON0bits.CHS = 0x04;
}

// Select the AN5 pin to be used for ADC
void selectAN5()
{
    ADCON0bits.CHS = 0x05;
}

// Select the AN6 pin to be used for ADC
void selectAN6()
{
    ADCON0bits.CHS = 0x06;
}

// Select the AN7 pin to be used for ADC
void selectAN7()
{
    ADCON0bits.CHS = 0x07;
}

unsigned int readADC()
{
    unsigned int digitalVal;
    //Start ADC for general channel
    ADCON0bits.GO_NOT_DONE = 1;
        
    //Wait for conversion to finish
    while (ADCON0bits.GO_NOT_DONE == 1);
        
    //Read ADC value from High/Low registers
    digitalVal = ADRESL;
    digitalVal += ADRESH << 8;
    return digitalVal;
}
void setAnalogIn()
{
    //Set Port AN0-AN10 to Analog
    ADCON1bits.PCFG0 = 1;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 1;
    ADCON1bits.PCFG3 = 1;
    ADCON2bits.ADFM = 1;    //Right justified - values converted to 0-1023
                            //Registers ADRESH and ADRESL hold this converted value
    
    //V-REF+ is VDD and V-REF- is VSS
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    //Set Acquisition Time to be 4T_AD
    ADCON2bits.ACQT2 = 0;
    ADCON2bits.ACQT1 = 1;
    ADCON2bits.ACQT0 = 0;
    
    //Set A/D Conversion clock frequency F_OSC/32
    ADCON2bits.ADCS2 = 0;
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS0 = 0;

    
    //Set Analog ports as Input
    TRISAbits.TRISA0 = 1;   //AN0
    TRISAbits.TRISA1 = 1;   //AN1
    /*
    TRISAbits.TRISA2 = 1;   //AN2
    TRISAbits.TRISA3 = 1;   //AN3
    TRISAbits.TRISA5 = 1;   //AN4
    TRISEbits.TRISE0 = 1;   //AN5
    TRISEbits.TRISE1 = 1;   //AN6
    TRISEbits.TRISE2 = 1;   //AN7
    TRISBbits.TRISB1 = 1;   //AN8
    TRISBbits.TRISB4 = 1;   //AN9
    TRISBbits.TRISB0 = 1;   //AN10
    */
    //Enable ADC
    ADCON0bits.ADON = 1; 
}