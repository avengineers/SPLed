# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

The Light Controller is responsible for managing the behavior of the LED based on the system's power state.

## Design Considerations

```{spec} State Management
:id: SWDD_LC-100
:refines: SWARCH_001

The light can be in one of two states: ON or OFF. The state transitions are triggered by changes in the system's power state.
```

{% if config.BLINKING %}

```{spec} Blinking Behavior
:id: SWDD_LC-101
:refines: SWARCH_001

When the light is ON, it may exhibit a blinking behavior. The blinking rate is configurable and is determined based on an external input (main knob value).
```

{% endif %}

```{spec} Color Management
:id: SWDD_LC-102
:refines: SWARCH_001

The color of the light is specified as an RGB value.
```

```{spec} Configurable Color
:id: SWDD_LC-103
:refines: SWARCH_001

The color of the light shall be fixed but configurable.
*Note:* The color is configurable in the sense that it is specified as a configuration parameter. The color is not configurable at runtime.
```

## Interfaces

```{spec} Runnable
:id: SWDD_LC-200
:refines: SWARCH_001

The Light Controller is called by its runnable `lightController()`.
```

```{spec} Power State Input
:id: SWDD_LC-201
:refines: SWARCH_001

The Light Controller uses the RTE interface `RteGetPowerState()` to get the current power state.
```

```{spec} Light Color Output
:id: SWDD_LC-202
:refines: SWARCH_001

The Light Controller uses the RTE interface `RteSetLightValue()` to set the light color.
```

{% if config.BLINKING %}

```{spec} Main Knob Input
:id: SWDD_LC-203
:refines: SWARCH_001

The Light Controller uses the RTE interface `RteGetMainKnobValue()` to get the main knob value for controlling the blinking rate.
```

{% endif %}

{% if config.BRIGHTNESS_ADJUSTMENT_ENABLED %}

```{spec} Brightness Adjustment
:id: SWDD_LC-204
:refines: SWARCH_001

The Light Controller uses the RTE interface `RteGetBrightnessValue()` to get the required brightness value for the light.
```

{% endif %}

## Internal Behavior

```{spec} State Machine
:id: SWDD_LC-300
:refines: SWARCH_001

The Light Controller is implemented as a state machine. The state machine is shown below.
```

```{mermaid}
stateDiagram-v2
    [*] --> LIGHT_OFF: Initial State
    LIGHT_OFF --> LIGHT_ON : Power State != OFF
    LIGHT_ON --> LIGHT_OFF : Power State == OFF
{% if config.BLINKING %}
    state LIGHT_ON {
        BlinkON --> BlinkOFF : Blink half-period elapsed
        BlinkOFF --> BlinkON : Blink half-period elapsed
    }
{% endif %}
```
