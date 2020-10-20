//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdManager.cpp
 *	@brief  �t�B�[���h�A�N�V�����R�}���h�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2011.05.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include "../include/FieldActionCmdManager.h"
#include "../include/FieldActionCmdNone.h"
#include "../include/FieldActionCmdAnalogMove.h"
#include "../include/FieldActionCmdTsuriTest.h"

namespace field{
namespace mmodel{

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

// �A�N�V�����R�}���h�e�[�u��
ActionCmdBase* ActionCmdManager::m_pTbl[ AC_MAX ];

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief    ������
 *
 *	@param	  p_heap    �q�[�v
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::Initialize( gfl2::heap::HeapBase* p_heap )
{
  // m_pTbl�𐶐�
  m_pTbl[ AC_NONE ] = GFL_NEW( p_heap ) ActionCmdNone();

  // �U������n

  // �ړ��n
   m_pTbl[ AC_WALK_32F ] = GFL_NEW( p_heap ) ActionCmdWalk_32F();

  // ���̏�ړ��n

  // �W�����v�n

  // �E�F�C�g�n

  // �`��t���O�n

  // ���[�v�n

  // �A�C�R���\��
  
  // ���[���[�X�P�[�g�n

  // �E�ё��n

  // ���]�Ԍn

  // �g���n

  // �T�C�z�[���n

  // �€�n

  // �A�j���[�V�����A�N�V����
   m_pTbl[ AC_TSURI_TEST ] = GFL_NEW( p_heap ) ActionCmdTsuriTest();

#if PM_DEBUG
  // �������`�F�b�N
  for( u32 i=0; i<AC_MAX; ++i )
  {
    if( m_pTbl[i]->GetAcmdID() != i ){
      GFL_ASSERT( 0 );
      break;
    }
  }
#endif
}


//----------------------------------------------------------------------------
/**
 *	@brief    �j��
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::Finalize( void )
{
  for( u32 i=0; i<AC_MAX; ++i ){

    if( m_pTbl[i] ){
      GFL_DELETE m_pTbl[i];
      m_pTbl[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    MoveModelWork���ł̃R�}���h�̎��s
 *
 *	*�R�}���h�̏I����Ԃ�ێ����Ă���B
 *	*�R�}���h�����s�����l���A���[�N���N���A����K�v������B
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::ActionCmd( ActionCmdBase::ActionCmdWork* p_wk )
{
  ActionCmdBase* p_acmd;
  
  if( p_wk->acmd < AC_MAX ){
   
    p_acmd = m_pTbl[p_wk->acmd];
    
  }else{
    GFL_ASSERT( p_wk->acmd < AC_MAX ); //@check
    p_acmd = m_pTbl[AC_NONE];

    p_wk->acmd  = AC_NONE;
    p_wk->seq   = 0;
  }

  // ���s
  p_acmd->UpdateAcmd( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief    ���샂�f��Push�����ɂ�鋭���I�����̏���
 *	@note     ActionCmdWork��Pop���ɃN���A�����
 *
 *	*�R�}���h�����s�����l���AActionCmdWork���N���A����K�v������B
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::PushCmd( ActionCmdBase::ActionCmdWork* p_wk )
{
  if( p_wk->acmd >= AC_MAX )
  {
    return;
  }

  // ���s
  m_pTbl[p_wk->acmd]->PushCmd( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief    �R�}���h�����҂�
 *
 *	@retval true    ����
 *	@retval false   �r��
 */
//-----------------------------------------------------------------------------
bool ActionCmdManager::IsEndCmd( const ActionCmdBase::ActionCmdWork* cp_wk )
{
  return ActionCmdBase::IsAcmdWorkEnd( cp_wk );
}

} // mmodel
} // field
