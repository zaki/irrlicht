#include "testUtils.h"

using namespace irr;
using namespace video;

bool rounding()
{
    SColorf colf(0.003922, 0.007843, 0.011765); // test-values by virion which once failed
    SColor col = colf.toSColor();
    return col.getRed() == 1 && col.getGreen() == 2 && col.getBlue() == 3;
}

//! Test SColor and SColorf
bool color(void)
{
	bool ok = true;

    ok &= rounding();

	return ok;
}
