/*
 * Projet : INF1633
 * Fichier : led_driver.h
 * Description : Pilote materiel GPIO pour la configuration et le controle
 *               des DEL de signalisation (LED Verte et LED Rouge) sur la
 *               carte NXP FRDM-KL27Z (PORTB / GPIOB, broches 18 et 19).
 */

#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mapping materiel des DEL de signalisation (FRDM-KL27Z).
 * Logique electrique active a l'etat bas (invers?e) :
 * 0U = DEL allumee, 1U = DEL eteinte.
 */
#define LED_RED_PORT            PORTB
#define LED_RED_GPIO            GPIOB
#define LED_RED_PIN             (18U)

#define LED_GREEN_PORT          PORTB
#define LED_GREEN_GPIO          GPIOB
#define LED_GREEN_PIN           (19U)

#define LED_STATE_ON            (0U)
#define LED_STATE_OFF           (1U)

/*
 * @brief Active les horloges GPIO, configure le multiplexage (Pin Muxing)
 *        en mode GPIO et initialise les broches de la LED Rouge (PTB18)
 *        et de la LED Verte (PTB19) en sortie numerique (kGPIO_DigitalOutput).
 *
 * @return true lorsque la configuration materielle est terminee.
 */
bool LED_Driver_Init(void);

/*
 * @brief Allume la LED Rouge (PTB18) en appliquant la logique active-low (0U).
 */
void Turn_On_Red_LED(void);

/*
 * @brief Eteint la LED Rouge (PTB18) en appliquant la logique active-low (1U).
 */
void Turn_Off_Red_LED(void);

/*
 * @brief Indique si la LED Rouge est actuellement allumee.
 *
 * @return true si la LED Rouge est allumee, false sinon.
 */
bool Is_Red_LED_On(void);

/*
 * @brief Commande l'etat de la LED Rouge (PTB18).
 *
 * @param on true pour allumer la DEL, false pour l'eteindre.
 */
void LED_Red_Set(bool on);

/*
 * @brief Inverse l'etat logique actuel de la LED Rouge (PTB18).
 */
void LED_Red_Toggle(void);

/*
 * @brief Commande l'etat de la LED Verte (PTB19).
 *
 * @param on true pour allumer la DEL, false pour l'eteindre.
 */
void LED_Green_Set(bool on);

/*
 * @brief Inverse l'etat logique actuel de la LED Verte (PTB19).
 */
void LED_Green_Toggle(void);

/*
 * @brief Eteint simultanement les deux DEL de signalisation.
 */
void LED_All_Off(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_DRIVER_H */
