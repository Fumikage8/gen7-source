#include <clr/include/math/gfl2_MathUtil_CLR.h>

namespace gfl2 { namespace clr { namespace math {

float Util::ConvDegToRad( float deg )
{
	return deg * PI / 180.0f;
}

float Util::ConvRadToDeg( float rad )
{
	return rad * 180.0f / PI;
}

}}}