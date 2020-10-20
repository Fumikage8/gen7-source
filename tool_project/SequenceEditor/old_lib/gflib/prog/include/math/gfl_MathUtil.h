#ifndef __GFL_MATH_UTIL_H__
#define __GFL_MATH_UTIL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathUtil.h
 *	@brief  �Z�p���[�e�B���e�B
 *	@author	masateru ishiguro
 *	@date		2011.04.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <nn/math.h>

#include <math/gfl_Vec.h>
#include <math/gfl_Mtx.h>
#include <math/gfl_MathType.h>

namespace gfl {
namespace math {

class MathUtil
{
public:

  //---------------------------------------------------------------------------
  /**
   * @brief �j�A�N���b�v�ʂ̂R�c���[���h���W�擾
   * @param p_view_matrix         �r���[�}�g���b�N�X
   * @param p_projection_matrix   �v���W�F�N�V�����}�g���b�N�X
   * @param x                     �X�N���[�����W�w(-1.0f ? 1.0f)3DS�͏c�����t�]���Ă���ꍇ������̂Œ��ӂ��Ă�������
   * @param y                     �X�N���[�����W�w(-1.0f ? 1.0f)3DS�͏c�����t�]���Ă���ꍇ������̂Œ��ӂ��Ă�������
   * @return                      �j�A�N���b�v�ʂ̂R�c���[���h���W
   */
  //---------------------------------------------------------------------------
  static gfl::math::VEC4 GetNearPlanePosition( gfl::math::MTX34 *p_view_matrix, gfl::math::MTX44 *p_projection_matrix, f32 x, f32 y );
  
  //---------------------------------------------------------------------------
  /**
   * @brief �J��������̃��C�擾
   * @param p_view_matrix         �r���[�}�g���b�N�X
   * @param p_projection_matrix   �v���W�F�N�V�����}�g���b�N�X
   * @param x                     �X�N���[�����W�w(-1.0f ? 1.0f)3DS�͏c�����t�]���Ă���ꍇ������̂Œ��ӂ��Ă�������
   * @param y                     �X�N���[�����W�w(-1.0f ? 1.0f)3DS�͏c�����t�]���Ă���ꍇ������̂Œ��ӂ��Ă�������
   * @return                      �J��������̃��C�i�����P�̃x�N�g���j
   */
  //---------------------------------------------------------------------------
  static gfl::math::VEC4 GetRayVector( gfl::math::MTX34 *p_view_matrix, gfl::math::MTX44 *p_projection_matrix, f32 x, f32 y );

  //---------------------------------------------------------------------------
  /**
   * @brief �����ƃ|���S���Ƃ̓����蔻��
   * @param pOutPos   ��_
   * @param ray_pos   �����ʒu
   * @param ray_vec   �����x�N�g��
   * @param vertex0   �|���S�����_�O�i�E���j
   * @param vertex1   �|���S�����_�P�i�E���j
   * @param vertex2   �|���S�����_�Q�i�E���j
   * @return          ����������
   */
  //---------------------------------------------------------------------------
  static b32 RayPickPolygon( gfl::math::VEC4 *pOutPos, const gfl::math::VEC4 &ray_pos, const gfl::math::VEC4 &ray_vec, gfl::math::VEC4 vertex0, gfl::math::VEC4 vertex1, gfl::math::VEC4 vertex2 );
  
private:
  MathUtil(){}
  ~MathUtil(){}

  static b32 RayPick( gfl::math::VEC4 *cross, const gfl::math::VEC4 &pos1, const gfl::math::VEC4 &vec1, const gfl::math::VEC4 &pos2, const gfl::math::VEC4 &vec2 );
  static b32 CheckInnerPoint( const gfl::math::VEC4 &pos0, const gfl::math::VEC4 &pos1, const gfl::math::VEC4 &pos2, const gfl::math::VEC4 &point, const gfl::math::VEC4 &normal );
  
};


} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_MATH_UTIL_H__
