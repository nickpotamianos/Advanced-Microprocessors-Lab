#include <avr/io.h>
#include <avr/interrupt.h>
#define low_moisture_limit 15
#define high_moisture_limit 30
int moisture_level_flag;
int watering_system_flag = 0;
int ventilation_system_flag = 0;
int calculated_time;
// Function to activate all LEDs indicating a wrong input
void activate_all_leds(void) {
 PORTD.OUTSET = PIN0_bm | PIN1_bm | PIN2_bm; // Turn on all LEDs
 deactivate_all_leds();
}
// Function to deactivate all LEDs
void deactivate_all_leds(void) {
 PORTD.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm; // Turn off all LEDs
}
ISR(ADC0_WCOMP_vect) {
 cli(); // Disable global interrupts
int adc_intflags = ADC0.INTFLAGS;
 ADC0.INTFLAGS = adc_intflags;
 // Handling the ADC result
 if (ADC0.RES < low_moisture_limit) {
 moisture_level_flag = 1;
 calculated_time = low_moisture_limit - ADC0.RES;
 } else if (ADC0.RES > high_moisture_limit) {
 moisture_level_flag = 2;
 }
 sei(); // Enable global interrupts
}
ISR(PORTF_PORT_vect) {
 cli();
 int portf_intflags = PORTF.INTFLAGS;
 PORTF.INTFLAGS = portf_intflags;
 if (portf_intflags & PIN5_bm) {
 if (moisture_level_flag == 1) {
 watering_system_flag = 1;
 } else {
 activate_all_leds(); // Wrong button pressed
 }
 }
 if (portf_intflags & PIN6_bm) {
 if (moisture_level_flag == 2) {
 ventilation_system_flag = 1;
 } else {
 activate_all_leds(); // Wrong button pressed
 }
 }
 sei();
}
ISR(TCA0_CMP0_vect) {
 cli();
 TCA0.SINGLE.CTRLA = 0;
 int intflags = TCA0.SINGLE.INTFLAGS;
 TCA0.SINGLE.INTFLAGS = intflags;
 watering_system_flag = 0;
 deactivate_all_leds(); // Reset LEDs to default state after watering
 sei();
}
void activate_watering_system(void) {
 TCA0.SINGLE.CNT = 0;
 TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
 TCA0.SINGLE.CMP0 = calculated_time;
 TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | 
TCA_SINGLE_ENABLE_bm;
 TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
 PORTD.OUTSET = PIN0_bm; // Turn on LED0
}
void activate_ventilation_system(void) {
 TCA0.SINGLE.CNT = 0;
 TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc;
 TCA0.SINGLE.PER = 0xFF;
 TCA0.SINGLE.CMP0 = 0x7F;
 TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc | TCA_SINGLE_ENABLE_bm;
 TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
 PORTD.OUTSET = PIN2_bm; // Turn on LED2
}
ISR(TCA0_OVF_vect) {
 static uint8_t pwm_count = 0;
 cli();
 pwm_count++;
 if (pwm_count == 4) {
 TCA0.SINGLE.CTRLA = 0;
 deactivate_all_leds(); // Reset LEDs to default state after 
ventilation
 ventilation_system_flag = 0;
 pwm_count = 0;
 }
 sei();
}
int main(void) {
 PORTD.DIRSET = PIN2_bm | PIN1_bm | PIN0_bm;
 PORTF.DIRCLR = PIN5_bm | PIN6_bm;
 PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
 PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
 ADC0.CTRLA |= ADC_RESSEL_10BIT_gc | ADC_FREERUN_bm | ADC_ENABLE_bm;
 ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc;
 ADC0.DBGCTRL |= ADC_DBGRUN_bm;
 ADC0.WINLT |= low_moisture_limit;
 ADC0.WINHT |= high_moisture_limit;
 ADC0.INTCTRL |= ADC_WCMP_bm;
 ADC0.CTRLE |= ADC_WINCM1_bm;
 sei();
 ADC0.COMMAND |= ADC_STCONV_bm;
 while (1) {
 switch (moisture_level_flag) {
 case 1:
 PORTD.OUTSET = PIN0_bm;
break;
 case 2:
 PORTD.OUTSET = PIN1_bm;
 break;
 default:
 PORTD.OUTCLR = PIN0_bm | PIN1_bm;
break;
 }
 if (watering_system_flag) {
 activate_watering_system();
 watering_system_flag = 0;
 }
 if (ventilation_system_flag) {
 activate_ventilation_system();
 ventilation_system_flag = 0;
 }
 moisture_level_flag = 0;
 }
}