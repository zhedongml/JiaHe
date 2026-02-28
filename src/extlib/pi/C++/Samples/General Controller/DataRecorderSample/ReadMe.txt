#########################################################
#
# PI Common C++ Data Recorder sample version 1.0.0
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
This sample connects to a controller using a connection dialog.
Then the data recorder is configured and samples of a step response are captured.
The recorded data is written to std::cout and also to a file, named "test.dat".
The positioner has to be referenced before starting this sample.

#
# Hardware requirements:
#
- controller with data recorder functionality
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API

#
# Unsupported controllers:
#
- C-891
- E-872
