#if !defined( __BTL_SICK_H__ )
#define __BTL_SICK_H__
//=============================================================================================
/**
 * @file  btl_sick.h
 * @brief ポケモンXY バトルシステム 状態異常計算処理
 * @author  taya
 *
 * @date  2011.01.25  作成
 */
//=============================================================================================
#pragma once

#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_sickcont.h"
#include "btl_HeManager.h"

GFL_NAMESPACE_BEGIN(btl)

  class ServerFlow;

  extern void BTL_SICK_TurnCheckCallback( BTL_POKEPARAM* bpp, WazaSick sick, const BTL_SICKCONT& oldCont, bool fCure, ServerFlow* flowWk );
  extern void BTL_SICKEVENT_CheckFlying( const BTL_POKEPARAM* bpp );
  extern void BTL_SICKEVENT_CheckEscapeForbit( const BTL_POKEPARAM* bpp );
  extern void BTL_SICKEVENT_CheckNotEffectByType( const BTL_POKEPARAM* defender );
  extern void BTL_SICKEVENT_CheckPushOutFail( ServerFlow* flowWk, const BTL_POKEPARAM* bpp );
  extern void BTL_SICKEVENT_CheckAttackType( const BTL_POKEPARAM* bpp );
  extern bool BTL_SICK_MakeDefaultCureMsg( WazaSick sickID, const BTL_SICKCONT& oldCont, const BTL_POKEPARAM* bpp, u16 itemID, BTL_HANDEX_STR_PARAMS* str );
  extern void BTL_SICK_MakeDefaultMsg( WazaSick sickID, const BTL_SICKCONT& cont, const BTL_POKEPARAM* bpp, BTL_HANDEX_STR_PARAMS* str );
  extern bool BTL_SICK_CheckBatonTouchInherit(WazaSick sick, const BTL_POKEPARAM* bpp);
  extern bool BTL_SICK_MakeSickDamageMsg( BTL_HANDEX_STR_PARAMS* strParam, const BTL_POKEPARAM* bpp, WazaSick sickID );

GFL_NAMESPACE_END(btl)



#endif // __BTL_SICK_H__
