#include <avr/io.h>
#include <avr/interrupt.h>
#define low_moisture_limit 15
#define high_moisture_limit 30
int moisture_level_flag;
int watering_system_flag = 0;
int ventilation_system_flag = 0;
int calculated_time;
ISR(ADC0_WCOMP_vect) {
 cli();
 // Check if the result is less than the minimum limit
 if (ADC0.RES < low_moisture_limit) {
 moisture_level_flag = 1; // Set flag for low moisture
 calculated_time = low_moisture_limit - ADC0.RES; // Calculate time 
for watering
 }
 // Check if the result is greater than the maximum limit
 else if (ADC0.RES > high_moisture_limit) {
 moisture_level_flag = 2; // Set flag for high moisture
 }
 sei();
}
ISR(PORTF_PORT_vect) {
 cli();
 int portf_intflags = PORTF.INTFLAGS; // Procedure to clear the 
interrupt flag
 PORTF.INTFLAGS = portf_intflags;
 if (portf_intflags & PIN5_bm) {
 watering_system_flag = 1; // Set flag to activate watering system
 } else if (portf_intflags & PIN6_bm) {
 ventilation_system_flag = 1; // Set flag to activate ventilation 
system
 }
 sei();
}
ISR(TCA0_CMP0_vect) {
 cli();
 TCA0.SINGLE.CTRLA = 0; // Disable timer
 int intflags = TCA0.SINGLE.INTFLAGS; // Procedure to clear interrupt 
flag
 TCA0.SINGLE.INTFLAGS = intflags;
 watering_system_flag = 0; // Reset watering system flag
 PORTD.OUTCLR = PIN0_bm; // Turn off LED0
 sei();
}
void activate_watering_system(void) {
 // Initialize timer
 TCA0.SINGLE.CNT = 0; // Clear counter
 TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // Normal Mode
 TCA0.SINGLE.CMP0 = calculated_time; // Set compare value
 TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | 
TCA_SINGLE_ENABLE_bm; // Set clock source and enable timer
 TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; // Enable compare interrupt
 PORTD.OUTSET = PIN0_bm; // Turn on LED0
}
void activate_ventilation_system(void) {
// Initialize PWM
TCA0.SINGLE.CNT = 0; // Clear counter
TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc; // Dual Slope 
Bottom PWM Mode
TCA0.SINGLE.PER = 0xFF; // Set period
TCA0.SINGLE.CMP0 = 0x7F; // Set compare value for 50% duty cycle
TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc | 
TCA_SINGLE_ENABLE_bm; // Set clock source and enable timer
TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Enable overflow interrupt
PORTD.OUTSET = PIN2_bm; // Turn on LED2
}
ISR(TCA0_OVF_vect) {
 static uint8_t pwm_count = 0;
 cli();
 pwm_count++;
 if (pwm_count == 4) {
 TCA0.SINGLE.CTRLA = 0; // Disable timer
 PORTD.OUTCLR = PIN1_bm | PIN2_bm; // Turn off LED1 and LED2
 ventilation_system_flag = 0; // Reset ventilation system flag
 pwm_count = 0; // Reset counter
 }
 sei();
}
int main(void) {
 // Initialize LEDs
 PORTD.DIRSET = PIN2_bm | PIN1_bm | PIN0_bm; // Set as outputs (2: 
Ventilation System, 1: High Moisture, 0: Low Moisture)
 PORTF.DIRCLR = PIN5_bm | PIN6_bm; // Set as inputs
 PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
 PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
 // Initialize the ADC for Free-Running mode
 ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; // 10-bit resolution
 ADC0.CTRLA |= ADC_FREERUN_bm; // Free-Running mode enabled
 ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
 ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; // Select sensor
 ADC0.DBGCTRL |= ADC_DBGRUN_bm; // Enable Debug Mode
 ADC0.WINLT |= low_moisture_limit; // Set lower limit
 ADC0.WINHT |= high_moisture_limit; // Set upper limit
 ADC0.INTCTRL |= ADC_WCMP_bm;
 ADC0.CTRLE |= ADC_WINCM1_bm; // Interrupt when RES is outside the 
window
 sei(); // Enable global interrupts
 ADC0.COMMAND |= ADC_STCONV_bm; // Start conversion
 while (1) {
 // Actions based on the moisture_level_flag value
 switch (moisture_level_flag) {
 case 1:
 PORTD.OUTSET = PIN0_bm; // Turn on LED0 for low moisture
 break;
 case 2:
 PORTD.OUTSET = PIN1_bm; // Turn on LED1 for high moisture
 break;
 default:
 // Clear LEDs if no specific condition
PORTD.OUTCLR = PIN0_bm | PIN1_bm;
 break;
 }
 // Check if the watering system needs to be activated
 if (watering_system_flag) {
 activate_watering_system();
 watering_system_flag = 0; // Reset the flag
 }
 // Check if the ventilation system needs to be activated
 if (ventilation_system_flag) {
 activate_ventilation_system();
 ventilation_system_flag = 0; // Reset the flag
 }
 // Reset the moisture_level_flag
 moisture_level_flag = 0;
 }
}