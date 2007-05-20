// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

namespace Irrlicht
{
	/// <summary>
	/// Interface for getting and manipulating the virtual time
	/// </summary>
	public __gc class ITimer
	{
	public:

		/// <summary>
		/// You should access the ITimer 
		/// through the IrrlichtDevice::get_Timer() property. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="font">The real, unmanaged C++ timer	</param>
		ITimer(irr::ITimer* timer);

		~ITimer();

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht timer.
		/// Do not use this, only needed by the internal .NET wrapper.
		///</summary>
		irr::ITimer* ITimer::get_NativeTimer();

		/// <summary>
		/// Returns current real time in milliseconds of the system. 
		/// This value does not start with 0 when the application starts.
		/// For example in one implementation the value returned could be the 
		/// amount of milliseconds which have elapsed since the system was started.
		/// </summary>
		__property unsigned int get_RealTime();

		/// <summary>
		/// Sets or returns current virtual time in milliseconds. 
		/// This value starts with 0 and can be manipulated using setTime(), stopTimer(),
		/// startTimer(), etc. This value depends on the set speed of the timer if the timer 
		/// is stopped, etc. If you need the system time, use getRealTime() 
		/// </summary>
		__property unsigned int get_Time();

		/// <summary>
		/// Sets or returns current virtual time in milliseconds. 
		/// This value starts with 0 and can be manipulated using setTime(), stopTimer(),
		/// startTimer(), etc. This value depends on the set speed of the timer if the timer 
		/// is stopped, etc. If you need the system time, use getRealTime() 
		/// </summary>
		__property void set_Time(unsigned int time);

		/// <summary>
		/// Stops the virtual timer. 
		/// The timer is reference counted, which means everything which calls 
		/// stop() will also have to call start(), otherwise the timer may not start/stop
		/// corretly again. 
		/// </summary>
		void Stop();

		/// <summary>
		/// Starts the virtual timer.
		/// The timer is reference counted, which means everything which calls 
		/// stop() will also have to call start(), otherwise the timer may not start/stop
		/// corretly again.
		/// </summary>
		void Start();

		/// <summary>
		/// Sets or returns the speed of the timer
		/// The speed is the factor with which the time is running faster or slower then the
		/// real system time. The default speed value is 1.0.
		/// <summary>
		__property void set_Speed(float speedFactor);

		/// <summary>
		/// Sets or returns the speed of the timer
		/// The speed is the factor with which the time is running faster or slower then the
		/// real system time. The default speed value is 1.0.
		/// <summary>
		__property float get_Speed();

		/// <summary>
		/// Returns if the virtual timer is currently stopped
		/// </summary>
		__property bool get_IsStopped();

		/// <summary>
		/// Advances the virtual time
		/// Makes the virtual timer update the time value based on the real time. This is
		/// called automaticly when calling IrrlichtDevice::run(), but you can call it manually
		/// if you don't use this method. 
		/// <summary>
		void Tick();

	private:

		irr::ITimer* Timer;
	};

}
