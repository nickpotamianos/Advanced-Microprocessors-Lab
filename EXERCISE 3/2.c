#include <avr/io.h>
#include <avr/interrupt.h>

volatile int blade_movement = 0; // 0: stop, 1: rotate

int main() {
    PORTF.DIR &= ~(PIN5_bm); // Set PIN5 as input for switch
    PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc; // Enable pull-up and set to trigger on falling edge
    
    PORTD.DIR |= PIN0_bm | PIN1_bm; // Set PIN0 and PIN1 as output for LEDs

    // Initialize TCA0 for Single-Slope PWM on both WO0 and WO1
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc; // Prescaler = 1024
    TCA0.SINGLE.PER = 255; // Set period
    TCA0.SINGLE.CMP0 = 0; // Initially off
    TCA0.SINGLE.CMP1 = 0; // Initially off
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm; // Enable Compare Channel 0 and 1
    
    sei(); // Enable global interrupts

    while (1) {
        if(blade_movement) {
            // Simulate circular movement by alternating PWM duty cycle between LEDs
            for (uint16_t i = 0; i <= TCA0.SINGLE.PER; i++) {
                TCA0.SINGLE.CMP0 = i; // Adjust duty cycle for LED 1
                TCA0.SINGLE.CMP1 = TCA0.SINGLE.PER - i; // Adjust duty cycle for LED 2
                _delay_ms(10); // Delay to visually distinguish the rotation effect
            }
        } else {
            // Stop the movement by turning off both LEDs
            TCA0.SINGLE.CMP0 = 0;
            TCA0.SINGLE.CMP1 = 0;
        }
    }
}

ISR(PORTF_PORT_vect) {
    if(PORTF.INTFLAGS & PIN5_bm) {
        blade_movement = !blade_movement; // Toggle blade movement state
        if(blade_movement) {
            TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // Enable TCA0 to start PWM
        } else {
            TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; // Disable TCA0 to stop PWM
        }
        PORTF.INTFLAGS = PIN5_bm; // Clear the interrupt flag for switch 5
    }
}
