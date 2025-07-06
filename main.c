
/**
 *

    Features:
        - MSP430G2553 MASP V2.0

    in:

    out:

    Other MSP430G2553 MASP standard outputs used:
    22. P1.7 SDA to OLED SDA ( best to give the OLED +5V supply )
    21. P1.6 SCL to OLED SCL
    26. P2.6 LED

  Description:

    The Watchdog ISR has higher priority, so it is used for input reading
    The Timer0 ISR has lower priority, so it is used for screen display and output control


        MSP430G2553 MASP V2.0
     .   -----------------
    /|\ |                 |
     |  |                 |
     ---|RST              |
        |                 |
        |                 | The ones below are MSP430G2553 MASP standard provided
        |                 |
        |     22. P1.7/SDA|--> to OLED SDA (best to give the OLED +5V supply)
        |     21. P1.6/SCL|--> to OLED SCL
        |                 |
        |     26.     P2.6|--> out status LED
        |                 |
        |                 |

 *
 */



#include "msp430g2553.h"
#include "sys/cdefs.h"
#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>


#include <battstor.h>



 volatile unsigned short int
    adcValues[ADC_CONSEQ_COUNT];
volatile unsigned int voltAvg=0;   


enum buttonstates buttState=BUTT_OFF_STATE;

int main(void)
{
    DCOCTL  = CALDCO_8MHZ;  // set and calibrate DCO and thus
    BCSCTL1 = CALBC1_8MHZ;  //      MCLK and SMCLK to 8MHz
    UCB0BR0 = 20;           // set UCB0BR0 to 20, for fSCL to 400kHz i2c
    UCB0BR1 = 0;            //      if needed. 800kHz might be fine too

    BCSCTL3 |= LFXT1S_2;    // set ACLK to use internal VLO (12 kHz clock)
    //  WDTCTL = WDT_ADLY_1_9;  // set a suitable Watchdog timer interval using ACLK
    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL +WDTIS0 ;// Configure WDT in interval timer mode using SMCKL, will generate interrupt every 4.096ms to be used for polling

    IE1 |= WDTIE;           // enable interrupts for the Watchdog timer


    // unusedPinsConfiguration();
    ledConfiguration();
    stateUpdateTimerConfiguration();
    // IdleTimerConfig();
    adcMultiConfiguration();
    adcStartConversionsAndDT();    
    Used_pin_configuration();
    // ADCinit();
    // LEDinit();
    // digital_init();

    

    _BIS_SR(LPM0_bits + GIE);   // Enter LPM0

	return 0;
}


void unusedPinsConfiguration(void)
{
    // set unused IO pins to input and enable internal pull-down resistor
    P1SEL = 0x00; // P1 selected as GPIO
    P1DIR = 0x00; // P1 to all inputs
    P1REN = 0xFF; // P1 enable internal pull-up/pull-down resistors
    P1OUT = 0x00; // P1 choose internal pull down resistor

    P2SEL = 0x00; // P2 selected as GPIO
    P2DIR = 0x00; // P2 to all inputs
    P2REN = 0xFF; // P2 enable internal pull-up/pull-down resistors
    P2OUT = 0x00; // P2 choose internal pull down resistor

    P3SEL = 0x00; // P3 selected as GPIO
    P3DIR = 0x00; // P3 to all inputs
    P3REN = 0xFF; // P3 enable internal pull-up/pull-down resistors
    P3OUT = 0x00; // P3 choose internal pull down resistor
}


void ledConfiguration(void)
{
    P3SEL &= ~(BIT0|BIT2|BIT3); // select pins 7,12 and 13 to be GPIO P3.0,2,3
    P3DIR |=   BIT0|BIT2|BIT3;  // P3.0,2,3 LED pins as output
    P3REN &= ~(BIT0|BIT2|BIT3); // disable internal pull-up/pull-down resistors
    P3OUT |=   BIT0|BIT2|BIT3;  // initially turn off the LEDs
}


