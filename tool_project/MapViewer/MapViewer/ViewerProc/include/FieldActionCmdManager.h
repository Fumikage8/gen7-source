//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdManager.h
 *	@brief  �t�B�[���h�A�N�V�����R�}���h�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2011.05.14
 *
 *	@sa  svn://svn-xy.gamefreak.co.jp/xy_project/branches/upper/document/takahashi_tomoya/�݌v����/FieldMoveModelManager.xls
 *	@sa  svn://svn-xy.gamefreak.co.jp/xy_project/branches/upper/document/takahashi_tomoya/���샂�f��/���샂�f���Q�@�\�ǉ����@.docx 
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELDACTIONCMDMANAGER_H__
#define __FIELDACTIONCMDMANAGER_H__

#include "FieldActionCmdBase.h"

namespace field{
namespace mmodel{

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
class ActionCmdManager
{

public:
  
  //----------------------------------------------------------------------------
  /**
   *	@brief    ������
   *
   *	@param	  p_heap    �q�[�v
   */
  //-----------------------------------------------------------------------------
  static void Initialize( gfl2::heap::HeapBase* p_heap );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �j��
   */
  //-----------------------------------------------------------------------------
  static void Finalize( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    MoveModelWork���ł̃R�}���h�̎��s
   *
   *	*�R�}���h�̏I����Ԃ�ێ����Ă���B
   *	*�R�}���h�����s�����l���AActionCmdWork���N���A����K�v������B
   */
  //-----------------------------------------------------------------------------
  static void ActionCmd( ActionCmdBase::ActionCmdWork* p_wk );

  //----------------------------------------------------------------------------
  /**
   *	@brief    ���샂�f��Push�����ɂ�鋭���I�����̏���
   *	@note     ActionCmdWork��Pop���ɃN���A�����
   *
   *	*�R�}���h�����s�����l���AActionCmdWork���N���A����K�v������B
   */
  //-----------------------------------------------------------------------------
  static void PushCmd( ActionCmdBase::ActionCmdWork* p_wk );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �R�}���h�����҂�
   *
   *	@retval true    ����
   *	@retval false   �r��
   */
  //-----------------------------------------------------------------------------
  static bool IsEndCmd( const ActionCmdBase::ActionCmdWork* cp_wk );

private:

  //! �A�N�V�����R�}���h�e�[�u��
  ///static ActionCmdBase* m_pTbl[ AC_MAX ];@saito del
  
};


} // mmodel
} // field

#endif	// __FIELDACTIONCMDMANAGER_H__

