Software Detailed Design
========================

This module is responsible for processing power signals based on key presses.

.. _module-header:

Module Header
-------------

.. code-block:: c

   #include "power_signal_processing.h"
   #include "rte.h"

.. _function-description:

Function Description
--------------------

.. code-block:: c

   void powerSignalProcessing(void)

.. spec:: Read power key press
   :id: SWDD_PSP-001
   :integrity: QM

   The function must check for the press of the power key.

.. spec:: Set power state to ON
   :id: SWDD_PSP-002
   :integrity: B

   If the retrieved power state is POWER_STATE_OFF, the function shall set the power state to POWER_STATE_ON.

.. spec:: Set power state to OFF
   :id: SWDD_PSP-003
   :integrity: C

   If the retrieved power state is not POWER_STATE_OFF, the function shall set the power state to POWER_STATE_OFF.

{% if config.AUTO_OFF %}

.. spec:: Auto off event handling
   :id: SWDD_PSP-004
   :integrity: B

   When no power key is pressed and the auto off state is TRUE, the function shall set the power state to POWER_STATE_OFF to initiate system shutdown.

.. spec:: No action on inactive state
   :id: SWDD_PSP-005
   :integrity: QM

   When no power key is pressed and the auto off state is FALSE, the function shall take no action regarding power state changes.

{% endif %}

Function Flow
-------------

.. mermaid:: 

   graph TD
      Start[Start]
      KeyCheck{Is 'P' key pressed?}
      GetState{Get current power state}
      IsOff{Is state OFF?}
      TurnOn[Set state to ON]
      TurnOff[Set state to OFF]
      {% if config.AUTO_OFF %}AutoOffCheck{Is auto off state TRUE?}
      AutoOffPowerDown[Set state to OFF]{% endif %}
      End[End]

      Start --> KeyCheck
      KeyCheck -->|Yes| GetState
      KeyCheck -->|No| {% if config.AUTO_OFF %}AutoOffCheck{% else %}End{% endif %}
      GetState --> IsOff
      IsOff -->|Yes| TurnOn --> End
      IsOff -->|No| TurnOff --> End
      {% if config.AUTO_OFF %}AutoOffCheck -->|Yes| AutoOffPowerDown --> End
      AutoOffCheck -->|No| End{% endif %}
