#ifndef GFLIB2_MATH_CLR_MATHUTIL_CLR_H_INCLUDED
#define GFLIB2_MATH_CLR_MATHUTIL_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_MathCommon.h>

namespace gfl2 { namespace clr { namespace math {



public ref class Util
{
public:
	static float ConvDegToRad( float deg );
	static float ConvRadToDeg( float rad );

	static property float Pai {
      float get() {
				return PI;
      }
   }
};


}}}

#endif