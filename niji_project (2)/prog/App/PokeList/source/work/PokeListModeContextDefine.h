#ifndef NIJI_PROJECT_APP_POKELIST_MODECONTEXT_DEFINE_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_MODECONTEXT_DEFINE_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListModeContextDefine.h
 * @author  fukushima_yuya
 * @date    2015.03.05
 * @brief   各モードコンテキスト一覧
 */
//==============================================================================

// pml
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//! @brief   通常のリストモードの設定
struct FieldModeContext
{
  //Field::FieldSkillUse::SKILL_USE_CHECK_WORK  skillUseWork;
  b8* pokelist_wait;   //!< 外部に待機中を通知するフラグ
};


//! @brief    参加ポケモン選択モードの設定
struct JoinModeContext
{
  bool cancel_enable;                             //!< キャンセルできるかどうか
  u32  join_upp_mode;                             //!< 上画面の表示モード
  u8 join_poke_count_min;                         //!< 選択する参加ポケモン最少数
  u8 join_poke_count_max;                         //!< 選択する参加ポケモン最大数
  bool join_enable[pml::PokeParty::MAX_MEMBERS];  //!< 自分の各ポケモンが参加できるかどうか
  u32 time_limit_s;

  b8* list_break;       //!< 外部から終了を受け取るフラグ
  b8* pokelist_wait;    //!< 外部に待機中を通知するフラグ

  u8* selected_wait;    //!< 選択後の待機表示フラグ
  u8* qrcode;           //!< QRコード表示フラグ
};


//! @brief    対象選択モード
struct SimpleModeContext
{
  bool cancel_enable;   //!< キャンセルできるかどうか
  bool poke_select_enable[pml::PokeParty::MAX_MEMBERS];   //!< 選択有効状況 0=選べない、1=選べる

  b8  send_box_flag;    //!< ボックス送りかどうか(true = ボックス送り)
  b8* pokelist_wait;    //!< ポケモンリストが終了したことを外部に通知するフラグ(true = 終了)
};


//! @brief    技覚え選択モードの設定
struct WazaModeContext
{
  u8 waza_set_param[pml::PokeParty::MAX_MEMBERS];   //!< 技の習得状況
};


//! @brief    育て屋の預け選択モードの設定
struct SodateyaModeContext
{
  b8* pokelist_wait;   //!< ポケモンリストが終了したことを外部に通知するフラグ(true = 終了)
};

//! @brief    バトルモードの設定
struct BattleModeContext
{
  bool cancel_flag;
  b8* list_break;       //!< 外部から終了を受け取るフラグ
  u32 btl_mode;         //!< 戦闘モード
  u8* btl_change_mode;  //!< バトルの入れ替えモード

  b8* pokelist_wait;    //!< 外部に待機中を通知するフラグ

  bool from_top;        //!< トップ画面からの遷移
};


//! @brief   いずれかの動作モードの設定を保持する共用体
union ModeContext
{
  FieldModeContext    field;      //!< 通常のリストモードの設定
  JoinModeContext     join;       //!< 参加ポケモン選択モードの設定
  SimpleModeContext   simple;     //!< 対象選択モードの設定
  SodateyaModeContext sodateya;   //!< 育て屋の預け選択モードの設定
  WazaModeContext     waza;       //!< 技覚え選択モードの設定
  BattleModeContext   battle;     //!< バトルモードの設定
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_MODECONTEXT_DEFINE_H_INCLUDED
