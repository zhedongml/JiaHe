#########################################################
#
# PI Hydra C++ TwoAxes sample version 1.0.0
#
# Creation timestamp: 2020-01-16 09:30
#
#########################################################
#                  IMPORTANT NOTICE                     #
#########################################################
#
# Please initialize your controller and your positioner with
# PIMikroMove before running any sample.
# See Linux manual for initialisation on Linux.
#
#########################################################
#                   MOTION WARNING                      #
#########################################################

#
# Description:
#
This sample connects to a two-axis controller.
Afterwards, it moves the two axes forward and backward repeatedly.
Both axes have to be referenced to use this sample, i.e. by using the reference sample.

#
# Hardware requirements:
#
- Hydra two-axis controller
- two positioners

#
# Software requirements:
#
- linked libaries, DLL and headers at ..\..\..\API
