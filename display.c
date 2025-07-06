/*
 * displayAndOutputStates.c
 *
 *  Created on: Jun 2, 2025
 *      Author: Ryan
 *
 *
 *
 *
 *      Manages the progression of states and for the display and outputs
 */


#include "msp430g2553.h"
#include <battstor.h>
#include <msp430.h>

#include <lib/i2c/i2c.h>
#include <lib/oled/ssd1306.h>

#include <display.h>


char n[]={'0','0','0','0','0'};//eg 40.2
const char* menu[]={"1 CH_100% ","2  CH_50% ","3 DCH_50% "};
volatile int current_option=0;
volatile unsigned int wait_time=0;
unsigned int voltage=0;
unsigned char r=0,f=0;

void updateDisplay(void)
{
    IdleTimeCounter();
        
    static unsigned short int
        displayDelayCounter = 0;
     
    voltage=voltAvg;//+120;
    switch (displayState) {

        case DISP_INIT_DELAY:
            displayState = DISP_INIT;
            break;

        case DISP_INIT:
            displayInit();
            displayState = DISP_INTRO;
            break;

        case DISP_INTRO:
            displayIntro();
            displayDelayCounter = 0;

            displayState = DISP_INTRO_DELAY;
            break;

        case DISP_INTRO_DELAY:
            if(displayDelayCounter++ == 10) // is it waiting for 20x 4000 clk cycle of 12k
                displayState = DISP_MENU;
            break;

        case DISP_MENU:
            if(displayState!=previousState){
                ssd1306_clearDisplay();
                previousState=displayState;
            }
            // dont know where to place this 
            switch(buttState){
                case BUTT_SHORT_STATE:
                current_option=(current_option+1)%opt_no;
                break;
                
                case BUTT_LONG_STATE:
                displayState=(enum displayStates)(current_option+5);
                break;
            } 
            buttState=BUTT_OFF_STATE;
            
            
            displayVolt();
            displayMenu();
            displayDelayCounter = 0;

            // displayState = DISP_MENU_DELAY;
            break;

        case DISP_CHARGE_100:
            if(displayState!=previousState){
                P3OUT&=~BIT3; // BLUE LED ON
                P1OUT|=BIT4; // CHARGING ON
                ssd1306_clearDisplay();
                draw5x7Str(10, 30, "Charging to 100%", 1);
                draw12x16Str(10,45,"EXIT", 1);
                previousState=displayState;
            }
            
            displayVolt();
            
            Exit();// checks for button press to exit
            if(displayState==DISP_MENU){
                P3OUT|=BIT3;// BLUE LED OFF
                P1OUT&=~BIT4; // CHARGING OFF
            }

            if((voltage > 939) ){//93958v
                displayState=DISP_MENU;
                //stop charging
                P1OUT&=~BIT4; // CHARGING OFF
                P3OUT|=BIT3;  // BLUE LED OFF
                wait_time=0;
                f=1;
                
                

            }

            break; 

        case DISP_CHARGE_50:
            if(displayState!=previousState){
                P3OUT&=~BIT3; // BLUE LED ON
                P1OUT|=BIT4; // CHARGING ON
                ssd1306_clearDisplay();
                // over here i could print V: and value in displayvolt fn
                draw5x7Str(10, 30, "Charging to 50%", 1);
                draw12x16Str(10,45,"EXIT", 1);
                previousState=displayState;
            }
            Exit();// checks for button press to exit
            displayVolt();
            
            if(displayState==DISP_MENU){
                P3OUT|=BIT3;// BLUE LED OFF
                P1OUT&=~BIT4; // CHARGING OFF
            }

            if(voltage>869){   // 53.7
                //stop charging
                P3OUT|=BIT3;// BLUE LED OFF
                P1OUT&=~BIT4; // CHARGING OFF
                
                displayState=DISP_MENU;

            }
            break; 

        case DISP_DISCHARGE_50:
            if(displayState!=previousState){
                P3OUT&=~BIT0; // RED LED ON
                P1OUT&=~BIT3; // DISCHARGING ON
                ssd1306_clearDisplay();
                draw5x7Str(10, 30, "Discharging to 50%", 1);
                draw12x16Str(10,45,"EXIT", 1);
                previousState=displayState;
            }
            Exit();// checks for button press to exit
            displayVolt();
            
            if(displayState==DISP_MENU){
                P3OUT|=BIT0; // RED LED OFF
                P1OUT|=BIT3;// DISCHARING OFF
            }

            if(voltage<839){
                //stop charging
                P1OUT|=BIT3; // DISCHARGING OFF
                P3OUT|=BIT0;  // BLUE LED OFF
                
                displayState=DISP_MENU;

            }
            break;         

        case DISP_MENU_DELAY:
            if(displayDelayCounter++ == 60)
                displayState = DISP_SLEEP;
            break;

        case DISP_SLEEP:
            displayClear();
            displayOff();
            break;

        default:
            break;
    }

    //P3OUT ^= BIT0;

}

