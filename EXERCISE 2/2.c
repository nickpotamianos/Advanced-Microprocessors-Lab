#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void check_right();
void turning_timer();
void initialize_ADC0();
void frunning_ADC0();
void start_ADC0();

int turn_left=0;
int forward=1;
int turn_right=0;

int end=0;

#define ped 10
#define limit 10  // Threshold for detecting a wall

int main(){
    PORTD.DIRSET = PIN0_bm|PIN1_bm|PIN2_bm; // 2:left, 1:forward, 0:right
    PORTD.OUT |= (PIN0_bm|PIN2_bm); // Start going forward

    initialize_ADC0();
    frunning_ADC0();
    start_ADC0();
    sei(); // Enable global interrupts

    while(1){
        check_right();

        if (end == 4) break; // Stop after completing the outline

        while (turn_left){
            end++;
            turning_timer();
            while (turn_left){}
        }

        while (turn_right){
            end--;
            turning_timer();
            while (turn_right){}
        }
    }

    cli(); // Disable global interrupts
    return 0;
}

ISR(ADC0_WCOMP_vect){
    cli(); // Disable interrupts
    // Clear the ADC interrupt flags
    ADC0.INTFLAGS = ADC0.INTFLAGS;

    // Decision making for left or right turn
    if (ADC0.RES < limit) { // Detects wall, prepare to turn left
        PORTD.OUT |= (PIN0_bm|PIN1_bm|PIN2_bm); // No movement
        PORTD.OUTCLR |= PIN2_bm; // Turn left
        forward = 0;
        turn_left = 1;
        turn_right = 0;
    } else { // No wall detected, prepare to turn right
        PORTD.OUT |= (PIN0_bm|PIN1_bm|PIN2_bm); // No movement
        PORTD.OUTCLR |= PIN0_bm; // Turn right
        forward = 0;
        turn_right = 1;
        turn_left = 0;
    }
    sei(); // Re-enable interrupts
}


void check_right(){
    // Temporarily disable free-running mode to check right
    ADC0.CTRLA &= ~ADC_FREERUN_bm; // Disable free-running
    ADC0.MUXPOS &= ~ADC_MUXPOS_AIN7_gc; // Change MUXPOS to right sensor if needed
    // ADC0.MUXPOS |= ADC_MUXPOS_AINx_gc; // Adjust to select the right sensor
    ADC0.COMMAND = ADC_STCONV_bm; // Start single conversion for right sensor
    _delay_ms(1); // Delay for ADC conversion to complete
    frunning_ADC0(); // Return to free-running mode for front sensor
}

void turning_timer(){
    // Set the timer for turning
    TCA0.SINGLE.CNT = 0; // Clear counter
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // Normal Mode
    TCA0.SINGLE.CMP0 = ped; // When reaches this value -> interrupt CLOCK Frequency/1024
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm; // Set clock and enable
	 TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; // Interrupt Enable
}

void initialize_ADC0(){
    ADC0.CTRLA = ADC_RESSEL_10BIT_gc | ADC_ENABLE_bm; // 10-bit resolution, Enable ADC
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc; // Select ADC channel (front sensor)
    ADC0.DBGCTRL = ADC_DBGRUN_bm; // Enable debug mode
}

void frunning_ADC0(){
    ADC0.CTRLA |= ADC_FREERUN_bm; // Free-running mode enabled
    ADC0.WINLT = limit; // Set the lower threshold for the front sensor
    ADC0.WINHT = 0xFFFF; // Set the upper threshold to max (unused in this context)
    ADC0.INTCTRL = ADC_WCMP_bm; // Enable interrupts for Window Comparator Mode
    ADC0.CTRLE = ADC_WINCM0_bm; // Interrupt when RESULT < WINLT (front sensor)
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc; // Select the front sensor channel
}

void start_ADC0(){
    ADC0.COMMAND = ADC_STCONV_bm; // Start Conversion (front sensor)
}

// Complete the ISR for the TCA0 compare match
ISR(TCA0_CMP0_vect) {
	TCA0.SINGLE.CTRLA = 0; // Disable the timer
	TCA0.SINGLE.INTFLAGS = TCA0.SINGLE.INTFLAGS; // Clear the timer interrupt flag

	// Decide what to do based on the flags set
	if (turn_left) {
		// Turn left logic
		turn_left = 0; // Reset the turn_left flag
		forward = 1; // Set forward to indicate moving straight after the turn
		end++; // Increment the end counter as we've completed a left turn
		} else if (turn_right) {
		// Turn right logic
		turn_right = 0; // Reset the turn_right flag
		forward = 1; // Set forward to indicate moving straight after the turn
		// Note: Depending on how you want to handle turns, you may want to adjust the end counter differently
		end--; // Decrement the end counter as we've completed a right turn
	}

	// Turn off all LEDs and then turn on the forward LED
	PORTD.OUT |= (PIN0_bm | PIN1_bm | PIN2_bm);
	PORTD.OUTCLR = PIN1_bm; // Forward LED on
}

