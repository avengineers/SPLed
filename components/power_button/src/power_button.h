#ifndef POWER_BUTTON_H
#define POWER_BUTTON_H

#define POWER_BUTTON_PRESS_DEBOUNCE 10u
#define POWER_BUTTON_RELEASE_DEBOUNCE 10u

void powerButtonInit(void);
void powerButton(void);

#endif /* POWER_BUTTON_H */
