//======================================================================
/**
 * @file ColliderCylinder.h
 * @date 2016/10/14 16:31:43
 * @author fang_yicheng
 * @brief 球体コライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COLLIDER_CYLINDER_H_INCLUDED__
#define __COLLIDER_CYLINDER_H_INCLUDED__
#pragma once

#include "ColliderBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

class ColliderCylinder : public ColliderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ColliderCylinder);

public:

  ColliderCylinder();
  virtual ~ColliderCylinder();

  f32  GetRadius();
  void SetRadius(f32 radius);

  f32  GetHeight();
  void SetHeight(f32 height);

  Line GetCenterLine();
  Line GetDiameterLine(const gfl2::math::Vector3 &center, const gfl2::math::Vector3 &direction);

protected:

  virtual bool TestSphereCollider(ColliderSphere *pTargetCollider);
  virtual bool TestCylinderCollider(ColliderCylinder *pTargetCollider);

private:

  f32   m_radius;   // 半径
  f32   m_height;   // 高さ

};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
GFL_NAMESPACE_END(Collision)

#endif // __COLLIDER_CYLINDER_H_INCLUDED__
