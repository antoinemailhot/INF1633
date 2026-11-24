/*
 * Projet : INF1633
 * Fichier : co2_decision.h
 * Description : Logique de decision et d'evaluation du niveau de CO2
 *               par rapport au seuil d'alerte (1000 ppm), decouplee
 *               des pilotes materiels (DEL, LCD, UART).
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
    CO2_STATUS_NORMAL = 0,  /* Concentration valide sous le seuil d'alerte */
    CO2_STATUS_ALERT  = 1,  /* Concentration atteignant ou depassant le seuil */
    CO2_STATUS_ERROR  = 2   /* Mesure invalide (NAN, infinie ou hors plage) */
} co2_status_t;

/*
 * @brief Evalue le niveau de CO2 et retourne son statut complet.
 *
 * @param ppm Concentration mesuree en parties par million (ppm).
 *
 * @return CO2_STATUS_ALERT  si ppm >= seuil actif (1000 ppm par defaut),
 *         CO2_STATUS_NORMAL si ppm < seuil actif (avec prise en compte de
 *                           l'hysteresis eventuel),
 *         CO2_STATUS_ERROR  si ppm est NAN, infini ou hors [MIN, MAX].
 */
co2_status_t Evaluate_CO2_Level(float ppm);

/*
 * @brief Verifie si la concentration de CO2 declenche une alerte.
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
 * @brief Met a jour a chaud le seuil d'alerte et son hysteresis en RAM.
 *
 * @param thresholdPpm Nouveau seuil d'alerte en ppm (doit etre dans la plage valide).
 * @param hysteresisPpm Bande morte en ppm (>= 0.0f et < thresholdPpm).
 *
 * @return true si les parametres sont valides et appliques, false sinon.
 */
bool Set_CO2_Alert_Config(float thresholdPpm, float hysteresisPpm);

/*
 * @brief Retourne le seuil d'alerte actuellement actif en RAM (en ppm).
 *
 * @return Seuil d'alerte actif en ppm.
 */
float Get_CO2_Alert_Threshold(void);

#ifdef __cplusplus
}
#endif

#endif /* CO2_DECISION_H */
