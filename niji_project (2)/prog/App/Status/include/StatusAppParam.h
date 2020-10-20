#if !defined( __APP_STATUS_APP_PARAM_H_INCLUDED__ )
#define __APP_STATUS_APP_PARAM_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusAppParam.h
 * @date    2015/11/27 15:54:06
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：外部設定パラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/PokeList/include/PokeListIntermediateData.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <Field/FieldStatic/include/BattleFes/BattleFesTool.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class TimeLimit;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Savedata)
class BoxPokemon;
class ResortSave;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! @brief  ステータス画面の表示モード
enum Mode {
  MODE_PARTY,         //!< 手持ちのポケモン
  MODE_BOX,           //!< ボックスのポケモン
  MODE_BATTLE_BOX,    //!< バトルボックスのポケモン
  MODE_BFC,           //!< バトルフェスサークル
  MODE_NUM,
};

static const u32 BOX_POKE_MAX = 30;

//------------------------------------------------------------------------------
// @brief   構造体
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [ in ]
  Mode  mode;                                   //!< ステータスの表示モード

  App::PokeList::IntermediateData party_data;   //!< パーティ

  Savedata::BoxPokemon* box_data;               //!< ボックスセーブデータ

  u32   box_tray_index;                         //!< 表示するトレーのインデックス

  App::Tool::TimeLimit* time_limit;             //!< 制限時間(通信モードで使用)

  bool  update_prohibition;                     //!< 更新禁止フラグ(true = 更新禁止、false = 更新可)

  bool  poke_lock[BOX_POKE_MAX];                //!< ポケモンの個別ロック(true = 更新禁止、false = 更新可)

  // リゾート用
  Savedata::ResortSave* resort_data;            //!< リゾートセーブデータ

  u8 resort_id;                                 //!< リゾートID

  u8 resort_group;                              //!< リゾートグループID

  // [ in / out ]
  u8    poke_index;                             //!< ポケモンのインデックス

  // ForBFC
  BattleFesTool::BFCTrainerInfo*  battle_fes_trainer_info_ptr[pml::PokeParty::MAX_MEMBERS];
  

  //------------------------------------------------------------------
  // @brief   コンストラクタ
  //------------------------------------------------------------------
  APP_PARAM( void )
    : mode(MODE_PARTY)
    , box_data(NULL)
    , box_tray_index(0)
    , time_limit(NULL)
    , update_prohibition(true)
    , resort_data(NULL)
    , resort_id(0)
    , resort_group(0)
    , poke_index(0)
  {
    for( u32 i=0; i<BOX_POKE_MAX; ++i )
    {
      poke_lock[i] = false;
    }
    for(u32 index = 0; index < GFL_NELEMS(battle_fes_trainer_info_ptr); ++index)
    {
      battle_fes_trainer_info_ptr[index] = NULL;
    }
  }

  bool  IsBFCMode(void ) const {return (mode == MODE_BFC);}
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_APP_PARAM_H_INCLUDED__
