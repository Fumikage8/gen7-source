#if !defined( __HAND_POS_H__ )
#define __HAND_POS_H__
//=============================================================================================
/**
 * @file    hand_pos.h
 * @brief   ポケモンXY バトルシステム イベントファクター [位置エフェクト]
 * @author  taya
 *
 * @date  2009.06.18  作成
 */
//=============================================================================================
#pragma once

#include "..\btl_common.h"
#include "..\btl_pokeparam.h"
#include "..\btl_server_const.h"
#include "..\btl_EventFactor.h"
#include "..\btl_PosEff.h"

GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace pos     {

extern BTL_EVENT_FACTOR*  Add( BtlPosEffect effect, BtlPokePos pos, u8 pokeID, const int* param, u8 param_cnt );
extern bool               IsRegistered( BtlPosEffect eff, BtlPokePos pos );

} // end of namespace 'pos'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)

#endif // __HAND_POS_H__
