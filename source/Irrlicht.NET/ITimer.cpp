// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ITimer.h"
#include "NativeConverter.h"

namespace Irrlicht
{
	ITimer::ITimer(irr::ITimer* tmr)
		: Timer(tmr)
	{
		Timer->grab();
	}

	ITimer::~ITimer()
	{
		Timer->drop();
	}

	irr::ITimer* ITimer::get_NativeTimer()
	{
		return Timer;
	}

	unsigned int ITimer::get_RealTime()
	{
		return Timer->getRealTime();
	}

	unsigned int ITimer::get_Time()
	{
		return Timer->getTime();
	}

	void ITimer::set_Time(unsigned int time)
	{
		Timer->setTime(time);
	}

	void ITimer::Stop()
	{
		Timer->stop();
	}

	void ITimer::Start()
	{
		Timer->start();
	}

	void ITimer::set_Speed(float speedFactor)
	{
		Timer->setSpeed(speedFactor);
	}

	float ITimer::get_Speed()
	{
		return Timer->getSpeed();
	}

	bool ITimer::get_IsStopped()
	{
		return Timer->isStopped();
	}

	void ITimer::Tick()
	{
		Timer->tick();
	}

}