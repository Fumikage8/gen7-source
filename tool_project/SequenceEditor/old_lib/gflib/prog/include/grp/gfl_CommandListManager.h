#ifndef __GFL_COMMANDLISTMANAGER_H__
#define __GFL_COMMANDLISTMANAGER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_CommandListManager.h
 *	@brief  �R�}���h���X�g�}�l�[�W��
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


namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define GFL_COMMAND_LIST_DEBUG_USE (1)  // ���i�łł��@�\���g�������V�`���G�[�V�����̂��߂ɕʒ�`�ɂ���B

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�R�}���h���X�g�Z�b�g�A�b�v�f�[�^
//=====================================
typedef struct {
  u32 bufferSize;     // �o�b�t�@�T�C�Y�i�R�}���h�o�b�t�@�̑��T�C�Y�j
  u32 queueingCount;  // �L���[�C���O�R�}���h���i�o�b�t�@�̒��ɉ��R�}���h������邩�j
} CommandListSetUp;



//-------------------------------------
/// �R�}���h���X�gID
//=====================================
typedef u32 CommandListID;
const CommandListID COMMANDLIST_ID_ERROR = 0xffffffff;

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
class CommandList;

//-------------------------------------
///	�R�}���h���X�g�}�l�[�W��
//=====================================
class CommandListManager{

  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(CommandListManager);

public:
  //-----------------------------------------------------------------------------
  /**
   *      �R���X�g���N�^�E�f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  CommandListManager( heap::NwAllocator* allocator, u32 arrayMax );
  ~CommandListManager();

  //----------------------------------------------------------------------------
  /**
   *	@brief    �R�}���h���X�g����
   *
   *	@param	�Z�b�g�A�b�v�f�[�^
   *
   *	@retval �R�}���h���X�gID
   *	@retval COMMANDLIST_ID_ERROR  �R�}���h���X�g�𐶐��ł��Ȃ������B
   */
  //-----------------------------------------------------------------------------
  CommandListID CreateBuffer( const CommandListSetUp& setup );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �R�}���h���X�g�j��
   *
   *	@param	id  �R�}���h���X�gID
   */
  //-----------------------------------------------------------------------------
  void DeleteBuffer( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���i�[���Ă���CommandList���w��
   *
   *	@param	id  �R�}���h���X�gID
   *
   *	*�����Ńo�C���h�����R�}���h���X�g���J�����g�R�}���h���X�g�ƂȂ�܂��B
   */
  //-----------------------------------------------------------------------------
  void Bind( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h�̓��e���N���A����
   *
   *	@param	id  �R�}���h���X�gID
   *
   *	�����s��J�����g�R�}���h���X�g���ăo�C���h���܂��B
   */
  //-----------------------------------------------------------------------------
  void Clear( CommandListID id );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���s����CommandList���w��
   *
   *	@param	id  �R�}���h���X�gID
   *
   *	�����s��J�����g�R�}���h���X�g���ăo�C���h���܂��B
   */
  //-----------------------------------------------------------------------------
  void RunAsync( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���s����������܂ő҂�
   *
   *	�����s��J�����g�R�}���h���X�g���ăo�C���h���܂��B
   */
  //-----------------------------------------------------------------------------
  void WaitDone( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���s�����`�F�b�N
   *	@retval true    �R�}���h���s��
   *	@retval false   �R�}���h���s���Ă��Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool IsRuning( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    ���s���Ă���R�}���hID���擾
   *
   *	@return ���s���Ă���R�}���hID
   */
  //-----------------------------------------------------------------------------
  CommandListID GetRuningCommandID(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    ���s�����R�}���hID���擾
   *
   *	@retval ���s�����R�}���hID
   *  @retval COMMANDLIST_ID_ERROR ���s��
   */
  //-----------------------------------------------------------------------------
  CommandListID GetRanCommandID(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���X�g�ւ̃R�}���h�Z�[�u�@�J�n
   *
   *	@param	�R�}���h���Z�[�u����R�}���h���X�gID
   *
   *	*id�̃R�}���h���X�g�ɂ��܂��Ă���R�}���h���N���A����ɂ́A�ʏ�Ɠ��lClear�֐����g�p���Ă��������B
   */
  //-----------------------------------------------------------------------------
  void StartCommandSave( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���X�g�ւ̃R�}���h�Z�[�u�@�I��
   *	*���̊֐����ŁASave�O�Ƀo�C���h����Ă����R�}���h���X�g���ăo�C���h���܂��B
   */
  //-----------------------------------------------------------------------------
  void EndCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���X�g�ւ̃R�}���h�Z�[�u�@�I��  �Z�[�u���擾�o�[�W����
   *	*���̊֐����ŁASave�O�Ƀo�C���h����Ă����R�}���h���X�g���ăo�C���h���܂��B
   *
   *	@param  p_buffoffset  �Z�[�u�J�n�o�b�t�@�I�t�Z�b�g
   *	@param  p_buffsize    �Z�[�u�o�b�t�@�T�C�Y
   *	@param  p_reqid       �Z�[�u�J�n���N�G�X�gID
   *	@param  p_reqsize     �Z�[�u���N�G�X�g�T�C�Y
   *	@param  split_cmd     SplitCmd
   */
  //-----------------------------------------------------------------------------
  void EndCommandSaveEx( u32* p_buffoffset, s32* p_buffsize, u32* p_reqid, s32* p_reqsize );

  //----------------------------------------------------------------------------
  /**
   *	@brief  1�O�ɕۑ������R�}���h�����o�C���h����Ă���R�}���h���X�g�ɒǉ�����B
   *
   *	@param	save_id    �ۑ��R�}���h���X�gID
   *	@param  is_copy    �R�}���h���X�g���R�s�[����̂��A����Ƃ������N���͂�̂�
   *	*�R�s�[����ƁAsave_id���̃R�}���h���N���A���邱�Ƃ��o���܂����A
   *	�@�R�s�[��CPU�̕��ׂ�������܂��B
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSave( CommandListID save_id, bool is_copy );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ۑ������R�}���h�����o�C���h����Ă���R�}���h���X�g�ɒǉ�����B
   *
   *	@param	save_id    �ۑ��R�}���h���X�gID
   *	@param  is_copy    �R�}���h���X�g���R�s�[����̂��A����Ƃ������N���͂�̂�
   *	*�R�s�[����ƁAsave_id���̃R�}���h���N���A���邱�Ƃ��o���܂����A
   *	�@�R�s�[��CPU�̕��ׂ�������܂��B
   *
   *	@param  buffoffset  �o�b�t�@�I�t�Z�b�g
   *	@param  buffsize    �o�b�t�@�T�C�Y
   *	@param  reqid       ���N�G�X�gID
   *	@param  reqsize     ���N�G�X�g�T�C�Y
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSaveEx( CommandListID save_id, bool is_copy, u32 buffoffset, s32 buffsize, u32 reqid, s32 reqsize );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �Z�[�u�����`�F�b�N
   *
   *	@retval true    �Z�[�u��
   *	@retval false   ���Ă��Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool IsSaving( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �Z�[�u�����R�}���h�ł͂Ȃ��A�R�}���h���X�g�̓��e��S�āA�J�����g�̃R�}���h�̌��ɒ~�ς�����B
   *
   *	@param	add_id    �~�ς���R�}���h���X�gID
   *	@param  is_copy    �R�}���h���X�g���R�s�[����̂��A����Ƃ������N���͂�̂�
   *	*�R�s�[����ƁAsave_id���̃R�}���h���N���A���邱�Ƃ��o���܂����A
   *	�@�R�s�[��CPU�̕��ׂ�������܂��B
   */
  //-----------------------------------------------------------------------------
  void AddCommand( CommandListID add_id, bool is_copy );


  //----------------------------------------------------------------------------
  /**
   *	@brief�o�b�t�@�̏�Ԃ��擾
   *
   *	@param	p_usebuff   �o�b�t�@�̎g�p��
   *	@param	p_usereq    ���N�G�X�g�J�E���g�̎g�p��
   *	@param	p_restbuff  �o�b�t�@�c��T�C�Y
   *	@param	p_restreq   ���N�G�X�g�J�E���g�o�b�t�@�c�萔
   */
  //-----------------------------------------------------------------------------
  void GetBufferStatus( CommandListID id, u32* p_usebuff, u32* p_usereq, u32* p_restbuff, u32* p_restreq );


  //----------------------------------------------------------------------------
  /**
   *	@brief    �o�C���h����Ă���o�b�t�@�̏�Ԃ����O�ɏo��
   */
  //-----------------------------------------------------------------------------
  void Dump( void ) const;

#if GFL_COMMAND_LIST_DEBUG_USE
  //-----------------------------------------------------------------------------
  /**
   *      �f�o�b�O�@�\�@�g�p���@
   *      1.�S�f�o�b�O�@�\��L���ɂ���B
   *        gfl::grp::CommandListManager::DebugSetFlag( true )
   *        
   *        ���̊֐���true��n�����ƂŁA�ȉ��̊֐����g�p�ł���悤�ɂȂ�܂��B
   *        DebugSetCommandEndCallback()
   *        DebugGetDoneMs()
   *        DebugGetWaitDoneMs()
   *        
   *      2.���[�U�[�R�[���o�b�N���g�p����B
   *        gfl::grp::CommandListManager::DebugSetCommandEndCallback()
   *        �R�[���o�b�N�֐��ƃ��[�U�[���[�N��n�����ƂŁA
   *        �R�}���h���X�g�̎��s�����������^�C�~���O��������R�[���o�b�N��ݒ�ł��܂��B
   *
   *      3.�������Ԃ��`�F�b�N����B
   *      �@�R�}���h���X�g�̏�������
   *        gfl::grp::CommandListManager::DebugGetDoneMs()
   *
   *        CPU�̏�������������AGPU�̏��������܂łɂ�����������
   *        �iGPU�̊�����CPU���҂��Ă��鎞�ԁj
   *        gfl::grp::CommandListManager::DebugGetWaitDoneMs()
   *
   *
   * �@�@�@�@���������Ԃ̎擾�́A�A
   * �@�@�@�@�@�ȉ��̃^�C�~���O�ōs���Ă��������B
   *        �@GraphicsSystem::WaitVSync();
   *        �@->���̊ԂŁA�擾���Ă��������B
   *        �@GraphicsSystem::VSyncAfter();
   *
   *      4.�~�ς���Ă���R�}���h�����擾����
   *        �R�}���h���N�G�X�g��
   *        gfl::grp::CommandListManager::DebugGetUseReqCount()
   *
   *        �R�}���h�~�Ϗ��̑��o�C�g��
   *        gfl::grp::CommandListManager::DebugGetUseBuffSize()
   *
   *      5.GPU�R�}���h�̓���̃^�C�~���O�ŕ������`�悷��B
   *        �o�^�ł��镶�����8�܂ŁB
   *        gfl::grp::CommandListManager::DebugPutGpuDebugPrint()
   *        
   */
  //-----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�o�b�O�p�@�f�o�b�O�@�\��ON�EOFF
   *
   *	@param	flag    true:ON�@�@false:OFF
   */
  //-----------------------------------------------------------------------------
  static void DebugSetFlag( bool flag );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�o�b�O�p�@GPU���̏��������R�[���o�b�N�@ON�EOFF
   *
   *	@param	func  �֐��|�C���^     �iOFF�FNULL)
   *	@param  work  ���[�U�[���[�N    �iOFF�FNULL�j
   */
  //-----------------------------------------------------------------------------
  static void DebugSetCommandEndCallback(void(*func)( void* ) = NULL, void* work = NULL );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�o�b�O�p�@�P�O�̃R�}���h���s�ɂ���������������
   *
   *  @retval �}�C�N���b
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetDoneMs(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�o�b�O�p�@�P�O�̃R�}���h���s��GPU�̏����҂�����������
   *
   *  @retval WaitDone�������܂����}�C�N���b
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetWaitDoneMs(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�o�b�O�p�@���s�R�}���h���X�g�ɒ~�ς���Ă��R�}���h��
   *
   *  @retval �R�}���h��
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetUseReqCount(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�o�b�O�p�@���s�R�}���h���X�g�ɒ~�ς���Ă���R�}���h���o�C�g�T�C�Y
   *
   *  @retval WaitDone�������܂����}�C�N���b
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetUseBuffSize(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  �����܂łɊi�[����Ă���R�}���h�����s���ꂽ��\�������f�o�b�O�v�����g
   *	�@�@�@�@��o�^
   *
   *	@param	str   ������
   */
  //-----------------------------------------------------------------------------
  static void DebugPutGpuDebugPrint( const char* str );
#endif  // GFL_COMMAND_LIST_DEBUG_USE

private:

  CommandList*  m_array;
  u32           m_arrayMax;

  CommandListID m_saveCommandID;
  CommandListID m_runCommandID;
  CommandListID m_bindCommandID;

  bool          m_runing;   // ���s��
  bool          m_saving;   // �Z�[�u��
  
#if GFL_COMMAND_LIST_DEBUG_USE
  static bool m_debug_flag;             // �R�}���h���X�g�f�o�b�O�t���O
  static s64  m_debug_commandwait_ms;   // �R�}���h���s�҂����ԁi�}�C�N���b�j
  static s64  m_debug_commandDoneMs;    // �R�}���h���s�ɂ����������ԁi�}�C�N���b�j
  static s64  m_debug_commandStartDoneMs;// �R�}���h���s���J�n�������ԁi�`�b�N���ԁj
  static void (*m_debug_commandCallbackFunc)(void*);  // �R�}���h�����R�[���o�b�N�֐��|�C���^
  static void* m_debug_commandCallbackWork;  // �R�}���h�����R�[���o�b�N�ɓn�����[�U�[���[�N
  static bool m_debug_commandCallbackFlag; // �R�[���o�b�N���Ă΂�邱�Ƃ�҂ׂ̃t���O
  static s32  m_debug_commandUseReqCount; // ���s���̃R�}���h�~�ϐ�
  static s32  m_debug_commandUseBuffSize;  // ���s���̃R�}���h�o�b�t�@�~�σT�C�Y
  static const int DEBUG_GPU_PRINT_BUF_MAX = 8;
  static const char* m_debug_gpuPrintBuf[2][DEBUG_GPU_PRINT_BUF_MAX];  // GPU�f�o�b�O�o�́@������o�b�t�@
  static s32  m_debug_gpuPrintBuffIndex;  //GPU�f�o�b�O�o�́@�_�u���o�b�t�@�p�C���f�b�N�X
  static s32  m_debug_gpuPrintBuffCount;  //GPU�f�o�b�O�o�́@�o�b�t�@�i�[�J�E���^
  static s32  m_debug_gpuPrintBuffPrintCount;  //GPU�f�o�b�O�o�́@�o�b�t�@�o�̓J�E���^
  static s32  m_debug_gpuPrintBuffPrintCountMax;  //GPU�f�o�b�O�o�́@�o�b�t�@�o�̓J�E���^�ő�l

#endif  // GFL_COMMAND_LIST_DEBUG_USE

private:

  void Initialize( heap::NwAllocator* allocator, u32 arrayMax );
  void Finalize( void );
  CommandListID GetClearBuffer( void ) const;
  void BindCurrentCommandList( void );
  
#if GFL_COMMAND_LIST_DEBUG_USE
  static void DebugCommandEndCallback(GLint id);
#endif  // GFL_COMMAND_LIST_DEBUG_USE
} ;




} /* namespace gfl */
} /* namespace grp */
#endif // __GFL_COMMANDLISTMANAGER_H__
