/*
 * Projet : INF1633
 * Module : sensor_mg811
 * Description : Acquisition ADC et conversion en tension du MG-811.
 */

#include <math.h>
#include <stddef.h>

#include "sensor_mg811.h"
#include "atom_conversion.h"

#include "fsl_adc16.h"
#include "fsl_clock.h"
#include "fsl_port.h"

/*
 * Référence nominale du convertisseur ADC en volts.
 *
 * Cette valeur est configurable à la compilation.
 * La tension réelle VREFH doit être mesurée pour améliorer la précision.
 */
#ifndef MG811_ADC_REFERENCE_VOLTAGE
#define MG811_ADC_REFERENCE_VOLTAGE 3.3f
#endif

/* Résolution utilisée par le convertisseur ADC16. */
#define MG811_ADC_RESOLUTION_BITS    12U

/*
 * Canal ADC0_SE1 :
 * Broche PTE16, connecteur J4[2] / A0.
 */
#define MG811_ADC_CHANNEL            1U
#define MG811_ADC_CHANNEL_GROUP      0U

/*
 * Limite de sécurité du polling.
 * Il s'agit d'un nombre d'itérations, et non d'une durée en millisecondes.
 */
#define MG811_ADC_TIMEOUT_ITERATIONS 1000000U

/* Indique si le convertisseur ADC a été initialisé. */
static bool s_adcInitialized = false;

/*
 * @brief Initialise le matériel nécessaire à la lecture ADC.
 *
 * @return true lorsque la séquence de configuration logicielle est effectuée.
 *
 * Remarque :
 * ADC16_Init() ne retourne aucun code d'erreur.
 * Le retour true ne constitue donc pas une confirmation électrique
 * du fonctionnement du capteur ou du convertisseur.
 */
bool MG811_Init(void)
{
    adc16_config_t adcConfig;

    /* Activer l'horloge du port E. */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* Activer l'horloge du convertisseur ADC0. */
    CLOCK_EnableClock(kCLOCK_Adc0);

    /* Configurer PTE16 en mode analogique. */
    PORT_SetPinMux(PORTE, 16U, kPORT_PinDisabledOrAnalog);

    /* Charger la configuration par défaut du convertisseur. */
    ADC16_GetDefaultConfig(&adcConfig);

    /* Définir explicitement la résolution à 12 bits. */
    adcConfig.resolution = kADC16_ResolutionSE12Bit;

    /* Initialiser le convertisseur ADC0. */
    ADC16_Init(ADC0, &adcConfig);

    /*
     * La calibration automatique est volontairement omise.
     * L'implémentation du SDK attend sans timeout.
     */

    s_adcInitialized = true;

    return true;
}

/*
 * @brief Déclenche une conversion et récupère la valeur brute.
 *
 * @param rawValue Pointeur recevant la valeur brute sur 12 bits.
 *
 * @return true si la conversion est terminée avec succès.
 *
 * En cas d'échec, la valeur pointée reste à zéro si le pointeur est valide.
 */
bool Read_Sensor_Raw(uint16_t *rawValue)
{
    adc16_channel_config_t channelConfig = {0};
    uint32_t timeout = MG811_ADC_TIMEOUT_ITERATIONS;

    /* Vérifier le pointeur avant toute opération sur la sortie. */
    if (rawValue == NULL)
    {
        return false;
    }

    /* Garantir une valeur de sortie déterministe en cas d'échec. */
    *rawValue = 0U;

    /* Vérifier que le convertisseur a été initialisé. */
    if (!s_adcInitialized)
    {
        return false;
    }

    /* Sélectionner le canal ADC0_SE1. */
    channelConfig.channelNumber = MG811_ADC_CHANNEL;

    /* Utiliser le polling plutôt qu'une interruption. */
    channelConfig.enableInterruptOnConversionCompleted = false;

#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
    /* Effectuer une conversion en mode simple. */
    channelConfig.enableDifferentialConversion = false;
#endif

    /*
     * Configurer le canal et déclencher la conversion logicielle.
     * Le groupe 0 est utilisé pour les conversions logicielles.
     */
    ADC16_SetChannelConfig(
        ADC0,
        MG811_ADC_CHANNEL_GROUP,
        &channelConfig
    );

    /*
     * Attendre la fin de conversion.
     * La boucle est bornée afin d'éviter un blocage permanent.
     */
    while (
        ((ADC16_GetChannelStatusFlags(
            ADC0,
            MG811_ADC_CHANNEL_GROUP
        ) & kADC16_ChannelConversionDoneFlag) == 0U)
        && (timeout > 0U)
    )
    {
        timeout--;
    }

    /* Vérifier si la conversion est réellement terminée. */
    if (
        (ADC16_GetChannelStatusFlags(
            ADC0,
            MG811_ADC_CHANNEL_GROUP
        ) & kADC16_ChannelConversionDoneFlag) == 0U
    )
    {
        return false;
    }

    /*
     * Lire le registre de résultat.
     * Cette lecture récupère la valeur et efface le drapeau COCO.
     */
    *rawValue = (uint16_t)ADC16_GetChannelConversionValue(
        ADC0,
        MG811_ADC_CHANNEL_GROUP
    );

    return true;
}

/*
 * @brief Lit le signal analogique et retourne sa tension en volts.
 *
 * @return Tension calculée en volts, ou NAN en cas d'erreur.
 *
 * La conversion utilise :
 * V = rawValue * VREF / 4096
 */
float Get_Sensor_Voltage(void)
{
    uint16_t rawValue = 0U;
    float voltage = 0.0f;

    /* Lire la valeur brute du convertisseur ADC. */
    if (!Read_Sensor_Raw(&rawValue))
    {
        return NAN;
    }

    /* Convertir la valeur brute en tension électrique. */
    if (!ADC_To_Voltage(
        rawValue,
        MG811_ADC_REFERENCE_VOLTAGE,
        MG811_ADC_RESOLUTION_BITS,
        &voltage
    ))
    {
        return NAN;
    }

    return voltage;
}
