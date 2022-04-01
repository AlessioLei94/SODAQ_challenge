#ifndef BTNS_H_
#define BTNS_H_

#include <drivers/gpio.h>

/*
 * Register callback function called when the button is pressed
 *
 * Return 0 on success, negative number in case of error
 */
int register_btn_cb(gpio_callback_handler_t cb);

/*
 * Initialize BTN driver
 *
 * Return 0 on success, negative number in case of error
 */
int btns_init(void);

#endif //BTNS_H_