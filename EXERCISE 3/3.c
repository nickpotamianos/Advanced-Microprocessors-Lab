#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 20000000 // 20MHz clock speed
#define PRESCALER 1024
#define BLADE_PWM_PERIOD 254 // For 1ms period approximation
#define BASE_PWM_PERIOD 508  // For 2ms period approximation

volatile uint8_t fan_state = 0; // 0: off, 1: on
volatile uint8_t pressCount = 0;

void initializeSystem(void);
void setupPWM(void);
void toggleFanState(void);

ISR(PORTF_PORT_vect) {
    if (PORTF.INTFLAGS & PIN5_bm) { // Check if the interrupt is from switch 5
        toggleFanState(); // Toggle the fan state on each button press
        PORTF.INTFLAGS = PIN5_bm; // Clear interrupt flag
    }
}

ISR(TCA0_LUNF_vect) {
    // Toggle LED0 on Low Underflow Event
    PORTD.OUTTGL = PIN0_bm; // Toggle LED connected to PIN0
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm; // Clear the Low Underflow interrupt flag
}

ISR(TCA0_HUNF_vect) {
    // Toggle LED1 on High Underflow Event
    PORTD.OUTTGL = PIN1_bm; // Toggle LED connected to PIN1
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm; // Clear the High Underflow interrupt flag
}

void toggleFanState(void) {
    pressCount++; // Increment on each switch press

    if (pressCount == 1) {
        // First press - activate fan with initial settings
        fan_state = 1;
        setupPWM(); // Setup initial PWM for fan blades and base
    } else if (pressCount == 2) {
        // Second press - Double the period for the blade's PWM, maintain duty cycle
        TCA0.SPLIT.LPER = BLADE_PWM_PERIOD * 2; // Double the period for blade motion
        TCA0.SPLIT.LCMP0 = (BLADE_PWM_PERIOD * 2) / 2; // Adjust duty cycle accordingly
    } else {
        // Third press - turn off fan
        fan_state = 0;
        TCA0.SPLIT.CTRLA &= ~TCA_SPLIT_ENABLE_bm; // Disable PWM
        PORTD.OUTCLR = PIN0_bm | PIN1_bm; // Turn off LEDs
        pressCount = 0; // Reset press count to allow reactivation
    }
}

void initializeSystem(void) {
    PORTD.DIR |= PIN0_bm | PIN1_bm; // Set LED pins as output
    PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc; // Setup switch with pull-up and interrupt on falling edge
    sei(); // Enable global interrupts
}

void setupPWM(void) {
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm; // Enable split mode

    TCA0.SPLIT.LPER = BLADE_PWM_PERIOD; // Set period for blade motion
    TCA0.SPLIT.HPER = BASE_PWM_PERIOD;  // Set period for base motion
    TCA0.SPLIT.LCMP0 = BLADE_PWM_PERIOD / 2; // 50% duty for blade
    TCA0.SPLIT.HCMP0 = BASE_PWM_PERIOD * 0.6; // 60% duty for base

    TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP0EN_bm | TCA_SPLIT_HCMP0EN_bm; // Enable PWM output

    // Enable High and Low Underflow interrupts
    TCA0.SPLIT.INTCTRL = TCA_SPLIT_LUNF_bm | TCA_SPLIT_HUNF_bm;

    TCA0.SPLIT.CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV1024_gc; // Start the PWM with the prescaler
}

int main(void) {
    initializeSystem();

    while (1) {
        // Main loop remains idle; functionality is interrupt-driven
    }
}
