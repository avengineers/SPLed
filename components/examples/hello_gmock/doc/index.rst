Software Detailed Design
========================

Define the behavior of an input interface
-----------------------------------------

In this chapter we will see some examples of how to use Google Mock to define the behavior of an input interface (function).

Get interface
-------------

.. spec:: Get interface
   :id: SWDD_COMP_A-010

   Manipulate the get interface to return a value.

Get by pointer
--------------

.. spec:: Get by pointer
   :id: SWDD_COMP_A-020

   Manipulate the interface to put a value into the provided pointer.

Get by pointer and return value
-------------------------------

.. spec:: Get by pointer and return value
   :id: SWDD_COMP_A-030

   Manipulate the interface to return a value while also putting a value into the provided pointer.

Init data structure
-------------------

.. spec:: Init data structure
   :id: SWDD_COMP_A-035

   Manipulate the interface to initialize the data structure pointed to by the provided pointer.

Get data structure by pointer
-----------------------------

.. spec:: Get data structure by pointer
   :id: SWDD_COMP_A-040

   Manipulate the interface to put a value into the provided pointer to a data structure.

Get data structure array
------------------------

.. spec:: Get data structure array
   :id: SWDD_COMP_A-050

   Manipulate the interface to put a value into the provided pointer to an array of data structures.

Verify that the output interface is called properly
---------------------------------------------------

In this chapter we will see some examples of how to use Google Mock to verify that the output interface (function) is called properly.

Set interface
-------------

.. spec:: Set interface
   :id: SWDD_COMP_A-110

   Verify that the set interface is called with the expected value.

Set by pointer
--------------

.. spec:: Set by pointer
   :id: SWDD_COMP_A-120

   Verify that the set interface is called with the expected value through the pointer.
