//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DirectAddressRenderTarget.h
 *	@brief  ���ڂ�address�w�背���_�[�^�[�Q�b�g
 *	@author tomoya takahashi	
 *	@date		2011.03.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_DIRECTADDRESSRENDERTARGET_H__
#define __GFL_DIRECTADDRESSRENDERTARGET_H__
#pragma once


#include <nn.h>

#include <nw.h>
#include <nw/gfx.h>

#include <grp/gfl_GraphicsSystem.h>

#include <gfl_Heap.h>


namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�_�C���N�g�A�h���X�@�����_�[�^�[�Q�b�g
//=====================================
class DirectAddressRenderTarget : public nw::gfx::IRenderTarget
{
  GFL_FORBID_COPY_AND_ASSIGN(DirectAddressRenderTarget);

public:
  // �ݒ�̎擾
  virtual const nw::gfx::IRenderTarget::Description& GetDescription() const { return m_Description; }

  // �t���[���o�b�t�@�I�u�W�F�N�g�̎擾
  virtual const nw::gfx::FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ����
   *  
   *	@param	allocator     �f�o�C�X�A���P�[�^
   *	@param  width         �o�b�t�@��
   *	@param  height        �o�b�t�@����
   *	@param  color_format  �J���[�o�b�t�@�t�H�[�}�b�g
   *	@param  depth_format  �f�v�X�o�b�t�@�t�H�[�}�b�g
   *	@param  color_address �J���[�o�b�t�@�A�h���X
   *	@param  depth_address �f�v�X�o�b�t�@�A�h���X
   *	@param  fboID         �t���[���o�b�t�@ID  �iGL�p�@�Ȃ���΂O�j
   */
  //-----------------------------------------------------------------------------
  static DirectAddressRenderTarget* Create( gfl::heap::NwAllocator* allocator, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, GLuint fboID = 0 );

private:

  //�R���X�g���N�^
  DirectAddressRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, GLuint fboID , u32 color_address, u32 depth_address );

  // �f�X�g���N�^
  virtual ~DirectAddressRenderTarget();


  nw::gfx::FrameBufferObject m_BackBufferObject;
  void* m_ActivateCommand;
  
  nw::gfx::IRenderTarget::Description m_Description;
  
  friend class nw::gfx::IRenderTarget;
  
};


}  // namespace grp
}  // namespace gfl

#endif	// __GFL_DIRECTADDRESSRENDERTARGET_H__

