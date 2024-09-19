#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 20000000 
#define PRESCALER 1024
#define BLADE_PWM_PERIOD 254
#define BASE_PWM_PERIOD 508 

volatile uint8_t fan_state = 0;
volatile uint8_t pressCount = 0;

void initializeSystem(void);
void setupPWM(void);
void toggleFanState(void);

ISR(PORTF_PORT_vect) {
    if (PORTF.INTFLAGS & PIN5_bm) {
        toggleFanState(); 
        PORTF.INTFLAGS = PIN5_bm; 
    }
}

ISR(TCA0_LUNF_vect) {
    PORTD.OUTTGL = PIN0_bm;
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm;
}

ISR(TCA0_HUNF_vect) {
    PORTD.OUTTGL = PIN1_bm;
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
}

void toggleFanState(void) {
    pressCount++;

    if (pressCount == 1) {
        fan_state = 1;
        setupPWM();
    } else if (pressCount == 2) {
        
    } else {
        fan_state = 0;
        TCA0.SPLIT.CTRLA &= ~TCA_SPLIT_ENABLE_bm;
        PORTD.OUTCLR = PIN0_bm | PIN1_bm;
        pressCount = 0;
    }
}

void initializeSystem(void) {
    PORTD.DIR |= PIN0_bm | PIN1_bm;
    PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    sei();
}

void setupPWM(void) {
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

    TCA0.SPLIT.LPER = BLADE_PWM_PERIOD;
    TCA0.SPLIT.HPER = BASE_PWM_PERIOD;
    TCA0.SPLIT.LCMP0 = BLADE_PWM_PERIOD / 2;
    TCA0.SPLIT.HCMP0 = BASE_PWM_PERIOD * 0.6;

    TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP0EN_bm | TCA_SPLIT_HCMP0EN_bm;

    TCA0.SPLIT.INTCTRL = TCA_SPLIT_LUNF_bm | TCA_SPLIT_HUNF_bm;

    TCA0.SPLIT.CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV1024_gc;
}

int main(void) {
    initializeSystem();

    while (1) {
        
    }
}
