#ifndef __GFL_MAPDIVLOAD_H__
#define __GFL_MAPDIVLOAD_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapDivLoad.h
 *	@brief  �}�b�v�u���b�N�����ǂݍ���
 *	@author	tomoya takahashi
 *	@date		2011.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gfl_Macros.h>
#include <gfl_Fs.h>

namespace gfl{
namespace grp{
namespace map{

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
class DivLoad
{
  GFL_FORBID_COPY_AND_ASSIGN( DivLoad );

public:
  enum Mode
  {
    MODE_PEACE_LOAD,      // �I�t�Z�b�g���炵�ł́A�����ǂݍ���
    MODE_THREAD_LOAD,     // THREAD���g�p���Ă̓ǂݍ���

    MODE_MAX,     // �V�X�e�����Ŏg�p
  };

public:

  //-----------------------------------------------------------------------------
  /**
   *      �R���X�g���N�^�E�f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  DivLoad( void );
  ~DivLoad();

  //----------------------------------------------------------------------------
  /**
   *	@brief  ������
   *
   *	@param	p_buff      �o�b�t�@
   *	@param	buffSize    �o�b�t�@�T�C�Y
   *	@param  p_tmp_heap  �X���b�h����ɕK�v�ȃe���|�������[�N�m�ۗp
   *	@param  mode        ���샂�[�h
   *	@param  priority    �X���b�h�D�揇��
   */
  //-----------------------------------------------------------------------------
  void Initialize( void* p_buff, u32 buffSize, gfl::heap::HeapBase* p_tmp_heap, Mode mode, gfl::base::Thread::Priority priority );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �j��
   */
  //-----------------------------------------------------------------------------
  void Finalize( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�@�C���ǂݍ��݊J�n
   *
   *	@param	  p_arc         �A�[�J�C�u�t�@�C��
   *	@param    dataID        �f�[�^ID
   *	@param    one_load_size 1��̓ǂݍ��݃T�C�Y
   */
  //-----------------------------------------------------------------------------
  void StartFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���k�̊|�������t�@�C���̓ǂݍ��݊J�n
   *
   *	@param	p_arc         �A�[�J�C�u�t�@�C��
   *	@param  dataID        �f�[�^�h�c
   *	@param  one_load_size ��x�ɓǂݍ��ރT�C�Y
   *	@param  gfl::heap::HeapBase* p_tmp_heap �e���|�����o�b�t�@�����p�q�[�v
   */
  //-----------------------------------------------------------------------------
  void StartCompFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�@�C���ǂݍ���1�񕪎��s
   *
   *  @retval true    �t�@�C���ǂݍ��݊���
   *  @retval false   �t�@�C���ǂݍ��ݓr��
   */
  //-----------------------------------------------------------------------------
  b32 ContinueFileLoad( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�@�C���ǂݍ��݁@���Z�b�g
   */
  //-----------------------------------------------------------------------------
  bool ResetFileLoad( void );
  


  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ��ݒ����`�F�b�N
   *
   *	@retval true    �ǂݍ��ݒ�
   *	@retval false   �ǂݍ���łȂ�
   */
  //-----------------------------------------------------------------------------
  b32 IsLoading( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  1�O�ɓǂݍ��񂾃t�@�C���T�C�Y��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  inline s32 GetLoadFileSize( void ) const { return m_FileSize; }

private:

  Mode              m_Mode;
  gfl::base::Thread::Priority m_Priority;
  b32               m_CompFlag;

  void*             m_pLoadBuff;
  void*             m_pBuff;
  u32               m_BuffSize;
  gfl::fs::ArcFile* m_pArcFile;
  u32               m_DataID;
  u32               m_OneLoadSize;
  u32               m_MemoryOffset;
  u32               m_FileOffset;
  u32               m_FileSize;

  gfl::heap::HeapBase* m_pTmpHeap;


private:

  // �����ǂݍ���
  void startFilePieceLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size );
  void startFilePieceLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap );
  b32 continueFilePieceLoad(void);
  void resetFilePieceLoad(void);
  b32 isLoadingFilePieceLoad(void) const;

  // �X���b�h�ǂݍ���
  void startFileSyncLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size );
  void startFileSyncLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap );
  b32 continueFileSyncLoad(void);
  bool resetFileSyncLoad(void);
  b32 isLoadingFileSyncLoad(void) const;
  
};



} //namespace map{
} //namespace grp{  
} //namespace gfl{



#endif // __GFL_MAPDIVLOAD_H__
