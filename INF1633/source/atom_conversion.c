/*
 * Projet : INF1633
 * Module : atom_conversion
 * Description : Conversions generiques de valeurs ADC en tension et calcul
 *               de concentration de CO2 en ppm (SEN0159 / MG-811 et MQ-135).
 */

#include <math.h>
#include <stddef.h>

#include "atom_conversion.h"

/* Calibration active en RAM (modifiable a chaud sans recompilation). */
static sen0159_calibration_t s_sen0159Cal = SEN0159_CALIBRATION_DEFAULT;
static mq135_calibration_t   s_mq135Cal   = MQ135_CALIBRATION_DEFAULT;

/*
 * @brief Convertit une valeur ADC brute en tension electrique.
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

    if (voltage == NULL)
    {
        return false;
    }

    *voltage = 0.0f;

    if (!isfinite(referenceVoltage) || (referenceVoltage <= 0.0f))
    {
        return false;
    }

    if ((resolutionBits == 0U) || (resolutionBits > 16U))
    {
        return false;
    }

    numberOfLevels = (uint32_t)1U << resolutionBits;
    maximumRawValue = numberOfLevels - 1U;

    if ((uint32_t)rawValue > maximumRawValue)
    {
        return false;
    }

    normalizedValue = (float)rawValue / (float)numberOfLevels;
    *voltage = normalizedValue * referenceVoltage;

    return true;
}

/*
 * @brief Met a jour a chaud la calibration active du capteur SEN0159.
 */
bool Set_SEN0159_Calibration(const sen0159_calibration_t *cal)
{
    if (cal == NULL)
    {
        return false;
    }

    if (!isfinite(cal->zeroPointVoltage) || (cal->zeroPointVoltage <= 0.0f) ||
        !isfinite(cal->reactionVoltage)  || (cal->reactionVoltage <= 0.0f)  ||
        !isfinite(cal->dcGain)           || (cal->dcGain <= 0.0f)           ||
        !isfinite(cal->voltageDividerRatio) || (cal->voltageDividerRatio <= 0.0f))
    {
        return false;
    }

    s_sen0159Cal = *cal;
    return true;
}

/*
 * @brief Met a jour a chaud la calibration active du capteur MQ-135.
 */
bool Set_MQ135_Calibration(const mq135_calibration_t *cal)
{
    if (cal == NULL)
    {
        return false;
    }

    if (!isfinite(cal->supplyVoltage)       || (cal->supplyVoltage <= 0.0f)       ||
        !isfinite(cal->loadResistanceKOhm)  || (cal->loadResistanceKOhm <= 0.0f)  ||
        !isfinite(cal->r0KOhm)              || (cal->r0KOhm <= 0.0f)              ||
        !isfinite(cal->cleanAirFactor)      || (cal->cleanAirFactor <= 0.0f)      ||
        !isfinite(cal->curveA)              || (cal->curveA <= 0.0f)              ||
        !isfinite(cal->curveB)              ||
        !isfinite(cal->voltageDividerRatio) || (cal->voltageDividerRatio <= 0.0f))
    {
        return false;
    }

    s_mq135Cal = *cal;
    return true;
}

/*
 * @brief Calcule la concentration de CO2 en ppm pour le SEN0159 (MG-811)
 *        selon la courbe logarithmique officielle (pow / log10).
 */
