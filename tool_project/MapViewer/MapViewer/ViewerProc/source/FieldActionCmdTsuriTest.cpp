//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    ActionCmdTsuriTest.cpp
 *  @brief   �t�B�[���h�A�N�V�����R�}���h�ނ�e�X�g
 *  @author  yy-ikeuchi
 *  @date    2015.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "../include/FieldActionCmdTsuriTest.h"
#include "ViewerProc/include/MoveModel/movemodel.h"
#include "test/FieldTest/include/field_transform.h"

namespace field
{
namespace mmodel
{

//----------------------------------------------------------------------------
/**
*	@brief �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
ActionCmdTsuriTest::ActionCmdTsuriTest( void )
{
  m_pFuncTbl[0] = ActionCmdTsuriTest::AcTsuriStart;
  m_pFuncTbl[1] = ActionCmdTsuriTest::AcTsuriWait;
  m_pFuncTbl[2] = ActionCmdBase::AcEnd;
  m_SeqNum = 3;
}

//----------------------------------------------------------------------------
/**
*	@brief  �������V�[�P���X
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdTsuriTest::AcTsuriStart( ActionCmdBase::ActionCmdWork* p_work )
{
  Work* p_wk      = reinterpret_cast<Work*>(ActionCmdBase::GetAcmdWork( p_work, sizeof(Work) ));    
  p_wk->frame_max = 64;
  p_wk->frame     = 0;
  p_work->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD );
  p_work->p_model->ChangeAnimation( chara::MOTION_ID_TSURI_START , false, 3 , 1.0f);

  // ����
  ActionCmdBase::AddAcmdWorkSeq( p_work, 1 );

  return ActionCmdBase::ACMD_RET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  �ړ�
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdTsuriTest::AcTsuriWait( ActionCmdBase::ActionCmdWork* p_work )
{
  Work* p_wk = reinterpret_cast<Work*>(ActionCmdBase::GetAcmdWork( p_work, sizeof(Work) ));
  p_wk->frame++;
  if( p_wk->frame < p_wk->frame_max )
  {
    return ActionCmdBase::ACMD_RET_ONE_TIME_END;
  }

  p_work->p_model->ChangeAnimation( chara::MOTION_ID_WAIT , false, 3 , 1.0f);

  p_work->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD_END );

	// ����
	ActionCmdBase::AddAcmdWorkSeq( p_work, 1 );

	return ActionCmdBase::ACMD_RET_CONTINUE;
}

} // mmodel
} // field
