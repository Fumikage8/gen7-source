//======================================================================
/**
 * @file ColliderSphere.h
 * @date 2016/10/14 16:31:43
 * @author fang_yicheng
 * @brief 球体コライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COLLIDER_SPHERE_H_INCLUDED__
#define __COLLIDER_SPHERE_H_INCLUDED__
#pragma once

#include "ColliderBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

class ColliderSphere : public ColliderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ColliderSphere);

public:

  ColliderSphere();
  virtual ~ColliderSphere();

  f32  GetRadius();
  void SetRadius(f32 radius);

protected:

  virtual bool TestSphereCollider(ColliderSphere *pTargetCollider);
  virtual bool TestCylinderCollider(ColliderCylinder *pTargetCollider);

private:

  f32   m_radius;   // 半径

};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
GFL_NAMESPACE_END(Collision)

#endif // __COLLIDER_SPHERE_H_INCLUDED__
