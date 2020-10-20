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

#ifndef __FIELDACTIONCMDANALOGMOVE_H__
#define __FIELDACTIONCMDANALOGMOVE_H__

#include "FieldActionCmdBase.h"
#include <math/include/gfl2_Quaternion.h>
#include <math/include/gfl2_Vector.h>

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
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
// アナログ移動共通処理
class ActionCmdAnalogMove
{
public:

	struct Work
	{
		s16						frame_max;
		s16						frame;
		gfl2::math::Vector		startTrans;
		gfl2::math::Vector		targetTrans;
		gfl2::math::Quaternion	startQua;
		gfl2::math::Quaternion	targetQua;
		b32						isQuaUpdate;
		u32						interpFrameForQua;	
	};
  
public:

	// ワーク初期化
	static void InitializeWork( ActionCmdBase::ActionCmdWork*	p_work,
								u32								frame,
								gfl2::math::Vector				addTrans			= gfl2::math::Vector    (0.0f,0.0f,0.0f,0.0f),
								gfl2::math::Quaternion			addQua				= gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f),
								b32								isQuaUpdate			= true,
								u32								interpFrameForQua	= 1 );

	//-------------------------------------------------------------------------
	/**
	*	@brief  移動
	*/
	//-------------------------------------------------------------------------
	static ActionCmdBase::AcmdRet AcGridMoveBase( ActionCmdBase::ActionCmdWork* p_work, b32 dispDirUpdate = true );
	static ActionCmdBase::AcmdRet AcGridMove	( ActionCmdBase::ActionCmdWork* p_work );
};

//-----------------------------------------------------------------------------
/**
*    32Fで移動
*/
//-----------------------------------------------------------------------------
class ActionCmdWalk_32F : public ActionCmdBase
{
public:
	// コンストラクタ
	ActionCmdWalk_32F( void );

	// デストラクタ
	virtual ~ActionCmdWalk_32F(){}

	//----------------------------------------------------------------------------
	/**
	*	@brief  対応するアクションコマンドを返す
	*/
	//-----------------------------------------------------------------------------
	virtual Acmd GetAcmdID( void ) const{ return AC_WALK_32F; }

private:

	// 初期化
	static ActionCmdBase::AcmdRet AcGridMoveInit( ActionCmdBase::ActionCmdWork* p_work );
  
};

} // mmodel
} // field

#endif	// __FIELDACTIONCMDANALOGMOVE_H__

