/*
 * Projet : INF1633
 * Fichier : system_time.h
 * Description : Base de temps systeme 1 ms (SysTick Cortex-M0+) et gestionnaire
 *               de temporisation non-bloquante sans derive pour le cycle
 *               d'acquisition de 5 secondes et le clignotement des DEL.
 */

#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Duree d'impulsion visuelle par defaut pour le clignotement DEL (en ms). */
#ifndef LED_PULSE_DURATION_MS
#define LED_PULSE_DURATION_MS           (150U)
#endif

/*
 * Timer periodique non-bloquant sans derive (drift-free).
 * Garantit une cadence exacte (ex. 5000 ms) independamment du temps
 * d'execution des capteurs, de l'ecran LCD ou du clignotement des DEL.
 */
typedef struct
{
    uint32_t periodMs;      /* Periode cible en millisecondes (ex. 5000 ms) */
    uint32_t nextTickMs;    /* Prochaine echeance planifiee */
    bool     initialized;   /* Indique si le timer a ete demarre */
} periodic_timer_t;

/*
 * Timer d'impulsion non-bloquant (one-shot) pour orchestrer le clignotement
 * d'une DEL sans bloquer l'execution du programme principal.
 */
typedef struct
{
    uint32_t startMs;       /* Instant de declenchement de l'impulsion */
    uint32_t durationMs;    /* Duree de maintien a l'etat actif (ms) */
    bool     active;        /* true tant que l'impulsion est en cours */
} pulse_timer_t;

/*
 * @brief Initialise la base de temps systeme avec une interruption SysTick de 1 ms.
 *
 * @return true si l'initialisation reussit, false autrement.
 */
bool SystemTime_Init(void);

/*
 * @brief Retourne le nombre de millisecondes ecoulees depuis l'initialisation.
 *
 * @return Compteur de millisecondes sur 32 bits.
 */
uint32_t SystemTime_GetMilliseconds(void);

/*
 * @brief Retourne le nombre de secondes entieres ecoulees depuis l'initialisation.
 *
 * @return Compteur de secondes sur 32 bits.
 */
uint32_t SystemTime_GetSeconds(void);

/*
 * @brief Verifie si une duree s'est ecoulee depuis un instant donne.
 *        La soustraction non signee gere automatiquement le debordement 32 bits.
 *
 * @param startMilliseconds Instant de depart.
 * @param durationMilliseconds Duree a verifier en millisecondes.
 *
 * @return true si la duree est ecoulee, false autrement.
 */
bool SystemTime_HasElapsed(uint32_t startMilliseconds,
                           uint32_t durationMilliseconds);

/*
 * @brief Initialise un timer periodique sans derive (drift-free).
 *
 * @param timer Pointeur vers la structure periodic_timer_t.
 * @param periodMs Periode souhaitee en millisecondes (ex. SYSTEM_ACQUISITION_PERIOD_MS).
 * @param triggerImmediately true pour declencher des le premier appel a PeriodicTimer_IsDue().
 */
void PeriodicTimer_Init(periodic_timer_t *timer,
                        uint32_t periodMs,
                        bool triggerImmediately);

/*
 * @brief Verifie si l'echeance du timer periodique est atteinte et planifie
 *        la suivante sans accumuler le retard d'execution (compensation de derive).
 *
 * @param timer Pointeur vers la structure periodic_timer_t.
 * @return true lorsque la periode est ecoulee, false sinon.
 */
bool PeriodicTimer_IsDue(periodic_timer_t *timer);

/*
 * @brief Declenche une impulsion non-bloquante d'une duree donnee.
 *
 * @param pulse Pointeur vers la structure pulse_timer_t.
 * @param durationMs Duree de l'impulsion en millisecondes (ex. LED_PULSE_DURATION_MS).
 */
void PulseTimer_Trigger(pulse_timer_t *pulse, uint32_t durationMs);

/*
 * @brief Met a jour l'etat de l'impulsion non-bloquante.
 *
 * @param pulse Pointeur vers la structure pulse_timer_t.
 * @return true tant que l'impulsion est active, false une fois terminee.
 */
bool PulseTimer_IsActive(pulse_timer_t *pulse);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_TIME_H */
