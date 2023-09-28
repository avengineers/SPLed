#ifndef __keyboard_interface_h__
#define __keyboard_interface_h__

#ifndef TARGET_KEY
#define TARGET_KEY 'P' /**< Target key to monitor. Default is 'P'. Can be overridden with a preprocessor define. */
#endif

#define CFG_DEBOUNCE_PRESS 10
#define CFG_DEBOUNCE_RELEASE 10

void keyboardInterface();

#endif // __keyboard_interface_h__
