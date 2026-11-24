/*
 * Projet : INF1633
 * Fichier : co2_formatter.h
 * Description : Formatage des chaines d'affichage de la concentration de CO2
 *               (en ppm avec une decimale : "Concentration CO2 : XXX.X ppm")
 *               pour la console serie UART et l'ecran LCD, sans dependance
 *               au formatage flottant printf (%f).
 */

#ifndef CO2_FORMATTER_H
#define CO2_FORMATTER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Taille minimale recommandee pour le tampon de formatage texte. */
#define CO2_FORMAT_BUFFER_MIN_SIZE      (64U)

/*
 * @brief Formate la concentration de CO2 sous la forme exacte :
 *        "Concentration CO2 : XXX.X ppm"
 *
 * @param ppm Concentration de CO2 en parties par million.
 * @param buffer Tampon de destination recevant la chaine terminee par '\0'.
 * @param bufferSize Taille du tampon en octets.
 *
 * @return true si le formatage reussit, false si ppm est invalide (NAN/negatif/infini)
 *         ou si le tampon est trop petit.
 */
bool Format_CO2_Concentration(float ppm, char *buffer, size_t bufferSize);

/*
 * @brief Formate la ligne complete prete pour la transmission serie UART :
 *        "Concentration CO2 : XXX.X ppm\r\n"
 *
 * @param ppm Concentration de CO2 en parties par million.
 * @param buffer Tampon de destination recevant la chaine terminee par '\0'.
 * @param bufferSize Taille du tampon en octets.
 *
 * @return true si le formatage reussit, false sinon.
 */
bool Format_CO2_Serial_Line(float ppm, char *buffer, size_t bufferSize);

#ifdef __cplusplus
}
#endif

#endif /* CO2_FORMATTER_H */
