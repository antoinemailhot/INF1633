#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initialise la base de temps système avec une interruption de 1 ms.
 *
 * @return true si l'initialisation réussit, false autrement.
 */
bool SystemTime_Init(void);

/**
 * @brief Retourne le nombre de millisecondes écoulées depuis l'initialisation.
 *
 * @return Compteur de millisecondes sur 32 bits.
 */
uint32_t SystemTime_GetMilliseconds(void);

/**
 * @brief Vérifie si une durée s'est écoulée depuis un instant donné.
 *
 * La soustraction non signée permet de gérer le débordement du compteur.
 *
 * @param startMilliseconds Instant de départ.
 * @param durationMilliseconds Durée à vérifier en millisecondes.
 *
 * @return true si la durée est écoulée, false autrement.
 */
bool SystemTime_HasElapsed(uint32_t startMilliseconds,
                           uint32_t durationMilliseconds);

#endif /* SYSTEM_TIME_H */
