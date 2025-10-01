/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Play songs on a 8 Ohm speaker
Author: Maynor Bac Itzep 

*/
#include <stdint.h>

// Our base addresses
#define RCC_BASE        0x40021000
#define GPIOB_BASE      0x48000400
#define TIM2_BASE       0x40000000

 // RCC
#define RCC_AHB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x4C)) //Lets the registers respond
#define RCC_APB1ENR1    (*(volatile uint32_t *)(RCC_BASE + 0x58)) //Will help enable our timer2

// GPIOB
#define GPIOB_MODER     (*(volatile uint32_t *)(GPIOB_BASE + 0x00)) //Helps us set output for pins
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x14)) //Will hold the value for our pins

// TIM2
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00)) //will help enable counter
#define TIM2_DIER       (*(volatile uint32_t *)(TIM2_BASE + 0x0C)) //Useful for overflow, requests intrupt
#define TIM2_SR         (*(volatile uint32_t *)(TIM2_BASE + 0x10)) //It is a flag, the actual intrupt., Manually needs to be cleared
#define TIM2_CNT        (*(volatile uint32_t *)(TIM2_BASE + 0x24)) //Holds the count
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28)) // Divides input clock
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C)) //Max count it can hold

#define cSharp 554
#define dSharp 622
#define aSharp 466
#define gSharp 415
#define fSharp 370
const int custom[][2] = {
{cSharp , 125}, {0, 50}, {cSharp, 125},{0, 50}, {dSharp, 125},
{cSharp, 125}, {aSharp, 125}, {gSharp, 125}, {0, 40}, {gSharp, 250},
{fSharp, 125},  {0, 40},{fSharp, 125},{cSharp , 125}, {0, 50},
 {cSharp, 125},{0, 50}, {dSharp, 125},
 {  0,	0}
};


const int notes[][2] = {
{659,	125},{623,	125},{659,	125},{623,	125},
{659,	125},{494,	125},{587,	125},{523,	125},
{440,	250},{  0,	125},{262,	125},{330,	125},
{440,	125},{494,	250},{  0,	125},{330,	125},
{416,	125},{494,	125},{523,	250},{  0,	125},
{330,	125},{659,	125},{623,	125},{659,	125},
{623,	125},{659,	125},{494,	125},{587,	125},
{523,	125},{440,	250},{  0,	125},{262,	125},
{330,	125},{440,	125},{494,	250},{  0,	125},
{330,	125},{523,	125},{494,	125},{440,	250},
{  0,	125},{494,	125},{523,	125},{587,	125},
{659,	375},{392,	125},{699,	125},{659,	125},
{587,	375},{349,	125},{659,	125},{587,	125},
{523,	375},{330,	125},{587,	125},{523,	125},
{494,	250},{  0,	125},{330,	125},{659,	125},
{  0,	250},{659,	125},{1319,	125},{  0,	250},
{623,	125},{659,	125},{  0,	250},{623,	125},
{659,	125},{623,	125},{659,	125},{623,	125},
{659,	125},{494,	125},{587,	125},{523,	125},
{440,	250},{  0,	125},{262,	125},{330,	125},
{440,	125},{494,	250},{  0,	125},{330,	125},
{416,	125},{494,	125},{523,	250},{  0,	125},
{330,	125},{659,	125},{623,	125},{659,	125},
{623,	125},{659,	125},{494,	125},{587,	125},
{523,	125},{440,	250},{  0,	125},{262,	125},
{330,	125},{440,	125},{494,	250},{  0,	125},
{330,	125},{523,	125},{494,	125},{440,	500},
{  0,	0}};

// Delay functioon that would hopefully run a delay
//Adjusted syntax from the clock configuration demo to something I understand
void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        TIM2_CNT = 0;                 // reset timer counter
        while (!(TIM2_SR & 1)){}  // wait for UIF
        TIM2_SR &= ~(1<<0);          // clear UIF
    }
}

//Wrapper function to delay in miliseconds
void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(100); 
    }
}

void playNote(int freq, int ms) {
    if (freq == 0) {
        delay_ms(ms); // Rest in µs
        return;
    }
  //Since we are in microseconds, we scale the traditional period equation by 1x10^6
  //Since we are doing square waves, we do a 2 for half periods up and down
    unsigned int half_period_us = 100000 / (2 * freq);
    unsigned int dur_us = ms * 100; //multiply to chnage tempo
    unsigned int elapsed = 0;

    while (elapsed < dur_us) {
        GPIOB_ODR ^= (1 << 3);   // Toggle PB3
        delay_us(half_period_us);
        elapsed += half_period_us;
    }
  }

int main(void) {
    int i = 0;

    // Enable clocks
    RCC_AHB2ENR  |= (1 << 1);   // GPIOB clock
    RCC_APB1ENR1 |= (1 << 0);   // TIM2 clock

    // PB5 as output. Clears everything and sets PB5 as output
    GPIOB_MODER &= ~(3 << (3 * 2)); //Every pin uses 2 bits, 
    GPIOB_MODER |=  (1 << (3 * 2)); //So pin numbrer * 2 is used

    // TIM2 Configure
    TIM2_PSC = 3;     //
    TIM2_ARR =  9;     // Frq = 4MHz / ((PSC + 1) * (ARR + 1))...Freq = 100KHz
    TIM2_CR1 |= 1;     // Enable counter



    //Checks if both the duratrion and frequency does not equal zero as thta implies trhye ending
    while(!(notes[i][0] == 0 && notes[i][1] == 0)){
      playNote(notes[i][0], notes[i][1]);
      i++;
      GPIOB_ODR = ~(1 << 3); //This helps with the speaker genrate constant square waves.
    }
    // notes, custom
    while(1){
    
    }
} 


/*************************** End of file ****************************/