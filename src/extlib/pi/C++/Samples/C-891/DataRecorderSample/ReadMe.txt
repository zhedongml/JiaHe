#########################################################
#
# PI C-891 C++ Data recorder sample version 1.0.0
#
# Creation timestamp: 2020-01-16 10:00
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
This sample connects to a C-891 controller, starts up the connected positioner and references it.
Then the data recorder is configured and samples of absolute and relative moves are captured.
They are written to std::cout and also to a file.

#
# Hardware requirements:
#
- C-891
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
