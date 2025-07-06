/*
 * display.h
 *
 *  Created on: Jun 17, 2025
 *      Author: Ryan
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_



enum displayStates {
    DISP_INIT_DELAY,        // wait before starting INIT
    DISP_INIT,
    DISP_INTRO,
    DISP_INTRO_DELAY,
    DISP_MENU,              // display is in MENU mode
    DISP_CHARGE_100,
    DISP_CHARGE_50,
    DISP_DISCHARGE_50,
    DISP_MENU_DELAY,
    DISP_SLEEP              // display is off: in power saving / idle mode
};

enum displayStates displayState = DISP_INIT_DELAY, previousState=DISP_SLEEP;
extern volatile unsigned int voltAvg;



# define vref 2.5
# define opt_no 3




void displayInit(void);
void displayClear(void);
void displayOff(void);
void displayOn(void);
void oledInit(void);
void oledCustomInit(void);
void displayIntro(void);
void displayMenu(void);
void displayVolt(void);
void converted(unsigned int);
void int_to_chars(unsigned int, char *);
void Exit(void);


#endif /* DISPLAY_H_ */
