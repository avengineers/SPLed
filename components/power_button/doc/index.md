# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

This module interfaces with the keyboard and debounces the `POWER_BUTTON_KEY` to provide a stable key press event.

## Design Considerations

```{spec} State Management
:id: SWDD_PB-100

The power button detection is implemented as a state machine with three states: `INIT`, `PRESSED`, and `RELEASED`.
```

```{spec} Debouncing
:id: SWDD_PB-101

The key press and release events are debounced to prevent spurious signals. The debounce periods are configurable via `POWER_BUTTON_PRESS_DEBOUNCE` and `POWER_BUTTON_RELEASE_DEBOUNCE`. A key press or release is only registered after its state is consistently detected for the configured number of periodic calls.
```

## Interfaces

```{spec} Runnable
:id: SWDD_PB-200

The Power Button component shall be called periodically by its runnable `powerButton()`.
```

```{spec} Initialization
:id: SWDD_PB-201

The component shall be initialized by calling `powerButtonInit()`.
```

```{spec} Key Input
:id: SWDD_PB-202

The component uses the RTE interface `RteIsKeyPressed(POWER_BUTTON_KEY)` to get the current raw state of the power button.
```

```{spec} Key Pressed Event Output
:id: SWDD_PB-203

The component uses the RTE interface `RteSetPowerKeyPressedEvent()` to signal a debounced key press event.
```

## Internal Behavior

```{spec} State Machine
:id: SWDD_PB-300

The internal state machine manages the debouncing logic. It transitions between states based on counters that track the duration of a consistent key state.
```

```{mermaid}
stateDiagram-v2
   [*] --> INIT: Initial State
   INIT --> PRESSED: Button pressed long enough
   INIT --> RELEASED: Button released long enough
   PRESSED --> RELEASED: Button released long enough
   RELEASED --> PRESSED: Button pressed long enough
```
