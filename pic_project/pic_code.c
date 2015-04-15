#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include "i2c_display.h"
#include "i2c_master_int.h"

// DEVCFGs here

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
#pragma config UPLLIDIV = DIV_1 // divide clock after FPLLMUL
#pragma config UPLLEN = ON // USB clock on
#pragma config FPLLODIV = DIV_2 // divide clock by 1 to output on pin

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid
#pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
#pragma config IOL1WAY = ON // not multimple reconfiguration, check this
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module

// ASCII

// lookup table for all of the ascii characters
static const char ASCII[96][5] = {
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20  (space)
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c �
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ?
,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f ?
}; // end char ASCII[96][5]

int readADC(void);

int main() {

    int potValue;
    int timerResets=0;
    // startup
    startup();


T2CONbits.TCKPS = 2; // Timer2 prescaler N=4 (1:4)
PR2 = 19999; // period = (PR2+1) * N * 12.5 ns = 100 us, 10 kHz
TMR2 = 0; // initial TMR2 count is 0
OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
OC1RS = 5000; // duty cycle = OC1RS/(PR2+1) = 25%
OC1R = 5000; // initialize before turning OC1 on; afterward it is read-only
T2CONbits.ON = 1; // turn on Timer2
OC1CONbits.ON = 1; // turn on OC1


    // set up USER pin as input
ANSELBbits.ANSB13 = 0; // 0 for digital, 1 for analog

    // set up LED1 pin as a digital output
ANSELBbits.ANSB15 = 0; // 0 for digital, 1 for analog

TRISBbits.TRISB15 = 0;

    // set up LED2 as OC1 using Timer2 at 1kHz
RPB7Rbits.RPB7R = 0b0101; // set B15 to OC1

//LATBbits.LATB15=1;

    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;

    int counter = 0;
    int user = 1;



/* /////////Testing section, old code///////////
    display_init();
    drawChar(72-0x20,45,50);
    drawChar(73-0x20,50,50);
    display_draw();
*/


    /////// Write to LCD
    display_init();
    int k=0;
    int L=0;
    int charCurrent;
    int x1=0;
    int xIndex = 0;
    int yIndex = 1;
    char message1[22];
    char message2[22];
    char message3[22];
    char message4[22];
    char message5[22];
    char message6[22];
    char message7[22];

    /*
     *
                  _      _      _
               __(.)< __(.)> __(.)=
               \___)  \___)  \___)  */
    sprintf(message1,"   _      _      _");
    sprintf(message2,"__(.)< __(.)< __(.)=");
    sprintf(message3,"\\___)  \\___)  \\___)");
    sprintf(message4,"");
    sprintf(message5,"");
    sprintf(message6,"");
    sprintf(message7,"quack");

    while(L==0){
        charCurrent = (int)message1[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 3);
            xIndex+=6;
            k+=1;
        }
    }
    k=0;
    L=0;
    xIndex=x1;
    while(L==0){
        charCurrent = (int)message2[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 12);
            xIndex+=6;
            k+=1;
        }
    }
    k=0;
    L=0;
    xIndex=x1;
    while(L==0){
        charCurrent = (int)message3[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 21);
            xIndex+=6;
            k+=1;
        }
    }
    k=0;
    L=0;
    xIndex=x1;
    while(L==0){
        charCurrent = (int)message4[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 30);
            xIndex+=6;
            k+=1;
        }
    }
    k=0;
    L=0;
    xIndex=x1;
    while(L==0){
        charCurrent = (int)message5[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 39);
            xIndex+=6;
            k+=1;
        }
    }
    k=0;
    L=0;
    xIndex=x1;
    while(L==0){
        charCurrent = (int)message6[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 48);
            xIndex+=6;
            k+=1;
        }
    }
    k=0;
    L=0;
    xIndex=x1;
    while(L==0){
        charCurrent = (int)message7[k];
        if(charCurrent==0){
            L=1;
        }
        else{
            drawChar(charCurrent - 0x20,xIndex, 57);
            xIndex+=6;
            k+=1;
        }
    }



    display_draw();







    while (1) {
        /*
        // invert pin every 0.5s
        _CP0_SET_COUNT(0);

        while( _CP0_GET_COUNT() < 10000000){

        user = PORTBbits.RB13;
        if(user==0) {
            counter = !counter; 
            LATBbits.LATB15=counter;
        }
        //set PWM duty cycle % to the pot voltage output %
        potValue = readADC();
        OC1RS=(potValue*20000)/1023;
        }
        counter = !counter;
        LATBbits.LATB15=counter;
        */
    }
}

int readADC(void) {
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {
    }
    a = ADC1BUF0;
    return a;
}

void startup(void) {
 //    Startup code to run as fast as possible and get pins back from bad defaults

__builtin_disable_interrupts();

// set the CP0 CONFIG register to indicate that
// kseg0 is cacheable (0x3) or uncacheable (0x2)
// see Chapter 2 "CPU for Devices with M4K Core"
// of the PIC32 reference manual
__builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

// no cache on this chip!

// 0 data RAM access wait states
BMXCONbits.BMXWSDRM = 0x0;

// enable multi vector interrupts
INTCONbits.MVEC = 0x1;

// disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
DDPCONbits.JTAGEN = 0;

__builtin_enable_interrupts();
}

/*void textSpeed(void) {
    while (1) {
    _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
    LATBINV = some_pin; // invert a pin

    // wait for half a second, setting LED brightness to pot angle while waiting
    while (_CP0_GET_COUNT() < 10000000) {
        val = readADC();
        OC1RS = val * normalize_to_pr;

        if (push_button_pin == 1) {
            // nothing
        } else {
            LATBINV = some_pin;
        }
    }
}
}*/

void drawChar(int tableIndex, int xPos, int yPos){
    int check;
    int i;
    int j;
    int number;



    //check = checkPos(xPos,yPos); //see if this position works
    for(i=0;i<5;i++){
        number = ASCII[tableIndex][i];
        for(j=0;j<=7;j++){
            //if((((int)number)&(1<<j))>>j){
            if(((int)number>>j)&1==1){
                display_pixel_set(yPos+j,xPos+i,1);
            }
            else{
                display_pixel_set(yPos+j,xPos+i,0);
            }
        }
    }
}

