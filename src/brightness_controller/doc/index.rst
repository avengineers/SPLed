Design Specification for Brightness Component
===================================================

.. contents:: Table of Contents
    :depth: 2

Introduction
------------

The Brightness Controller module is responsible for adjusting the brightness of a light based on a knob value (percentage) input. This document outlines the design considerations and the high-level structure of the module.


Design Considerations
---------------------

.. spec:: Brigther Adjustment
   :id: SWDD_BC-001
   :integrity: B

    The brightness of the light can be increased up to 255. The color brightness is configurable and can be increased based on an external input (main knob value in percentage).

.. spec::  Darker Adjustment
    :id: SWDD_BC-002
    :integrity: B

    The brightness of the light can be decreased up to 0. The color brightness is configurable and can be decreased based on an external input (main knob value in percentage).


Interfaces
----------

1. **External Functions**:
    - ``RteSetBrightnesstValue``: Function to update the value of the light brightness.
    - ``RteGetBrightnesstValue``: Function to get the current value of the light brightness.
    - ``RteGetMainKnobValue``: Function to get the main knob value to increase or decrease the brightness accordingly.

2. **Internal Functions**:
    - ``calculateBrightness``: Internal function to calculate the brightness of the light.

Internal Behavior
-----------------

.. mermaid::


    graph TD
     subgraph External Functions
      BrightnessController -->|Get Main Knob Value| RteGetMainKnobValue
      BrightnessController -->|Get Current Light Value| RteGetCurrentLightValue
      BrightnessController -->|Set Light Value| RteSetLightValue
     end

     subgraph Internal Behavior
      BrightnessController -->|Calculate New Brightness| calculateBrightness
      calculateBrightness -->|Adjust Brightness| BrightnessController
     end