# Software Detailed Design

## Introduction

The Auto Off component decides *when* the light has been idle long enough to be switched off. It
watches the user input keys, counts the time since the last key press, and reports the result as
the auto off state.

It does not switch the light off itself. `power_signal_processing` reads the auto off state and
sets the power state, so the two components together deliver the automatic power off behaviour.

## Design Considerations

```{spec} Inactivity timeout
:id: SWDD_AO-110
:refines: SWARCH_001, REQ_39
:integrity: B

The component shall report the auto off state as TRUE when no user activity has been detected for
the configured auto off period, and as FALSE while that period has not yet elapsed.
```

```{spec} User activity detection
:id: SWDD_AO-111
:refines: SWARCH_001, REQ_40
:integrity: B

The component shall treat a press of any of the following keys as user activity:

- the power button (`POWER_BUTTON_KEY`)
- the up control key (`CONTROL_KEY_UP`)
- the down control key (`CONTROL_KEY_DOWN`)

A press of any one of them is sufficient; no other input is user activity.
```

```{spec} Configurable auto off period
:id: SWDD_AO-112
:refines: SWARCH_001, REQ_39
:integrity: B

The auto off period shall be taken from `CONFIG_AUTO_OFF_PERIOD_SECONDS`, which the feature model
restricts to 5 to 7200 seconds. The component shall convert it to milliseconds, so that the timer
and the task period share one unit.
```

```{spec} Timer countdown
:id: SWDD_AO-113
:refines: SWARCH_001, REQ_39
:integrity: B

While no user activity is detected, the component shall reduce the inactivity timer by
`CONFIG_OS_TASK_PERIOD` on every execution. The subtraction shall saturate at zero, so that a
timer holding less than one task period reaches zero rather than wrapping around.
```

```{spec} Timer reset on activity
:id: SWDD_AO-114
:refines: SWARCH_001, REQ_40
:integrity: B

When user activity is detected, the component shall set the inactivity timer to the full auto off
period, regardless of its previous value.
```

```{spec} State before the first user activity
:id: SWDD_AO-115
:refines: SWARCH_001
:integrity: QM

Until user activity is detected for the first time, the component shall report the auto off state
as TRUE, which keeps the light off after a reset. The inactivity timer starts at zero, so this is
the same behaviour as an elapsed period.
```

## Interfaces

```{spec} Runnable
:id: SWDD_AO-210
:refines: SWARCH_001
:integrity: B

The component is called by its runnable `autoOff()`. One call is one execution, and the
application calls it once per task period.
```

```{spec} Key press input
:id: SWDD_AO-211
:refines: SWARCH_001, REQ_40, REQ_56
:integrity: B

The component shall read the key states through the RTE interface `RteIsKeyPressed()`, at most once
per monitored key and per execution. It may stop reading as soon as one key reports a press.
```

```{spec} Auto off state output
:id: SWDD_AO-212
:refines: SWARCH_001, REQ_59
:integrity: B

The component shall report the auto off state through the RTE interface `RteSetAutoOffState()`. It
shall write the state on every execution, not only when it changes.
```

## Internal Behaviour

```{spec} Flow chart
:id: SWDD_AO-310
:refines: SWARCH_001
:integrity: QM

One execution of the component follows this decision process:
```

```{mermaid}

flowchart TD
    Start([Start])
    activity{"user activity?"}
    reset["timer = auto off period"]
    counting{"timer >= task period?"}
    decrement["timer -= task period"]
    saturate["timer = 0"]
    expired{"timer == 0?"}
    setTrue["RteSetAutoOffState(TRUE)"]
    setFalse["RteSetAutoOffState(FALSE)"]
    End([End])

    Start --> activity
    activity -- "yes" --> reset --> expired
    activity -- "no" --> counting
    counting -- "yes" --> decrement --> expired
    counting -- "no" --> saturate --> expired
    expired -- "yes" --> setTrue --> End
    expired -- "no" --> setFalse --> End

```

```{spec} Timeout accuracy
:id: SWDD_AO-311
:refines: SWARCH_001, REQ_39
:integrity: QM

The timer is reset to the auto off period and reduced by one task period per execution, so the
auto off state turns TRUE after exactly `CONFIG_AUTO_OFF_PERIOD_SECONDS` of inactivity, with a
resolution of one task period.
```
