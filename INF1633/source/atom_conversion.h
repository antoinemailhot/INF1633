#ifndef ATOM_CONVERSION_H
#define ATOM_CONVERSION_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Convertit une valeur ADC brute en tension électrique.
 *
 * @param rawValue Valeur brute provenant du convertisseur ADC.
 * @param referenceVoltage Tension de référence du convertisseur en volts.
 * @param resolutionBits Résolution du convertisseur en bits.
 * @param voltage Pointeur recevant la tension calculée en volts.
 *
 * @return true si la conversion est valide, false autrement.
 *
 * La formule utilisée est :
 * V = rawValue * referenceVoltage / (2^resolutionBits)
 *
 * Exemple pour un ADC de 12 bits :
 * V = rawValue * referenceVoltage / 4096
 */
bool ADC_To_Voltage(
    uint16_t rawValue,
    float referenceVoltage,
    uint8_t resolutionBits,
    float *voltage
);

#ifdef __cplusplus
}
#endif

#endif /* ATOM_CONVERSION_H */