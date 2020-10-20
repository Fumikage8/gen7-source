#ifndef __GFL_VIEWPORT_H__
#define __GFL_VIEWPORT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Viewport.h
 *	@brief  �r���[�|�[�g
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>


namespace gfl {
namespace grp {

  
// �O���錾
//class FrameBuffer;  // @note ������GetNwViewport�𗘗p����N���X�������ĉ������B
class RenderTarget;
namespace g3d {
  class RenderSystem;
}  // namespace g3d


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�r���[�|�[�g�N���X
//=====================================
class Viewport
{
  GFL_FORBID_COPY_AND_ASSIGN(Viewport);


  // ���L�N���X�ɂ���private�����o�֐��ւ̃A�N�Z�X������
  //friend class gfl::grp::FrameBuffer;  // @note ������GetNwViewport�𗘗p����N���X�������ĉ������B
  friend class gfl::grp::RenderTarget;
  friend class gfl::grp::g3d::RenderSystem;




public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^ / �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  Viewport(void);
  Viewport(
      f32 left,
      f32 top,
      f32 right,
      f32 bottom,
      f32 near = 0.0f,
      f32 far  = 1.0f);
  virtual ~Viewport();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾 / �ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetBound(
      f32 left,
      f32 top,
      f32 right,
      f32 bottom );
  void GetBound(
      f32* left,
      f32* top,
      f32* right,
      f32* bottom ) const;
  void SetNear(f32 near);
  f32 GetNear(void) const;
  void SetFar(f32 far);
  f32 GetFar(void) const;
  void SetDepthRange(f32 near, f32 far);

  
 
  
  // �ȉ��̃����o�֐���this�y��friend���炵���A�N�Z�X���܂���
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  const nw::gfx::Viewport& GetNwViewport(void) const
  {
    return m_nw_viewport;
  }

  


  // �ȉ��̃����o�ϐ���this���炵���A�N�Z�X���܂���
private:
  nw::gfx::Viewport  m_nw_viewport;
};

}  // namespace grp
}  // namespace gfl

#endif // __GFL_VIEWPORT_H__
