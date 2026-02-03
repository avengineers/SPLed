Software Detailed Design
========================

.. toctree::
   :maxdepth: 2
   :caption: Table of Contents

Introduction
------------

The Auto Off Controller is responsible for monitoring user activity and automatically triggering system shutdown after a configurable period of inactivity. This component helps conserve power by detecting when no user input has occurred for a specified duration.

Design Considerations
---------------------

.. spec:: Timer Resolution
   :id: SWDD_AO-100

   The auto off timer operates with millisecond precision, converting the configured period from seconds to milliseconds for internal timing calculations.

.. spec:: Inactivity Detection
   :id: SWDD_AO-101

   The Auto Off Controller monitors three types of user input to determine system activity:
   - Power button presses (POWER_BUTTON_KEY)
   - Up control key presses (CONTROL_KEY_UP)
   - Down control key presses (CONTROL_KEY_DOWN)

   Any of these inputs will reset the inactivity timer.

{% if config.AUTO_OFF %}

.. spec:: Configurable Timeout Period
   :id: SWDD_AO-102

   The auto off timeout period is configurable through CONFIG_AUTO_OFF_PERIOD_SECONDS, with a valid range of 5 to 7200 seconds (5 seconds to 2 hours).

{% endif %}

.. spec:: Timer Countdown Behavior
   :id: SWDD_AO-103

   The internal timer decrements by CONFIG_OS_TASK_PERIOD (typically 10ms) on each execution cycle. When the timer reaches zero or below, the auto off state is triggered.

.. spec:: Timer Reset Logic
   :id: SWDD_AO-104

   When user activity is detected, the timer is reset to (CONFIG_AUTO_OFF_PERIOD_SECONDS * 1000) + CONFIG_OS_TASK_PERIOD milliseconds. The additional task period ensures proper timing alignment with the execution cycle.

.. spec:: Auto Off State Management
   :id: SWDD_AO-105

   The component manages two distinct states:
   - Active state (FALSE): System is active, timer is counting down
   - Auto off state (TRUE): Timeout reached, system should enter low power mode

Interfaces
----------

.. spec:: Initialization Function
   :id: SWDD_AO-200

   The Auto Off Controller provides an initialization function `autoOffInit()` that resets the internal timer to zero, establishing a clean initial state.

.. spec:: Main Runnable
   :id: SWDD_AO-201

   The Auto Off Controller shall be called by its runnable `autoOff()`. This function executes the main auto off logic including activity detection, timer management, and state updates.

.. spec:: Key Activity Monitoring
   :id: SWDD_AO-202

   The Auto Off Controller shall use the RTE interface `RteIsKeyPressed()` to monitor user activity from multiple input sources for timer reset purposes:
   - Power button activity via `RteIsKeyPressed(POWER_BUTTON_KEY)`
   - Up control key activity via `RteIsKeyPressed(CONTROL_KEY_UP)`
   - Down control key activity via `RteIsKeyPressed(CONTROL_KEY_DOWN)`

   Any activity detected from these monitored keys will reset the inactivity timer.

{% if config.AUTO_OFF %}

.. spec:: Auto Off State Output
   :id: SWDD_AO-205

   The Auto Off Controller shall use the RTE interface `RteSetAutoOffState()` to communicate the current auto off state to other system components. The state is set to FALSE when the system is active and TRUE when the timeout period has elapsed.

{% endif %}

Timing Behavior
---------------

.. spec:: Execution Frequency
   :id: SWDD_AO-300

   The auto off function executes every CONFIG_OS_TASK_PERIOD milliseconds (typically 10ms), providing consistent timer resolution and responsive key detection.

.. spec:: Timer Calculation
   :id: SWDD_AO-301

   The internal timer calculation follows the formula:
   - Reset value: (CONFIG_AUTO_OFF_PERIOD_SECONDS * 1000) + CONFIG_OS_TASK_PERIOD
   - Decrement value: CONFIG_OS_TASK_PERIOD per execution cycle
   - Trigger threshold: When timer ≤ CONFIG_OS_TASK_PERIOD

.. spec:: Activity Response Time
   :id: SWDD_AO-302

   Key press detection and timer reset occur within one execution cycle (CONFIG_OS_TASK_PERIOD), ensuring immediate response to user activity.
