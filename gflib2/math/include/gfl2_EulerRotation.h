#ifndef GFLIB2_MATH_EULER_ROTATION_H_INCLUDED
#define GFLIB2_MATH_EULER_ROTATION_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>

#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Quaternion.h>

namespace gfl2 { namespace math {

/**
 * @brief スケール、回転、平行移動管理
 */
class EulerRotation
{
public:

  /**
   * @brief 回転順
   */
  enum RotationOrder
  {
    RotationOrderXYZ,
    RotationOrderXZY,

    RotationOrderYXZ,
    RotationOrderYZX,

    RotationOrderZXY,
    RotationOrderZYX,
  };

public:

  /**
   * @brief コンストラクタ
   */
  EulerRotation() :
    x(0.0f), y(0.0f), z(0.0f), rotationOrder(RotationOrderZYX) // Maya準拠でデフォルトはZYXにしておく
  {
  }

  /**
   * @brief 値指定コンストラクタ
   */
  EulerRotation(f32 x, f32 y, f32 z, RotationOrder rotationOrder) :
    x(x), y(y), z(z), rotationOrder(rotationOrder)
  {
  }

  /**
   * @brief クォータニオン指定コンストラクタ
   */
  EulerRotation(Quaternion quat)
  {
#if 0
    // ZYX固定 90度辺りで安定しない？かも。。
    // 行列を通すので無駄かも
    Matrix34 matrix;
    quat.QuaternionToMatrix(matrix);

    math::Vector3 rot = matrix.GetElemRotationZYX();
    this->x = rot.x;
    this->y = rot.y;
    this->z = rot.z;

    this->rotationOrder = RotationOrderZYX;
#else
    // yaw pitch roll 分解
    this->x = atan2(2 * quat.x * quat.w - 2 * quat.y * quat.z, 1 - 2 * quat.x * quat.x - 2 * quat.z * quat.z);
    this->y = atan2(2 * quat.y * quat.w - 2 * quat.x * quat.z, 1 - 2 * quat.y * quat.y - 2 * quat.z * quat.z);
    this->z = asin(2 * quat.x * quat.y + 2 * quat.z * quat.w);
    this->rotationOrder = RotationOrderYZX;
#endif
  }

  /**
   * @brief 値を設定
   */
  void Set(const gfl2::math::Vector3& rotation, gfl2::math::EulerRotation::RotationOrder rotationOrder)
  {
    this->x = rotation.x;
    this->y = rotation.y;
    this->z = rotation.z;
    this->rotationOrder = rotationOrder;
  }

  /**
   * @brief 行列へ変換
   */
  gfl2::math::Matrix34 ToMatrix() const
  {
    static gfl2::math::Matrix34 (*s_ToMatrixList[])(f32 x, f32 y, f32 z) =
    {
      &GetMatrixXYZ, // RotationOrderXYZ,
      &GetMatrixXZY, // RotationOrderXZY,

      &GetMatrixYXZ, // RotationOrderYXZ,
      &GetMatrixYZX, // RotationOrderYZX,

      &GetMatrixZXY, // RotationOrderZXY,
      &GetMatrixZYX, // RotationOrderZYX,
    };

    return s_ToMatrixList[this->rotationOrder](this->x, this->y, this->z);
  }

  /**
   * @brief クォータニオンへ変換
   */
  gfl2::math::Quaternion ToQuaternion() const
  {
    gfl2::math::Quaternion quat;
    quat.MatrixToQuaternion(this->ToMatrix());

    return quat;
  }

  static gfl2::math::Matrix34 GetMatrixXYZ(f32 x, f32 y, f32 z)
  {
    // @attention 最適化
    return
      math::Matrix34::GetRotationX(x) *
      math::Matrix34::GetRotationY(y) *
      math::Matrix34::GetRotationZ(z);
  }
  static gfl2::math::Matrix34 GetMatrixXZY(f32 x, f32 y, f32 z)
  {
    // @attention 最適化
    return
      math::Matrix34::GetRotationX(x) *
      math::Matrix34::GetRotationZ(z) *
      math::Matrix34::GetRotationY(y);
  }

  static gfl2::math::Matrix34 GetMatrixYXZ(f32 x, f32 y, f32 z)
  {
    // @attention 最適化
    return
      math::Matrix34::GetRotationY(y) *
      math::Matrix34::GetRotationX(x) *
      math::Matrix34::GetRotationZ(z);
  }
  static gfl2::math::Matrix34 GetMatrixYZX(f32 x, f32 y, f32 z)
  {
    // @attention 最適化
    return
      math::Matrix34::GetRotationY(y) *
      math::Matrix34::GetRotationZ(z) *
      math::Matrix34::GetRotationX(x);
  }

  static gfl2::math::Matrix34 GetMatrixZXY(f32 x, f32 y, f32 z)
  {
    // @attention 最適化
    return
      math::Matrix34::GetRotationZ(z) *
      math::Matrix34::GetRotationX(x) *
      math::Matrix34::GetRotationY(y);
  }
  static gfl2::math::Matrix34 GetMatrixZYX(f32 x, f32 y, f32 z)
  {
    return math::Matrix34::GetRotationZYX(z, y, x);
  }

public:

  f32 x;                        ///< X軸回転量：ラジアン
  f32 y;                        ///< Y軸回転量：ラジアン
  f32 z;                        ///< Z軸回転量：ラジアン
  RotationOrder rotationOrder;  ///< 回転順
};

}}


#endif