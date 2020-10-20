//==============================================================================
/**
 * @file        TestBagAppParam.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/12/24(木) 16:41:41
 */
//==============================================================================
#if PM_DEBUG

#if !defined(__TESTBAGAPPPARAM_H__) // 重複定義防止
#define __TESTBAGAPPPARAM_H__ // 重複定義防止マクロ
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


GFL_NAMESPACE_BEGIN(takedaTest_app)

//! 呼び出しモード
enum CallMode
{
  CALL_FIELD_MENU = 0,    //!< フィールド: メニュー
  CALL_FIELD_POKELIST,    //!< フィールド: ポケモンリスト

  CALL_BATTLE_MENU,       //!< バトル: 通常
  CALL_BATTLE_USE,        //!< バトル: 道具使用

  CALL_BOX,               //!< ボックス
  CALL_SHOP,              //!< ショップ
};

//! 終了モード
enum EndMode
{
  END_CANCEL = 0,     //!< 終了（キャンセル）
  END_SELECT,         //!< 道具を選択

  END_CALL_WAZA_OBOE, //!< 技覚え画面を呼び出す
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
  u32 pokmon_index;                           //!< 対象ポケモンのインデックス　※ポケモンリスト/ボックスのみ

  itemman::ITEM_MANAGER * item_manager;       //!< アイテムマネージャ
  POCKET_DATA pocket[BATTLE_BAG_POCKET_MAX];  //!< ポケットデータ　※バトルのみ
  u16 pocket_max;                             //!< ポケット数　　　※バトルのみ

  // [in/out]
  u16 pocket_no;        //!< [in]: 開始時のポケット番号  [out]: 終了時のポケット番号

  u16 select_item;      //!< [in]: 使用する道具  [out]: 選択された道具
  u16 select_skill;     //!< [in]: 忘れる技      [out]: 覚えさせる技

  // [out]
  EndMode end_mode;       //!< 終了モード
  u16 select_pokemon_pos; //!< 選択したポケモンの位置
  u16 select_skill_pos;   //!< 選択した技の位置
};


GFL_NAMESPACE_END(takedaTest_app)

#endif  // BAG_EVENT_H_INCLUDED

#endif // PM_DEBUG
