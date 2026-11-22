/*
 * Projet : INF1633
 * Auteurs : Antoine Mailhot, William Lamoureux, Micael Racine-Leblanc
 * Carte cible : FRDM-KL27Z
 * Microcontroleur : MKL27Z64VLH4
 *
 * Description :
 * Diagnostic des trois DEL et acquisition de la tension du MG-811.
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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
static void Init_Hardware(void);
static void Process_CO2(void);

/*
 * @brief Point d'entree du programme.
 *
 * Initialise le materiel, puis effectue periodiquement :
 * - une acquisition de la tension du MG-811 ;
 * - un clignotement des trois DEL.
 */
int main(void)
{
    /* Initialiser l'ensemble du materiel. */
    Init_Hardware();

    while (1)
    {
        /* Acquerir et afficher la tension du MG-811. */
        Process_CO2();

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
static void Init_Hardware(void)
{
    /* Initialiser les horloges du microcontroleur. */
    BOARD_InitBootClocks();

    /* Initialiser les broches configurees pour la carte. */
    BOARD_InitBootPins();

    /* Initialiser explicitement les broches des trois DEL. */
    BOARD_InitLEDsPins();

    /* Eteindre les trois DEL au demarrage. */
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();

    /* Initialiser les autres peripheriques configures. */
    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Initialiser la console de debogage. */
    BOARD_InitDebugConsole();
#endif

    /* Initialiser le convertisseur ADC du MG-811. */
    s_mg811Initialized = MG811_Init();

    if (s_mg811Initialized)
    {
        PRINTF("MG-811 : initialisation reussie.\r\n");
    }
    else
    {
        PRINTF("ERREUR : initialisation du MG-811 impossible.\r\n");
    }
}

/*
 * @brief Acquiert et affiche la tension du capteur MG-811.
 *
 * Get_Sensor_Voltage() effectue elle-meme la lecture ADC
 * et retourne la tension en volts.
 *
 * NAN indique une erreur de lecture ou de conversion.
 * Aucune conversion en ppm n'est effectuee ici.
 */
static void Process_CO2(void)
{
    float voltage;
    uint32_t voltageMillivolts;

    /* Verifier que le capteur a ete initialise correctement. */
    if (!s_mg811Initialized)
    {
        PRINTF("ERREUR : le capteur MG-811 n'est pas initialise.\r\n");
        return;
    }

    /* Lire la tension calculee a partir de la valeur ADC brute. */
    voltage = Get_Sensor_Voltage();

    /* Verifier si la mesure est valide et non negative. */
    if (!isfinite(voltage) || (voltage < 0.0f))
    {
        PRINTF("ERREUR : acquisition ou conversion ADC impossible.\r\n");
        return;
    }

    /* Convertir les volts en millivolts avec arrondi. */
    voltageMillivolts = (uint32_t)((voltage * 1000.0f) + 0.5f);

    /* Afficher la tension sans formatage flottant. */
    PRINTF("MG-811 - Tension : %lu mV\r\n",
           (unsigned long)voltageMillivolts);
}
