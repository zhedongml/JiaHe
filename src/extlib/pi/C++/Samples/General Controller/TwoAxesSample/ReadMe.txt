 od#########################################################
#
# PI Common C++ TwoAxes sample version 1.0.0
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
This sample connects to a two-axis controller and references both axes.
Afterwards, it moves the two axes forward and backward repeatedly.

#
# Hardware requirements:
#
- two-axis controller
- two positioners or two axis positioner

#
# Software requirements:
#
- linked libaries and headers in ../../../../API respectively

#
# Unsupported controllers:
#
- E-872