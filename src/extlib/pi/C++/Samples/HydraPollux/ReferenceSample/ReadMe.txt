#########################################################
#
# PI Hydra C++ Reference sample version 1.0.0
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
This sample connects to a controller using a connection dialog, and references the positioner.
The controller and positioner have to be initialized with the correct configuration, i.e using PIMikroMove, before starting this sample.

#
# Hardware requirements:
#
- Hydra controller
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
