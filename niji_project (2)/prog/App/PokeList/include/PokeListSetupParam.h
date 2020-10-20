#ifndef NIJI_PROJECT_APP_POKELIST_SETUPPARAM_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_SETUPPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListSetupParam.h
 * @author  fukushima_yuya
 * @date    2015.03.26
 * @brief   外部設定パラメータ定義ヘッダー
 * @note    他の画面へのパラメータの受け渡しに必要なデータを定義
 */
//==============================================================================

// niji
#include "App/PokeList/include/PokeListIntermediateData.h"

// pml
#include <pml/include/pmlib.h>


//-----------------------------------------------------------------------------
// @brief   前方宣言
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class TimeLimit;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( Savedata )
class MyStatus;
GFL_NAMESPACE_END( Savedata )

class Regulation;


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//-----------------------------------------------------------------------------
/**
 * @brief   定数
 */
//-----------------------------------------------------------------------------
//! @brief   動作モード
enum Mode
{
  MODE_FIELD,         //!< フィールド(通常のリスト)
  MODE_JOIN,          //!< 参加選択
  MODE_SODATEYA,      //!< 育て屋
  MODE_SIMPLE,        //!< 対象選択
  MODE_SIMPLE2,       //!< 対象選択２
  MODE_WAZAOSHIE,     //!< 技教え
  MODE_BATTLE,        //!< バトル(ポケモン選択)
  MODE_COLOSSEUM,
  MODE_PROMOTION,
  MODE_NUM
};


//! @brief   終了モード
enum EndMode
{
  END_MODE_RETURN,        //!< 終了：戻るボタン
  END_MODE_EXIT,          //!< 終了：×ボタン
  END_MODE_SELECT,        //!< ポケモンが選択された
  END_MODE_SKILLUSE,      //!< 技が選択された
  END_MODE_DECIDE,        //!< 決定
  END_MODE_CANCEL,        //!< キャンセル
  END_MODE_TIMEUP,        //!< タイムアップ
  //END_MODE_OOZORAWOTOBU,  //!< おおぞらをとぶ」を選択

  END_MODE_CALL_STATUS,   //!< ステータス画面のコール
  END_MODE_CALL_BAG,      //!< バッグ画面のコール

  END_MODE_FORCED_RETURN_MANAGER   //!< ForcedReturnManagerによる終了
};

//! @brief    戦闘用のモード
enum BattleMode
{
  BTL_MODE_NORMAL,        //!< 通常
  BTL_MODE_MULTI_LEFT,    //!< マルチ戦闘(自分左側配置)
  BTL_MODE_MULTI_RIGHT,   //!< マルチ戦闘(自分右側配置)
  BTL_MODE_EXCHANGE,      //!< ポケモン入れ替え
  BTL_MODE_NUM,
  BTL_MODE_NULL
};

//! @brief    参加選択上画面の表示モード
enum JoinUppMode
{
  JOIN_UPP_MODE_SINGLE,   //!< 1vs1
  JOIN_UPP_MODE_TAG,      //!< 2vs2
  JOIN_UPP_MODE_ROYALE,   //!< 1vs1vs1vs1
  JOIN_UPP_MODE_BTL_TREE, //!< バトルツリー
  JOIN_UPP_MODE_NONE,     //!< パーティの表示をしない

  JOIN_UPP_MODE_NUM
};

enum 
{
  JOIN_SELECT_WAIT_NONE = 0,    //!< 選択中
  JOIN_SELECT_WAITING,          //!< 待機中
  JOIN_SELECT_NUM,

  JOIN_QRCODE_NONE = 0,         //!< QRコードなし
  JOIN_QRCODE_HAVE,             //!< QRコードあり
  JOIN_QRCODE_NUM,
};

//! @brief パーティID
enum PartyID
{
  PARTY_SELF,     //!< 自分のパーティ
  PARTY_FRIEND,   //!< 味方のパーティ
  PARTY_ENEMY0,   //!< 相手1のパーティ
  PARTY_ENEMY1,   //!< 相手1のパーティ
  PARTY_NUM
};

//! @brief  バトルの入れ替えモード
enum BattleChangeMode {
  BTL_CHANGE_SINGLE,    //!< １体入れ替え
  BTL_CHANGE_DOUBLE,    //!< ２体入れ替え
  BTL_CHANGE_NUM,
};


//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

/// 外部設定パラメータ
struct CONTEXT_PARAM {
  //------------------------------------------------------------
  // [ IN ]
  Mode mode;                            //!< 画面モード
  IntermediateData my_party;            //!< 自分のパーティ

