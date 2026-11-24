/*
 * Projet : INF1633
 * Auteurs : Antoine Mailhot, William Lamoureux, Micael Racine-Leblanc
 * Carte cible : FRDM-KL27Z
 * Microcontroleur : MKL27Z64VLH4
 *
 * Description :
 * Integration finale de la boucle applicative non-bloquante (cycle de 5 s) :
 * - Phase de prechauffage non-bloquante avec signalisation DEL et UART ;
 * - Sequence nominale stricte :
 *   Clignotement vert -> Lecture -> Calcul -> Affichage -> Verification Seuil -> Attente ;
 * - Allocation 100 % statique (aucune fuite memoire) et supervision sans blocage.
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

/* Etat d'initialisation et de prechauffage du systeme (memoire statique). */
static bool s_sensorInitialized = false;
static bool s_timeInitialized   = false;
static bool s_warmupCompleted   = false;

/* Timers non-bloquants statiques (zero allocation dynamique). */
static periodic_timer_t s_acquisitionTimer;
static periodic_timer_t s_warmupBlinkTimer;
static periodic_timer_t s_warmupLogTimer;
static pulse_timer_t    s_greenLedPulse;

/* Prototypes des fonctions applicatives. */
static void Init_Hardware(void);
static void Process_Warmup_Phase(void);
static void Process_CO2_Cycle(void);

/*
 * @brief Point d'entree principal du firmware.
 */
int main(void)
{
    Init_Hardware();

    /* Initialiser les timers non-bloquants de prechauffage et de cycle (5 s). */
    PeriodicTimer_Init(&s_warmupBlinkTimer, LED_WARMUP_BLINK_PERIOD_MS, true);
    PeriodicTimer_Init(&s_warmupLogTimer, SYSTEM_ACQUISITION_PERIOD_MS, true);
    PeriodicTimer_Init(&s_acquisitionTimer, SYSTEM_ACQUISITION_PERIOD_MS, true);

    while (1)
    {
        /* 1. Phase de preparation initiale (prechauffage sonde) non-bloquante */
        if (!s_warmupCompleted)
        {
            Process_Warmup_Phase();
            continue;
        }

        /* 2. Cycle nominal toutes les 5 secondes :
         *    Clignotement vert -> Lecture -> Calcul -> Affichage -> Verification Seuil -> Attente
         */
        if (PeriodicTimer_IsDue(&s_acquisitionTimer))
        {
            Process_CO2_Cycle();
        }

        /* 3. Attente non-bloquante : extinction automatique de l'impulsion DEL Verte */
        if (!PulseTimer_IsActive(&s_greenLedPulse))
        {
            Turn_Off_Green_LED();
        }
    }
}

/*
 * @brief Initialise les horloges, broches, DEL, console UART, base de temps et capteur.
 */
static void Init_Hardware(void)
{
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

    /* Initialiser le pilote GPIO des DEL Verte et Rouge (eteintes au depart). */
    LED_Driver_Init();

    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    PRINTF("\r\n=== INF1633 - Station de surveillance environnementale CO2 ===\r\n");

    /* Initialiser la base de temps systeme 1 ms (SysTick). */
    s_timeInitialized = SystemTime_Init();
    if (!s_timeInitialized)
    {
        PRINTF("ERREUR : initialisation de la base de temps impossible.\r\n");
    }

    /* Initialiser le convertisseur ADC du capteur de CO2. */
    s_sensorInitialized = MG811_Init();
    if (!s_sensorInitialized)
    {
        PRINTF("ERREUR : initialisation du capteur CO2 impossible.\r\n");
        Turn_On_Red_LED();
    }

    /* Si le temps de prechauffage est nul, passer directement en regime nominal. */
    if (CO2_DEFAULT_WARMUP_SEC == 0U)
    {
        s_warmupCompleted = true;
    }
}

/*
 * @brief Gere la periode de prechauffage de maniere non-bloquante avec
 *        clignotement rapide de la DEL Verte (250 ms) et messages UART.
 */
static void Process_Warmup_Phase(void)
{
    uint32_t elapsedSec = SystemTime_GetSeconds();
    uint32_t remainingSec;

    if (elapsedSec >= CO2_DEFAULT_WARMUP_SEC)
    {
        s_warmupCompleted = true;
        Turn_Off_Green_LED();
        PeriodicTimer_Init(&s_acquisitionTimer, SYSTEM_ACQUISITION_PERIOD_MS, true);
        PRINTF("Capteur pret : demarrage des mesures toutes les 5 secondes.\r\n");
        return;
    }

    if (PeriodicTimer_IsDue(&s_warmupBlinkTimer))
    {
        Toggle_Green_LED();
    }

    if (PeriodicTimer_IsDue(&s_warmupLogTimer))
    {
        remainingSec = CO2_DEFAULT_WARMUP_SEC - elapsedSec;
        PRINTF(MSG_WARMUP_UART_FMT, (unsigned int)remainingSec);
    }
}

/*
 * @brief Execute la sequence applicative complete a chaque cycle de 5 secondes :
 *        1. Clignotement vert
 *        2. Lecture
 *        3. Calcul
 *        4. Affichage
 *        5. Verification Seuil
 */
static void Process_CO2_Cycle(void)
{
    float voltage;
    float co2Ppm;
    co2_status_t status;
    char serialLine[CO2_FORMAT_BUFFER_MIN_SIZE];

    /* Etape 1 : Clignotement vert (impulsion non-bloquante de debut de cycle) */
    Turn_On_Green_LED();
    PulseTimer_Trigger(&s_greenLedPulse, LED_PULSE_DURATION_MS);

    if (!s_sensorInitialized)
    {
        PRINTF("ERREUR : capteur CO2 non initialise.\r\n");
        Turn_On_Red_LED();
        return;
    }

    /* Etape 2 : Lecture analogique (tension en volts) */
    voltage = Get_Sensor_Voltage();
    if (!isfinite(voltage) || (voltage < 0.0f))
    {
        PRINTF("ERREUR : lecture ADC invalide.\r\n");
        Turn_On_Red_LED();
        return;
    }

    /* Etape 3 : Calcul de la concentration en ppm */
    co2Ppm = Calculate_CO2_PPM(voltage);
    if (!isfinite(co2Ppm))
    {
        PRINTF("ERREUR : calcul de concentration CO2 impossible.\r\n");
        Turn_On_Red_LED();
        return;
    }

    /* Etape 4 : Affichage sur le terminal serie au format normalise */
    if (Format_CO2_Serial_Line(co2Ppm, serialLine, sizeof(serialLine)))
    {
        PRINTF("%s", serialLine);
    }

    /* Etape 5 : Verification du seuil d'alerte (1000 ppm avec hysteresis 950 ppm) */
    status = Evaluate_CO2_Level(co2Ppm);
    if (status == CO2_STATUS_ALERT)
    {
        Turn_On_Red_LED();
    }
    else if (status == CO2_STATUS_NORMAL)
    {
        Turn_Off_Red_LED();
    }
    else
    {
        Turn_On_Red_LED();
    }
}
