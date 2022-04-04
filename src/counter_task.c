/* 
 * File containing the implementation of a counter task.
 * Increase a counter every time BTN2 is pressed and print the count every second
 */
#include <zephyr.h>
#include <sys/reboot.h>
#include <task_wdt/task_wdt.h>

#include <btns.h>

//Define mutex at compile time, used to control the access to counter variable
K_MUTEX_DEFINE(counter_mux);

static int counter = 0;

/*
 * BTN pressed callback used to increase a counter
 */
void button1_pressed_cb(const struct device *port, struct gpio_callback *cb, uint32_t pins) {
	printk("Button1 pressed cb\n");
    
    if(k_mutex_lock(&counter_mux, K_MSEC(100U)) == 0) {
    	counter++;

        k_mutex_unlock(&counter_mux);
    } else {
        printk("Failed to get lock to increase value!");
    }
}

/*
 * Counter task function.
 * Register BTN2 pressed callback that increases a counter.
 * Infinite loop that prints the value of the counter.
 * Access to the counter is controlled by a mutex
 */
void counter_task_fun(void) {
    int ret = register_btn2_cb(button1_pressed_cb);
    if(ret != 0) {
        printk("register_btn2_cb failed (%d)", ret);
        return;
    }

    printk("counter_task_fun started!\n");

    //Add wdt channel for counter task, passing NULL instead of cb triggers reset
    int wdt_id = task_wdt_add(1200U, NULL, NULL);

    while(1) {
        if(k_mutex_lock(&counter_mux, K_MSEC(100U)) == 0) {
            printk("Counter task count is %d\n", counter);

            k_mutex_unlock(&counter_mux);
        } else {
            printk("Failed to get lock to print value!");
        }

        task_wdt_feed(wdt_id);
        k_sleep(K_MSEC(1000U));
    }
}

//Define thread at compile time
K_THREAD_DEFINE(counter_task, 1024, counter_task_fun, NULL, NULL, NULL, 0, 0, 1000);
