/*
* Test Project for State Estimators
* Avionics14
* 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
*/

#include "stdlib.h"
#include <stdio.h>
#include <FTEngine.h>
#include "MainScene.h"

#include <Data/SerialPort.h>

int main(void)
{
	SerialPort * port = new SerialPort("\\\\.\\COM3", 9600);
	if (!port->portIsOpen())
		FTLOG("Failed to open port!");
	
	uint8_t buff[1025];
	

	


	port->release();

	int ret = -1;
	if (FTEngine::setup()) {
		FTScene *scene = new MainScene();
		FTDirector::getSharedInstance()->setCurrentScene(scene);
		scene->release();

		ret = FTDirector::getSharedInstance()->run();
		FTEngine::cleanup();
	}
	return ret;
}