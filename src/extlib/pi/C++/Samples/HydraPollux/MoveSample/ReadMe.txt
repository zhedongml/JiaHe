#########################################################
#
# PI Hydra C++ Move sample version 1.0.0
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
This sample connects to a controller using a connection dialog.
It shows how to determine min/max position limits, and how to do absolute and relative moves.
The positioner has to be referenced before starting this sample, i.e. using the reference sample.

#
# Hardware requirements:
#
- Hydra controller
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
