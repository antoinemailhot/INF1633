/*
 * Projet : INF1633
 * Auteurs : Antoine Mailhot, William Lamoureux, Micael Racine-Leblanc
 * Carte cible : FRDM-KL27Z
 * Microcontroleur : MKL27Z64VLH4
 *
 * Description :
 * Orchestration applicative non-bloquante (cycle de 5 secondes) :
 * - Acquisition de la tension du capteur CO2 ;
 * - Conversion en concentration (ppm) et evaluation du seuil d'alerte ;
 * - Formatage et transmission sur la console serie UART toutes les 5 s ;
 * - Signalisation visuelle par DEL Verte (cycle normal) et DEL Rouge (alerte).
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

#include "config.h"
#include "system_time.h"
#include "led_driver.h"
#include "sensor_mg811.h"
#include "atom_conversion.h"
#include "co2_decision.h"
#include "co2_formatter.h"

/* Etat d'initialisation des sous-systemes. */
static bool s_mg811Initialized = false;
static bool s_timeInitialized  = false;

/* Timers non-bloquants pour le cycle de 5 s et l'impulsion de la DEL Verte. */
static periodic_timer_t s_acquisitionTimer;
static pulse_timer_t    s_greenLedPulse;

/* Prototypes des fonctions de l'application. */
static void Init_Hardware(void);
static void Process_CO2_Cycle(void);

/*
 * @brief Point d'entree du programme.
 */
int main(void)
{
    Init_Hardware();

    /* Planifier le cycle nominal d'acquisition et d'affichage toutes les 5 s. */
    PeriodicTimer_Init(&s_acquisitionTimer, SYSTEM_ACQUISITION_PERIOD_MS, true);

    while (1)
    {
        /* Executer l'acquisition et l'affichage uniquement chaque 5 secondes. */
        if (PeriodicTimer_IsDue(&s_acquisitionTimer))
        {
            Process_CO2_Cycle();
        }

        /* Orchestrer de maniere non-bloquante l'extinction de la DEL Verte. */
        if (!PulseTimer_IsActive(&s_greenLedPulse))
        {
            Turn_Off_Green_LED();
        }
    }
}

/*
 * @brief Initialise les peripheriques, horloges, GPIO, base de temps et capteurs.
 */
static void Init_Hardware(void)
{
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

    /* Initialiser le pilote GPIO des DEL Verte et Rouge (toutes deux eteintes). */
    LED_Driver_Init();

    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    /* Initialiser la base de temps systeme (1 ms via SysTick). */
    s_timeInitialized = SystemTime_Init();
    if (!s_timeInitialized)
    {
        PRINTF("ERREUR : initialisation de la base de temps impossible.\r\n");
    }

    /* Initialiser le capteur analogique de CO2. */
    s_mg811Initialized = MG811_Init();
    if (!s_mg811Initialized)
    {
        PRINTF("ERREUR : initialisation du capteur CO2 impossible.\r\n");
        Turn_On_Red_LED();
    }
}

/*
 * @brief Execute un cycle complet d'acquisition, conversion, decision et
 *        transmission serie toutes les 5 secondes.
 */
static void Process_CO2_Cycle(void)
{
    float voltage;
    float co2Ppm;
    co2_status_t status;
    char serialLine[CO2_FORMAT_BUFFER_MIN_SIZE];

    if (!s_mg811Initialized)
    {
        PRINTF("ERREUR : capteur CO2 non initialise.\r\n");
        Turn_On_Red_LED();
        return;
    }

    /* 1. Acquisition de la tension en volts. */
    voltage = Get_Sensor_Voltage();
    if (!isfinite(voltage) || (voltage < 0.0f))
    {
        PRINTF("ERREUR : lecture ADC invalide.\r\n");
        Turn_On_Red_LED();
        return;
    }

    /* 2. Conversion tension -> concentration en ppm. */
    co2Ppm = Calculate_CO2_PPM(voltage);

    /* 3. Evaluation de l'etat d'alerte avec hysteresis (1000 ppm / 950 ppm). */
    status = Evaluate_CO2_Level(co2Ppm);

    if (status == CO2_STATUS_ERROR)
    {
        PRINTF("ERREUR : concentration CO2 hors plage valide.\r\n");
        Turn_On_Red_LED();
        return;
    }

    /* 4. Formater et transmettre la chaine exacte sur le terminal serie. */
    if (Format_CO2_Serial_Line(co2Ppm, serialLine, sizeof(serialLine)))
    {
        PRINTF("%s", serialLine);
    }

    /* 5. Piloter la DEL Rouge (alerte) et pulser la DEL Verte (cycle normal). */
    if (status == CO2_STATUS_ALERT)
    {
        Turn_On_Red_LED();
    }
    else
    {
        Turn_Off_Red_LED();
    }

    Turn_On_Green_LED();
    PulseTimer_Trigger(&s_greenLedPulse, LED_PULSE_DURATION_MS);
}
