Software Detailed Design
========================

.. contents:: Table of Contents
    :depth: 2

Introduction
------------

The Brightness Controller component is responsible for adjusting the brightness of a light based on a knob input.
This document outlines the design considerations and the high-level structure of the module.

Design Considerations
---------------------

.. spec:: Brightness Value
   :id: SWDD_BC-001
   :integrity: B

    The brightness of the light is represented by an integer value from 0 up to 255.

.. spec::  Brightness Adjustment
    :id: SWDD_BC-002
    :integrity: B

    The brightness of the light is adjustable by an external input (main knob value in percentage).


Interfaces
----------

- ``RteSetBrightnessValue``: Function to update the value of the light brightness.
- ``RteGetMainKnobValue``: Function to get the main knob value to increase or decrease the brightness accordingly.