void stateUpdateTimerConfiguration(void)
{
    // setup Timer A0 to periodically refresh the oled screens

    // Configure Timer_A0 Capture Compare 0's Control Register (CCR0's CCTL = CCTL0)
    //         Enable timer interrupts (slau144j.pdf pg 372)
    TA0CCTL0 = CCIE;

    // Init Timer_A1 Capture Compare Reg 0 with desired count (slau144j.pdf pg 371)
    TA0CCR0 = OLED_UPDATE_INTERVAL_IN_ACLOCK_TICKS;

    // Configure Timer_A1 Control Register: TA1CTL
    //        ACLOCK  + Up mode + no division + clear (slau144j.pdf pg 370)
    TA0CTL = TASSEL_1 + MC_1    + ID_0        + TACLR;

}
// void IdleTimerConfig(void){
//     // Timer: 1A
//     TA1CCTL0= CCIE;
//     TA1CCR0= 12000-1;
//     TA1CTL= TASSEL_1 | MC_0 | ID0;
// }

void Used_pin_configuration(void){
    // output pins config
    P1DIR |= BIT1 | BIT3 | BIT4;// Set directions: P1.1, P1.3, P1.4 as outputs (discharge, main, charge)

    P1OUT = 0x00;  // All P1 outputs low
    P1OUT |= BIT3; // turn OFF discharge (P1.3 high) 
    P1OUT |=BIT1;  // MAIN SWITCH ON
}

// void Polling_Timer_configuration(void)
// {
//     // using A1 to poll P1.5 and P1.2 for voltage and input 
//      // Timer: 1A
//     TA1CCTL0=CCIE;
    
//     TA1CTL=TASSEL2 |MC_1 |ID0;
//     TA1CCR0=40000-1;// 5ms
// }

// void ADCinit(){
//     // Configure ADC
//     ADC10CTL1 = INCH_5;         // Input from A5 (P1.5)
//     ADC10CTL0 = SREF_1 + REFON + REF2_5V + ADC10SHT_3 + ADC10ON; // V+ = AVcc, V- = AVss, 64 x ADC10CLKs
//     __delay_cycles(30000); 


//     // Configure P1.5 as analog input (A5)
//     P1DIR &= ~ANALOG_IP;             // Make P1.5 input (optional)
//     P1SEL |= ANALOG_IP;              // Select analog function for P1.5

// }

// void LEDinit(){
//     // Set P3.2 P3.3 and P3.0 as outputs
//     P3DIR |= BIT2 + BIT0 + BIT3;
//     P3OUT = 0xFF;  // LED OFF
// }

// void digital_init(){
//       // configure p1.2 as digital input
//     // Set P1.2 as input with pull-up resistor and interrupt
//     P1DIR &= ~DIGITAL_IP;                 // Input
//     P1REN |= DIGITAL_IP;                  // Enable pull-up/down
//     P1OUT |= DIGITAL_IP;                  // Pull-up
//     P1IE  |= DIGITAL_IP;                  // Interrupt enable
//     P1IES |= DIGITAL_IP;                  // High-to-low edge
//     P1IFG &= ~DIGITAL_IP;                 // Clear flag

//     // // Set P1.2 as input with pull-down resistor and interrupt
//     // P1DIR &= ~DIGITAL_IP;                 // Input
//     // P1REN |= DIGITAL_IP;                  // Enable pull-up/down
//     // P1OUT &=~DIGITAL_IP;                  // Pull-down
//     // P1IE  |= DIGITAL_IP;                  // Interrupt enable
//     // P1IES &=~DIGITAL_IP;                  // low-to-high edge
//     // P1IFG &= ~DIGITAL_IP;                 // Clear flag
// }

