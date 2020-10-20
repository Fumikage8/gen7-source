#if !defined( TACO_NATIVEMATH_UTIL_H_INCLUDED )
#define TACO_NATIVEMATH_UTIL_H_INCLUDED

#include "qt/Shader/NativeMath/Math.h"

namespace gfl { namespace maya { namespace Math {

class Util
{
public:
  static inline float DegreeToPai( float kakudo )
  {
	  float 			pai = 3.1415926535898f / 180.0f;

	  while ( kakudo >  180.0f ){
		  kakudo -= 360.0f;
	  }
  	
	  while ( kakudo < -180.0f ){
		  kakudo += 360.0f;
	  }

	  return ( kakudo * pai );
  }

	static inline bool IsPow2(unsigned int value)
	{
		return ((value & (value - 1)) == 0);
	}

	static bool RayPickPolygon( Vector *pOutPos, const Vector &ray_pos, const Vector &ray_vec, Vector vertex0, Vector vertex1, Vector vertex2 );
	static bool CheckInnerPoint( const Vector &pos0, const Vector &pos1, const Vector &pos2, const Vector &point, const Vector &normal );
	static bool RayPick( Vector *cross, const Vector &pos1, const Vector &vec1, const Vector &pos2, const Vector &vec2 );

};



}}}

#endif