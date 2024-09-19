#include <avr/io.h>
#include <avr/interrupt.h>
#define low_moisture_limit 15
#define high_moisture_limit 30
int moisture_level_flag;
ISR(ADC0_WCOMP_vect) {
cli();
// Ελέγχος αν το αποτέλεσμα είναι μικρότερο από το ελάχιστο όριο
if (ADC0.RES < low_moisture_limit) {
moisture_level_flag = 1; // Ορισμός flag για χαμηλή υγρασία
}
// Ελέγχος αν το αποτέλεσμα είναι μεγαλύτερο από το μέγιστο όριο
else if (ADC0.RES > high_moisture_limit) {
moisture_level_flag = 2; // Ορισμός flag για υψηλή υγρασία
}
sei();
}
int main(void) {
// Αρχικοποίηση LED
PORTD.DIRSET = PIN2_bm | PIN1_bm | PIN0_bm; // Ρύθμιση εξόδων (2: 
Σύστημα Αερισμού, 1: Υψηλή Υγρασία, 0: Χαμηλή Υγρασία)
PORTF.DIRCLR = PIN5_bm | PIN6_bm;
ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; // 10-bit resolution
ADC0.CTRLA |= ADC_FREERUN_bm; // Free-Running mode enabled
ADC0.CTRLA |= ADC_ENABLE_bm; // Ενεργοποίηση ADC
ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; // Επιλογή Αισθητήρα
ADC0.DBGCTRL |= ADC_DBGRUN_bm; // Enable Debug Mode
ADC0.WINLT |= low_moisture_limit; // Ορισμός κατώτερου ορίου
ADC0.WINHT |= high_moisture_limit; // Ορισμός ανώτερου ορίου
ADC0.INTCTRL |= ADC_WCMP_bm;
ADC0.CTRLE |= ADC_WINCM1_bm; // Διακοπή του RES όταν είναι εκτός
sei();
ADC0.COMMAND |= ADC_STCONV_bm;
while(1) {
// Ενέργειες με βάση την τιμή της moisture_level_flag
switch(moisture_level_flag) {
case 1:
PORTD.OUTSET = PIN0_bm; // Ενεργοποίηση LED0 για χαμηλή
υγρασία
break;
case 2:
PORTD.OUTSET = PIN1_bm; // Ενεργοποίηση LED1 για υψηλή
υγρασία
break;
default:
// Επαναφορά των LED αν δεν υπάρχει ιδιαίτερη συνθήκη
PORTD.OUTCLR = PIN0_bm | PIN1_bm;
}
// Επαναφορά της σημαίας moisture_level_flag
moisture_level_flag = 0;
}
}