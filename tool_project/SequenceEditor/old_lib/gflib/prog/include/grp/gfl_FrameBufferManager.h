#ifndef __GFL_FRAMEBUFFERMANAGER_H__
#define __GFL_FRAMEBUFFERMANAGER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FrameBufferManager.h
 *	@brief  �t���[���o�b�t�@�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>

#include <grp/gfl_GraphicsType.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>

#include <grp/gfl_RenderTarget.h>

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

//-------------------------------------
/// �t���[���o�b�t�@�[ID
//=====================================
typedef u32 FrameBufferID;
const FrameBufferID FRAMEBUFFER_ID_ERROR = 0xffffffff;


//-------------------------------------
///	�����_�����O�o�b�t�@�@�Z�b�g�A�b�v���
//=====================================
typedef struct{

  DisplayType           targetDisplay;  // �ǂ̖ʂ�Ώۂɂ��Ă���̂��B
  s32                   width;          // �o�b�t�@�̉����ł��B
  s32                   height;         // �o�b�t�@�̏c���ł��B
  RenderColorFormat     colorFormat;    // �J���[�o�b�t�@�t�H�[�}�b�g
  RenderDepthFormat     depthFormat;    // �[�x�o�b�t�@�t�H�[�}�b�g

  MemoryArea            colorArea;      // �`��Ώۂ̃J���[�o�b�t�@�̔z�u�ꏊ�ł��B
  MemoryArea            depthArea;      // �`��Ώۂ̃f�v�X�o�b�t�@�̔z�u�ꏊ�ł��B
  DisplayType           shareBuffer;    // �����_�[�^�[�Q�b�g��share����B
  f32                   left;           // Viewport���ł��B
  f32                   top;            // Viewport��ł��B
  f32                   right;          // Viewport�E�ł��B
  f32                   bottom;         // Viewport���ł��B

} RenderBufferSetUp;

//-------------------------------------
///	�t���[���o�b�t�@�N���A�J���[
//=====================================
typedef struct {
  f32   red;      // 0.0�`1.0
  f32   green;    // 0.0�`1.0
  f32   blue;     // 0.0�`1.0
  f32   alpha;    // 0.0�`1.0
  f32   depth;    // 0.0�`1.0
  f32   stencil;  // 0.0�`1.0
} FrameBufferClearColor;

//-------------------------------------
///	�t���[���o�b�t�@�N���A�t���O
//=====================================
struct FrameBufferClearFlag{
  b32   f_color;
  b32   f_depth;
  b32   f_stencil;
  
  FrameBufferClearFlag():
    f_color( true ),
    f_depth( true ),
    f_stencil( true )
  {
    
  }
};

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
/// �t���[���o�b�t�@�[�}�l�[�W���N���X
//=====================================
class FrameBufferManager
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(FrameBufferManager);

public:


  //-----------------------------------------------------------------------------
  /*
   * �R���X�g���N�^�E�f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  FrameBufferManager( heap::NwAllocator* allocator, u32 arrayMax );
  ~FrameBufferManager();


  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�@����
   *
   *	@param    setup	�@�Z�b�g�A�b�v���
   *
   *	@return �t���[���o�b�t�@ID
   */
  //-----------------------------------------------------------------------------
  FrameBufferID CreateBuffer( heap::NwAllocator* allocator, const RenderBufferSetUp& setup );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�@�j��
   *
   *	@param	id    �t���[���o�b�t�@ID
   */
  //-----------------------------------------------------------------------------
  void DeleteBuffer( FrameBufferID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@���J�����g�ɐݒ�
   *
   *	@param	  id  �t���[���o�b�t�@�h�c
   */
  //-----------------------------------------------------------------------------
  void Bind( FrameBufferID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �o�C���h���Ă���t���[���o�b�t�@���N���A����
   *
   *	@param	  id  �t���[���o�b�t�@�h�c
   *	@param    clearColoer �N���A�J���[�ݒ�
   */
  //-----------------------------------------------------------------------------
  void BindAndClear( FrameBufferID id, const FrameBufferClearColor& clearColor, const FrameBufferClearFlag& clearFlag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�̏����������ɃR�s�[
   *
   *	@param	id      �t���[���o�b�t�@ID
   *	@param  type    �o�b�t�@�^�C�v
   *	@param  memory  �R�s�[��A�h���X
   *
   *	*�����ŃJ�����g�o�b�t�@��ύX���܂��B
   */
  //-----------------------------------------------------------------------------
  void OutputImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ����������t���[���o�b�t�@�֏����R�s�[
   *
   *  @param  id      �t���[���o�b�t�@ID
   *  @param  memory  �R�s�[���A�h���X
   *
   *  *�����ŃJ�����g�o�b�t�@��ύX���܂��B
   */
  //-----------------------------------------------------------------------------
  void InputImage( FrameBufferID id, void* memory );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�̏����������ɃR�s�[ (Liner�t�H�[�}�b�g�j
   *
   *	@param	id      �t���[���o�b�t�@ID
   *	@param  type    �o�b�t�@�^�C�v
   *	@param  memory  �R�s�[��A�h���X
   *
   *	*�����ŃJ�����g�o�b�t�@��ύX���܂��B
   */
  //-----------------------------------------------------------------------------
  void OutputB2LImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �����_�[�^�[�Q�b�g�̎擾
   *
   *	@param	id      �t���[���o�b�t�@ID
   *
   *	@return ID�ɑΉ����������_�[�^�[�Q�b�g�擾
   */
  //-----------------------------------------------------------------------------
  gfl::grp::RenderTarget* GetRenderTarget( FrameBufferID id ) const;

private:
  RenderTarget*   m_array;
  u16             m_arrayMax;

private:
  void Initialize( heap::NwAllocator* allocator, u32 arrayMax );
  void Finalize(void);
  FrameBufferID GetClearBuffer( void ) const;
  FrameBufferID GetDisplayTypeBuffer( DisplayType displayType ) const;

};



} /* namespace end grp */
} /* namespace end gfl */
#endif // __GFL_FRAMEBUFFERMANAGER_H__
