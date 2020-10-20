//=============================================================================
/**
 * @file    BagAppParam.h
 * @brief   バッグ画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================
#if !defined( BAG_APP_PARAM_H_INCLUDED )
#define BAG_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/MyItemSaveDef.h"
#include "App/PokeList/include/PokeListSetupParam.h"
#include "Field/FieldStatic/include/Item/FieldItemUse.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//! 呼び出しモード
enum CallMode
{
  CALL_FIELD_MENU = 0,    //!< フィールド: メニュー
  CALL_FIELD_POKELIST,    //!< フィールド: ポケモンリスト

  CALL_BATTLE_MENU,       //!< バトル: 通常

  CALL_BOX,               //!< ボックス
  CALL_SHOP,              //!< ショップ
};

//! 終了モード
enum EndMode
{
  END_CANCEL = 0,     //!< 終了（キャンセル）
  END_SELECT,         //!< 道具を選択

  END_CALL_WAZA_WASURE, //!< 技忘れ画面を呼び出す
  END_CALL_EVOLUTION,   //!< 進化画面を呼び出す
};

//! 復帰モード
enum RcvMode
{
  RCV_NONE = 0,   //!< 指定なし
  RCV_WAZAOSHIE,  //!< 技教え画面
  RCV_EVOLUTION,  //!< 進化デモ
};

//! ポケットデータ
struct POCKET_DATA
{
  Savedata::MyItem::ITEM_ST * table;  //!< 道具データテーブル
  u16 max;                            //!< テーブルの要素数

  u16 list_scroll:12;   //!< 道具リストのスクロール値
  u16 cursor_pos:4;     //!< 道具リストのカーソル位置
};

//! 外部設定パラメータ
struct APP_PARAM
{
  // [in]
  CallMode call_mode;   //!< 呼び出しモード

  App::PokeList::IntermediateData * pokemon;  //!< ポケモンデータ
  u32 pokemon_index;                          //!< 対象ポケモンのインデックス　※１匹表示するモードの場合

  itemman::ITEM_MANAGER * item_manager;       //!< アイテムマネージャ
  POCKET_DATA pocket[BATTLE_BAG_POCKET_MAX];  //!< ポケットデータ　※バトルのみ
  u8 pocket_max;                              //!< ポケット数　　　※バトルのみ

  u8 sasiosae_index;    //!<  技「さしおさえ」の影響を受けているポケモン（ビット管理）　※バトルのみ

  RcvMode rcv_mode;     //!< 復帰モード

  Field::FieldItemUse::ITEM_USE_CHECK_WORK itemuse_work;  //!< アイテム使用チェックワーク　※フィールドのみ

  Field::weather::WeatherKind weather;  //!< 天候（進化用）　※フォールドのみ

  // [in/out]
  u16 pocket_no;        //!< [in]: 開始時のポケット番号  [out]: 終了時のポケット番号

  u16 select_item;      //!< [in]: 使用する道具  [out]: 選択された道具
  u16 select_skill;     //!< 覚えさせる技
  u16 select_skill_pos; //!< 選択した技の位置

  // [out]
  EndMode end_mode;       //!< 終了モード
  u16 select_pokemon_pos; //!< 選択したポケモンの位置

  pml::pokepara::EvolveSituation evoSituation;  //!< 進化条件
  u32 lvupWazaSetIndex;                         //!< レベルアップ技覚えのインデッスクス
};


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)

#endif  // BAG_APP_PARAM_H_INCLUDED
