#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int turn_left = 0;
int forward = 1;
int turn_right = 0;

int end = 0;

#define ped 10

int main() {
	PORTD.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm; // 2:left 1:forward 0:right
	PORTD.OUT &= ~(PIN1_bm); // Start going forward by turning LED1 on

	// Initialize the ADC for Free-Running mode
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc | ADC_FREERUN_bm | ADC_ENABLE_bm;
	ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc; // Select AIN7 for the front sensor
	ADC0.DBGCTRL = ADC_DBGRUN_bm; // Enable Debug Mode
	ADC0.WINLT = 5; // Set threshold
	ADC0.INTCTRL = ADC_WCMP_bm; // Enable Interrupts for Window Comparator Mode
	ADC0.CTRLE = ADC_WINCM0_bm; // Interrupt when RESULT < WINLT
	sei(); // Enable global interrupts
	ADC0.COMMAND = ADC_STCONV_bm; // Start Conversion

	while (1) {
		if (end == 4) break; // Stop after completing the square

		if (forward) {
			PORTD.OUT &= ~(PIN1_bm); // Ensure forward LED is on
		}

		if (turn_left) {
			// Configure timer for left turn delay
			TCA0.SINGLE.CNT = 0; // Clear counter
			TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // Set to Normal Mode
			TCA0.SINGLE.CMP0 = ped; // Set compare value for interrupt
			TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | 1; // Set clock and enable
			TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; // Enable compare interrupt
			while (turn_left) {
				// Wait for turn to complete, interrupt will clear turn_left
			}
			PORTD.OUT |= (PIN0_bm | PIN1_bm | PIN2_bm); // Turn off all movement LEDs
			PORTD.OUTCLR = PIN1_bm; // Resume forward movement
			forward = 1; // Set forward movement flag
		}
	}
	cli(); // Disable global interrupts
	return 0;
}

ISR(ADC0_WCOMP_vect) {
	cli();
	// Clear the ADC interrupt flag
	ADC0.INTFLAGS = ADC0.INTFLAGS;

	PORTD.OUT |= (PIN0_bm | PIN1_bm | PIN2_bm); // Turn off all movement LEDs
	PORTD.OUTCLR = PIN2_bm; // Turn on left turn LED
	forward = 0;
	turn_left = 1; // Indicate turning left
	sei();
}

ISR(TCA0_CMP0_vect) {
	// Disable timer and clear flag
	TCA0.SINGLE.CTRLA = 0;
	TCA0.SINGLE.INTFLAGS = TCA0.SINGLE.INTFLAGS;

	turn_left = 0; // Clear turn left flag
	end++; // Increment corner counter
}
