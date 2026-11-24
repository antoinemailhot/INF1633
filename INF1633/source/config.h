/*
 * Projet : INF1633
 * Fichier : config.h
 * Description : Constantes logicielles d'etalonnage, detection plug-and-play,
 *               parametres de prechauffage (DEL/LCD) et structures de
 *               configuration modulaires pour SEN0159 (MG-811) et MQ-135.
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 1. Detection Plug-and-Play et selection du capteur de CO2 principal
 * ============================================================================ */

#define CO2_SENSOR_NONE                 (0U)
#define CO2_SENSOR_AUTO                 (1U)
#define CO2_SENSOR_SEN0159              (2U)
#define CO2_SENSOR_MQ135                (3U)

/*
 * Mode de selection actif :
 * - CO2_SENSOR_AUTO    : detection automatique au demarrage (plug-and-play)
 * - CO2_SENSOR_SEN0159 : forcer le module DFRobot SEN0159 (MG-811)
 * - CO2_SENSOR_MQ135   : forcer le module Flying-Fish MQ-135
 */
#ifndef CO2_SENSOR_SELECTION_MODE
#define CO2_SENSOR_SELECTION_MODE       CO2_SENSOR_AUTO
#endif

/*
 * Plages de tension caracteristiques au demarrage (en volts a l'entree ADC)
 * pour l'identification automatique analogique (plug-and-play) :
 * - En dessous de CO2_DETECT_MIN_VALID_V : entree deconnectee / court-circuit
 * - Entre MQ135_DETECT_MIN_V et MQ135_DETECT_MAX_V : signature typique MQ-135
 * - Au-dessus de SEN0159_DETECT_MIN_V : signature typique SEN0159 (air pur ~1.87V)
 */
#ifndef CO2_DETECT_MIN_VALID_V
#define CO2_DETECT_MIN_VALID_V          (0.15f)
#endif

#ifndef MQ135_DETECT_MIN_V
#define MQ135_DETECT_MIN_V              (0.15f)
#endif

#ifndef MQ135_DETECT_MAX_V
#define MQ135_DETECT_MAX_V              (1.35f)
#endif

#ifndef SEN0159_DETECT_MIN_V
#define SEN0159_DETECT_MIN_V            (1.35f)
#endif

#ifndef SEN0159_DETECT_MAX_V
#define SEN0159_DETECT_MAX_V            (3.25f)
#endif

/* ============================================================================
 * 2. Seuils applicatifs et plage physique valide
 * ============================================================================ */

/* Seuil d'alerte de concentration en CO2 (en ppm) pour la DEL rouge. */
#ifndef CO2_ALERT_THRESHOLD_PPM
#define CO2_ALERT_THRESHOLD_PPM         (1000.0f)
#endif

/* Concentration de reference de l'air ambiant sain (en ppm). */
#ifndef CO2_AMBIENT_REFERENCE_PPM
#define CO2_AMBIENT_REFERENCE_PPM       (400.0f)
#endif

/* Plage de mesure physique valide du capteur de CO2 (en ppm). */
#ifndef CO2_MIN_VALID_PPM
#define CO2_MIN_VALID_PPM               (0.0f)
#endif

#ifndef CO2_MAX_VALID_PPM
#define CO2_MAX_VALID_PPM               (10000.0f)
#endif

/* ============================================================================
 * 3. Configuration du prechauffage (Warm-up) et signalisation DEL / LCD
 * ============================================================================ */

/* Duree de prechauffage par defaut au demarrage (en secondes). */
#ifndef CO2_DEFAULT_WARMUP_SEC
#define CO2_DEFAULT_WARMUP_SEC          (60U)
#endif

/* Periode de clignotement rapide de la DEL pendant le prechauffage (ms). */
#ifndef LED_WARMUP_BLINK_PERIOD_MS
#define LED_WARMUP_BLINK_PERIOD_MS      (250U)
#endif

/* Periode nominale d'acquisition et de clignotement DEL en regime normal (ms). */
#ifndef SYSTEM_ACQUISITION_PERIOD_MS
#define SYSTEM_ACQUISITION_PERIOD_MS    (5000U)
#endif

/* Messages standardises pour la console UART et l'ecran LCD */
#define MSG_WARMUP_UART_FMT             "Materiel en preparation, merci de patienter (%u s restantes)...\r\n"
#define MSG_WARMUP_LCD_LINE1            "Preparation..."
#define MSG_WARMUP_LCD_LINE2_FMT        "Patientez %3u s"

/* ============================================================================
 * 4. Constantes d'etalonnage : Capteur SEN0159 (sonde MG-811)
 * Source : Fiche technique DFRobot Gravity Analog CO2 Gas Sensor (SEN0159)
 * ============================================================================ */

/*
 * Tension de sortie de la sonde MG-811 au point zero (400 ppm de CO2),
 * en volts avant amplification (0.220 V = 220 mV).
 */
#ifndef SEN0159_ZERO_POINT_VOLTAGE
#define SEN0159_ZERO_POINT_VOLTAGE      (0.220f)
#endif

/*
 * Tension de reaction (chute de tension de la sonde MG-811 entre 400 ppm
 * et 1000 ppm de CO2), en volts avant amplification (0.020 V = 20 mV).
 */
#ifndef SEN0159_REACTION_VOLTAGE
#define SEN0159_REACTION_VOLTAGE        (0.020f)
#endif

/*
 * Rapport d'amplification (DC Gain) de l'ampli-op embarque sur le module SEN0159.
 */
#ifndef SEN0159_DC_GAIN
#define SEN0159_DC_GAIN                 (8.5f)
#endif

