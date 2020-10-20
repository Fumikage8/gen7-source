//=============================================================================
/**
 * @file   hand_default_power_up.h
 * @date   2015/09/09 11:14:44
 * @author obata_toshihiro
 * @brief  イベントファクター( デフォルト能力アップ )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined HAND_DEFAULT_POWER_UP_H_INCLUDED
#define HAND_DEFAULT_POWER_UP_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_DefaultPowerUp.h>

namespace btl {

  class BTL_POKEPARAM;


  namespace handler {
    namespace default_power_up {

      
      //-----------------------------------------------------------------------
      /**
       * @brief デフォルト能力アップに対するイベントファクターをシステムに追加する
       *
       * @param poke   能力アップするポケモン
       *
       * @retval true   ハンドラを追加した
       * @retval false  ハンドラを追加できなかった
       */
      //-----------------------------------------------------------------------
      extern bool Add( const BTL_POKEPARAM* poke );

      //-----------------------------------------------------------------------
      /**
       * @brief デフォルト能力アップに対するイベントファクターをシステムから除外する
       *
       * @param poke   能力アップするポケモン
       */
      //-----------------------------------------------------------------------
      extern void Remove( const BTL_POKEPARAM* poke );


    } // namespace default_power_up
  } // namespace handler
} // namespace btl


#endif // HAND_DEFAULT_POWER_UP_H_INCLUDED