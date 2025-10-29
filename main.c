
/*
main.c
Maynor Bac Itzep
This code uses timer 2 to control music being played with a speaker, where notes is an array of frequencies and durations of these notes.
*/
#include <stdint.h>
#include "lab4.h"

#define pin      3 //PB3

const int notes[][2] = {
{659,	125},{623,	125},{659,	125},{623,	125},{659,	125},{494,	125},{587,	125},{523,	125},
{440,	250},{  0,	125},{262,	125},{330,	125},{440,	125},{494,	250},{  0,	125},{330,	125},
{416,	125},{494,	125},{523,	250},{  0,	125},{330,	125},{659,	125},{623,	125},{659,	125},
{623,	125},{659,	125},{494,	125},{587,	125},{523,	125},{440,	250},{  0,	125},{262,	125},
{330,	125},{440,	125},{494,	250},{  0,	125},{330,	125},{523,	125},{494,	125},{440,	250},
{  0,	125},{494,	125},{523,	125},{587,	125},{659,	375},{392,	125},{699,	125},{659,	125},
{587,	375},{349,	125},{659,	125},{587,	125},{523,	375},{330,	125},{587,	125},{523,	125},
{494,	250},{  0,	125},{330,	125},{659,	125},{  0,	250},{659,	125},{1319,	125},{  0,	250},
{623,	125},{659,	125},{  0,	250},{623,	125},{659,	125},{623,	125},{659,	125},{623,	125},
{659,	125},{494,	125},{587,	125},{523,	125},{440,	250},{  0,	125},{262,	125},{330,	125},
{440,	125},{494,	250},{  0,	125},{330,	125},{416,	125},{494,	125},{523,	250},{  0,	125},
{330,	125},{659,	125},{623,	125},{659,	125},{623,	125},{659,	125},{494,	125},{587,	125},
{523,	125},{440,	250},{  0,	125},{262,	125},{330,	125},{440,	125},{494,	250},{  0,	125},
{330,	125},{523,	125},{494,	125},{440,	500}, {440,	500},{0,	40},{440,	80},{0,	40},
};


void delayf(uint32_t cycles) {
    TIM2->ARR = 2000000/(1000*(40))-1; //for 1000 Hz, precise ms delay
    TIM2->CNT = 0;
    for (uint32_t i = 1; i <= cycles; i++) {
        while (!(TIM2->SR & 1)){}  //UIF 
        TIM2->SR &= ~(1<<0); 
    }
}

void freqf(int freq, int cycles) {
    TIM2->ARR = 2000000/(freq*(40))-1; //for up to 10,000/2 Hz, precise delay
    TIM2->CNT = 0;
    for (uint32_t i = 1; i <= cycles; i++) {
        while (!(TIM2->SR & 1)){}  //UIF 
        TIM2->SR &= ~(1<<0); 
        GPIOB->ODR ^= (1 << pin);
    }
}

int main(void){
    // RCC SET UP
    RCC->AHB2ENR |= (1 << 1);
    RCC->APB1ENR1 |= (1 << 0);

    //PIN SETUP
    GPIOB->MODER &= ~(3 << (pin * 2)); 
    GPIOB->MODER |=  (1 << (pin * 2));

    //TIMER SETUP
    TIM2->PSC = 39;     //PSC constant
    TIM2->CR1 |= 1;     // Enable counter

    int i = 0;
    while(!(notes[i][0] == 0 && notes[i][1] == 0)){
      int freq = notes[i][0];
      int delay = notes[i][1];
      if (freq == 0) { //Uses different function as original function is dynamic based off frequency
        delayf(delay*2); 
      }else{
        uint32_t cycles = freq*delay/500; //500 since 1000ms/s and we want a half cycle
        delayf(5);
        freqf(freq, cycles);
      }
      i++;
      GPIOB->ODR = ~(1 << pin);
    }
} 
