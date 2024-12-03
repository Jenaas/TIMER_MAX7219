#include <main.h>
#include <stm8s.h>
#include <stdbool.h>
#include <stdio.h>
#include "max7219.h"

/* Makra na Porty a Piny*/
#define CLK_GPIO GPIOB     
#define CLK_PIN GPIO_PIN_0  
#define DATA_GPIO GPIOB     
#define DATA_PIN GPIO_PIN_2 
#define CS_GPIO GPIOB       
#define CS_PIN GPIO_PIN_1   

/* Makra na práci s Piny */
#define CLK_HIGH GPIO_WriteHigh(CLK_GPIO, CLK_PIN)
#define CLK_LOW GPIO_WriteLow(CLK_GPIO, CLK_PIN)
#define DATA_HIGH GPIO_WriteHigh(DATA_GPIO, DATA_PIN)
#define DATA_LOW GPIO_WriteLow(DATA_GPIO, DATA_PIN)
#define CS_HIGH GPIO_WriteHigh(CS_GPIO, CS_PIN)
#define CS_LOW GPIO_WriteLow(CS_GPIO, CS_PIN)

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

void max7219_init(void)
{   /* Nastavení pinu do začáteční pozice */
    GPIO_Init(CS_GPIO, CS_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(CLK_GPIO, CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(DATA_GPIO, DATA_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    // nastavíme základní parametry budiče
    max7219_send(DECODE_MODE, DECODE_ALL);        // Nějakej dekodér
    max7219_send(SCAN_LIMIT, 3);                  // Kolik cifer zapneme
    max7219_send(INTENSITY, 5);                   // Jas
    max7219_send(DISPLAY_TEST, DISPLAY_TEST_OFF); // test displeje
    max7219_send(SHUTDOWN, SHUTDOWN_ON);           // On/OFF

}

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovat MCU na 16MHz

    init_milis();
    

    max7219_init();
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

    uint8_t time = 0;
    uint8_t number = 0;
    uint8_t desitky = 0;
    uint8_t stovky = 0;
    uint8_t tisice = 0;

    /* Funkce pro zobrazení čísel */
    while (1)
    {
        if ((milis() - time) > 200)
        {
            time = milis();
            max7219_send(DIGIT0, number);
            max7219_send(DIGIT1, desitky);
            max7219_send(DIGIT2, stovky);
            max7219_send(DIGIT3, tisice);
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
                tisice++;
                stovky =0;
            }
            if (tisice > 9){
                number = 0;
                desitky = 0;
                stovky = 0;
                tisice = 0;
            }

        }
    
    
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"