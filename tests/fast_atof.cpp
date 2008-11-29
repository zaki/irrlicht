// Test the accuracy and speed of 

#include "irrlicht.h"
#include <assert.h>
#include <float.h>
#include <limits.h>

using namespace irr;
using namespace core;

static inline u32 old_strtol10(const char* in, const char** out=0)
{
	u32 value = 0;

	while ( ( *in >= '0') && ( *in <= '9' ))
	{
		value = ( value * 10 ) + ( *in - '0' );
		++in;
	}
	if (out)
		*out = in;
	return value;
}

//! Provides a fast function for converting a string into a float,
//! about 6 times faster than atof in win32.
// If you find any bugs, please send them to me, niko (at) irrlicht3d.org.
static inline const char* old_fast_atof_move( const char* c, float& out)
{
	bool inv = false;
	const char *t;
	float f;

	if (*c=='-')
	{
		++c;
		inv = true;
	}

	//f = (float)strtol(c, &t, 10);
	f = (float) old_strtol10 ( c, &c );

	if (*c == '.')
	{
		++c;

		//float pl = (float)strtol(c, &t, 10);
		float pl = (float) old_strtol10 ( c, &t );
		pl *= fast_atof_table[t-c];

		f += pl;

		c = t;

		if (*c == 'e')
		{
			++c;
			//float exp = (float)strtol(c, &t, 10);
			bool einv = (*c=='-');
			if (einv)
				++c;

			float exp = (float)old_strtol10(c, &c);
			if (einv)
				exp *= -1.0f;

			f *= (float)pow(10.0f, exp);
		}
	}

	if (inv)
		f *= -1.0f;

	out = f;
	return c;
}

static inline float old_fast_atof(const char* c)
{
	float ret;
	old_fast_atof_move(c, ret);
	return ret;
}


static bool testCalculation(const char * valueString)
{
	const f32 newFastValue = fast_atof(valueString);
	const f32 oldFastValue = old_fast_atof(valueString);
	const f32 atofValue = (f32)atof(valueString);

	(void)printf("\n String '%s'\n New fast %.40f\n Old fast %.40f\n     atof %.40f\n",
		valueString, newFastValue, oldFastValue, atofValue);

	bool accurate = fabs(newFastValue - atofValue) <= fabs(oldFastValue - atofValue);

	if(!accurate)
		(void)printf("*** ERROR - less accurate than old method ***\n\n");

	return accurate;
}

bool fast_atof(void)
{
	bool accurate = true;

	accurate &= testCalculation("340282346638528859811704183484516925440.000000");
	accurate &= testCalculation("3.402823466e+38F");
	accurate &= testCalculation("3402823466e+29F");
	accurate &= testCalculation("-340282346638528859811704183484516925440.000000");
	accurate &= testCalculation("-3.402823466e+38F");
	accurate &= testCalculation("-3402823466e+29F");
	accurate &= testCalculation("34028234663852885981170418348451692544.000000");
	accurate &= testCalculation("3.402823466e+37F");
	accurate &= testCalculation("3402823466e+28F");
	accurate &= testCalculation("-34028234663852885981170418348451692544.000000");
	accurate &= testCalculation("-3.402823466e+37F");
	accurate &= testCalculation("-3402823466e+28F");
	accurate &= testCalculation(".00234567");
	accurate &= testCalculation("-.00234567");
	accurate &= testCalculation("0.00234567");
	accurate &= testCalculation("-0.00234567");
	accurate &= testCalculation("1.175494351e-38F");
	accurate &= testCalculation("1175494351e-47F");
	accurate &= testCalculation("1.175494351e-37F");
	accurate &= testCalculation("1.175494351e-36F");
	accurate &= testCalculation("-1.175494351e-36F");
	accurate &= testCalculation("123456.789");
	accurate &= testCalculation("-123456.789");
	accurate &= testCalculation("0000123456.789");
	accurate &= testCalculation("-0000123456.789");

	if(!accurate)
	{
		(void)printf("Calculation is not accurate, so the speed is irrelevant\n");
		return false;
	}

	IrrlichtDevice* device = createDevice(video::EDT_NULL);
	if (!device)
		return false;
	ITimer* timer = device->getTimer();

	enum { ITERATIONS = 100000 };
	int i;
	
	f32 value;
	u32 then = timer->getRealTime();
	for(i = 0; i < ITERATIONS; ++i)
		value = (f32)atof("-340282346638528859811704183484516925440.000000");

	const u32 atofTime = timer->getRealTime() - then;

	then += atofTime;
	for(i = 0; i < ITERATIONS; ++i)
		value = fast_atof("-340282346638528859811704183484516925440.000000");
	const u32 fastAtofTime = timer->getRealTime() - then;

	then += fastAtofTime;
	for(i = 0; i < ITERATIONS; ++i)
		value = old_fast_atof("-340282346638528859811704183484516925440.000000");
	const u32 oldFastAtofTime = timer->getRealTime() - then;

	(void)printf("         atof time = %d\n    fast_atof Time = %d\nold fast_atof time = %d\n",
		atofTime, fastAtofTime, oldFastAtofTime);

	device->drop();
	if(fastAtofTime > atofTime)
	{
		(void)printf("The fast method is slower than atof()\n");
		return false;
	}

	return true;
}

