#include <main.h>
#include <stm8s.h>
#include <stdbool.h>
#include <stdio.h>
#include "max7219.h"
#include "milis.h"

// makra pro zapojení pinů
#define CLK_GPIO GPIOB      // port na kterém je CLK vstup
#define CLK_PIN GPIO_PIN_0  // pin na kterém je CLK vstup
#define DATA_GPIO GPIOB     // port na kterém je DIN vstup
#define DATA_PIN GPIO_PIN_2 // pin na kterém je DIN vstup
#define CS_GPIO GPIOB       // port na kterém je LOAD/CS vstup
#define CS_PIN GPIO_PIN_1   // pin na kterém je LOAD/CS vstup

// příkazová makra pro přehlednost kódu
#define CLK_HIGH GPIO_WriteHigh(CLK_GPIO, CLK_PIN)
#define CLK_LOW GPIO_WriteLow(CLK_GPIO, CLK_PIN)
#define DATA_HIGH GPIO_WriteHigh(DATA_GPIO, DATA_PIN)
#define DATA_LOW GPIO_WriteLow(DATA_GPIO, DATA_PIN)
#define CS_HIGH GPIO_WriteHigh(CS_GPIO, CS_PIN)
#define CS_LOW GPIO_WriteLow(CS_GPIO, CS_PIN)

uint32_t vlastni_cas = 0;

/* Nějaká super funkce co posílá data */
void max7219_send(uint8_t address, uint8_t data)
{
    uint8_t mask; 
    CS_LOW;       

    
    mask = 0b10000000; 
    CLK_LOW;          
    while (mask)
    { 
        if (mask & address)
        {              
            DATA_HIGH;
        }
        else
        {             
            DATA_LOW; 
        }
        CLK_HIGH;         
        mask = mask >> 1; 
        CLK_LOW;          
    }

   
    mask = 0b10000000;
    while (mask)
    {
        if (mask & data)
        {              
            DATA_HIGH; 
        }
        else
        {             
            DATA_LOW;
        }
        CLK_HIGH;         
        mask = mask >> 1;
        CLK_LOW;          
    }

    CS_HIGH; 
}
void TIM2_init(void)
{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,ENABLE);
    TIM2_TimeBaseInit(TIM2_PRESCALER_16,999);
    TIM2_ClearFlag(TIM2_FLAG_UPDATE);
    TIM2_ITConfig(TIM2_IT_UPDATE,ENABLE);
    TIM2_Cmd(ENABLE);
}

void max7219_init(void)           // inicializace displeje
{   
    GPIO_Init(CS_GPIO, CS_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(CLK_GPIO, CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(DATA_GPIO, DATA_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    // nastavíme základní parametry budiče
    max7219_send(DECODE_MODE, DECODE_ALL);        // Nějakej dekodér
    max7219_send(SCAN_LIMIT, 7);                  // Kolik cifer zapneme
    max7219_send(INTENSITY, 4);                   // Jas
    max7219_send(DISPLAY_TEST, DISPLAY_TEST_OFF); // test displeje
    max7219_send(SHUTDOWN, SHUTDOWN_ON);           // On/OFF

}

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovat MCU na 16MHz

    //init_milis();
    max7219_init();
    TIM2_init();
    enableInterrupts();
}

int main(void)
{

    setup();
    

    max7219_send(DIGIT0, 0);
    max7219_send(DIGIT1, 0);
    max7219_send(DIGIT2, 0);
    max7219_send(DIGIT3, 0);
    max7219_send(DIGIT4, 0);
    max7219_send(DIGIT5, 0);
    max7219_send(DIGIT6, 0);
    max7219_send(DIGIT7, 0);

    uint32_t time = 0;
    uint8_t number = 0;
    uint8_t desitky = 0;
    uint8_t stovky = 0;

    
    while (1)
    {
        if ((vlastni_cas - time) > 1000)
        {
            time = vlastni_cas;
            max7219_send(DIGIT0, number);
            max7219_send(DIGIT1, desitky);
            max7219_send(DIGIT2, stovky);
            max7219_send(DIGIT3, 0);
            max7219_send(DIGIT4, 0);
            max7219_send(DIGIT5, 0);
            max7219_send(DIGIT6, 0);
            max7219_send(DIGIT7, 0);
            number++;
            if (number>9){
                number=0;
                desitky++;
            }
            if (desitky > 9){
                stovky++;
                desitky = 0;
            }
            if (stovky > 9){
                number = 0;
                desitky = 0;
                stovky = 0; 
            }

        }
    
    
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"