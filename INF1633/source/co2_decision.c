/*
 * Projet : INF1633
 * Module : co2_decision
 * Description : Logique de decision et d'evaluation du niveau de CO2
 *               par rapport au seuil d'alerte (1000 ppm), avec support
 *               d'hysteresis et ajustement a chaud en RAM.
 */

#include <math.h>

#include "co2_decision.h"

/* Parametres actifs en RAM (modifiables a chaud via Set_CO2_Alert_Config). */
static float        s_alertThresholdPpm  = CO2_THRESHOLD_PPM;
static float        s_alertHysteresisPpm = CO2_ALERT_HYSTERESIS_PPM;
static co2_status_t s_currentStatus      = CO2_STATUS_NORMAL;

/*
 * @brief Evalue le niveau de CO2 et retourne son statut complet.
 */
co2_status_t Evaluate_CO2_Level(float ppm)
{
    float releaseThreshold;

    /* Verifier que la mesure est finie et dans la plage physique du capteur. */
    if (!isfinite(ppm) ||
        (ppm < CO2_MIN_VALID_PPM) ||
        (ppm > CO2_MAX_VALID_PPM))
    {
        s_currentStatus = CO2_STATUS_ERROR;
        return s_currentStatus;
    }

    /*
     * Si une alerte est deja active, appliquer la bande morte (hysteresis)
     * avant de repasser en etat NORMAL.
     */
    if ((s_currentStatus == CO2_STATUS_ALERT) && (s_alertHysteresisPpm > 0.0f))
    {
        releaseThreshold = s_alertThresholdPpm - s_alertHysteresisPpm;
        if (ppm >= releaseThreshold)
        {
            return CO2_STATUS_ALERT;
        }
    }

    if (ppm >= s_alertThresholdPpm)
    {
        s_currentStatus = CO2_STATUS_ALERT;
    }
    else
    {
        s_currentStatus = CO2_STATUS_NORMAL;
    }

    return s_currentStatus;
}

/*
 * @brief Verifie si la concentration de CO2 declenche une alerte.
 */
bool Is_CO2_Alert(float ppm)
{
    return (Evaluate_CO2_Level(ppm) == CO2_STATUS_ALERT);
}

/*
 * @brief Retourne l'etat courant memorise de l'evaluation CO2.
 */
co2_status_t Get_Current_CO2_Status(void)
{
    return s_currentStatus;
}

/*
 * @brief Met a jour a chaud le seuil d'alerte et son hysteresis en RAM.
 */
bool Set_CO2_Alert_Config(float thresholdPpm, float hysteresisPpm)
{
    if (!isfinite(thresholdPpm) ||
        (thresholdPpm <= CO2_MIN_VALID_PPM) ||
        (thresholdPpm > CO2_MAX_VALID_PPM))
    {
        return false;
    }

    if (!isfinite(hysteresisPpm) ||
        (hysteresisPpm < 0.0f) ||
        (hysteresisPpm >= thresholdPpm))
    {
        return false;
    }

    s_alertThresholdPpm = thresholdPpm;
    s_alertHysteresisPpm = hysteresisPpm;
    return true;
}

/*
 * @brief Retourne le seuil d'alerte actuellement actif en RAM (en ppm).
 */
float Get_CO2_Alert_Threshold(void)
{
    return s_alertThresholdPpm;
}
