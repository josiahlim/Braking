/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void initializePorts(void);
void selectAN0();
void selectAN1();
void selectAN2();
void selectAN3();
void selectAN4();
void selectAN5();
void selectAN6();
void selectAN7();
void setAnalogIn();

unsigned int readADC();
