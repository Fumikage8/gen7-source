#include <clr/include/math/gfl2_MathAABB_CLR.h>

namespace gfl2 { namespace clr { namespace math {

AABB::AABB()
{
  m_pNative = new gfl2::math::AABB();
}

AABB::AABB(AABB^ other)
{
  m_pNative = new gfl2::math::AABB(*other->m_pNative);
}

AABB::AABB(const gfl2::math::AABB& aabb)
{
  m_pNative = new gfl2::math::AABB(aabb);
}

AABB::AABB(Vector^ minValue, Vector^ maxValue)
{
  m_pNative = new gfl2::math::AABB(minValue->GetNativeOldVector(), maxValue->GetNativeOldVector());
}

AABB::!AABB()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = 0;
  }
}

void AABB::SetMin(Vector^ minValue)
{
  m_pNative->SetMin(minValue->GetNativeOldVector());
}

Vector^ AABB::GetMin()
{
  return gcnew Vector(m_pNative->GetMin());
}

void AABB::SetMax(Vector^ minValue)
{
  m_pNative->SetMax(minValue->GetNativeOldVector());
}

Vector^ AABB::GetMax()
{
  return gcnew Vector(m_pNative->GetMax());
}

}}}