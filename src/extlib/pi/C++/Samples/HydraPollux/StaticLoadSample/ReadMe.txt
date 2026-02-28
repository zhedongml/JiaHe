#########################################################
#
# PI Hydra C++ StaticLoad sample version 1.0.0
#
# Creation timestamp: 2020-01-16 16:30
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

#
# Description:
#
This sample shows how to load the PI_GCS2_DLL and its methods statically,
connect to a controller using a connection dialog and read the device identification.
Lib and dll files have to exist in ..\..\..\API.

#
# Hardware requirements:
#
- Hydra controller
- compatible positioner

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
