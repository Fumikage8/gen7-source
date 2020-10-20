#if !defined(GFLIB2_CLR_MATH_MATHAABB_CLR_H_INCLUDED)
#define GFLIB2_CLR_MATH_MATHAABB_CLR_H_INCLUDED
#pragma once

#include <clr/include/math/gfl2_Vector_CLR.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace clr { namespace math {

public ref class AABB
{
public:
  AABB();
  AABB(AABB^ other);
  AABB(const gfl2::math::AABB& aabb);
  AABB(Vector^ minValue, Vector^ maxValue);
  ~AABB() { this->!AABB(); }
  !AABB();
  void SetMin(Vector^ minValue);
  Vector^ GetMin();
  void SetMax(Vector^ maxValue);
  Vector^ GetMax();
  gfl2::math::AABB* GetNative() { return m_pNative; }

private:
  gfl2::math::AABB* m_pNative;
};

}}}

#endif