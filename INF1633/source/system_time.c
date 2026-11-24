/*
 * Projet : INF1633
 * Module : system_time
 * Description : Implementation de la base de temps systeme 1 ms via SysTick
 *               et des utilitaires de temporisation non-bloquante sans derive
 *               pour le cycle nominal de 5 secondes.
 */

#include <stddef.h>

#include "system_time.h"
#include "MKL27Z644.h"

/* Variable globale de frequence horloge CPU fournie par system_MKL27Z644.c */
extern uint32_t SystemCoreClock;

/* Compteur de millisecondes incremente toutes les 1 ms par SysTick_Handler. */
static volatile uint32_t s_systemMilliseconds = 0U;
static bool s_timeInitialized = false;

/*
 * @brief Routine d'interruption materielle SysTick executee chaque milliseconde.
 */
void SysTick_Handler(void)
{
    s_systemMilliseconds++;
}

/*
 * @brief Initialise la base de temps systeme avec une interruption de 1 ms.
 */
bool SystemTime_Init(void)
{
    uint32_t ticksPerMs;

    if (SystemCoreClock < 1000U)
    {
        return false;
    }

    ticksPerMs = SystemCoreClock / 1000U;
    s_systemMilliseconds = 0U;

    /* SysTick_Config retourne 0 en cas de succes, 1 si reload > 24 bits. */
    if (SysTick_Config(ticksPerMs) != 0U)
    {
        return false;
    }

    s_timeInitialized = true;
    return true;
}

/*
 * @brief Retourne le nombre de millisecondes ecoulees depuis l'initialisation.
 */
uint32_t SystemTime_GetMilliseconds(void)
{
    return s_systemMilliseconds;
}

/*
 * @brief Retourne le nombre de secondes entieres ecoulees depuis l'initialisation.
 */
uint32_t SystemTime_GetSeconds(void)
{
    return (s_systemMilliseconds / 1000U);
}

/*
 * @brief Verifie si une duree s'est ecoulee depuis un instant donne.
 */
bool SystemTime_HasElapsed(uint32_t startMilliseconds,
                           uint32_t durationMilliseconds)
{
    uint32_t now = SystemTime_GetMilliseconds();
    return ((uint32_t)(now - startMilliseconds) >= durationMilliseconds);
}

/*
 * @brief Initialise un timer periodique sans derive (drift-free).
 */
void PeriodicTimer_Init(periodic_timer_t *timer,
                        uint32_t periodMs,
                        bool triggerImmediately)
{
    uint32_t now;

    if ((timer == NULL) || (periodMs == 0U))
    {
        return;
    }

    now = SystemTime_GetMilliseconds();
    timer->periodMs = periodMs;
    timer->nextTickMs = triggerImmediately ? now : (now + periodMs);
    timer->initialized = true;
}

/*
 * @brief Verifie si l'echeance du timer periodique est atteinte et planifie
 *        la suivante en compensant le temps d'execution des taches (drift-free).
 */
bool PeriodicTimer_IsDue(periodic_timer_t *timer)
{
    uint32_t now;

    if ((timer == NULL) || (!timer->initialized) || (timer->periodMs == 0U))
    {
        return false;
    }

    now = SystemTime_GetMilliseconds();

    /* Comparaison signee de la difference pour gerer le debordement 32 bits */
    if ((int32_t)(now - timer->nextTickMs) >= 0)
    {
        /* Avancer l'echeance par pas exacts pour eviter toute derive cumulative */
        timer->nextTickMs += timer->periodMs;

        /* Securite : si plus d'une periode complete a ete manquee, resynchroniser */
        if ((int32_t)(now - timer->nextTickMs) >= 0)
        {
            timer->nextTickMs = now + timer->periodMs;
        }

        return true;
    }

    return false;
}

/*
 * @brief Declenche une impulsion non-bloquante d'une duree donnee.
 */
void PulseTimer_Trigger(pulse_timer_t *pulse, uint32_t durationMs)
{
    if ((pulse == NULL) || (durationMs == 0U))
    {
        return;
    }

    pulse->startMs = SystemTime_GetMilliseconds();
    pulse->durationMs = durationMs;
    pulse->active = true;
}

/*
 * @brief Met a jour et retourne l'etat actif de l'impulsion non-bloquante.
 */
bool PulseTimer_IsActive(pulse_timer_t *pulse)
{
    if ((pulse == NULL) || (!pulse->active))
    {
        return false;
    }

    if (SystemTime_HasElapsed(pulse->startMs, pulse->durationMs))
    {
        pulse->active = false;
        return false;
    }

    return true;
}
