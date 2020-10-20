//======================================================================
/**
 * @file ModelBaseChara.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief ベースプレイヤーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_BASE_CHARA_H_INCLUDED__
#define __MODEL_BASE_CHARA_H_INCLUDED__
#pragma once

#include "ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelBaseChara : public ModelBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelBaseChara);

public:

  enum DIRECTION
  {
    DIR_RIGHT = 0,     // 右手方向
    DIR_UP,            // 頭上方向
    DIR_FRONT,         // 手前方向
  };

private:

  enum LocalRotationAxis
  {
    LRA_X = 0,
    LRA_Y,
    LRA_Z,
    LRA_MAX,
    LRA_MIN = 0
  };

public:

  ModelBaseChara();
  virtual ~ModelBaseChara();

  virtual void Update();

  void ChangeSurfMotion(s32 animeId, float t);

  gfl2::math::Vector3 GetDirection(DIRECTION dir);

  void ChangeTurnOverAngle(f32 angle);

  gfl2::math::Vector3 QuatTransformNormal(const gfl2::math::Quaternion &quat, const gfl2::math::Vector3 &rhs);

  f32 GetRotationX();
  f32 GetRotationY();
  f32 GetRotationZ();

  void ChangeRotationX(f32 angle, f32 changeRate);
  void ChangeRotationY(f32 angle, f32 changeRate);
  void ChangeRotationZ(f32 angle, f32 changeRate);
  void ChangeRoation(f32 angleX, f32 angleY, f32 angleZ, f32 changeRate);

  void ChangeLookDirection(const gfl2::math::Vector3 &vecZX, f32 changeRate, f32 maxAngle = PI);
  void ChangeUpDirection(const gfl2::math::Vector3 &vecYZ, f32 changeRate, f32 maxAngle = PI);
  void ChangeInclination(const gfl2::math::Vector3 &vecYX, f32 changeRate, f32 maxAngle = PI);

protected:

  virtual void UpdateAnimation();
  virtual s32 GetSurfStraightMotionId() { return -1; };

  f32  GetRotation(LocalRotationAxis axis);
  void ChangeRotation(LocalRotationAxis axis, f32 angle, f32 changeRate);
  void ChangeDirection(LocalRotationAxis axis, f32 a, f32 b, f32 changeRate, f32 maxAngle);
  void ApplyRotation();

  gfl2::math::Matrix34 GetRotationMatrix(float rad, gfl2::math::Vector3 &axis);

private:

  gfl2::math::Quaternion m_localRotation[LRA_MAX];  // X軸、Y軸、Z軸それぞれの回転量
  gfl2::math::EulerRotation::RotationOrder m_rotationOrder;

  s32 m_surfMotionIdFrom;
  s32 m_surfMotionIdTo;
  f32 m_surfMotionInter;

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_BASE_CHARA_H_INCLUDED__
