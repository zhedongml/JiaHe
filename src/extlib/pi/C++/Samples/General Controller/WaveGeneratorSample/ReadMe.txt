#########################################################
#
# PI Common C++ WaveGenerator sample version 1.0.0
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
Then the wave generator is configured and started.
The positioner has to be referenced before starting this sample.

#
# Hardware requirements:
#
- controller with wave generator support, e.g. C-891
- for C-887 and E-727 use specific configuration defined in header file
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API

#
# Unsupported controllers:
#
- E-872