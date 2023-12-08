Component A
===========


.. spec:: Read interface with pointer argument
   :id: SWDD_COMP_A-010

   Read interface gets the pointer to a variable and update its value.
   We want to write data to the input pointer.

.. spec:: Write interface with pointer argument
   :id: SWDD_COMP_A-011

   Write interface gets the pointer to a variable and update its value.
   We want to check that the write method was called with a certain value.

.. spec:: Read interface with pointer argument and return value
   :id: SWDD_COMP_A-020

   Read interface gets the pointer to a variable and update its value.
   It return zero for success and non-zero for failure.
   We want to write data to the input pointer and define the return value.

.. spec:: Read interface with multiple pointer arguments and return value
   :id: SWDD_COMP_A-030

   Read interface gets multiple pointer arguments and update their value.
   It return zero for success and non-zero for failure.
   We want to write data to all inputs and define the return value.

.. spec:: Read interface with pointer to data structure
   :id: SWDD_COMP_A-040

   Read interface gets the pointer to a data structure and must update the fields inside it.
   We want to put data into all data structure fields.

.. spec:: Write interface with pointer to data structure
   :id: SWDD_COMP_A-050

   Write interface gets the pointer to a data structure will copy the data.
   We want to check if this interface was called with the expected data.

.. spec:: Read interface with array input
   :id: SWDD_COMP_A-060

   Read interface gets an array as input and updates all values.
   We want to put data into all elements in an array.

.. spec:: Write interface with array input
   :id: SWDD_COMP_A-070

   Write interface gets an array as input and copy all its data.
   We want to check if this interface was called with the expected data.
   Check all elements in the array for values.


.. spec:: Use different test data (parametrized test cases)
   :id: SWDD_COMP_A-080

   It is common to test a functionality with different input/output data
   and have the same template for the test case.
   We want to have the data for testing separate from the test case and
   be able to test our function under test for every set in the test data.
