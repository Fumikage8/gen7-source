#if !defined( __HAND_ITEM_H__ )
#define __HAND_ITEM_H__
//=============================================================================================
/**
 * @file  hand_item.h
 * @brief ポケモンXY バトルシステム イベントファクター [アイテム]
 * @author  taya
 *
 * @date  2011.01.18  作成
 */
//=============================================================================================
#pragma once

#include "../btl_common.h"
#include "../btl_pokeparam.h"
#include "../btl_EventFactor.h"

GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace item    {

extern BTL_EVENT_FACTOR*  Add( const BTL_POKEPARAM* bpp );
extern void               Remove( const BTL_POKEPARAM* bpp );
extern BTL_EVENT_FACTOR*  TMP_Add( const BTL_POKEPARAM* bpp, u16 itemID );
extern void               TMP_Remove( BTL_EVENT_FACTOR* factor );
extern void               RotationSleep( const BTL_POKEPARAM* bpp );
extern void               RotationWake( const BTL_POKEPARAM* bpp );
extern void               RotationWakeNoAdd( const BTL_POKEPARAM* bpp );


} // end of namespace 'item'
} // end of namespace 'handler'

GFL_NAMESPACE_END(btl)

#endif // __HAND_ITEM_H__
