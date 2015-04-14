#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

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
    while (1) {
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