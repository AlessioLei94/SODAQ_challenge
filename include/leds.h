#ifndef LEDS_H_
#define LEDS_H_

#include <stdbool.h>

/*
 * Set led status to ON or OFF
 *
 * Return 0 on success, negative number in case of error
 */
int led_set_status(bool status);

/*
 * Initialize LED driver
 *
 * Return 0 on success, negative number in case of error
 */
int led_init(void);

#endif //LEDS_H_