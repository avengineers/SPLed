Design Specification for Light Controller Component
===================================================

.. contents:: Table of Contents
    :depth: 2

Introduction
------------

The Light Controller module is responsible for managing the behavior of a light based on the system's power state. This document outlines the design considerations and the high-level structure of the module.

Design Considerations
---------------------

.. spec:: State Management
   :id: SWDD_LC-001
   :integrity: B

    The light can be in one of two states: ON or OFF. The state transitions are triggered by changes in the system's power state.

{% if config.BLINKING %}
.. spec::  Blinking Behavior
    :id: SWDD_LC-002
    :integrity: B

    When the light is ON, it may exhibit a blinking behavior. The blinking rate is configurable and is determined based on an external input (main knob value).
{% endif %}

.. spec:: Color Management
    :id: SWDD_LC-003

    The color of the light when it is ON is specified as an RGB value.

{% if config.BRIGHTNESS_ADJUSTMENT %}
.. spec::  Brightness Adjustment
    :id: SWDD_LC-005

    When the light is ON, it may have a variable brightness. The brightness value is determined based on an external input (main knob value).
{% endif %}

.. spec:: Configurable color
    :id: SWDD_LC-006

    The color of the light shall be fixed but configurable.
    *Note:* The color is configurable in the sense that it is specified as a configuration parameter. The color is not configurable at runtime.

Interfaces
----------

1. **External Functions**:
    - ``RteSetLightValue``: Function to set the light color.
    - ``RteGetPowerState``: Function to get the current power state.
    - ``RteGetMainKnobValue``: Function to get the main knob value.

2. **Internal Functions**:
    - ``turnLightOff``: Internal function to turn the light off.
    - ``turnLightOn``: Internal function to turn the light on.
{% if config.BLINKING %}
    - ``calculateBlinkPeriod``: Internal function to calculate the blink period.
{% endif %}
    - ``lightController``: Main interface function to control the light behavior.


Internal Behavior
-----------------

.. spec::  State Machine
    :id: SWDD_LC-004

    The light controller module is implemented as a state machine. The state machine is shown below.

.. mermaid::

    stateDiagram-v2
        [*] --> LIGHT_OFF: Initial State
        LIGHT_OFF --> LIGHT_ON : Power State != OFF
        LIGHT_ON --> LIGHT_OFF : Power State == OFF
{% if config.BLINKING %}
        LIGHT_ON --> BlinkON : Blink Counter >= Blink Period
        BlinkON --> BlinkOFF : Blink State == TRUE
        BlinkOFF --> BlinkON : Blink State == FALSE
        BlinkON --> LIGHT_ON : Reset Blink Counter
        BlinkOFF --> LIGHT_ON : Reset Blink Counter
{% endif %}
