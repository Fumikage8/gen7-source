
//=============================================================================
/**
 * @file   gfl_MathPlane.h
 * @brief  ���ʐ���N���X�w�b�_
 * @author �r�c �N��
 * @date   2012.08.25
 */
//=============================================================================

#ifndef __GFL_MATH_PLANE_H__
#define __GFL_MATH_PLANE_H__
#pragma once

namespace gfl { namespace math
{
  //-----------------------------------------------------------------------------
  /**
   * @brief ���ʐ���N���X
   */
  //-----------------------------------------------------------------------------
  class Plane
  {
  public:
    VEC4 abcd; ///< ax + by + cz + d = 0

  public:
    Plane() { return ; } ///< �R���X�g���N�^�P
    Plane(const VEC4& __abcd__) : abcd(__abcd__) { return ; } ///< �R���X�g���N�^�Q
    Plane(const float a, const float b, const float c, const float d) : abcd(a, b, c, d) { return ; } ///< �R���X�g���N�^�R
    Plane(const VEC3& normVec, const VEC3& posi) { Set(normVec, posi); } ///< �R���X�g���N�^�S

    inline void Set(const VEC4& __abcd__) { abcd = __abcd__; }
    inline void Set(const float a, const float b, const float c, const float d) { abcd = VEC4(a, b, c, d); } ///< �l�ݒ�P
    //-----------------------------------------------------------------------------
    /**
     * @brief �l�ݒ�Q
     *
     * @param normVec �@���x�N�^(���K������Ă���K�v�͂Ȃ�)
     * @param posi    ���ʂɊ܂܂��C�ӂ̈ʒu
     */
    //-----------------------------------------------------------------------------
    inline void Set(const VEC3& normVec, const VEC3& posi) { abcd = VEC4(normVec.x, normVec.y, normVec.z, -VEC3Dot(&normVec, &posi)); }

    inline void AddD(const float d) { abcd.w += d; } ///< D�l�����Z����
    inline void Normalize() ///< ���K������
    {
      const float len = VEC3Len(reinterpret_cast<VEC3* >(&abcd));
      VEC4Scale(&abcd, &abcd, 1.0f/len);
    }

    inline const gfl::math::VEC4& GetPlaneVector() const { return abcd; } ///< ���ʃx�N�^���擾����
    inline void GetNormalVector(VEC3* v) const { v->x = abcd.x, v->y = abcd.y, v->z = abcd.z; } ///< �@���x�N�^���擾����(���K������Ă���ۏ؂͂Ȃ�)

    /// ���͈ʒu���畽�ʂ܂ł̍ŒZ�������擾����
    inline float GetDistance(const VEC3& posi) const { return VEC3Dot(reinterpret_cast<const VEC3* >(&abcd), &posi) + abcd.w; }

    //-----------------------------------------------------------------------------
    /**
     * @brief �_�Ƃ̏Փ˔�����s��
     *
     * @param posi    �_�̈ʒu
     * @param epsilon �Փ˂��Ă���Ɣ��f���Ă悢�C�v�V�����l
     *
     * @retval true  �Փ˂��Ă���
     * @retval false �Փ˂��Ă��Ȃ�
     */
    //-----------------------------------------------------------------------------
    inline bool IsHitPoint(const VEC3& posi, const float epsilon = 0.001f) const { return (FAbs(GetDistance(posi)) <= epsilon); }

    //-----------------------------------------------------------------------------
    /**
     * @brief ���Ƃ̏Փ˔�����s��(���F�d�Ȃ��Ă��鎞��false��Ԃ�)
     *
     * @param hitPosi  �Փˈʒu
     * @param srcPosi  �n�_
     * @param destPosi �I�_
     *
     * @retval true  �Փ˂��Ă���
     * @retval false �Փ˂��Ă��Ȃ�
     */
    //-----------------------------------------------------------------------------
    bool IsHitLine(VEC3* hitPosi, const VEC3& srcPosi, const VEC3& destPosi) const ;
  };

} } // namespace gfl::math

#endif // __GFL_MATH_PLANE_H__

/*  EOF  */





