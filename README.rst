.. _SODAQ-challenge:

SODAQ Challenge
###############

Design choices
**************

My solution for the challege consists in a simple firmware that is split between few source files.
I separated the files having some of them containing the implementation of drivers while the others
contain the implementation of tasks that are using some peripherals importing the drivers' header files.

This way is possible to reuse the drivers code in different files and tasks without duplicating it.
In this firmware the drivers are very simple because they handle just one led and one button and
basic functionalities.

This implementation design makes it easy to expand drivers for example to handle more than one led
or button, or having higher level functionalities like making the led blink in different ways or
handling different types of button press (long, short, double press).

For building the firmware i used nRF Connect SDK v1.9.1 with the nRF Connect extension for
Visual Sudio Code.

Implementation
**************

The source files containing the implementation of a task are:

* main.c
* task_contol.c

main.c
======

Contains the main() function, entry point of the fw, that first gets the reset
cause from the hwinfo module then it prints its value after resetting the flags in orded to always have
only the cause of the last reset.

Then if the device has a WDT_NODE, representing the HW watchdog, its node_id will be used to get the
pointer to the device struct and pass this one to the function that initializes the watchdog.
If the HW watchdog is present it will be used as a fallback in case the SW watchdog malfunctions.

After the WDT initialization a new channel is added to it, this channel will be the one monitoring
the main task. Once inside the infinite loop this task will just print a message and feed the
wdt every second.

control_task.c
==============

Contains the Implementation of another task, that is defined at compile time using
the K_THREAD_DEFINE macro. This will statically allocate the memory used as stack by the task.

Once the task is started it will initialize the LED and the BTN calling the corresponding init functions,
and registers the button press callback.
Then a new wdt channel is created to monitor the control_task, this time assigning also a callback function
and passing the thread id as parameter to the callback. Once the watchdog times out it will call this
function where we print the thread id and then reset the board. This would be useful if there is the
need to perform any meaningful operation before resetting the board.

If the button is pressed the registered callback will be called and this will trigger the flip of a 
boolean flag. If this flag is true at the moment when the inifite loop evaluates its value, it will
cause the task to sleep forever triggering the WDT.
If the flag is false it will just switch ON and OFF the led to show the task is alive then feeds the wdt.
