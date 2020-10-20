#ifndef GFLIB2_MATH_SRT_H_INCLUDED
#define GFLIB2_MATH_SRT_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_Assert.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>

#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Matrix44.h>
#include <math/include/gfl2_Quaternion.h>

namespace gfl2 { namespace math {

/**
 * @brief スケール、回転、平行移動管理
 */
class SRT
{
public:

  /**
   * @brief コンストラクタ
   */
  SRT() :
    scale(1,1,1),
    rotation(0,0,0,1),
    translate(0,0,0)
  {
  }

  /**
   * @brief 値指定コンストラクタ
   */
  SRT(const math::Vector3& scale, const math::Quaternion& rotation, const math::Vector3& translate)
  {
    this->scale = scale;
    this->rotation = rotation;
    this->translate = translate;
  }

  /**
   * @brief 値指定コンストラクタ
   */
  SRT(const f32 scale[3], const f32 rotate_xyz[3], const f32 translate[3])
  {
    this->scale.Set(scale[0], scale[1], scale[2]);
    this->rotation.MatrixToQuaternion(math::Matrix34::GetRotationZYX(rotate_xyz[2], rotate_xyz[1], rotate_xyz[0])); // ZYX固定
    this->translate.Set(translate[0], translate[1], translate[2]);
  }

  /**
   * @brief 球面線形補間
   *
   * @param srt 補間先の値
   * @param t 補間時間 0.0f ~ 1.0f
   */
  void Slerp(const SRT& srt, f32 t)
  {
    scale.Lerp(srt.scale, t);
    rotation.Slerp(srt.rotation, t);
    translate.Lerp(srt.translate, t);
  }

  /**
   * @brief 行列へ変換
   */
  inline math::Matrix34 ToMatrix() const
  {
    math::Matrix34 result;
    ToMatrix(result);
    return result;
  }

  /**
   * @brief 行列へ変換
   */
  inline void ToMatrix(math::Matrix34& result) const
  {
    //math::Matrix34::GetRotationZYX(result, this->rotation.z, this->rotation.y, this->rotation.x);
    this->rotation.QuaternionToMatrix(result);

    result[0][3] = this->translate.x;
    result[1][3] = this->translate.y;
    result[2][3] = this->translate.z;

    result[0][0] *= this->scale.x;
    result[1][0] *= this->scale.x;
    result[2][0] *= this->scale.x;

    result[0][1] *= this->scale.y;
    result[1][1] *= this->scale.y;
    result[2][1] *= this->scale.y;

    result[0][2] *= this->scale.z;
    result[1][2] *= this->scale.z;
    result[2][2] *= this->scale.z;
  }

  /**
   * @brief 親のスケールを引き継がずに行列へ変換：Mayaの行列計算準拠
   *
   * @param parentScale 親のスケール
   */
  inline math::Matrix34 ToMatrixNonInheritanceScale(const math::Vector3& parentScale) const
  {
    math::Matrix34 result;
    ToMatrixNonInheritanceScale(result, parentScale);
    return result;
  }
  /**
   * @brief 親のスケールを引き継がずに行列へ変換：Mayaの行列計算準拠
   *
   * @param parentScale 親のスケール
   */
  inline void ToMatrixNonInheritanceScale(math::Matrix34& result, const math::Vector3& parentScale) const
  {
    GFL_ASSERT(parentScale.x != 0.0f);
    GFL_ASSERT(parentScale.y != 0.0f);
    GFL_ASSERT(parentScale.z != 0.0f);

    f32 invParentScaleX = 1.0f / parentScale.x;
    f32 invParentScaleY = 1.0f / parentScale.y;
    f32 invParentScaleZ = 1.0f / parentScale.z;

    math::Matrix34 tmp;
    tmp[0][0] = invParentScaleX;
    tmp[1][1] = invParentScaleY;
    tmp[2][2] = invParentScaleZ;

    tmp[0][3] = this->translate.x;
    tmp[1][3] = this->translate.y;
    tmp[2][3] = this->translate.z;

    math::Matrix34 tmp2;
    this->rotation.QuaternionToMatrix(tmp2);
    result = tmp * tmp2;

    result[0][0] *= this->scale.x;
    result[1][0] *= this->scale.x;
    result[2][0] *= this->scale.x;

    result[0][1] *= this->scale.y;
    result[1][1] *= this->scale.y;
    result[2][1] *= this->scale.y;

    result[0][2] *= this->scale.z;
    result[1][2] *= this->scale.z;
    result[2][2] *= this->scale.z;
  }

  /**
   * @note 左辺のscaleが均一でないときの結果は保証されません
   */
  inline SRT operator*(const SRT& rhs) const
  {
    return SRT(Vector3(this->scale.x * rhs.scale.x, this->scale.y * rhs.scale.y, this->scale.z * rhs.scale.z),
               this->rotation * rhs.rotation,
               this->rotation.TransformWithScale(rhs.translate, this->scale) + this->translate
               );
  }


  math::Vector3 translate;         ///< 平行移動成分
  math::Vector3 scale;             ///< スケール成分
  math::Quaternion rotation;       ///< 回転成分
};

}}


#endif