.. _SODAQ-challenge:

SODAQ Challenge
######

Design choices
********

My solution for the challege consists in a simple firmware that is split between few source files.
For simplicity i decided to create files that contain a task and few other functions needed for what
the task has to do. If this was a bigger project i would have separated this differently, having a file
to represent drivers for each peripheral with the needed funcitions exported in header files that can
be imported from files containing tasks implementation, possibly reusing the drivers code across
different tasks.

Implementation
********

The source files are:
- main.c
- led_control.c

main.c contains the main() function, entry point of the fw, that first gets and the reset cause from
the hwinfo module and it prints its value after resetting the flags in orded to always have
only the cause of the last reset.

Then if the device has a WDT_NODE, representing the HW watchdog, its node_id will be used to get the
pointer to the device struct and pass this one to the  function that initializes the watchdog.
If the HW watchdog is present it will be used as a fallback in case the SW watchdog malfunctions.

After the WDT initialization a new channel is added to it, this channel will be the one monitoring
the infinite loop that is present at the end of the main() function. Once inside the loop this task
will just print a message and feed the wdt every second.

led_control.c contains the Implementation of another task, that is defined at compile time using
the K_THREAD_DEFINE macro. This will statically allocate the memory used as stack for the task.

Once the task is started it will initialize the LED device, getting a pointer to the device struct using
its node_id and using this to set the GPIO, assigning a pin number and configuration flags needed to
define the GPIO operation mode.

Then a new wdt channel is created to monitor the led_task, this time assigning also a callback function
and passing the thread id as parameter to the callback. Once the watchdog times out it will call this
function where we print the thread id and then reset the board. This would be useful if there is the
need to perform any meaningful operation before resetting the board.

The led task just switched ON and OFF the led associated to the led0 node then feeds the wdt.