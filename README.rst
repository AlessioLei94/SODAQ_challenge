.. _SODAQ-challenge:

SODAQ Challenge
###############

Design choices
**************

My solution for the challege consists in a simple firmware that is split between few source files.
I separated the files having some of them containing the implementation of drivers while the others
contain the implementation of tasks that are using some peripherals importing the drivers' header files.

This way is possible to reuse the drivers code in different files and tasks without duplicating it.
In this firmware the drivers are very simple because they handle basic functionalities of
the peripherals.

This implementation design makes it easy to expand drivers in order to have higher level
functionalities like making the led blink in different ways or handling different types of
button press (long, short, double press).

For building the firmware i used nRF Connect SDK v1.9.1 with the nRF Connect extension for
Visual Sudio Code.

Implementation
**************

The source files containing the implementation of a task are:

* main.c
* control_task.c
* counter_task.c

main.c
======

Contains the main() function, entry point of the fw, that first gets the reset
cause from the hwinfo module then it prints its value after resetting the flags in orded to always have
only the cause of the last reset.

After this leds and buttons are initialized, then if the device has a WDT_NODE, representing the 
HW watchdog, its node_id will be used to get the pointer to the device struct and pass this one to 
the function that initializes the watchdog.
If the HW watchdog is present it will be used as a fallback in case the SW watchdog malfunctions.

After the WDT initialization a new channel is added to it, this channel will be the one monitoring
the main task. Once inside the infinite loop this task will just print a message and feed the
wdt every second.

control_task.c
==============

Contains the Implementation of another task, that is defined at compile time using
the K_THREAD_DEFINE macro. This will statically allocate the memory used as stack by the task.

Once the task is started it will register a callback function to be called when BTN1 is pressed.
Then a new wdt channel is created to monitor the control_task, this time assigning also a callback function
and passing the thread id as parameter to the callback. Once the watchdog times out it will call this
function where we print the thread id and then reset the board. This would be useful if there is the
need to perform any meaningful operation before resetting the board.

If the button is pressed the registered callback will be called and this will trigger the flip of a 
boolean flag. If this flag is true at the moment when the inifite loop evaluates its value, it will
cause the task to sleep forever triggering the WDT.
If the flag is false it will just switch ON and OFF the leds to show the task is alive then feeds the wdt.


counter_task.c
==============

Contains the implementation of a task that first registers the a callback function that is called
when BTN2 is pressed then simply print the value of a variable.
The value of the variable is increased by 1 every time BTN2 is pressed.
To be sure that the thread prints a consisten value the counter variable is protected by a mutex
that is locked before accessing or changing the variable's value and unlock right after.

The drivers files are:

* led.c
* btn.c

led.c
=====

File containing a driver for the board's LEDs.
The init function is used to initialize the LEDs devices, configuring the GPIOs and their mode.

The function led_set_status() is exported in the leds.h file and it allows external callers to turn the LEDs ON or
OFF passing the LED index and the value to set as arguments to the function.

btns.c
======

File containing a driver for the board's BTNs.
The init function is used to initialize the BTNs devices, configuring the GPIOs and the interrupts.

Two functions are exported in the btns.h file in order to allow extrnal callers to register a callback
function that will be called when the corresponding button is pressed.
