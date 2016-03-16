/******************************************************************************/
/* Function prototypes and definitions                                        */
/******************************************************************************/

typedef struct Message {
    char data[8];
    short sid;
    char len;
} Message;

void ecan_init();
void ecan_send(Message* mess);
void ecan_receive_rxb0(Message* mess);
void ecan_receive_rxb1(Message* mess);
char ecan_rxb0_data_ready();
char ecan_rxb1_data_ready();
void ecan_rxb0_clear();
void ecan_rxb1_clear();
