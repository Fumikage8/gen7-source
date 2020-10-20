#if !defined( __HAND_TOKUSEI_H__ )
#define __HAND_TOKUSEI_H__
//=============================================================================================
/**
 * @file  hand_tokusei.h
 * @brief ポケモンXY バトルシステム イベントファクター [とくせい]
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
namespace tokusei {

extern BTL_EVENT_FACTOR*  Add( const BTL_POKEPARAM* bpp );
extern void               Remove( const BTL_POKEPARAM* bpp );
extern void               RotationSleep( const BTL_POKEPARAM* bpp );
extern void               RotationWake( const BTL_POKEPARAM* bpp );
extern void               RotationWakeNoAdd( const BTL_POKEPARAM* bpp );
extern void               Swap( const BTL_POKEPARAM* pp1, const BTL_POKEPARAM* pp2 );


} // end of namespace 'tokusei'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)


#endif // __HAND_TOKUSEI_H__
