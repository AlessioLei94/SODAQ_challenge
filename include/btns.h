#ifndef BTNS_H_
#define BTNS_H_

#include <drivers/gpio.h>

/*
 * Register callback function called when the button0 is pressed
 *
 * Return 0 on success, negative number in case of error
 */
int register_btn1_cb(gpio_callback_handler_t cb);

/*
 * Register callback function called when the button1 is pressed
 *
 * Return 0 on success, negative number in case of error
 */
int register_btn2_cb(gpio_callback_handler_t cb);

/*
 * Initialize BTN driver
 *
 * Return 0 on success, negative number in case of error
 */
int btns_init(void);

#endif //BTNS_H_