float Calculate_SEN0159_PPM_Calibrated(
    float voltage,
    const sen0159_calibration_t *cal
)
{
    const sen0159_calibration_t *activeCal = (cal != NULL) ? cal : &s_sen0159Cal;
    double sensorOutputVoltage;
    double probeVoltage;
    double slope;
    double logAmbient;
    double exponent;
    double ppm;

    if (!isfinite(voltage) || (voltage < 0.0f))
    {
        return NAN;
    }

    if ((activeCal->voltageDividerRatio <= 0.0f) ||
        (activeCal->dcGain <= 0.0f) ||
        (activeCal->reactionVoltage <= 0.0f))
    {
        return NAN;
    }

    /* Compenser le pont diviseur materiel puis le gain d'amplification DC. */
    sensorOutputVoltage = (double)voltage / (double)activeCal->voltageDividerRatio;
    probeVoltage = sensorOutputVoltage / (double)activeCal->dcGain;

    /*
     * Cas limite (critere issue #10) :
     * La tension du MG-811 diminue lorsque le CO2 augmente.
     * Si la tension mesuree depasse ou egale la tension a l'air propre
     * (400 ppm), on borne la sortie a la concentration ambiante de reference.
     */
    if (probeVoltage >= (double)activeCal->zeroPointVoltage)
    {
        return CO2_AMBIENT_REFERENCE_PPM;
    }

    /*
     * Pente logarithmique entre 400 ppm et 1000 ppm :
     * m = -REACTION_VOLTAGE / (log10(1000) - log10(400))
     */
    logAmbient = log10((double)CO2_AMBIENT_REFERENCE_PPM);
    slope = -(double)activeCal->reactionVoltage /
            (log10((double)CO2_ALERT_THRESHOLD_PPM) - logAmbient);

    exponent = ((probeVoltage - (double)activeCal->zeroPointVoltage) / slope) + logAmbient;
    ppm = pow(10.0, exponent);

    if (!isfinite(ppm))
    {
        return NAN;
    }

    if (ppm < (double)CO2_AMBIENT_REFERENCE_PPM)
    {
        ppm = (double)CO2_AMBIENT_REFERENCE_PPM;
    }
    else if (ppm > (double)CO2_MAX_VALID_PPM)
    {
        ppm = (double)CO2_MAX_VALID_PPM;
    }

    return (float)ppm;
}

/*
 * @brief Calcule la concentration de CO2 en ppm pour le MQ-135
 *        selon la regression en loi de puissance.
 */
float Calculate_MQ135_PPM_Calibrated(
    float voltage,
    const mq135_calibration_t *cal
)
{
    const mq135_calibration_t *activeCal = (cal != NULL) ? cal : &s_mq135Cal;
    double sensorOutputVoltage;
    double rsKOhm;
    double ratio;
    double ppm;

    if (!isfinite(voltage) || (voltage <= 0.0f))
    {
        return NAN;
    }

    if ((activeCal->voltageDividerRatio <= 0.0f) ||
        (activeCal->loadResistanceKOhm <= 0.0f) ||
        (activeCal->r0KOhm <= 0.0f))
    {
        return NAN;
    }

    sensorOutputVoltage = (double)voltage / (double)activeCal->voltageDividerRatio;

    if (sensorOutputVoltage >= (double)activeCal->supplyVoltage)
    {
        return CO2_MIN_VALID_PPM;
    }

    /* Rs = ((Vc - Vout) / Vout) * RL */
    rsKOhm = (((double)activeCal->supplyVoltage - sensorOutputVoltage) / sensorOutputVoltage) *
             (double)activeCal->loadResistanceKOhm;
    ratio = rsKOhm / (double)activeCal->r0KOhm;

    /* Cas limite air propre : si Rs/R0 >= facteur air pur, borner a 400 ppm. */
    if (ratio >= (double)activeCal->cleanAirFactor)
    {
        return CO2_AMBIENT_REFERENCE_PPM;
    }

    ppm = (double)activeCal->curveA * pow(ratio, (double)activeCal->curveB);

    if (!isfinite(ppm))
    {
        return NAN;
    }

    if (ppm < (double)CO2_AMBIENT_REFERENCE_PPM)
    {
        ppm = (double)CO2_AMBIENT_REFERENCE_PPM;
    }
    else if (ppm > (double)CO2_MAX_VALID_PPM)
    {
        ppm = (double)CO2_MAX_VALID_PPM;
    }

    return (float)ppm;
}

/*
 * @brief Point d'entree principal conforme aux exigences de l'issue #10.
 */
float Calculate_CO2_PPM(float voltage)
{
#if (CO2_SENSOR_SELECTION_MODE == CO2_SENSOR_MQ135)
    return Calculate_MQ135_PPM_Calibrated(voltage, NULL);
#elif (CO2_SENSOR_SELECTION_MODE == CO2_SENSOR_AUTO)
    /* Detection automatique analogique au repos si le mode AUTO est actif */
    if (isfinite(voltage) &&
        (voltage >= MQ135_DETECT_MIN_V) &&
        (voltage < MQ135_DETECT_MAX_V))
    {
        return Calculate_MQ135_PPM_Calibrated(voltage, NULL);
    }
    return Calculate_SEN0159_PPM_Calibrated(voltage, NULL);
#else
    return Calculate_SEN0159_PPM_Calibrated(voltage, NULL);
#endif
}
