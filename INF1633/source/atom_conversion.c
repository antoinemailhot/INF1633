/*
 * Projet : INF1633
 * Module : atom_conversion
 * Description : Conversions génériques de valeurs ADC en tension.
 */

#include <math.h>
#include <stddef.h>

#include "atom_conversion.h"

/*
 * @brief Convertit une valeur ADC brute en tension électrique.
 *
 * @param rawValue Valeur brute du convertisseur.
 * @param referenceVoltage Tension de référence en volts.
 * @param resolutionBits Résolution ADC en bits.
 * @param voltage Pointeur recevant la tension calculée.
 *
 * @return true si la conversion est valide.
 *
 * Formule :
 * V = (rawValue / 2^resolutionBits) * referenceVoltage
 *
 * Ce module ne dépend d'aucun pilote de capteur ni du SDK NXP.
 */
bool ADC_To_Voltage(
    uint16_t rawValue,
    float referenceVoltage,
    uint8_t resolutionBits,
    float *voltage
)
{
    uint32_t numberOfLevels;
    uint32_t maximumRawValue;
    float normalizedValue;

    /* Vérifier le pointeur de sortie. */
    if (voltage == NULL)
    {
        return false;
    }

    /* Garantir une sortie déterministe en cas d'erreur. */
    *voltage = 0.0f;

    /* Vérifier que la référence est strictement positive et finie. */
    if (!isfinite(referenceVoltage) || (referenceVoltage <= 0.0f))
    {
        return false;
    }

    /*
     * Limiter la résolution à 16 bits.
     * La valeur brute est représentée par uint16_t.
     */
    if ((resolutionBits == 0U) || (resolutionBits > 16U))
    {
        return false;
    }

    /* Calculer le nombre de niveaux : 2^resolutionBits. */
    numberOfLevels = (uint32_t)1U << resolutionBits;

    /* Calculer la valeur numérique maximale : 2^n - 1. */
    maximumRawValue = numberOfLevels - 1U;

    /* Refuser une valeur brute hors de la plage ADC. */
    if ((uint32_t)rawValue > maximumRawValue)
    {
        return false;
    }

    /*
     * Normaliser la valeur brute avant la multiplication.
     * Cette opération limite les risques de dépassement flottant.
     */
    normalizedValue = (float)rawValue / (float)numberOfLevels;

    /* Calculer la tension électrique. */
    *voltage = normalizedValue * referenceVoltage;

    return true;
}