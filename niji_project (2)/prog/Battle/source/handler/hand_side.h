#if !defined( __HAND_SIDE_H__ )
#define __HAND_SIDE_H__
//=============================================================================================
/**
 * @file    hand_side.h
 * @brief   ポケモンXY バトルシステム イベントファクター [サイドエフェクト]
 * @author  taya
 *
 * @date  2009.06.18  作成
 */
//=============================================================================================
#pragma once

#include "..\btl_common.h"
#include "..\btl_pokeparam.h"
#include "..\btl_sickcont.h"
#include "..\btl_server_const.h"
#include "..\btl_EventFactor.h"
#include "..\btl_ServerFlow.h"
#include "..\btl_SideEff.h"


GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace side    {

extern bool IsExist( BtlSide side, BtlSideEffect effect );

extern BTL_EVENT_FACTOR*  Add( BtlSide side, BtlSideEffect sideEffect, const BTL_SICKCONT& contParam );
extern bool               Remove( BtlSide side, BtlSideEffect sideEffect );
extern bool               Sleep( BtlSide side, BtlSideEffect sideEffect );
extern bool               Weak( BtlSide side, BtlSideEffect sideEffect );

#if 0

/**
 *  ターンチェック時に効果が切れたエフェクトの種類＆サイドを通知するためのコールバック関数型
 */
typedef void (*pSideEffEndCallBack)( BtlSide side, BtlSideEffect sideEffect, void* arg );

extern void BTL_HANDLER_SIDE_TurnCheck( pSideEffEndCallBack callBack, void* callbackArg );
#endif

//----------------------------------------------------------------------------------
} // end of namespace 'side'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)

#endif // __HAND_SIDE_H__
