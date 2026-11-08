/*
 * Projet : INF1633
 * Auteurs : Antoine Mailhot, William Lamoureux, Micaël Racine-Leblanc
 * Carte cible : FRDM-KL27Z
 * Microcontrôleur : MKL27Z64VLH4
 *
 * Description :
 * Point d'entrée principal de la station de surveillance environnementale.
 */

#include <stdint.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

/* Prototypes des fonctions de l'application. */
void Init_Hardware(void);
void Process_CO2(void);

/*
 * @brief Point d'entrée principal du programme.
 */
int main(void)
{
    /* Initialiser le matériel avant de commencer le traitement. */
    Init_Hardware();

    /* Afficher un message de confirmation sur la console UART. */
    PRINTF("\r\n");
    PRINTF("========================================\r\n");
    PRINTF(" Station de surveillance environnementale\r\n");
    PRINTF(" Carte : FRDM-KL27Z\r\n");
    PRINTF(" Console UART : 115200 bauds\r\n");
    PRINTF(" Systeme pret.\r\n");
    PRINTF("========================================\r\n");

    /* Boucle principale du programme. */
    while (1)
    {
        Process_CO2();
    }
}

/*
 * @brief Initialise les périphériques et le matériel de la carte.
 */
void Init_Hardware(void)
{
    /* Initialiser les broches de la carte. */
    BOARD_InitBootPins();

    /* Initialiser les horloges du microcontrôleur. */
    BOARD_InitBootClocks();

    /* Initialiser les périphériques configurés dans MCUXpresso. */
    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Initialiser la console de débogage. */
    BOARD_InitDebugConsole();
#endif
}

/*
 * @brief Effectue le traitement des données du capteur CO2.
 *
 * Cette fonction sera complétée lors de l'intégration du capteur.
 */
void Process_CO2(void)
{
    /* Traitement du capteur CO2 à implémenter. */
}
