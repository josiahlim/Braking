#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <pic18f4685.h>        /* For true/false definition */

#include "can.h"

/* Initialize ECAN Module */
void ecan_init()
{
    /* Set up pins */
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 1;
    
    /* Request Configuration mode, REQOP[2:0] = 1XX */
    CANCONbits.REQOP2 = 1;
    
    /* Wait for OPMODE = 100 (Configuration mode) */
    while (CANSTATbits.OPMODE2 != 1 && CANSTATbits.OPMODE1 != 0 &&
           CANSTATbits.OPMODE0 != 0);
    
    /* Tx will drive high when recessive, 
     * might need to be 0 (high z) w/o transceiver */
    CIOCONbits.ENDRHI = 0;
    
    /* Set the baud rate to 500K bits/s */
    BRGCON1bits.SJW1 = 0;       // Synchronization jump width time = 1 x TQ
    BRGCON1bits.SJW0 = 0;
    BRGCON1bits.BRP5 = 0;       // Baud Rate Pre-scaler bits
    BRGCON1bits.BRP4 = 0;       // TQ = 2 x (BRP + 1)/FOSC = (250 ns)
    BRGCON1bits.BRP3 = 0;
    BRGCON1bits.BRP2 = 0;
    BRGCON1bits.BRP1 = 0;
    BRGCON1bits.BRP0 = 1;
    
    BRGCON2bits.SEG2PHTS = 1;   // Phase Segment 2 Time Select, freely programmable
    BRGCON2bits.SAM = 1;        // Sample bus three times prior to the sample point
    BRGCON2bits.SEG1PH2 = 0;    // Phase Segment 1 time = 2 x TQ
    BRGCON2bits.SEG1PH1 = 0;
    BRGCON2bits.SEG1PH0 = 1;
    BRGCON2bits.PRSEG2 = 0;     // Propagation time = 3 x TQ
    BRGCON2bits.PRSEG1 = 1;
    BRGCON2bits.PRSEG0 = 0;
    
    //BRGCON3bits.WAKDIS = 1;     // Wake-up disable (feature is disabled)
    //BRGCON3bits.WAKFIL = 0;     // Filter not used for wake-up
    BRGCON3bits.SEG2PH2 = 0;      // Phase Segment 2 time = 2 x TQ
    BRGCON3bits. SEG2PH1 = 0;
    BRGCON3bits. SEG2PH0 = 1;

    /* Switch CAN module to "Legacy mode" */
    ECANCONbits.MDSEL1 = 0;
    ECANCONbits.MDSEL0 = 0;
    
    /* Set receive buffer 0 mode: 
     * 11 = Receive all messages (including those with errors); filter criteria is ignored
     * 10 = Receive only valid messages with extended identifier; EXIDEN in RXFnSIDL must be ?1?
     * 01 = Receive only valid messages with standard identifier; EXIDEN in RXFnSIDL must be ?0?
     * 00 = Receive all valid messages as per EXIDEN bit in RXFnSIDL register */
    RXB0CONbits.RXM1 = 0;
    RXB0CONbits.RXM0 = 1;
    
    
    /* Set receive buffer 1 mode: (Bits are the same as buffer 0) */
    RXB1CONbits.RXM1 = 0;
    RXB1CONbits.RXM0 = 1;
    
    /* Receive 0 filter & mask control:
     * If the result of anding SID (from message) and mask0 is equal to sid0,
     * then the message will be taken into receive buffer. Set sid0 and mask0
     * appropriately in order to receive messages. */
    /* Example: if you have a mask0[0] = 1, you care that SID[0] matches sid0[0].
     * If mask[0] = 0, then you don't care if SID[0] matches sid0[0]. These
     * comparisons are done for every bit. */
    short sid0 = 0x001;
    short mask0 = 0x001;
    
    RXF0SIDH = sid0 >> 3;           // Filter 0, mask 0 -> RXB0
    RXF0SIDL = sid0 << 5;
    RXM0SIDH = mask0 >> 3;
    RXM0SIDL = mask0 << 5;
    RXF0SIDLbits.EXIDEN = 0;
    RXFCON0bits.RXF0EN = 1;         // Enable filter 0

    /* Receive 1 filter & mask control: 
     */
    short sid1 = 0x000;
    short mask1 = 0x001;
    
    RXF2SIDH = sid1 >> 3;           // Filter 2, mask 1 -> RXB1
    RXF2SIDL = sid1 << 5;
    RXM1SIDH = mask1 >> 3;
    RXM1SIDL = mask1 << 5;
    RXF1SIDLbits.EXIDEN = 0;
    RXFCON0bits.RXF2EN = 1;         // Enable filter 2
   
    /* Request normal mode, 000 */
    CANCONbits.REQOP2 = 0;
    CANCONbits.REQOP1 = 0;
    CANCONbits.REQOP0 = 0;
    
    /* Wait until CAN module enters to normal mode */
    while (CANSTATbits.OPMODE2 != 0 && CANSTATbits.OPMODE1 != 0 &&
           CANSTATbits.OPMODE0 != 0);    
}