  IntermediateData friend_party;        //!< 味方のパーティ
  IntermediateData enemy_party_0;       //!< 相手０のパーティ
  IntermediateData enemy_party_1;       //!< 相手１のパーティ

  Savedata::MyStatus* my_status;        //!< 自分のステータス
  Savedata::MyStatus* friend_status;    //!< 味方のステータス
  Savedata::MyStatus* enemy_status_0;   //!< 相手０のステータス
  Savedata::MyStatus* enemy_status_1;   //!< 相手１のステータス

  b8  list_break;                       //!< 外部からリストを強制終了させるフラグ(true = 終了)
  b8  cancel_flag;                      //!< キャンセルボタンの有効フラグ(true = 有効)

  //------------------------------
  // JOIN
  JoinUppMode join_upp_mode;          //!< 参加選択上画面の表示モード
  u8  join_count_min;                 //!< 参加させるポケモンの最少数
  u8  join_count_max;                 //!< 参加させるポケモンの最大数
  App::Tool::TimeLimit* time_limit;   //!< 制限時間クラス
  u8  selected_wait[PARTY_NUM];       //!< 選択後の待機表示フラグ(0：非表示、1：表示)
  u8  qrcord[PARTY_NUM];              //!< QRコード表示フラグ(0：非表示、1：表示)
  Regulation* regulation;             //!< レギュレーションデータ

  //------------------------------
  // WAZA
  u8 waza_set_param[pml::PokeParty::MAX_MEMBERS];     //!< 技習得状況 0=覚えられない、1=覚えられる、2=覚えている

  //------------------------------
  // SIMPLE
  b8 send_box_flag;   //!< ボックス送りかどうか(true = ボックス送り)

  //------------------------------
  // SIMPLE2
  u8 poke_select_enable[pml::PokeParty::MAX_MEMBERS]; //!< 選択有効状況 0=選べない、1=選べる

  //------------------------------
  // BATTLE
  BattleMode btl_mode;    //!< バトルモード
  const pml::pokepara::PokemonParam* select_poke_addr;    //!< 選択したポケモンのアドレス
  BattleChangeMode btl_change_mode;   //!< バトルの入れ替えモード
  bool play_sound;    //!< サウンドの再生フラグ false=鳴らさない, true=鳴らす
  bool from_top;      //!< 戦闘トップ画面からの遷移


  //------------------------------------------------------------
  // [ OUT ]
  EndMode end_mode;       //!< 終了モード
  b8  pokelist_wait;      //!< ポケモンリストが終了したことを外部に通知するフラグ(true = 終了)
  u8  poke_index;         //!< 選択されたポケモンのインデックス

  u16 f_waza_zone_id;     //!<「そらをとぶ」のタウンマップで選択したゾーンID
  WazaNo f_waza_wazano;   //!< 選択したフィールド技番号

  // 参加ポケモンのインデックス配列
  // 参加するポケモンのパーティ内インデックスが参加順に格納されます
  // 参加するポケモンがいない場合, -1 が格納されます
  s8 join_order[pml::PokeParty::MAX_MEMBERS];   //!< 参加ポケモンのインデックス配列


  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  CONTEXT_PARAM( void )
    : mode( MODE_NUM )
    , my_party()
    , friend_party()
    , enemy_party_0()
    , enemy_party_1()
    , my_status( NULL )
    , friend_status( NULL )
    , enemy_status_0( NULL )
    , enemy_status_1( NULL )
    , list_break( false )
    , cancel_flag( true )
    , join_upp_mode( JOIN_UPP_MODE_NUM )
    , join_count_min( 0 )
    , join_count_max( 0 )
    , time_limit( NULL )
    , send_box_flag( false )
    , btl_mode( BTL_MODE_NULL )
    , select_poke_addr( NULL )
    , btl_change_mode( BTL_CHANGE_SINGLE )
    , play_sound(true)
    , from_top(false)
    // [OUT]
    , end_mode( END_MODE_CANCEL )
    , pokelist_wait( false )
    , poke_index( 0 )
    , f_waza_zone_id( 0 )
    , f_waza_wazano( WAZANO_NULL )
  {
    for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      waza_set_param[i] = 0;
      poke_select_enable[i] = 0;
      join_order[i] = -1;
    }

    for( u8 i=0; i<PARTY_NUM; ++i )
    {
      selected_wait[i] = 0;
      qrcord[i] = 0;
    }
  }
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_SETUPPARAM_H_INCLUDED
