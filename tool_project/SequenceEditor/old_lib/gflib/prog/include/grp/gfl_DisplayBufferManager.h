#ifndef __GFL_DISPLAYBUFFERMANAGER_H__
#define __GFL_DISPLAYBUFFERMANAGER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DisplayBufferManager.h
 *	@brief  �f�B�X�v���C�o�b�t�@�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>

#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_FrameBufferManager.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>


namespace gfl {
namespace grp {


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�o�b�t�@�]�����[�h
//=====================================
typedef enum 
{
  TRANSFER_MODE_ANTIALIASE_NOT_USED,  // �A���`�G�C���A�X�͍s���܂���B
  TRANSFER_MODE_ANTIALIASE_2x1,       // 2�~1�A���`�G�C���A�X�œ]�����܂��B
  TRANSFER_MODE_ANTIALIASE_2x2,       // 2�~2�A���`�G�C���A�X�œ]�����܂��B
  TRANSFER_MODE_ANTIALIASE_COUNT,      // �]�����[�h�̐���\���܂��B
    
  ENUM_FORCE_DWORD(DisplayTransferMode)
} DisplayTransferMode;
  
//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�f�B�X�v���C�o�b�t�@�ݒ�f�[�^
//=====================================
typedef struct {
  DisplayType           displayType;    // �f�B�X�v���C�̎�ނł��B
  s32                   width;          // �f�B�X�v���C�̉����ł��B
  s32                   height;         // �f�B�X�v���C�̏c���ł��B
  DisplayTransferMode   transMode;      // �e�B�X�v���C�o�b�t�@�̓]�����[�h�ł��B
  MemoryArea            memArea;        // �������m�ۃG���A�ł��B
  RenderColorFormat     colorFormat;    // �J���[�t�H�[�}�b�g�ł�
  bool                  isTransferFlipX;// �]�����ɂw�����Ƀt���b�v���邩�ǂ�����\���܂��B
  s32                   displayOffsetX; // �\�����s���n�_�w���W�ł��B
  s32                   displayOffsetY; // �\�����s���n�_�x���W�ł��B
} DisplayBufferSetUp;


//-------------------------------------
/// �f�B�X�v���C�o�b�t�@�[ID
//=====================================
typedef u32 DisplayBufferID;
const DisplayBufferID DISPLAYBUFFER_ID_ERROR = 0xffffffff;
  
//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
/// �f�B�X�v���C�o�b�t�@�[�N���X
//=====================================
class DisplayBuffer;



//-------------------------------------
/// �f�B�X�v���C�o�b�t�@�[�}�l�[�W���N���X
//=====================================
class DisplayBufferManager
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(DisplayBufferManager);

public:
  //-----------------------------------------------------------------------------
  /*
   * �R���X�g���N�^�E�f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  DisplayBufferManager( heap::NwAllocator* allocator, u32 arrayMax );
  ~DisplayBufferManager();

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�B�X�v���C�o�b�t�@�̊m��
   *
   *	@param	setup   �o�^���
   *
   *	@return �f�B�X�v���C�o�b�t�@ID
   */
  //-----------------------------------------------------------------------------
  DisplayBufferID CreateBuffer( const DisplayBufferSetUp& setup );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�B�X�v���C�o�b�t�@�̔j��
   *  
   *	@param	displayBufferID  �f�B�X�v���C�o�b�t�@ID
   */
  //-----------------------------------------------------------------------------
  void DeleteBuffer( DisplayBufferID displayBufferID );


  //----------------------------------------------------------------------------
  /**
   *	@brief    �f�B�X�v���C�o�b�t�@���A�N�e�B�u��Ԃɂ���
   *
   *	@param	  displayBufferID �f�B�X�v���C�o�b�t�@
   */
  //-----------------------------------------------------------------------------
  void ActivateBuffer( DisplayBufferID displayBufferID );
  void ActivateBuffer( DisplayBufferID displayBufferID, gfl::grp::DisplayType display_type );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�̃J���[����]��
   *
   *	@param  displayBufferID   �f�B�X�v���C�o�b�t�@ID  
   *	@param	renderTarget      �J���[�̓]�����ƂȂ郌���_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void TransferRenderImage( DisplayBufferID displayBufferID, const RenderTarget& renderTarget );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �Z�b�g�A�b�v���̎擾
   *
   *	@param	displayBufferID   �f�B�X�v���C�o�b�t�@�h�c
   *
   *	@return �Z�b�g�A�b�v���
   */
  //-----------------------------------------------------------------------------
  const DisplayBufferSetUp& GetSetupData( DisplayBufferID displayBufferID )const;
  

private:

  //�����o�ϐ�
  DisplayBuffer*  m_array;
  u32             m_arrayMax;
  
private:
  // �B�����\�b�h
  void Initialize( heap::NwAllocator* allocator, u32 arrayMax );
  void Finalize( void );
  DisplayBufferID GetClearBuffer( void ) const;

  

};






} /* end namespace grp */
} /* end namespace gfl */

#endif // __GFL_DISPLAYBUFFERMANAGER_H__
