#########################################################
#
# PI E-727 C++ AutoZero sample version 1.0.0
#
# Creation timestamp: 2020-01-16 09:30
#
#########################################################
#                  IMPORTANT NOTICE                     #
#########################################################
#
# Please initialize your controller and your positioner
# before running any sample.
# For the initialization use PIMikroMove on Windows and 
# PITerminal on Linux. See corresponding manual for details.
#
#########################################################
#                   MOTION WARNING                      #
#########################################################

#
# Description:
#
This sample connects to a E-727 controller with a positioner connected
and runs an auto zero procedure to set the zero position of the axis.

#
# Hardware requirements:
#
- E-727
- compatible positioner, e.g. P-733.3CD

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