/*
 * Facteur d'attenuation du pont diviseur de tension entre la sortie 5 V
 * du module et l'entree ADC 3.3 V de la FRDM-KL27Z : R2 / (R1 + R2).
 */
#ifndef SEN0159_VOLTAGE_DIVIDER_RATIO
#define SEN0159_VOLTAGE_DIVIDER_RATIO   (1.0f)
#endif

/* Temps de prechauffage specifique au module SEN0159 (en secondes). */
#ifndef SEN0159_WARMUP_TIME_SEC
#define SEN0159_WARMUP_TIME_SEC         CO2_DEFAULT_WARMUP_SEC
#endif

/* Alias compatibles avec les specifications de l'issue #9 */
#ifndef ZERO_POINT_VOLTAGE
#define ZERO_POINT_VOLTAGE              SEN0159_ZERO_POINT_VOLTAGE
#endif

#ifndef REACTION_VOLTAGE
#define REACTION_VOLTAGE                SEN0159_REACTION_VOLTAGE
#endif

#ifndef DC_GAIN
#define DC_GAIN                         SEN0159_DC_GAIN
#endif

#ifndef GAIN
#define GAIN                            SEN0159_DC_GAIN
#endif

/* Modele de tension amplifiee au point zero (400 ppm) en volts */
#define ZERO_POINT_AMPLIFIED_V          (ZERO_POINT_VOLTAGE * DC_GAIN)

/* ============================================================================
 * 5. Constantes d'etalonnage : Capteur MQ-135 (module Flying-Fish)
 * ============================================================================ */

/* Tension d'alimentation du pont capteur MQ-135 (en volts). */
#ifndef MQ135_SUPPLY_VOLTAGE
#define MQ135_SUPPLY_VOLTAGE            (5.0f)
#endif

/* Resistance de charge RL sur le module MQ-135 (en kOhms, typique : 1.0 a 10.0). */
#ifndef MQ135_RL_KOHM
#define MQ135_RL_KOHM                   (1.0f)
#endif

/* Resistance de base R0 en air pur calibree (en kOhms). */
#ifndef MQ135_R0_DEFAULT_KOHM
#define MQ135_R0_DEFAULT_KOHM           (10.0f)
#endif

/* Rapport Rs / R0 en air pur (400 ppm CO2) selon la courbe caracteristique. */
#ifndef MQ135_CLEAN_AIR_FACTOR
#define MQ135_CLEAN_AIR_FACTOR          (3.6f)
#endif

/*
 * Coefficients de la courbe de regression puissance pour le CO2 :
 * ppm = MQ135_CURVE_A * powf(Rs / R0, MQ135_CURVE_B)
 */
#ifndef MQ135_CURVE_A
#define MQ135_CURVE_A                   (116.6020682f)
#endif

#ifndef MQ135_CURVE_B
#define MQ135_CURVE_B                   (-2.769034857f)
#endif

/* Facteur d'attenuation du pont diviseur vers l'entree ADC 3.3 V : R2 / (R1 + R2). */
#ifndef MQ135_VOLTAGE_DIVIDER_RATIO
#define MQ135_VOLTAGE_DIVIDER_RATIO     (1.0f)
#endif

/* Temps de prechauffage specifique au module MQ-135 (en secondes). */
#ifndef MQ135_WARMUP_TIME_SEC
#define MQ135_WARMUP_TIME_SEC           CO2_DEFAULT_WARMUP_SEC
#endif

/* ============================================================================
 * 6. Structures de calibration dynamique (recalibration a chaud)
 * ============================================================================ */

/* Parametres modifiables a chaud pour le capteur SEN0159 / MG-811 */
typedef struct
{
    float zeroPointVoltage;     /* Tension sonde a 400 ppm (V) */
    float reactionVoltage;      /* Chute de tension 400 -> 1000 ppm (V) */
    float dcGain;               /* Gain d'amplification DC */
    float voltageDividerRatio;  /* Ratio du pont diviseur materiel */
} sen0159_calibration_t;

/* Initialiseur par defaut pour SEN0159 */
#define SEN0159_CALIBRATION_DEFAULT \
    { \
        .zeroPointVoltage = SEN0159_ZERO_POINT_VOLTAGE, \
        .reactionVoltage = SEN0159_REACTION_VOLTAGE, \
        .dcGain = SEN0159_DC_GAIN, \
        .voltageDividerRatio = SEN0159_VOLTAGE_DIVIDER_RATIO \
    }

/* Parametres modifiables a chaud pour le capteur MQ-135 */
typedef struct
{
    float supplyVoltage;        /* Tension d'alimentation Vc (V) */
    float loadResistanceKOhm;   /* Resistance de charge RL (kOhms) */
    float r0KOhm;               /* Resistance de reference R0 calibree (kOhms) */
    float cleanAirFactor;       /* Ratio Rs/R0 a 400 ppm */
    float curveA;               /* Coefficient A de la courbe puissance */
    float curveB;               /* Exposant B de la courbe puissance */
    float voltageDividerRatio;  /* Ratio du pont diviseur materiel */
} mq135_calibration_t;

/* Initialiseur par defaut pour MQ-135 */
#define MQ135_CALIBRATION_DEFAULT \
    { \
        .supplyVoltage = MQ135_SUPPLY_VOLTAGE, \
        .loadResistanceKOhm = MQ135_RL_KOHM, \
        .r0KOhm = MQ135_R0_DEFAULT_KOHM, \
        .cleanAirFactor = MQ135_CLEAN_AIR_FACTOR, \
        .curveA = MQ135_CURVE_A, \
        .curveB = MQ135_CURVE_B, \
        .voltageDividerRatio = MQ135_VOLTAGE_DIVIDER_RATIO \
    }

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
