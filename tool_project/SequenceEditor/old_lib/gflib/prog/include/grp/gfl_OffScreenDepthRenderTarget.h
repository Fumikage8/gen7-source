#ifndef __GFL_OFFSCREENDEPTHRENDERTARGET_H__
#define __GFL_OFFSCREENDEPTHRENDERTARGET_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_OffScreenDepthRenderTarget.h
 *	@brief  �I�t�X�N���[���f�v�X���背���_�[�^�[�Q�b�g
 *	@author	tomoya takahashi
 *	@date		2010.12.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nn.h>

// NW4C
#include <nw.h>
#include <nw/gfx.h>

//  gflib grp
#include <grp/gfl_GraphicsSystem.h>

// gflib
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
///	�I�t�X�N���[�������_���[�^�[�Q�b�g�@�f�v�X�o�b�t�@����B
//=====================================
class OffScreenDepthRenderTarget : public nw::gfx::IRenderTarget
{
  GFL_FORBID_COPY_AND_ASSIGN(OffScreenDepthRenderTarget);

public:

  // �ݒ�̎擾
  virtual const nw::gfx::IRenderTarget::Description& GetDescription() const { return m_Description; }

  // �t���[���o�b�t�@�I�u�W�F�N�g�̎擾
  virtual const nw::gfx::FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ����
   *  
   *	@param	allocator     �A���P�[�^
   *	@param  resTexture    �e�N�X�`�����\�[�X
   *	@param  depth_format  �f�v�X�o�b�t�@�t�H�[�}�b�g
   *	@param  depth_area    �f�v�X�o�b�t�@���m�ۂ��郁�����^�C�v
   *	@param  depthAddress  �m�ۍς݃A�h���X�w��B�O�̂Ƃ��A���P�[�g���܂��B�i�f�t�H���g�O�j
   */
  //-----------------------------------------------------------------------------
  static OffScreenDepthRenderTarget* Create( gfl::heap::NwAllocator* allocator, nw::gfx::res::ResTexture resTexture, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea depth_area, u32 depthAddress = 0 );

private:

  //�R���X�g���N�^
  OffScreenDepthRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, nw::gfx::res::ResPixelBasedTexture resTexture , u32 depthAddress );

  // �f�X�g���N�^
  virtual ~OffScreenDepthRenderTarget();


  nw::gfx::FrameBufferObject m_BackBufferObject;
  void* m_ActivateCommand;
  
  nw::gfx::IRenderTarget::Description m_Description;

  nw::gfx::res::ResTexture m_Texture;

  u32 m_DepthBufferID;
  u32 m_DepthAddr;
  
  friend class nw::gfx::IRenderTarget;
};


}  // namespace grp
}  // namespace gfl


#endif // __GFL_OFFSCREENDEPTHRENDERTARGET_H__
