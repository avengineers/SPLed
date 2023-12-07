Component A
===========


.. spec:: Read interface with pointer argument
   :id: SWDD_COMP_A-001

   Read interface gets the pointer to a variable and update its value.
   We want to write data to the input pointer.

.. spec:: Read interface with pointer argument and return value
   :id: SWDD_COMP_A-002

   Read interface gets the pointer to a variable and update its value.
   It return zero for success and non-zero for failure.
   We want to write data to the input pointer and define the return value.

.. spec:: Read interface with multiple pointer arguments and return value
   :id: SWDD_COMP_A-003

   Read interface gets multiple pointer arguments and update their value.
   It return zero for success and non-zero for failure.
   We want to write data to all inputs and define the return value.

.. spec:: Read interface with pointer to data structure
   :id: SWDD_COMP_A-004

   Read interface gets the pointer to a data structure and must update the fields inside it.
   We want to put data into all data structure fields.

.. spec:: Write interface with pointer to data structure
   :id: SWDD_COMP_A-005

   Write interface gets the pointer to a data structure will copy the data.
   We want to check if this interface was called with the expected data.

.. spec:: Read interface with array input
   :id: SWDD_COMP_A-006

   Read interface gets an array as input and updates all values.
   We want to put data into all elements in an array.

.. spec:: Write interface with array input
   :id: SWDD_COMP_A-007

   Write interface gets an array as input and copy all its data.
   We want to check if this interface was called with the expected data.
   Check all elements in the array for values.
