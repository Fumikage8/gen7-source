#if !defined( __BTL_ACTION_H__ )
#define __BTL_ACTION_H__
//=============================================================================================
/**
 * @file  btl_action.h
 * @brief ポケモンXYバトル プレイヤーが選択した動作内容の受け渡し構造体定義
 * @author  taya
 *
 * @date  2010.12.29  作成
 */
//=============================================================================================
#pragma once
#include <debug/include/gfl2_DebugPrint.h>

#include "./btl_Common.h"

GFL_NAMESPACE_BEGIN(btl)

class BTL_POKEPARAM;


//--------------------------------------------------------------
/**
 *  コマンド選択
 */
//--------------------------------------------------------------
typedef enum {
  BTL_ACTION_NULL=0,  ///< 何もしない（相手のポケモン選択待ち中など）

  BTL_ACTION_FIGHT,
  BTL_ACTION_ITEM,
  BTL_ACTION_CHANGE,
  BTL_ACTION_ESCAPE,
  BTL_ACTION_SKIP,      ///< 反動などで動けない
  BTL_ACTION_MEGAEVO,

  BTL_ACTION_RECPLAY_TIMEOVER,   ///< 録画データの時間制限による終了
  BTL_ACTION_RECPLAY_ERROR,

}BtlAction;


typedef union {

  u32 raw;

  struct {
    u32 cmd   : 4;
    u32 param : 28;
  }gen;

  struct {
    u32 cmd              :  4;
    u32 targetPos        :  4;
    u32 waza             : 16; ///< 使用する技の技No.( 全力技を使用する場合、元技の技No. )
    u32 wazaInfoFlag     :  1; ///< ワザ説明画面
    u32 megaEvoFlag      :  1; ///< メガ進化フラグ
    u32 zenryokuWazaFlag :  1; ///< 全力技フラグ
    u32 urtlaBurstFlag   :  1; ///< ウルトラバーストフラグ
    u32 _0               :  4;
  }fight;

  struct {
    u32 cmd             : 4;
    u32 targetIdx       : 3;  ///< 対象ポケモンのパーティ内インデックス
    u32 number          : 16; ///< アイテムID
    u32 param           : 8;  ///< サブパラメータ（PP回復時、どのワザに適用するか、など）
    u32 _1              : 1;
  }item;

  struct {
    u32 cmd         : 4;
    u32 posIdx      : 3;  // 入れ替え対象位置ID
    u32 memberIdx   : 3;  // 選ばれたポケモンのパーティ内インデックス
    u32 depleteFlag : 1;  // 入れ替えるポケモンがもういないことを通知するフラグ
    u32 _2          : 21;
  }change;

  struct {
    u32 cmd     : 4;
    u32 _3      : 28;
  }escape;

  struct {
    u32 cmd     :  4;
    u32 pokeIdx :  4;
    u32 _6      : 24;
  }mega_evo;

}BTL_ACTION_PARAM;


#if 1
// たたかうアクション
extern void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, WazaID waza, u8 targetPos, bool isZenryokuWaza=false );

extern void BTL_ACTION_ChangeFightTargetPos( BTL_ACTION_PARAM* p, u8 nextTargetPos );

extern void BTL_ACTION_FightParamToWazaInfoMode( BTL_ACTION_PARAM* p );

extern bool BTL_ACTION_IsWazaInfoMode( const BTL_ACTION_PARAM* p );

extern bool BTL_ACTION_IsFightWithMegaEvo( const BTL_ACTION_PARAM* p );
extern bool BTL_ACTION_IsFightWithZenryokuWaza( const BTL_ACTION_PARAM* p );
extern bool BTL_ACTION_IsFightWithUltraBurst( const BTL_ACTION_PARAM* p );

extern void BTL_ACTION_OffMegaEvo( BTL_ACTION_PARAM* p );
extern void BTL_ACTION_OffUltraBurst(BTL_ACTION_PARAM* p);

extern void BTL_ACTION_SetMegaEvoParam( BTL_ACTION_PARAM* p, u8 pokeIdx );
extern bool BTL_ACTION_IsMegaEvoParam( const BTL_ACTION_PARAM* p );

// アイテムつかうアクション
extern void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx, u8 wazaIdx );


// 入れ替えポケモン選択アクション（選択対象は未定）
extern void BTL_ACTION_SetChangeBegin( BTL_ACTION_PARAM* p );

// 入れ替えポケモン選択アクション（通常）
extern void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx );

// 入れ替えポケモン選択アクション（もう戦えるポケモンがいない）
extern void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p );


extern bool BTL_ACTION_IsDeplete( const BTL_ACTION_PARAM* p );

// 逃げるパラメータ設定
extern void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p );

// NULL（死んでいるので飛ばす）パラメータ設定
extern void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p );

extern void BTL_ACTION_SetSkip( BTL_ACTION_PARAM* p );

// コマンド取得
extern BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p );

// ワザID取得( たたかうアクション設定時のみ有効 )
extern WazaNo BTL_ACTION_GetWazaID( const BTL_ACTION_PARAM* act, const BTL_POKEPARAM* attackPoke );
extern WazaNo BTL_ACTION_GetOriginalWazaID( const BTL_ACTION_PARAM* act );

// ワザ対象位置を取得（たたかうアクション設定時のみ有効）
extern BtlPokePos  BTL_ACTION_GetWazaTargetPos( const BTL_ACTION_PARAM* act );


// 録画バッファあふれ通知コマンド通知
extern void BTL_ACTION_SetRecPlayOver( BTL_ACTION_PARAM* act );

inline void BTL_ACTION_Print( const BTL_ACTION_PARAM* ary, u32 num )
{
  static const char* name[] = {
    "NULL ",
    "FIGHT",
    "ITEM ",
    "CHANG",
    "ESCAP",
    "MOVE ",
    "SKIP ",
    "MEGEV",
    "REC_T",
    "REC_E",
  };


  for(u32 i=0; i<num; ++i)
  {
    int cmd = ary->gen.cmd;
    BTL_PRINT("[act:%d] Adrs:%p, cmd=%s, ", i, ary, name[cmd] );
    switch( cmd ){
    case BTL_ACTION_FIGHT:
        BTL_PRINT( "waza=%d\n", ary->fight.waza );
        break;
    case BTL_ACTION_ITEM:
        BTL_PRINT( "item=%d\n", ary->item.number );
        break;
    case BTL_ACTION_MEGAEVO:
        BTL_PRINT( "pokeIdx=%d\n", ary->mega_evo.pokeIdx );
        break;

    default:
      BTL_PRINT("\n");
    }
    ++ary;
  }
}



#endif

// 録画読み込み失敗（不正データ）通知
extern void BTL_ACTION_SetRecPlayError( BTL_ACTION_PARAM* act );

GFL_NAMESPACE_END(btl)


#endif // __BTL_ACTION_H__
