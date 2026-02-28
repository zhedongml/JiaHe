#########################################################
#
# PI C-887 C++ Data recorder sample version 1.0.0
#
# Creation timestamp: 2020-01-15 10:00
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
This sample connects to a C-887 controller using a connection dialog.
Then the data recorder is configured and samples of a step are captured.
They are written to std::cout and also to a file, named "test.dat".
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
