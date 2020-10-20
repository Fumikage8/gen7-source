#if !defined( __HAND_WAZA_H__ )
#define __HAND_WAZA_H__
//=============================================================================================
/**
 * @file  hand_waza.h
 * @brief ポケモンXY バトルシステム イベントファクター [ワザ]
 * @author  taya
 *
 * @date  2011.01.20  作成
 */
//=============================================================================================
#pragma once

#include "../btl_common.h"
#include "../btl_pokeparam.h"
#include "../btl_EventFactor.h"


GFL_NAMESPACE_BEGIN(btl)
  namespace handler {
    namespace waza    {

      extern bool  Add( const BTL_POKEPARAM* bpp, WazaID waza, u32 subPri );
      extern void  Remove( const BTL_POKEPARAM* bpp, WazaID waza );
      extern void  RemoveForce( const BTL_POKEPARAM* bpp, WazaID waza );
      extern void  RemoveForceAll( const BTL_POKEPARAM* bpp );
      extern bool  IsStick( BTL_EVENT_FACTOR* myHandle, const int* work );
      extern bool IsExist( u8 pokeID, WazaID waza );

    } // end of namespace 'waza'
  } // end of namespace 'handler'
GFL_NAMESPACE_END(btl)



#endif // __HAND_WAZA_H__
