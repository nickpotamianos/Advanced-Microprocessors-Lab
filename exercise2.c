#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

// Time Definitions
#define T1 2000 // Tram passage interval in timer ticks
#define T2 600  // Pedestrian green light duration in timer ticks
#define T3 1000 // Cool down period for pedestrian button in timer ticks

volatile int button_pressed = 0; // Flag to track pedestrian button press
volatile int tram_active = 0;    // Flag to indicate tram passage

// Setup function for traffic lights and pedestrian button
void setup_traffic_lights_and_button() {
    // Configure traffic light pins as output
    PORTD.DIR |= PIN0_bm | PIN1_bm | PIN2_bm;
    
    // Initial state: main road green, pedestrian red
    PORTD.OUT &= ~PIN0_bm; // Main road green
    PORTD.OUT |= PIN1_bm;  // Pedestrian red

    // Enable pull-up resistor and interrupt for the pedestrian button
    PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

// Setup function for all timers, including tram and pedestrian light control
void setup_timers() {
    // Enable Split Mode for TCA0
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

    // Configure lower byte (LCMP0) for pedestrian light duration (T2)
    TCA0.SPLIT.LCMP0 = T2;
    
    // Configure higher byte (HCMP0) for tram passage interval (T1)
    TCA0.SPLIT.HCMP0 = T1;

    // Set clock frequency and enable the timer with a prescaler of 1024 for both split timers
    TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV1024_gc | 1;

    // Enable Compare Match interrupts for both split timers
    TCA0.SPLIT.INTCTRL = TCA_SPLIT_LCMP0EN_bm | TCA_SPLIT_HCMP0EN_bm;
}

int main() {
    setup_traffic_lights_and_button();
    setup_timers();

    sei(); // Enable global interrupts

    while (1) {
        // Main loop remains empty, as functionality is interrupt-driven
    }
}

// Interrupt Service Routine for the pedestrian button press
ISR(PORTF_PORT_vect) {
    // Only act if button was not recently pressed (implement cool down logic here if needed)
    if (!tram_active && !button_pressed) {
        button_pressed = 1; // Set flag to handle button press
        
        // Change traffic lights: main road red, pedestrian green
        PORTD.OUT |= PIN0_bm;  // Main road red
        PORTD.OUT &= ~PIN1_bm; // Pedestrian green
    }
    
    // Clear the interrupt flag
    PORTF.INTFLAGS = PORTF.INTFLAGS;
}

// ISR for pedestrian light duration timer (T2) using TCA0's lower byte
ISR(TCA0_LUNF_vect) {
    // Reset traffic lights to default state after pedestrian light duration
    PORTD.OUT &= ~PIN0_bm; // Main road green
    PORTD.OUT |= PIN1_bm;  // Pedestrian red
    
    button_pressed = 0; // Reset button press flag to allow reactivation

    // Clear the interrupt flag for the lower byte compare match
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm;
}

// ISR for tram passage timer (T1) using TCA0's higher byte
ISR(TCA0_HUNF_vect) {
    tram_active = 1; // Indicate tram is active
    
    // Activate tram logic: all lights red to simulate tram passing
    PORTD.OUT |= PIN0_bm | PIN1_bm; // Main road and pedestrian red
    
    // Implement tram passage duration (T2) handling here, similar to pedestrian light logic
    
    tram_active = 0; // Reset tram flag after handling

    // Clear the interrupt flag for the higher byte compare match
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
}
