#include <main.h>
#include <stm8s.h>
#include <stdbool.h>
#include <stdio.h>



#define NOOP 0          // No operation
#define DIGIT0 1        // zápis hodnoty na 1. cifru
#define DIGIT1 2        // zápis hodnoty na 1. cifru
#define DIGIT2 3        // zápis hodnoty na 1. cifru
#define DIGIT3 4        // zápis hodnoty na 1. cifru
#define DIGIT4 5        // zápis hodnoty na 1. cifru
#define DIGIT5 6        // zápis hodnoty na 1. cifru
#define DIGIT6 7        // zápis hodnoty na 1. cifru
#define DIGIT7 8        // zápis hodnoty na 1. cifru
#define DECODE_MODE 9   // Aktivace/Deaktivace znakové sady (my volíme vždy hodnotu DECODE_ALL)
#define INTENSITY 10    // Nastavení jasu - argument je číslo 0 až 15 (větší číslo větší jas)
#define SCAN_LIMIT 11   // Volba počtu cifer (velikosti displeje) - argument je číslo 0 až 7 (my dáváme vždy 7)
#define SHUTDOWN 12     // Aktivace/Deaktivace displeje (ON / OFF)
#define DISPLAY_TEST 15 // Aktivace/Deaktivace "testu" (rozsvítí všechny segmenty)

// makra argumentů
// argumenty pro SHUTDOWN
#define DISPLAY_ON 1  // zapne displej
#define DISPLAY_OFF 0 // vypne displej
// argumenty pro DISPLAY_TEST
#define DISPLAY_TEST_ON 1  // zapne test displeje
#define DISPLAY_TEST_OFF 0 // vypne test displeje
// argumenty pro DECODE_MOD
#define DECODE_ALL 0b11111111 // (lepší zápis 0xff) zapíná znakovou sadu pro všechny cifry
#define DECODE_NONE 0         // vypíná znakovou sadu pro všechny cifry


#define CLK_GPIO GPIOB     // port na kterém je CLK vstup budiče
#define CLK_PIN GPIO_PIN_0  // pin na kterém je CLK vstup budiče
#define DATA_GPIO GPIOB     // port na kterém je DIN vstup budiče
#define DATA_PIN GPIO_PIN_2 // pin na kterém je DIN vstup budiče
#define CS_GPIO GPIOB       // port na kterém je LOAD/CS vstup budiče
#define CS_PIN GPIO_PIN_1   // pin na kterém je LOAD/CS vstup budiče

#define CLK_HIGH GPIO_WriteHigh(CLK_GPIO, CLK_PIN)
#define CLK_LOW GPIO_WriteLow(CLK_GPIO, CLK_PIN)
#define DATA_HIGH GPIO_WriteHigh(DATA_GPIO, DATA_PIN)
#define DATA_LOW GPIO_WriteLow(DATA_GPIO, DATA_PIN)
#define CS_HIGH GPIO_WriteHigh(CS_GPIO, CS_PIN)
#define CS_LOW GPIO_WriteLow(CS_GPIO, CS_PIN)

void max7219_send(uint8_t address, uint8_t data)
{
    uint8_t mask; // pomocná proměnná, která bude sloužit k procházení dat bit po bitu
    CS_LOW;       // nastavíme linku LOAD/CS do úrovně Low (abychom po zapsání všech 16ti bytů mohli vygenerovat na CS vzestupnou hranu)

    // nejprve odešleme prvních 8bitů zprávy (adresa/příkaz)
    mask = 0b10000000; // lepší zápis je: maska = 1<<7
    CLK_LOW;           // připravíme si na CLK vstup budiče úroveň Low
    while (mask)
    { // dokud jsme neposlali všech 8 bitů
        if (mask & address)
        {              // pokud má právě vysílaný bit hodnotu 1
            DATA_HIGH; // nastavíme budiči vstup DIN do úrovně High
        }
        else
        {             // jinak má právě vysílaný bit hodnotu 0 a...
            DATA_LOW; // ... nastavíme budiči vstup DIN do úrovně Low
        }
        CLK_HIGH;         // přejdeme na CLK z úrovně Low do úrovně High, a budič si zapíše hodnotu bitu, kterou jsme nastavili na DIN
        mask = mask >> 1; // rotujeme masku abychom v příštím kroku vysílali nižší bit
        CLK_LOW;          // vrátíme CLK zpět do Low abychom mohli celý proces vysílání bitu opakovat
    }

    // poté pošleme dolních 8 bitů zprávy (data/argument)
    mask = 0b10000000;
    while (mask)
    { // dokud jsme neposlali všech 8 bitů
        if (mask & data)
        {              // pokud má právě vysílaný bit hodnotu 1
            DATA_HIGH; // nastavíme budiči vstup DIN do úrovně High
        }
        else
        {             // jinak má právě vysílaný bit hodnotu 0 a...
            DATA_LOW; // ... nastavíme budiči vstup DIN do úrovně Low
        }
        CLK_HIGH;         // přejdeme na CLK z úrovně Low do úrovně High, a v budič si zapíše hodnotu bitu, kterou jsme nastavili na DIN
        mask = mask >> 1; // rotujeme masku abychom v příštím kroku vysílali nižší bit
        CLK_LOW;          // vrátíme CLK zpět do Low abychom mohli celý proces vysílání bitu opakovat
    }

    CS_HIGH; // nastavíme LOAD/CS z úrovně Low do úrovně High a vygenerujeme tím vzestupnou hranu (pokyn pro MAX7219 aby zpracoval náš příkaz)
}

void max7219_init(void)
{
    GPIO_Init(CS_GPIO, CS_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(CLK_GPIO, CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(DATA_GPIO, DATA_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    // nastavíme základní parametry budiče
    max7219_send(DECODE_MODE, DECODE_ALL);        // zapnout znakovou sadu na všech cifrách
    max7219_send(SCAN_LIMIT, 3);                  // velikost displeje 8 cifer (počítáno od nuly, proto je argument číslo 7)
    max7219_send(INTENSITY, 5);                   // volíme ze začátku nízký jas (vysoký jas může mít velkou spotřebu - až 0.25A !)
    max7219_send(DISPLAY_TEST, DISPLAY_TEST_OFF); // Funkci "test" nechceme mít zapnutou
    max7219_send(SHUTDOWN, DISPLAY_ON);           // zapneme displej
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

    uint32_t time = 0;
    uint32_t number = 0;

    uint32_t desitky = 0;
    uint32_t stovky = 0;
    uint32_t tisice = 0;
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
