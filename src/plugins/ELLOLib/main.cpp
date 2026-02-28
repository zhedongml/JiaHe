#include "ELLOLib.h"
#include <msclr/gcroot.h>
#include <msclr/marshal_cppstd.h>
#include <windows.h>

void main()
{
	ELLOMOTOR motor1, motor2;
	Result res = motor1.Connect("COM12");
	//bool res1 = motor2.Connect("COM13");
	int pos1 = motor1.GetPosition();
	//int pos2 = motor2.GetPosition();
	return;

	motor1.SetSpeed(85);
	motor2.SetSpeed(85);

	motor1.MoveAbs(250000);
	motor2.MoveAbs(100000);
	int pos3 = motor1.GetPosition();
	int pos4 = motor2.GetPosition();
	return;

	motor1.Homing();
	motor2.Homing();
}