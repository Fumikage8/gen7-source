//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdAnalogMove.cpp
 *	@brief		�t�B�[���h�A�N�V�����R�}���h�A�i���O�ړ�����
 *	@author		yy-ikeuchi
 *	@date		2015.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "../include/FieldActionCmdAnalogMove.h"
#include "ViewerProc/include/MoveModel/movemodel.h"
#include "test/FieldTest/include/field_transform.h"

namespace field
{
namespace mmodel
{

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
*	@brief  �O���b�h�ړ����ʏ���    ���[�N������
*
*	@param	p_work    ���[�N
*	@param	dir       ����
*	@param	frame     �t���[��
*	@param  offs_x    �ړ��l�ɑ�������X���W
*	@param  offs_z        �ړ��l�ɑ�������Z���W
*	@pram   dispDirUpdate �����ڂ̌������X�V���邩
*/
//-----------------------------------------------------------------------------
void ActionCmdAnalogMove::InitializeWork( ActionCmdBase::ActionCmdWork* p_work, u32 frame, gfl2::math::Vector addTrans, gfl2::math::Quaternion qua, b32 isQuaUpdate, u32 interpFrameForQua )
{
	Work* p_wk = reinterpret_cast<Work*>(ActionCmdBase::GetAcmdWork( p_work, sizeof(Work) ));    
	p_wk->frame_max			= frame;
	p_wk->frame				= 0;
	p_wk->startTrans		= p_work->p_model->GetTransform()->GetTranslation();
	p_wk->targetTrans		= p_work->p_model->GetTransform()->GetTranslation() + addTrans;	//����
	p_wk->startQua			= p_work->p_model->GetTransform()->GetQuaternion();
	p_wk->targetQua			= qua;	// ���
	p_wk->isQuaUpdate		= isQuaUpdate;
	p_wk->interpFrameForQua = interpFrameForQua;
	p_work->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD );

	// ����
	ActionCmdBase::AddAcmdWorkSeq( p_work, 1 );
}

//----------------------------------------------------------------------------
/**
*	@brief  �ړ�
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdAnalogMove::AcGridMoveBase( ActionCmdBase::ActionCmdWork* p_work, b32 dispDirUpdate )
{
	Work* p_wk = reinterpret_cast<Work*>(ActionCmdBase::GetAcmdWork( p_work, sizeof(Work) ));

	// �O���b�h�J�n�ʒu����Aframe���i�񂾕��������B
	fld::mmodel::MoveModel* p_model = static_cast<fld::mmodel::MoveModel*>(p_work->p_model);
    
	p_wk->frame++;

	// �ړ�
	{
		f32 rate = static_cast<f32>(p_wk->frame) / static_cast<f32>(p_wk->frame_max);
		gfl2::math::Vector addTrans( (p_wk->targetTrans - p_wk->startTrans) * rate );
		p_model->GetTransform()->SetTranslation( p_wk->startTrans + addTrans );
	}

	// �p��
	if( p_wk->isQuaUpdate )
	{
		f32 rate = gfl2::math::min( 1.0f , static_cast<f32>(p_wk->frame) / static_cast<f32>(p_wk->interpFrameForQua) );
		gfl2::math::Quaternion qua = p_wk->startQua;
		qua.Slerp( p_wk->targetQua,rate );
		p_model->GetTransform()->SetQuaternion( qua );
	}

	// ���W�X�V
	p_model->GetTransform()->Update();
	
	if( p_wk->frame < p_wk->frame_max )
	{
		// �P�t���[�����ړ�
		return ActionCmdBase::ACMD_RET_ONE_TIME_END;
	}

	p_work->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD_END );

	// ����
	ActionCmdBase::AddAcmdWorkSeq( p_work, 1 );

	return ActionCmdBase::ACMD_RET_CONTINUE;
}
 
//----------------------------------------------------------------------------
/**
*	@brief  �ړ�
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdAnalogMove::AcGridMove( ActionCmdBase::ActionCmdWork* p_work )
{
	return ActionCmdAnalogMove::AcGridMoveBase( p_work, true );
}

//===============================================================================================================================
/**
*    32F�ňړ�
*/
//===============================================================================================================================

//----------------------------------------------------------------------------
/**
*	@brief �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
ActionCmdWalk_32F::ActionCmdWalk_32F( void )
{
	m_pFuncTbl[0] = ActionCmdWalk_32F::AcGridMoveInit;
	m_pFuncTbl[1] = ActionCmdAnalogMove::AcGridMove;
	m_pFuncTbl[2] = ActionCmdBase::AcEnd;
	m_SeqNum = 3;
}

//----------------------------------------------------------------------------
/**
*	@brief  �������V�[�P���X
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdWalk_32F::AcGridMoveInit( ActionCmdBase::ActionCmdWork* p_work )
{
	u32 frame = 32;
	ActionCmdAnalogMove::InitializeWork(  p_work,
										frame,
										gfl2::math::Vector(0.0f,0.0f,-100.0f,0.0f),	//�ړ��ʂ͓K���ł�
										p_work->qua,
										true,
										8 );										//�p���̕⊮�X�s�[�h�͓K���ł�
	return ActionCmdBase::ACMD_RET_CONTINUE;
}

} // mmodel
} // field
