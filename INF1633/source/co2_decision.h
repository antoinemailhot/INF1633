/*
 * Projet : INF1633
 * Fichier : co2_decision.h
 * Description : Logique de decision et d'evaluation du niveau de CO2
 *               avec hysteresis (activation >= 1000 ppm, desactivation < 950 ppm),
 *               decouplee des pilotes materiels (DEL, LCD, UART).
 */

#ifndef CO2_DECISION_H
#define CO2_DECISION_H

#include <stdbool.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Etats possibles issus de l'evaluation du niveau de CO2.
 */
typedef enum
{
    CO2_STATUS_NORMAL = 0,  /* Concentration valide sous le seuil */
    CO2_STATUS_ALERT  = 1,  /* Concentration en alerte (>= 1000 ppm ou >= 950 ppm si deja active) */
    CO2_STATUS_ERROR  = 2   /* Mesure invalide (NAN, infinie ou hors plage) */
} co2_status_t;

/*
 * @brief Evalue le niveau de CO2 avec hysteresis et retourne son statut complet.
 *
 * @param ppm Concentration mesuree en parties par million (ppm).
 *
 * @return CO2_STATUS_ALERT  si ppm >= 1000 ppm (ou >= 950 ppm si deja en alerte),
 *         CO2_STATUS_NORMAL si ppm < 950 ppm (ou < 1000 ppm hors alerte),
 *         CO2_STATUS_ERROR  si ppm est NAN, infini ou hors [MIN, MAX].
 */
co2_status_t Evaluate_CO2_Level(float ppm);

/*
 * @brief Verifie si la concentration de CO2 declenche ou maintient une alerte.
 *
 * @param ppm Concentration mesuree en parties par million (ppm).
 *
 * @return true si le statut evalue est CO2_STATUS_ALERT, false autrement.
 */
bool Is_CO2_Alert(float ppm);

/*
 * @brief Retourne l'etat courant memorise de l'evaluation CO2.
 *
 * @return Dernier statut calcule par Evaluate_CO2_Level().
 */
co2_status_t Get_Current_CO2_Status(void);

/*
 * @brief Reinitialise l'etat interne de decision a CO2_STATUS_NORMAL.
 */
void Reset_CO2_Decision_State(void);

/*
 * @brief Met a jour a chaud le seuil d'alerte et son hysteresis en RAM.
 *
 * @param thresholdPpm Nouveau seuil d'activation en ppm (ex: 1000.0f).
 * @param hysteresisPpm Bande morte en ppm (ex: 50.0f pour relachement < 950.0f).
 *
 * @return true si les parametres sont valides et appliques, false sinon.
 */
bool Set_CO2_Alert_Config(float thresholdPpm, float hysteresisPpm);

/*
 * @brief Retourne le seuil d'activation d'alerte actif en RAM (en ppm).
 */
float Get_CO2_Alert_Threshold(void);

/*
 * @brief Retourne le seuil de desactivation d'alerte actif en RAM (en ppm).
 */
float Get_CO2_Release_Threshold(void);

#ifdef __cplusplus
}
#endif

#endif /* CO2_DECISION_H */
