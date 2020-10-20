//======================================================================
/**
 * @file ColliderBase.h
 * @date 2016/10/14 16:31:43
 * @author fang_yicheng
 * @brief ベースコライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COLLIDER_BASE_H_INCLUDED__
#define __COLLIDER_BASE_H_INCLUDED__
#pragma once

#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
class ModelBase;
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

// 前方宣言
class ColliderSphere;
class ColliderCylinder;

class ColliderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ColliderBase);

public:

  enum TYPE
  {
    TYPE_NONE   = -1,
    TYPE_SPHERE,    // 球体
    TYPE_CYLINDER,  // シリンダー(円柱体)
  };

  struct CollisionInfo
  {
    bool                  m_isCollided;     // 衝突したか
    Model::ModelBase*     m_pTargetModel;   // 衝突対象
    gfl2::math::Vector3   m_normal;         // 衝突点の法線
    gfl2::math::Vector3   m_solveVector;    // 衝突解消の押し返し量

    CollisionInfo()
    {
      Initialize();
    }

    void Initialize()
    {
      m_isCollided    = false;
      m_pTargetModel  = NULL;
      m_normal        = gfl2::math::Vector3::GetZero();
      m_solveVector   = gfl2::math::Vector3::GetZero();
    }
  };

protected:

  // 線分の定義
  struct Line
  {
    gfl2::math::Vector3 m_start;
    gfl2::math::Vector3 m_end;

    Line(const gfl2::math::Vector3 &start, const gfl2::math::Vector3 &end)
    {
      m_start = start;
      m_end   = end;
    }

    gfl2::math::Vector3 LineVector() const
    {
      return m_end - m_start;
    }

    gfl2::math::Vector3 Direction() const
    {
      return LineVector().Normalize();
    }

    f32 Length() const
    {
      return LineVector().Length();
    }
  };

public:

  ColliderBase();
  virtual ~ColliderBase();

  void Initialize(Model::ModelBase* pBaseModel, s32 jointIndex = -1);

  TYPE GetType();

  void PreSetup();
  gfl2::math::Vector3 GetPosition();
  Model::ModelBase* GetModel();

  bool TestCollider(ColliderBase *pTargetCollider);

  CollisionInfo* GetCollisionInfo();

protected:

  virtual bool TestSphereCollider(ColliderSphere *pTargetCollider)      { return false; };
  virtual bool TestCylinderCollider(ColliderCylinder *pTargetCollider)  { return false; };

  gfl2::math::Vector3 CalcClosestPointOnLine(const gfl2::math::Vector3 &point, const Line &line, f32 *out_pjLength = NULL);

protected:

  TYPE                    m_type;
  CollisionInfo           m_collisionInfo;
  gfl2::math::Vector3     m_prePosition;

  Model::ModelBase*       m_pBaseModel;
  s32                     m_jointIndex;

};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
GFL_NAMESPACE_END(Collision)

#endif // __COLLIDER_BASE_H_INCLUDED__