void adcMultiConfiguration(void)
{
    // Disable pull down resistor for the ADC inputs A2, A5
    // Else it will mess with the input voltage if it is being generated
    // by a potentiometer or similar voltage divider circuit
    P1REN &= ~(BIT2 | BIT5);

    // Select Input channel = A5 (as starting point in this sequence)
    // Sample-and-hold source = ADC10SC bit
    // ADC10 clock divider = no clock division
    // ADC10 clock source = ADC10OSC
    // Conversion sequence mode = Repeat-sequence-of-channels
    ADC10CTL1 = INCH_5 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_3;

    // ADC10 = on
    // Reference generator = on
    // Vr+ = Vref+ and Vr- = Vss
    // Reference-generator voltage = 2.5V
    // ADC10 sample-and-hold time = 4 ADC10CLKs
    // Slow down the Sampling Rate to 50ksps. Reduces current consumption
    // Multiple sample and conversion - no need to wait for trigger
    // We don't add ENC here at the end because multi needs more for setup
    ADC10CTL0 = ADC10ON | REFON | SREF_1 | REF2_5V | ADC10SHT_0 | ADC10SR | MSC ;

    ADC10AE0 = BIT2 | BIT5;         // Enable Analog Input on A2, A5 ( P1.2, P1.5 )

    ADC10DTC1 = ADC_CONSEQ_COUNT;   // Number of data transfers in each block
    ADC10DTC0|=ADC10CT;
}


void adcStartConversionsAndDT(void)
{
    ADC10CTL0 &= ~ENC;                      // signal stop of any pending ADC sampling
    while (ADC10CTL1 & ADC10BUSY);          // wait for pending ADC conversions
    ADC10CTL0 |= ENC + ADC10SC;             // start ADC sampling and conversion
    ADC10SA = (unsigned short int) adcValues;  // start DTC transfers
}



// Timer0 has lower priority than WDT ISR so we use it for display and outputs
void __attribute__((interrupt(TIMER0_A0_VECTOR))) Timer0A0_ISR(void)
{
    updateDisplay();
    // P3OUT ^= BIT0;
}
// void __attribute__((interrupt(TIMER0_A1_VECTOR))) Timer0A1_ISR(void)
// {   
//      P3OUT ^= BIT2;
//     IdleTimeCounter();
   
// }


// using the WDT ISR for ADC and button input reading
// as it has higher priority than Timer0_A0
void __attribute__((interrupt(WDT_VECTOR))) watchdog_timer(void)
{
    //P3OUT ^= BIT3;
        static unsigned short int
        adcValueBattery = 0,
        adcValueButton = 0;

    adcValueBattery = adcValues[0];     // read Battery Voltage
    adcValueButton  = adcValues[3];     // read Button Voltage
                                        // this array fills in reverse order
                                        // A2 -> [0], A1 -> [1
    
    unsigned int voltRawSum16;
    unsigned int voltRaw = adcValueBattery;
    voltRawSum16 = voltRaw + 15*voltAvg;
    voltAvg = voltRawSum16>>4; // div by 16
    
    
   

    unsigned int buttRaw,buttRawSum16;
    static unsigned int buttAvg;
    static unsigned short int buttPressCount=0;

    buttRaw = adcValueButton;

    buttRawSum16 =  buttRaw + 15*buttAvg;

    buttAvg = buttRawSum16 >> 4 ;
    

        if ( buttAvg < 300 )
    {
        switch(buttPressCount)
        {
            case BUTT_LONG_COUNT:    // 20
                buttState =  BUTT_LONG_STATE;
                buttPressCount = 0;
                break;

            case BUTT_SHORT_COUNT:    // 5
                buttState = BUTT_SHORT_STATE;
                buttPressCount = 0;
                break;

            //case BUTT_OFF_COUNT:    // 0
            //    buttSate = BUTT_OFF_STATE;
            //    buttPressCount =  0;
        }

        if ( buttPressCount != 0 )
            buttPressCount-- ;

    
    }
    else if ( buttAvg > 700 )
    {
        buttPressCount++;

    }



}
