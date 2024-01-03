Examplary Usage of GitHub Copilot
=================================

Steps to add two additional colors to the Sleep Light for Customer B by using GitHub Copilot:

* Open guiconfig of variant CustB/Sleep to show the configuration of the light controller.
* Close it.
* Build the product and show that the LED is blue for the Sleep Light for Customer B.
* Open KConfig of light controller and add two additional colors to the list of colors by marking the whole file, hit Ctrl+I and ask:
::

   "Please add two additional colors red and purple to the list of colors."

* Open guiconfig of variant CustB/Sleep again and configure purple as color.
* Open test_light_controller.cc and adapt the tests to support purple as configured color by marking the color config section and asking:
::

 "We have 4 possible configuration values now for the selected code: blue, green, red and purple. Could you adapt this code part to reflect that?"

* Run the tests and see that they fail.
* Open light_controller.c and adapt the code to support purple as configured color by marking the function turnLightOn() and asking:
::

 "We have 4 possible configuration values now for the selected code: blue, green, red and purple. Could you adapt this code part to reflect that?"

* Run the tests and see that they pass. Actually they might fail due to different definition of the colors in the test and the code.
* Adapt the test to use the same definition as the code by asking:
::

 "In the productive code we used half the brightness value for red. I would like to use this color definition here, too."

* Build the product and show that the LED is purple for the Sleep Light for Customer B.
* Done.
