/*
 * Projet : INF1633
 * Module : led_driver
 * Description : Configuration des ports/broches GPIO (horloge, Pin Muxing,
 *               direction kGPIO_DigitalOutput) et pilotage des DEL Verte
 *               (PTB19) et Rouge (PTB18) sur la carte FRDM-KL27Z.
 */

#include "led_driver.h"

#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

/* Indique si le pilote GPIO des DEL a ete initialise. */
static bool s_ledsInitialized = false;

/*
 * @brief Initialise les ports et broches GPIO des DEL Verte et Rouge.
 */
bool LED_Driver_Init(void)
{
    gpio_pin_config_t ledPinConfig = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic  = LED_STATE_OFF
    };

    /* 1. Activer l'horloge du port B (porte PTB18 et PTB19). */
    CLOCK_EnableClock(kCLOCK_PortB);

    /* 2. Configurer le multiplexage (Pin Muxing) en mode GPIO (ALT1). */
    PORT_SetPinMux(LED_RED_PORT, LED_RED_PIN, kPORT_MuxAsGpio);
    PORT_SetPinMux(LED_GREEN_PORT, LED_GREEN_PIN, kPORT_MuxAsGpio);

    /*
     * 3. Configurer les broches en sortie numerique (kGPIO_DigitalOutput)
     *    avec niveau logique initial eteint (active-low -> 1U).
     */
    GPIO_PinInit(LED_RED_GPIO, LED_RED_PIN, &ledPinConfig);
    GPIO_PinInit(LED_GREEN_GPIO, LED_GREEN_PIN, &ledPinConfig);

    s_ledsInitialized = true;

    return true;
}

/*
 * @brief Commande l'etat de la LED Verte (PTB19).
 */
void LED_Green_Set(bool on)
{
    if (!s_ledsInitialized)
    {
        return;
    }

    GPIO_PinWrite(
        LED_GREEN_GPIO,
        LED_GREEN_PIN,
        on ? LED_STATE_ON : LED_STATE_OFF
    );
}

/*
 * @brief Inverse l'etat logique actuel de la LED Verte (PTB19).
 */
void LED_Green_Toggle(void)
{
    if (!s_ledsInitialized)
    {
        return;
    }

    GPIO_PortToggle(LED_GREEN_GPIO, (1U << LED_GREEN_PIN));
}

/*
 * @brief Commande l'etat de la LED Rouge (PTB18).
 */
void LED_Red_Set(bool on)
{
    if (!s_ledsInitialized)
    {
        return;
    }

    GPIO_PinWrite(
        LED_RED_GPIO,
        LED_RED_PIN,
        on ? LED_STATE_ON : LED_STATE_OFF
    );
}

/*
 * @brief Inverse l'etat logique actuel de la LED Rouge (PTB18).
 */
void LED_Red_Toggle(void)
{
    if (!s_ledsInitialized)
    {
        return;
    }

    GPIO_PortToggle(LED_RED_GPIO, (1U << LED_RED_PIN));
}

/*
 * @brief Eteint simultanement les deux DEL de signalisation.
 */
void LED_All_Off(void)
{
    LED_Red_Set(false);
    LED_Green_Set(false);
}
