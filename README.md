433.92-Raspberry-Pi
===================

This is the Windows CLI that can be used to control 433.92Mhz devices. It communicates with the 433-daemon.
This is only a proof of concept because i'm not a Windows C programmer. If people want to rewrite it or start all over, feel free. 
The purpose of this program is just to show what the 433-daemon (and it's JSON api) is capable of.

Currently, only the `433-control` and the `433-send` work, however, buggy. The `433-receive` doesn't work (yet).
If any of the CLI arguments are incorrect the program will hang.
So, check what you need to do use the programs with the `-h` argument.