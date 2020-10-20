//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdAnalogMove.cpp
 *	@brief		フィールドアクションコマンドアナログ移動動作
 *	@author		yy-ikeuchi
 *	@date		2015.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "../include/FieldActionCmdAnalogMove.h"
#include "../include/movemodel.h"
#include "../include/field_transform.h"

namespace field
{
namespace mmodel
{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
*	@brief  グリッド移動共通処理    ワーク初期化
*
*	@param	p_work    ワーク
*	@param	dir       方向
*	@param	frame     フレーム
*	@param  offs_x    移動値に足しこむX座標
*	@param  offs_z        移動値に足しこむZ座標
*	@pram   dispDirUpdate 見た目の向きを更新するか
*/
//-----------------------------------------------------------------------------
void ActionCmdAnalogMove::InitializeWork( ActionCmdBase::ActionCmdWork* p_work, u32 frame, gfl2::math::Vector addTrans, gfl2::math::Quaternion qua, b32 isQuaUpdate, u32 interpFrameForQua )
{
	Work* p_wk = reinterpret_cast<Work*>(ActionCmdBase::GetAcmdWork( p_work, sizeof(Work) ));    
	p_wk->frame_max			= frame;
	p_wk->frame				= 0;
	p_wk->startTrans		= p_work->p_model->GetTransform()->GetTranslation();
	p_wk->targetTrans		= p_work->p_model->GetTransform()->GetTranslation() + addTrans;	//相対
	p_wk->startQua			= p_work->p_model->GetTransform()->GetQuaternion();
	p_wk->targetQua			= qua;	// 絶対
	p_wk->isQuaUpdate		= isQuaUpdate;
	p_wk->interpFrameForQua = interpFrameForQua;
	p_work->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD );

	// 次へ
	ActionCmdBase::AddAcmdWorkSeq( p_work, 1 );
}

//----------------------------------------------------------------------------
/**
*	@brief  移動
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdAnalogMove::AcGridMoveBase( ActionCmdBase::ActionCmdWork* p_work, b32 dispDirUpdate )
{
	Work* p_wk = reinterpret_cast<Work*>(ActionCmdBase::GetAcmdWork( p_work, sizeof(Work) ));

	// グリッド開始位置から、frameが進んだ分動かす。
	fld::mmodel::MoveModel* p_model = static_cast<fld::mmodel::MoveModel*>(p_work->p_model);
    
	p_wk->frame++;

	// 移動
	{
		f32 rate = static_cast<f32>(p_wk->frame) / static_cast<f32>(p_wk->frame_max);
		gfl2::math::Vector addTrans( (p_wk->targetTrans - p_wk->startTrans) * rate );
		p_model->GetTransform()->SetTranslation( p_wk->startTrans + addTrans );
	}

	// 姿勢
	if( p_wk->isQuaUpdate )
	{
		f32 rate = gfl2::math::min( 1.0f , static_cast<f32>(p_wk->frame) / static_cast<f32>(p_wk->interpFrameForQua) );
		gfl2::math::Quaternion qua = p_wk->startQua;
		qua.Slerp( p_wk->targetQua,rate );
		p_model->GetTransform()->SetQuaternion( qua );
	}

	// 座標更新
	p_model->GetTransform()->Update();
	
	if( p_wk->frame < p_wk->frame_max )
	{
		// １フレーム分移動
		return ActionCmdBase::ACMD_RET_ONE_TIME_END;
	}

	p_work->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD_END );

	// 次へ
	ActionCmdBase::AddAcmdWorkSeq( p_work, 1 );

	return ActionCmdBase::ACMD_RET_CONTINUE;
}
 
//----------------------------------------------------------------------------
/**
*	@brief  移動
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdAnalogMove::AcGridMove( ActionCmdBase::ActionCmdWork* p_work )
{
	return ActionCmdAnalogMove::AcGridMoveBase( p_work, true );
}

//===============================================================================================================================
/**
*    32Fで移動
*/
//===============================================================================================================================

//----------------------------------------------------------------------------
/**
*	@brief コンストラクタ
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
*	@brief  初期化シーケンス
*/
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdWalk_32F::AcGridMoveInit( ActionCmdBase::ActionCmdWork* p_work )
{
	u32 frame = 32;
	ActionCmdAnalogMove::InitializeWork(  p_work,
										frame,
										gfl2::math::Vector(0.0f,0.0f,-100.0f,0.0f),	//移動量は適当です
										p_work->qua,
										true,
										8 );										//姿勢の補完スピードは適当です
	return ActionCmdBase::ACMD_RET_CONTINUE;
}

} // mmodel
} // field
