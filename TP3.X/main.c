/**
 * Auteur 
 * Maxime Champagne
 * 3 mars 2022
 * 
 * Modifié par Sebastian Lencinas
 *
 *
 * SPI/main.c
 * 
*/
#include <stdio.h>
#include "mcc_generated_files/mcc.h"

#define MAX 60
uint8_t const sin[MAX] ={
              254,254,252,249,244,238,231,222,213,202,
              191,179,167,154,141,127,114,101,88,76,
              64,53,42,33,24,17,11,6,3,1,
              0,1,3,6,11,17,24,33,42,53,
              64,76,88,101,114,128,141,154,167,179,
              191,202,213,222,231,238,244,249,252,254};

uint8_t const car[MAX] ={
             0,0,0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,0,0,
			  255,255,255,255,255,255,255,255,255,255,
			  255,255,255,255,255,255,255,255,255,255,
			  255,255,255,255,255,255,255,255,255,255};

uint8_t const tri[MAX] ={
            9,17,26,34,43,51,60,68,77,85,
			 94,102,111,119,128,136,145,153,162,170,
			 179,187,196,204,213,221,230,238,247,255,
			 247,238,230,221,213,204,196,187,179,170,
			 162,153,145,136,128,119,111,102,94,86,
			 77,68,60,51,43,34,26,17,9,0};

void out_dig(uint8_t x);
void sinus_60(void);
void myTimer1_ISR(void);
void changement(void);

char TypeOndes;
char ChoixHz;

uint8_t Hz[5] = {20, 40, 60, 80, 100};
                       // 20Hz   40Hz   60Hz   80Hz   100Hz
uint16_t frequence[5] = {0xF05F,0xF448,0xF830,0xFC18,0xFE0C};
uint8_t ChoixF = 4;

/*
                         Main application
 */
void main(void)
{
    SYSTEM_Initialize();

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    TMR1_SetInterruptHandler(myTimer1_ISR);

    SSPCON1bits.SSPEN = 1;
    IO_RA5_SetHigh();

    while (1)
    {
        // Selection du type l'onde
        printf("Appuyer sur s, c ou t pour choisir le type d'ondes\n\r");

        do {
            TypeOndes = EUSART1_Read(); // Lecture EUSART

            switch (TypeOndes) {
                case 's':
                    printf("Ondes sinusoidales\n\r");
                    break;
                case 't':
                    printf("Ondes triangulaires\n\r");
                    break;
                case 'c':
                    printf("Ondes carrees\n\r");
                    break;
                default:
                    printf("Mauvais caractere choisi\n\r");
            }

        } while (!(TypeOndes == 's' || TypeOndes == 'c' || TypeOndes == 't'));  
        // Boucle pour attendre le bon caractere

        // Choix de frequence
        do {
            printf("+ ou - pour changer la fréquence\n\r");
            ChoixHz = EUSART1_Read(); // Lecture EUSART

            switch (ChoixHz) { 
                case '+':
                    if (ChoixF < 4) { //si on augmente la frequence
                        ChoixF++;
                        printf("%d Hz\n\r", Hz[ChoixF]); // Vitesse frequence Hz
                    } else {
                        printf("Maximum atteint (100 Hz)\n\r");
                    }
                    break;
                case '-':
                    if (ChoixF > 0) { //si on diminue la frequence
                        ChoixF--;
                        printf("%d Hz\n\r", Hz[ChoixF]); // Vitesse frequence Hz
                    } else {
                        printf("Minimum atteint (20 Hz)\n\r");
                    }
                    break;
                default:
                    printf("Impossible de changer la fréquence, mauvais caractère\n\r");
            }

        } while (!(ChoixHz == '-' || ChoixHz == '+'));  // Boucle si ce n'est pas + ou -

    }           
}

//---------------------------------------------------------------
// Routine d'interruption du Timer1
//---------------------------------------------------------------
void myTimer1_ISR(void){
    static uint8_t i;

    TMR1_WriteTimer(frequence[ChoixF]);

    switch (TypeOndes) {
        case 's':
            out_dig(sin[i]); // Ondes sinusoidale 
            break;
        case 't':
            out_dig(tri[i]); // Ondes triangulaire
            break;
        case 'c':
            out_dig(car[i]); // Ondes carrée
            break;
    }

    i++;
    if (i == MAX) {
        i = 0;
    }
}
    
//----------------------------------------------------------------
// Transmission au pot. d'une onde comprenant 60 points par cycle.
//----------------------------------------------------------------
void sinus_60(void) {
    uint8_t i;
    while(1) {
        for (i=0;i<MAX;i++) {
            out_dig(sin[i]);
            }
        } 
}


//----------------------------------------------------------------
//  Transmission d'une donnee a la sortie du pot. numerique
//----------------------------------------------------------------
void out_dig(uint8_t x) 
{
   IO_RA5_SetLow();   // selection du potentiometre
   SPI_ExchangeByte(0x11);  // ecriture, pot. 0
   SPI_ExchangeByte(x);
   IO_RA5_SetHigh();
   //__delay_ms(1);
}

