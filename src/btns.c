/*
 * File representing a BTN driver.
 * Simple implemetation because only one button is initialized and handled,
 * could be extended to handle more than one.
 */
#include <btns.h>

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

int register_btn_cb(gpio_callback_handler_t cb) {
    static bool registered = false;

    if(registered) {
        printk("Button cn already registered!");
        return -1;
    }

    gpio_init_callback(&button_cb_data, cb, BIT(button.pin));
    int ret = gpio_add_callback(button.port, &button_cb_data);
    if(ret != 0) {
        printk("gpio_add_callback failed (%d)", ret);
        return ret;
    }

    registered = true;

    return ret;
}

int btns_init(void) {
    int ret;

	if (!device_is_ready(button.port)) {
		printk("Error: button device %s is not ready\n",
		       button.port->name);
		return -1;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return ret;
	}

	printk("Set up button at %s pin %d\n", button.port->name, button.pin);

    return ret;
}