void displayInit(void)
{
    // init cannot happen soon after a cold boot, introduce a delay before calling this
    oledInit();
    displayOff();
    oledCustomInit();

}

void displayClear(void)
{
    ssd1306_clearDisplay();
}

void displayOff(void)
{
    ssd1306_command(SSD1306_DISPLAYOFF);
}

void displayOn(void)
{
    ssd1306_command(SSD1306_DISPLAYON);
}

void oledInit(void)
{
    // i2c and OLED initializations
    i2c_init();
    ssd1306_init();
}

void oledCustomInit(void)
{
    // Brightness/contrast to full for daylight viewing
    ssd1306_command(SSD1306_SETCONTRAST);                               // 0x81
    ssd1306_command(0xFF);
    displayOn();
}



void displayIntro(void)
{
    ssd1306_clearDisplay();

    draw12x16Str(15, 15, "BattStor", 1);
    draw5x7Str  (80, 40, "v1.0",  1);
    draw5x7Str  (60, 50, "BY PHPP",  1);

}

void displayMenu(void)
{
   
    int next_option;
    // draw12x16Str(5,  5, "FONT 12x16", 0);
    draw12x16Str(5, 30, menu[current_option], 0);
    next_option=(current_option+1)%opt_no;
    draw5x7Str  (25, 55,menu[next_option],  1);

}






void int_to_chars(unsigned int number, char* n) {
    // Assumes number is like 428, for 42.8
    int hundreds = number / 100;          // 4
    int tens = (number / 10) % 10;        // 2
    int ones = number % 10;               // 8

    n[0] = '0' + hundreds;
    n[1] = '0' + tens;
    n[2] = '.';
    n[3] = '0' + ones;
    n[4] = '\0'; // Null-terminate
   
}

void converted(unsigned int ADCMEM) {// should take some arguments
    
    // float vref = 2.5; // Internal 2.5V reference
    float voltage;
    uint16_t volts;
    
    // Assuming you need to convert ADC value to voltage with reference
    voltage = (104.12 * (ADCMEM* vref / 1023.0) / 4.12);
    volts = (voltage * 10 + 0.5);  // 42.77 to 428
    
    // Convert voltage to characters
    int_to_chars(volts, n);
    
    
}
void displayVolt(void){
    draw12x16Str(5,  5, "V:", 1);
    converted(voltage);
    draw12x16Str(30,  5, n, 1);

}

void Exit(void){
    
    if(buttState==BUTT_SHORT_STATE){
        draw12x16Str(10,45,"EXIT", 0);
            buttState=BUTT_OFF_STATE;
            r++;

    }
    else if(buttState==BUTT_LONG_STATE){
        if(r>0){
            displayState=DISP_MENU;
            // P3OUT&=~BIT3;
            // P3OUT|=BIT2;
            r=0;
            }
        buttState=BUTT_OFF_STATE;
    }  
        
       
    
}

void IdleTimeCounter(void){
    if(f==1){
    P3OUT ^= BIT2;
    wait_time++;
    }
    if(wait_time==30){
        wait_time=0;
        f=0;
        displayState=DISP_DISCHARGE_50;
    }
}
