
/*
 * Projet : INF1633
 * Auteurs : Antoine Mailhot, William Lamoureux, Micael Racine-Leblanc
 * Carte cible : FRDM-KL27Z
 * Microcontroleur : MKL27Z64VLH4
 *
 * Description :
 * Diagnostic independant de la DEL rouge et de la transmission UART.
 */

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"

#include "sensor_mg811.h"

/* Etat de l'initialisation du capteur MG-811. */
static bool s_mg811Initialized = false;

/* Prototypes des fonctions de l'application. */
void Init_Hardware(void);
void Process_CO2(void);

/*
 * @brief Test des trois DEL de la carte FRDM-KL27Z.
 */
int main(void)
{
    /* Initialiser les horloges du microcontroleur. */
    BOARD_InitBootClocks();

    /* Initialiser les broches de la carte. */
    BOARD_InitBootPins();

    /* Configurer les broches des trois DEL. */
    BOARD_InitLEDsPins();

    /* Eteindre initialement les trois DEL. */
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();

    while (1)
    {
        /* Allumer simultanement les trois DEL. */
        LED_RED_ON();
        LED_GREEN_ON();
        LED_BLUE_ON();

        /* Maintenir les DEL allumees pendant 500 ms. */
        SDK_DelayAtLeastUs(500000U, SystemCoreClock);

        /* Eteindre simultanement les trois DEL. */
        LED_RED_OFF();
        LED_GREEN_OFF();
        LED_BLUE_OFF();

        /* Maintenir les DEL eteintes pendant 500 ms. */
        SDK_DelayAtLeastUs(500000U, SystemCoreClock);
    }
}

/*
 * @brief Initialise les peripheriques et le materiel de la carte.
 */
void Init_Hardware(void)
{
    /* Initialiser les horloges du microcontroleur. */
    BOARD_InitBootClocks();

    /* Initialiser les broches configurees pour la carte. */
    BOARD_InitBootPins();

    /* Initialiser explicitement les broches des trois DEL. */
    BOARD_InitLEDsPins();

    /* Eteindre la DEL rouge au demarrage. */
    LED_RED_OFF();

    /* Initialiser les autres peripheriques configures. */
    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Initialiser la console de debogage. */
    BOARD_InitDebugConsole();
#endif

    /* Initialiser le convertisseur ADC du MG-811. */
    s_mg811Initialized = MG811_Init();
}

/*
 * @brief Effectue une acquisition brute du capteur MG-811.
 *
 * La valeur retournee correspond a la conversion ADC sur 12 bits.
 * Aucune conversion en ppm n'est effectuee ici.
 */
void Process_CO2(void)
{
    uint16_t rawValue = 0U;

    /* Verifier que le capteur a ete initialise correctement. */
    if (!s_mg811Initialized)
    {
        PRINTF("ERREUR : le capteur MG-811 n'est pas initialise.\r\n");
        return;
    }

    /* Declencher une conversion ADC et recuperer sa valeur brute. */
    if (Read_Sensor_Raw(&rawValue))
    {
        PRINTF("MG-811 - Valeur ADC brute : %u / 4095\r\n",
               (unsigned int)rawValue);
    }
    else
    {
        PRINTF("ERREUR : echec de la conversion ADC du MG-811.\r\n");
    }
}
