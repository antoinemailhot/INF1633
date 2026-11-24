/*
 * Projet : INF1633
 * Module : co2_formatter
 * Description : Implementation du formatage deterministe de la chaine
 *               "Concentration CO2 : XXX.X ppm" en arithmetique entiere
 *               (compatibilite totale avec fsl_debug_console sans %f).
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "co2_formatter.h"
#include "config.h"

/*
 * @brief Formate la concentration sous la forme "Concentration CO2 : XXX.X ppm".
 */
bool Format_CO2_Concentration(float ppm, char *buffer, size_t bufferSize)
{
    uint32_t scaledTenths;
    uint32_t integerPart;
    uint32_t decimalPart;
    int written;

    if ((buffer == NULL) || (bufferSize == 0U))
    {
        return false;
    }

    buffer[0] = '\0';

    if (!isfinite(ppm) || (ppm < CO2_MIN_VALID_PPM) || (ppm > CO2_MAX_VALID_PPM))
    {
        return false;
    }

    /* Arrondi au dixieme le plus proche sans formatage flottant printf. */
    scaledTenths = (uint32_t)((ppm * 10.0f) + 0.5f);
    integerPart  = scaledTenths / 10U;
    decimalPart  = scaledTenths % 10U;

    written = snprintf(
        buffer,
        bufferSize,
        "Concentration CO2 : %lu.%lu ppm",
        (unsigned long)integerPart,
        (unsigned long)decimalPart
    );

    return ((written > 0) && ((size_t)written < bufferSize));
}

/*
 * @brief Formate la ligne serie complete avec fin de ligne CRLF.
 */
bool Format_CO2_Serial_Line(float ppm, char *buffer, size_t bufferSize)
{
    uint32_t scaledTenths;
    uint32_t integerPart;
    uint32_t decimalPart;
    int written;

    if ((buffer == NULL) || (bufferSize == 0U))
    {
        return false;
    }

    buffer[0] = '\0';

    if (!isfinite(ppm) || (ppm < CO2_MIN_VALID_PPM) || (ppm > CO2_MAX_VALID_PPM))
    {
        return false;
    }

    scaledTenths = (uint32_t)((ppm * 10.0f) + 0.5f);
    integerPart  = scaledTenths / 10U;
    decimalPart  = scaledTenths % 10U;

    written = snprintf(
        buffer,
        bufferSize,
        "Concentration CO2 : %lu.%lu ppm\r\n",
        (unsigned long)integerPart,
        (unsigned long)decimalPart
    );

    return ((written > 0) && ((size_t)written < bufferSize));
}
