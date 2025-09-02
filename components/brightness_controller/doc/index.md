# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

The Brightness Controller is responsible for adjusting the brightness of the LED.

## Design Considerations

```{spec} Brightness Value
:id: SWDD_BC-100

The brightness of the light is represented by an integer value from 0 up to 255.
```

{% if config.BRIGHTNESS_ADJUSTMENT_MANUAL %}
```{spec} Manual Brightness Adjustment
:id: SWDD_BC-101
The brightness of the light is adjustable by an external input (main knob value in percentage).
```
{% endif %}

{% if config.BRIGHTNESS_ADJUSTMENT_AUTOMATIC %}
```{spec} Automatic Brightness Adjustment
:id: SWDD_BC-102
The brightness of the light is automatically adjusted based on a given period.
```
{% endif %}

## Interfaces

```{spec} Runnable
:id: SWDD_BC-200

The Brightness Controller is be called by its runnable `brightnessController()`.
```

{% if config.BRIGHTNESS_ADJUSTMENT_MANUAL %}
```{spec} Main Knob Input
:id: SWDD_BC-201
The Brightness Controller is use the RTE interface `RteGetMainKnobValue()` for manual brightness adjustment.
```
{% endif %}

```{spec} Brightness Value Output
:id: SWDD_BC-202

The Brightness Controller is use the RTE interface `RteSetBrightnessValue()` to set the brightness value.
```

{% if config.BRIGHTNESS_ADJUSTMENT_AUTOMATIC %}
```{spec} Periodic Dimming Counter Output
:id: SWDD_BC-203
The Brightness Controller is use the RTE interface `RteSetBrightnessAdjustmentCounter()` to set the brightness adjustment counter.
```
{% endif %}
