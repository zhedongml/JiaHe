#########################################################
#
# PI E-135 C++ Move sample version 1.0.0
#
# Creation timestamp: 2020-01-16 15:16
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
This sample connects to a E-135 controller using a connection dialog. It shows how to execute an open-loop motion on
two channels (channels 1 and 2) using three different methods (POL, SVA and OSM/OSN?). The controller and the positioners
have to be initialized with PIMikroMove before starting this sample.

#
# Hardware requirements:
#
- controller
- compatible positioners

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API

