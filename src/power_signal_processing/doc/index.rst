Power Signal Processing
=========================

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

.. c:function:: void powerSignalProcessing(void)


.. spec:: Read power key press
   :id: SWDD_PSP-001
   :integrity: QM

    The function must check for the press of the "P" key.

.. spec:: Set power state to ON
   :id: SWDD_PSP-002
   :integrity: B

    If the retrieved power state is POWER_STATE_OFF, the function shall set the power state to POWER_STATE_ON.

.. spec:: Set power state to OFF
   :id: SWDD_PSP-003
   :integrity: C

    If the retrieved power state is not POWER_STATE_OFF, the function shall set the power state to POWER_STATE_OFF.



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
      End[End]

      Start --> KeyCheck
      KeyCheck -->|Yes| GetState
      KeyCheck -->|No| End
      GetState --> IsOff
      IsOff -->|Yes| TurnOn --> End
      IsOff -->|No| TurnOff --> End
