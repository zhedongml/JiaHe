#########################################################
#
# PI C-887 C++ Reference sample version 1.0.0
#
# Creation timestamp: 2020-01-15 09:30
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
This sample connects to a C-887 controller using the connection dialog and references the stage.
The controller and the stage has to be initialized with PIMikroMove before starting this sample.

#
# Hardware requirements:
#
- C-887
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
