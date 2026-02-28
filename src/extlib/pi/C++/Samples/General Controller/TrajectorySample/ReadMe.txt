#########################################################
#
# PI Common C++ Trajectory sample version 1.0.0
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
Then a trajectory is set up, transferred to the controller and started.
The positioner has to be intialized and referenced before starting this sample.

#
# Hardware requirements:
#
- controller supporting trajectories for motion patterns, e.g. C-867.2U2
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API

#
# Unsupported controllers:
#
- E-872