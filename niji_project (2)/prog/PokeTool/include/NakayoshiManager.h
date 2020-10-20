//=============================================================================
/**
 * @brief  ポケモンの仲良し度更新管理
 * @file   NakayoshiManager.h
 * @author tamada
 * @date   2012.06.10
 */
//=============================================================================
#ifndef __NAKAYOSHIMANAGER_H__
#define __NAKAYOSHIMANAGER_H__

//#include <gflib.h>
#include "pml/include/pmlib.h"

#include "Savedata/include/MyStatus.h"
#include "Field/FieldRo/include/Weather/WeatherTypeDef.h"

///
namespace PokeTool {


  //------------------------------------------------------------------------
  /**
   * @brief 仲良し度更新マネジャー
   */
  //------------------------------------------------------------------------
  namespace nakayoshi {


      static const s32 WALK_EVENT_VALUE = -1;   ///<連れ歩いたときの減少値
      static const s32 POODLE_EVENT_VALUE = +1; ///<プードルイベントでの増加値

      static const s32 EASY_PLAY_MAX = 50;      ///< フィールド・バトルでの遊び度上昇限界

      //---------------------------------------------------------------------
      /**
       * @brief なかよし度取得処理
       * @param pp        対象となるポケモン
       * @param myStatus  対象となるトレーナーのMyStatus
       *
       * @return  u8    なかよし度
       */
      //---------------------------------------------------------------------
      extern u8 GetFriendship( const pml::pokepara::CoreParam * pp, const Savedata::MyStatus * myStatus );

      //---------------------------------------------------------------------
      /**
       * @brief 仲良し度更新処理
       * @param pp        対象となるポケモン
       * @param myStatus  現在のプレイヤーのMyStatus
       * @param value     なかよし度の変化量
       *
       * @note  親or登録されている仲良しトレーナーでない場合、何もしない
       */
      //---------------------------------------------------------------------
      extern void UpdateFriendship( pml::pokepara::CoreParam * pp, const Savedata::MyStatus * myStatus, s32 value );


      //---------------------------------------------------------------------
      /**
       * @brief   満腹度・なでなで度更新（フィールド移動中）
       * @param   party   手持ちポケパーティー
       */
      //---------------------------------------------------------------------
      extern void UpdateFeedAndNadenadeOnField( pml::PokeParty * party );

      //---------------------------------------------------------------------
      /**
       * @brief   満腹度・なでなで度更新（バトル参加後）
       * @param   party                 手持ちポケパーティー
       * @param   p_fight_index_array   戦闘に参加したかどうかのフラグ配列 BattleSetupParamのメンバを渡す
       */
      //---------------------------------------------------------------------
      extern void UpdateFeedAndNadenadeAfterBattle( pml::PokeParty * party, const u8* p_fight_index_array);

  } // namespace nakayoshi
}  // namespace PokeTool


#endif  // __NAKAYOSHIMANAGER_H__
