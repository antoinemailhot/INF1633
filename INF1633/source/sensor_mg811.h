#ifndef SENSOR_MG811_H
#define SENSOR_MG811_H

#include <stdbool.h>
#include <stdint.h>

/*
 * @brief Initialise le convertisseur ADC utilisé par le MG-811.
 *
 * @return true lorsque la séquence de configuration logicielle est effectuée.
 */
bool MG811_Init(void);

/*
 * @brief Effectue une conversion ADC et retourne la valeur brute.
 *
 * @param rawValue Pointeur recevant la valeur brute sur 12 bits.
 *
 * @return true si la conversion est terminée avec succès.
 *
 * En cas d'échec, *rawValue vaut zéro si le pointeur est valide.
 * Aucun calcul de concentration en ppm n'est effectué.
 */
bool Read_Sensor_Raw(uint16_t *rawValue);

#endif /* SENSOR_MG811_H */