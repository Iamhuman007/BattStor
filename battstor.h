/*
 * battstor.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Ryan
 */

#ifndef BATTSTOR_H_
#define BATTSTOR_H_


#define OLED_UPDATE_INTERVAL_IN_ACLOCK_TICKS 8000

enum buttonstates {
    BUTT_LONG_STATE,
    BUTT_SHORT_STATE,
    BUTT_OFF_STATE
};

extern enum buttonstates buttState;

# define DIGITAL_IP BIT2
# define ANALOG_IP  BIT5
# define BUTT_LONG_COUNT 750
# define BUTT_SHORT_COUNT 200
// # define BUTT_LONG_COUNT 20
# define ADC_CONSEQ_COUNT 6







void unusedPinsConfiguration(void);
void ledConfiguration(void);
void stateUpdateTimerConfiguration(void);
void IdleTimerConfig(void);



void determineDisplayState(void);
void updateDisplay(void);

void IdleTimeCounter(void);


void Used_pin_configuration(void);
void ADCinit(void);
void LEDinit(void);
void digital_init(void);

void adcMultiConfiguration(void);
void adcStartConversionsAndDT(void); 



#endif /* BATTSTOR_H_ */
