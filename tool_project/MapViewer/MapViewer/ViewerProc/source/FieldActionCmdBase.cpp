//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdBase.cpp
 *	@brief  �t�B�[���h�A�N�V�����R�}���h�@���N���X
 *	@author	tomoya takahashi
 *	@date		2011.05.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "../include/FieldActionCmdBase.h"
#include "ViewerProc/include/MoveModel/movemodel.h"

namespace field{
namespace mmodel{

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

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
 *	@brief  �A�N�V�����R�}���h���[�N�@������
 *
 *	@param	p_wk        ���[�N
 *	@param	p_model     ���샂�f�����[�N�x�[�X
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::InitializeAcmdWork( ActionCmdWork* p_wk, fld::mmodel::MoveModel* p_model )
{
  p_wk->p_model   = p_model;
  p_wk->acmd      = static_cast<Acmd>(0);
  p_wk->seq       = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���L���[�N�擾
 *
 *	@param	p_wk      ���[�N
 *	@param	size      �T�C�Y
 *
 *	@return ���L���[�N�T�C�Y
 */
//-----------------------------------------------------------------------------
void* ActionCmdBase::GetAcmdWork( ActionCmdWork* p_wk, u32 size )
{
  GFL_ASSERT( size < WORK_SIZE ); //@check
  return reinterpret_cast<void*>(p_wk->work);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�N�V�����R�}���h�̐ݒ�
 *
 *	@param	p_wk
 *	@param	acmd 
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::SetAcmd( ActionCmdWork* p_wk, Acmd acmd, gfl2::math::Quaternion qua )
{
  GFL_ASSERT( acmd < AC_MAX ); // , "MMDL ACMD CODE ERROR" ); //@check
  p_wk->acmd  = acmd;
  p_wk->seq   = 0;
  p_wk->qua   = qua;
  p_wk->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD );
  p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD_END );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X��i�߂�
 *
 *	@param	p_wk    ���[�N
 *	@param	add     �ϓ��l
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::AddAcmdWorkSeq( ActionCmdWork* p_wk, s32 add )
{
  if( (static_cast<s32>(p_wk->seq) + add) >= 0 ){
    p_wk->seq += add;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X��ݒ�
 *
 *	@param	p_wk    ���[�N
 *	@param	seq     �V�[�P���X
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::SetAcmdWorkSeq( ActionCmdWork* p_wk, s32 seq )
{
  p_wk->seq = seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X���擾
 *
 *	@param	p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
s32 ActionCmdBase::GetAcmdWorkSeq( ActionCmdWork* p_wk )
{
  return p_wk->seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I���`�F�b�N
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval true    �I��
 *	@retval false   �r��
 */
//-----------------------------------------------------------------------------
bool ActionCmdBase::IsAcmdWorkEnd( const ActionCmdWork* cp_wk )
{
	if( (cp_wk->acmd == 0) || (cp_wk->p_model->CheckMoveBit(fld::mmodel::MOVEBIT_ACMD_END)) ){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N���Z�b�g
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::ClearAcmdWork( ActionCmdWork* p_wk )
{
  p_wk->acmd  = static_cast<Acmd>(0);
  p_wk->seq   = 0;

  p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD );
  p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD_END );

}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�N�V�����R�}���h�̍X�V
 *
 *	@retval ACMD_RET_ONE_TIME_END,      ///<����
 *	@retval ACMD_RET_END,           ///<�I���
 */
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdBase::UpdateAcmd( ActionCmdWork* p_wk ) const
{
  AcmdRet ret = ACMD_RET_END;
  s32 roop_count = 0;

  GFL_ASSERT( ACMDFUNC_SEQ_MAX >= m_SeqNum ); //@check
  
  // �V�[�P���X�`�F�b�N
  if( m_SeqNum > p_wk->seq ){
  
    do{
      
      ret = (m_pFuncTbl[ p_wk->seq ])( p_wk );

      // �V�[�P���X�I�[�o�[�`�F�b�N
      GFL_ASSERT( m_SeqNum > p_wk->seq ); //@check

      ++roop_count;

      // �R���e�B�j���[��1000�񑱂�����A���炩�ɂ����������߃A�T�[�g�X�g�b�v
      GFL_ASSERT_STOP( roop_count < 1000 );
      
    }while( ret == ACMD_RET_CONTINUE ); // CONTINUE�̊ԓ���  @whilecheck
  }

  if( ret == ACMD_RET_END ){
    // �A�N�V�����R�}���h�̎��s�������������Ƃ��A
    // ���f���ɐݒ�
    p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD );
    p_wk->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD_END );
  }


  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���ʏI������
 *
 *	@param	p_work  ���[�N
 *
 *	@retval ACMD_RET_END
 */
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdBase::AcEnd( ActionCmdWork* /*p_work*/ )
{
  return ACMD_RET_END;
}

} // mmodel
} // field
