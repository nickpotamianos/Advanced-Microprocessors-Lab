#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define PED_LIGHT_DURATION 600 // Duration for pedestrian green light in timer ticks

volatile int button_pressed = 0; // Flag to track pedestrian button press

void setup_traffic_lights_and_button() {
    // Configure traffic light pins as output
    PORTD.DIR |= PIN0_bm | PIN1_bm | PIN2_bm; // PIN0: Main road, PIN1: Pedestrians, PIN2: Not used currently
    
    // Set initial state: main road green, pedestrian red
    PORTD.OUT &= ~PIN0_bm; // Main road green
    PORTD.OUT |= PIN1_bm;  // Pedestrian red

    // Enable pull-up resistor and interrupt on both edges for the pedestrian button
    PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

void setup_timer_for_pedestrian_light() {
    // Clear the timer counter
    TCA0.SINGLE.CNT = 0;
    // Set Normal Mode
    TCA0.SINGLE.CTRLB = 0;
    // Set compare match value for pedestrian light duration
    TCA0.SINGLE.CMP0 = PED_LIGHT_DURATION;
    // Set clock frequency and enable the timer with a prescaler of 1024
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | 1;
    // Enable Compare Match interrupt
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}

int main() {
    setup_traffic_lights_and_button();
    setup_timer_for_pedestrian_light();

    sei(); // Enable global interrupts

    while (1) {
        if (button_pressed) {
            // Start the timer for pedestrian green light
            TCA0.SINGLE.CTRLA |= 1; // Re-enable the timer in case it was disabled
            button_pressed = 0; // Reset button press flag
        }
    }
}

ISR(PORTF_PORT_vect) {
    // Clear the interrupt flag
    PORTF.INTFLAGS = PORTF.INTFLAGS;
    // Set flag to indicate pedestrian button was pressed
    button_pressed = 1;

    // Change traffic lights: main road red, pedestrian green
    PORTD.OUT |= PIN0_bm; // Main road red
    PORTD.OUT &= ~PIN1_bm; // Pedestrian green
}

ISR(TCA0_CMP0_vect) {
    // Disable the timer to stop the pedestrian light duration
    TCA0.SINGLE.CTRLA &= ~1;

    // Clear the compare match interrupt flag
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;

    // Reset traffic lights to default state: main road green, pedestrian red
    PORTD.OUT &= ~PIN0_bm; // Main road green
    PORTD.OUT |= PIN1_bm;  // Pedestrian red
}
