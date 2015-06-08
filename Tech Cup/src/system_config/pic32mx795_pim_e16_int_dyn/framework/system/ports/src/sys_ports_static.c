/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_static.c

  Summary:
    SYS PORTS static function implementations for the Ports System Service.

  Description:
    The Ports System Service provides a simple interface to manage the ports
    on Microchip microcontrollers. This file defines the static implementation for the 
    Ports System Service.
    
  Remarks:
    Static functions incorporate all system ports configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.  It provides 
    static version of the routines, eliminating the need for an object ID or 
    object handle.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#include "system_config.h"
#include "peripheral/ports/plib_ports.h"
#include "peripheral/int/plib_int.h"


void SYS_PORTS_Initialize(void)
{
    /* AN and CN Pins Initialization */
    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, SYS_PORT_AD1PCFG, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, SYS_PORT_CNPUE);
    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, SYS_PORT_CNEN);
    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);

    
    /* PORT A Initialization */
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_B,  0b1010000010000000);
    //PLIB_PORTS_Toggle( PORTS_ID_0, PORT_CHANNEL_A,  SYS_PORT_A_LAT);
    //PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_ODC);

        // set up USER pin as input
ANSELBbits.ANSB13 = 0; // 0 for digital, 1 for analog
TRISBbits.TRISB13 = 0;



TRISBbits.TRISB7 = 0;

    T2CONbits.TCKPS = 2; // Timer2 prescaler N=4 (1:4)
    PR2 = 19999; // period = (PR2+1) * N * 12.5 ns = 100 us, 10 kHz
    TMR2 = 0; // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 0; // duty cycle = OC1RS/(PR2+1) = 25%
    OC1R = 5000; // initialize before turning OC1 on; afterward it is read-only
    T2CONbits.ON = 1; // turn on Timer2
    OC1CONbits.ON = 1; // turn on OC1


    OC2CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC2RS = 0; // duty cycle = OC1RS/(PR2+1) = 25%
    OC2R = 5000; // initialize before turning OC1 on; afterward it is read-only
    OC2CONbits.ON = 1; // turn on OC1

    RPB15Rbits.RPB15R = 0b0101;
    TRISBbits.TRISB15 = 0;
    LATBbits.LATB15=0;
    RPB1Rbits.RPB1R = 0b0101;
    TRISBbits.TRISB1 = 0;
    LATBbits.LATB1=0;

    LATBbits.LATB7=1;
    LATBbits.LATB13=1;
    
}


/*******************************************************************************
 End of File
*/
