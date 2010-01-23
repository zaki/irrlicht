#include "testUtils.h"
#include <irrlicht.h>
#include <assert.h>

using namespace irr;
using namespace core;


// Test the functionality of the Irrlicht timer
bool testTimer(void)
{
	bool success = true;

	IrrlichtDevice* device = createDevice(video::EDT_NULL);
	if (!device)
		return false;
	ITimer* timer = device->getTimer();

	// must be running at start
	success &= !timer->isStopped();

	// starting more often should not stop the timer
	timer->start();
	success &= !timer->isStopped();

	// one stop should not stop the timer because it's started twice now
	timer->stop();
	success &= !timer->isStopped();

	// another stop should really stop it
	timer->stop();
	success &= timer->isStopped();

	// third stop - timer should still be stopped
	timer->stop();
	success &= timer->isStopped();

	// should not start yet
	timer->start();
	success &= timer->isStopped();

	// start again
	timer->start();
	success &= !timer->isStopped();

	return success;
}
