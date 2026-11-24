#ifndef ATOM_CONVERSION_H
#define ATOM_CONVERSION_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Convertit une valeur ADC brute en tension electrique.
 *
 * @param rawValue Valeur brute provenant du convertisseur ADC.
 * @param referenceVoltage Tension de reference du convertisseur en volts.
 * @param resolutionBits Resolution du convertisseur en bits.
 * @param voltage Pointeur recevant la tension calculee en volts.
 *
 * @return true si la conversion est valide, false autrement.
 */
bool ADC_To_Voltage(
    uint16_t rawValue,
    float referenceVoltage,
    uint8_t resolutionBits,
    float *voltage
);

/*
 * @brief Calcule la concentration de CO2 en ppm a partir de la tension mesuree
 *        (utilise la configuration et calibration actives du capteur principal).
 *
 * @param voltage Tension mesuree a l'entree ADC en volts.
 *
 * @return Concentration en ppm (bornee entre 400 ppm et CO2_MAX_VALID_PPM),
 *         ou NAN si la tension d'entree est invalide.
 */
float Calculate_CO2_PPM(float voltage);

/*
 * @brief Calcule la concentration de CO2 en ppm pour le capteur SEN0159 (MG-811)
 *        en appliquant la courbe logarithmique (pow / log10) avec une
 *        structure de calibration explicite.
 *
 * @param voltage Tension mesuree a l'entree ADC en volts.
 * @param cal Pointeur vers les parametres d'etalonnage (NULL = defaut).
 *
 * @return Concentration en ppm, ou NAN si les parametres sont invalides.
 */
float Calculate_SEN0159_PPM_Calibrated(
    float voltage,
    const sen0159_calibration_t *cal
);

/*
 * @brief Calcule la concentration de CO2 en ppm pour le capteur MQ-135
 *        en appliquant la loi de puissance avec calibration explicite.
 *
 * @param voltage Tension mesuree a l'entree ADC en volts.
 * @param cal Pointeur vers les parametres d'etalonnage (NULL = defaut).
 *
 * @return Concentration en ppm, ou NAN si les parametres sont invalides.
 */
float Calculate_MQ135_PPM_Calibrated(
    float voltage,
    const mq135_calibration_t *cal
);

/*
 * @brief Met a jour a chaud la calibration active du capteur SEN0159.
 *
 * @param cal Nouvelle structure de calibration a appliquer en RAM.
 * @return true si les parametres sont valides et appliques, false sinon.
 */
bool Set_SEN0159_Calibration(const sen0159_calibration_t *cal);

/*
 * @brief Met a jour a chaud la calibration active du capteur MQ-135.
 *
 * @param cal Nouvelle structure de calibration a appliquer en RAM.
 * @return true si les parametres sont valides et appliques, false sinon.
 */
bool Set_MQ135_Calibration(const mq135_calibration_t *cal);

#ifdef __cplusplus
}
#endif

#endif /* ATOM_CONVERSION_H */