/* Send eight bytes out of ECAN */
void ecan_send(Message* mess)
{
    TXB0D7 = mess->data[7];
    TXB0D6 = mess->data[6];
    TXB0D5 = mess->data[5];
    TXB0D4 = mess->data[4];
    TXB0D3 = mess->data[3];
    TXB0D2 = mess->data[2];
    TXB0D1 = mess->data[1];
    TXB0D0 = mess->data[0];
    
    TXB0DLC = mess->len;              // Load data length (up to 8 (100))

    /* Load message ID & enable standard ID */
    TXB0SIDH = (mess->sid) >> 3;
    TXB0SIDL = (mess->sid) << 5;
    TXB0SIDLbits.EXIDE = 0;

    /* Send message */
    TXB0CONbits.TXREQ = 1;
}

/* Read data from ECAN from receive buffer 0 */
void ecan_receive_rxb0(Message* mess)
{
    /* Make sure that there is a message pending in RXB0. */
    while(!RXB0CONbits.RXB0FUL);    // Wait until message is ready
     
    /* Save data from receive buffer 0*/
    mess->data[7] = RXB0D7;
    mess->data[6] = RXB0D6;
    mess->data[5] = RXB0D5;
    mess->data[4] = RXB0D4;
    mess->data[3] = RXB0D3;
    mess->data[2] = RXB0D2;
    mess->data[1] = RXB0D1;
    mess->data[0] = RXB0D0;
    
    /* Save sid from message */
    mess->sid = RXB0SIDH << 3;
    mess->sid |= RXB0SIDL >> 5;
    
    /* Save length from message*/
    mess->len = 0x0F & RXB0DLC; 
    
    /* Once entire message is read, mark the RXB0 that it is read and no longer FULL. */
    RXB0CONbits.RXB0FUL = 0;
}

/* Read data from ECAN from receive buffer 1 */
void ecan_receive_rxb1(Message* mess)
{
    /* Make sure that there is a message pending in RXB0. */
    while(!RXB1CONbits.RXB1FUL);    // Wait until message is ready
     
    /* Save data from receive buffer 0*/
    mess->data[7] = RXB1D7;
    mess->data[6] = RXB1D6;
    mess->data[5] = RXB1D5;
    mess->data[4] = RXB1D4;
    mess->data[3] = RXB1D3;
    mess->data[2] = RXB1D2;
    mess->data[1] = RXB1D1;
    mess->data[0] = RXB1D0;
    
    /* Save sid from message */
    mess->sid = RXB1SIDH << 3;
    mess->sid |= RXB1SIDL >> 5;
    
    /* Save length from message*/
    mess->len = 0x0F & RXB1DLC; 
    
    /* Once entire message is read, mark the RXB1 that it is read and no longer FULL. */
    RXB1CONbits.RXB1FUL = 0;
}

/* Check if receive buffer 0 has received a message */
char ecan_rxb0_data_ready()
{
    return RXB0CONbits.RXB0FUL;
}

/* Check if receive buffer 0 has received a message */
char ecan_rxb1_data_ready()
{
    return RXB1CONbits.RXB1FUL;
}

/* Clear receiver buffer 0 flag */
void ecan_rxb0_clear()
{
    RXB0CONbits.RXB0FUL = 0;
}

/* Clear receiver buffer 1 flag */
void ecan_rxb1_clear()
{
    RXB1CONbits.RXB1FUL = 0;
}