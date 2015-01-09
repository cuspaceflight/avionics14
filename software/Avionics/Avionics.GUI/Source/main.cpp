/*
* Test Project for State Estimators
* Avionics14
* 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
*/

#include "stdlib.h"
#include <stdio.h>
#include <FTEngine.h>
#include "MainScene.h"

int main(void)
{
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