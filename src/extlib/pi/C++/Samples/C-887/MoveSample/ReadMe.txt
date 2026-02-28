#########################################################
#
# PI C-887 C++ Move sample version 1.0.0
#
# Creation timestamp: 2020-01-15 12:00
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
This sample connects a C-887 controller using a connection dialog.
It shows how to determine min/max position limits, and how to do absolute and relative moves.
The positioner has to be referenced before starting this sample.

#
# Hardware requirements:
#
- C-887
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
