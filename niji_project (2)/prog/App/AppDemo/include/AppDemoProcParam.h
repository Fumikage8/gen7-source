#if !defined(APPDEMOPROCPARAM_H_INCLUDED)
#define APPDEMOPROCPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   AppDemoProcParam.h
 * @date   2015/08/17 Mon. 17:40:07
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <arc_index/demo_seq_script.gaix>

#include  <pml/include/pmlib.h>
#include  <Savedata/include/MyStatus.h>

// レギュレーション
#include "Battle/Regulation/include/Regulation.h" // for BATTLE_RULE

namespace App {
namespace AppDemo {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/08/17 Mon. 17:40:49
  */
//==============================================================================
typedef struct AppDemoProcParam
{
  u32   demoGARCID;   /**<  再生したいDemoのGARCID（arc_index/demo_seq_script.gaix）  */

  /** ctor  */
  AppDemoProcParam(void)
    : demoGARCID(0)
  {}

  /** ctor  */
  AppDemoProcParam(const u32 _demoGARCID)
    : demoGARCID(_demoGARCID)
  {}
}AppDemoProcParam;



//------------------------------------------------------------------
/**
  *  @brief    ふしぎなおくりもの：おくりもの受信デモ
  */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_Gift
  : public  AppDemoProcParam
{
  static const u32  DEMO_GARCID_GIFT      = GARC_demo_seq_script_sd9120_mysterygift_BSEQ;
  
  enum  Type
  {
    TYPE_Pokemon_Gold,
    TYPE_Pokemon_Silver,
    TYPE_Pokemon_Bronze,
    TYPE_Item_Gold,
    TYPE_Item_Silver,
    TYPE_Item_Bronze,
    
    TYPE_MAX
  };



  pml::pokepara::PokemonParam*  pPokemonParams[1];
  Type                          type;

  /*  ctor  */
  AppDemoProcParam_Gift(void)
    : AppDemoProcParam(DEMO_GARCID_GIFT)
    , type(TYPE_Item_Bronze)
  {
    for(u32 index = 0; index < GFL_NELEMS(pPokemonParams); ++index)
    {
      pPokemonParams[index] = NULL;
    }
  }
}AppDemoProcParam_Gift;


//------------------------------------------------------------------
/**
  *  @brief    進化デモ
  */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_Evolution
  : public  AppDemoProcParam
{
  static const u32  DEMO_GARCID_EVOLUTION = GARC_demo_seq_script_sd9120_mysterygift_BSEQ;

  /*  in  */
  pml::pokepara::PokemonParam*  pPokemonParams[1];

  /*  out  */
  bool                          isEvolved;


  /*  ctor  */
  AppDemoProcParam_Evolution(void)
    : AppDemoProcParam(DEMO_GARCID_EVOLUTION)
    , isEvolved(false)
  {
    for(u32 index = 0; index < GFL_NELEMS(pPokemonParams); ++index)
    {
      pPokemonParams[index] = NULL;
    }
  }
}AppDemoProcParam_Evolution;


//------------------------------------------------------------------
/**
  *  @brief    GTS
  */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_GTS
  : public  AppDemoProcParam
{
  pml::pokepara::PokemonParam*  pSendPokemonParam;
  pml::pokepara::PokemonParam*  pRecvPokemonParam;
  bool                          bDownloadTrade;

  /*  ctor  */
  AppDemoProcParam_GTS(void) :
    AppDemoProcParam( GARC_demo_seq_script_sd9110_trade_all_BSEQ ),
    pSendPokemonParam( NULL ),
    pRecvPokemonParam( NULL ),
    bDownloadTrade( false )
  {
  }
}AppDemoProcParam_GTS;


//------------------------------------------------------------------
/**
  *  @brief    ミラクル交換
  */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_MiracleTrade
  : public  AppDemoProcParam
{
  pml::pokepara::PokemonParam*  pSendPokemonParam;
  pml::pokepara::PokemonParam*  pRecvPokemonParam;
  Savedata::MyStatus*           pTradePersonStatus;
  gfl2::heap::HeapBase*         pHeap;


  /*  ctor  */
  AppDemoProcParam_MiracleTrade(void) :
    AppDemoProcParam( GARC_demo_seq_script_sd9110_trade_all_BSEQ ),
    pSendPokemonParam( NULL ),
    pRecvPokemonParam( NULL ),
    pTradePersonStatus( NULL ),
    pHeap( NULL )
  {
  }
}AppDemoProcParam_MiracleTrade;

//------------------------------------------------------------------
/**
  *  @brief    おめでとう演出
  *  @auther  k.ohno
  */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_Congrats
  : public  AppDemoProcParam
{
  /* in */
  pml::PokeParty*               pPokeParty;
  Savedata::MyStatus*           pMyStatus;
  gfl2::heap::HeapBase*         pHeap;
  u32                           defensedCount;  //!< 防衛回数
  bool                          isFirstClear;   //!< 初回クリアかどうか


  /*  ctor  */
  AppDemoProcParam_Congrats(void) :
    AppDemoProcParam( 0 ),  //分岐するのでSetGArcIdタ内で行う
    pPokeParty( NULL ),
    pMyStatus( NULL ),
    pHeap( NULL ),
    defensedCount(0),
    isFirstClear( true )
  {
  }

  void SetGArcId()
  {
    static const u32 DEMO_GARCID_CONGRATS[6] = {
      GARC_demo_seq_script_sd9060_dendouiri01_BSEQ, //1匹
      GARC_demo_seq_script_sd9060_dendouiri02_BSEQ, //2匹
      GARC_demo_seq_script_sd9060_dendouiri03_BSEQ, //3匹
      GARC_demo_seq_script_sd9060_dendouiri04_BSEQ, //4匹
      GARC_demo_seq_script_sd9060_dendouiri05_BSEQ, //5匹
      GARC_demo_seq_script_sd9060_dendouiri06_BSEQ, //6匹
    };

    //タマゴは除外
    u32 count = pPokeParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_NOT_EGG );
    if( count == 0 )
    {
      GFL_ASSERT_STOP(0);
    }
    else
    {
      demoGARCID = DEMO_GARCID_CONGRATS[count-1];
    }
  }
}AppDemoProcParam_Congrats;



//------------------------------------------------------------------
/**
  *  @brief    VSデモ
  *  @auther  ichiraku
  */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_VS
  : public  AppDemoProcParam
{
  Savedata::MyStatus*           pMyStatus[4];         //!< プレイヤー
  u8                            pokemonStatus[4][6];  //!< ポケモン状態(0:無効値 1:通常 2:状態異常 3:瀕死）
  int                           bgType;               //!< 背景(0:通常背景 1:WCS予選 2:WCS決勝）

  // デモ種類の定義
  /*
  enum
  {
    VSBATTLE_PRESTART = 0,
    VSBATTLE_WIN,
    VSBATTLE_LOSE,
    VSBATTLE_DRAW,  
  };
  */
  int                           demoType;             //!< デモ種類
  Regulation::BATTLE_RULE       battleRule;           //!< バトルルール

  /*  ctor  */
  AppDemoProcParam_VS( Regulation::BATTLE_RULE battleRule ) :
    AppDemoProcParam(),
    battleRule(battleRule)
  {
    for( u32 i=0; i<4; i++ )
    {
      pMyStatus[i] = NULL;
      for( u32 j=0; j<6; j++){ pokemonStatus[i][j] = 0; }
    }
    bgType = 0;
    demoType = 0;

    if( battleRule == Regulation::BATTLE_RULE_ROYAL ) 
    {
      demoGARCID = GARC_demo_seq_script_sd9244_battle_royal_BSEQ;
      ICHI_PRINT("VSデモ ロイヤル\n");
    }
    else if( battleRule == Regulation::BATTLE_RULE_MULTI ) 
    {
      demoGARCID = GARC_demo_seq_script_sd9241_tsushin_battle_multi_BSEQ;
      ICHI_PRINT("VSデモ マルチ\n");
    }
    else
    {
      demoGARCID = GARC_demo_seq_script_sd9240_tsushin_battle_BSEQ;
      ICHI_PRINT("VSデモ 通常\n");
    }
  }
}AppDemoProcParam_VS;


//------------------------------------------------------------------
/**
 * @brief   イントロ後半
 * @auther  Hiroyuki Nakamura
 */
//------------------------------------------------------------------
typedef struct  AppDemoProcParam_Intro2nd
  : public  AppDemoProcParam
{
  u32 hero_index;   //!< 容姿選択で選択されたインデックス

  /*  ctor  */
  AppDemoProcParam_Intro2nd( u32 arc_id, u32 hero )
    : AppDemoProcParam( arc_id )
    , hero_index( hero )
  {
  }
}AppDemoProcParam_Intro2nd;


} /*  namespace AppDemo */
} /*  namespace App     */
#endif  /*  #if !defined(APPDEMOPROCPARAM_H_INCLUDED)  */
