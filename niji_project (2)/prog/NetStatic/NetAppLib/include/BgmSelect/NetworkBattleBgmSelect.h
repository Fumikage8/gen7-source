//======================================================================
/**
 * @file NetworkBattleBgmSelect.h
 * @brief 選べる戦闘曲
 */
//======================================================================

#if !defined __NETWORKBATTLEBGMSELECT_H_INCLUDED__
#define __NETWORKBATTLEBGMSELECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include "Savedata/include/EventWork.h"

#include <niji_conv_header/message/msg_battle_bgm_select.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BgmSelect)

enum
{
  FIXED_BGM_INDEX_MAX = 1,//最初の2つのBGMは開放固定
  BGM_TABLE_ELEM_NUM = 3,
  BGM_TABLE_COUNT_MAX = 44,
  BGM_TABLE_COUNT_MAX_NIJI = 19,
};

static const u32 SELECT_BGM_TABLE[BGM_TABLE_COUNT_MAX][BGM_TABLE_ELEM_NUM] =
{
  // BGMラベル、解放フラグ、メッセージラベル
  { STRM_BGM_VS_TRAINER_MJ     ,0                            , battle_bgm_select_cap_44 },//はじめから開放
  { STRM_BGM_VS_TRAINER_NJ     ,SYS_FLAG_BATTLE_BGM_SELECT_19, battle_bgm_select_cap_01 },//はじめから開放
  { STRM_BGM_MJ_M02            ,SYS_FLAG_BATTLE_BGM_SELECT_19, battle_bgm_select_cap_45 },//はじめから開放
  { STRM_BGM_VS_NORAPOKE_02    ,SYS_FLAG_BATTLE_BGM_SELECT_01, battle_bgm_select_cap_03 },
  { STRM_BGM_VS_GIVER		       ,SYS_FLAG_BATTLE_BGM_SELECT_02, battle_bgm_select_cap_04 },
  { STRM_BGM_VS_FRIEND_NJ		   ,SYS_FLAG_BATTLE_BGM_SELECT_05, battle_bgm_select_cap_05 }, 
  { STRM_BGM_VS_RIVAL_NJ		   ,SYS_FLAG_BATTLE_BGM_SELECT_06, battle_bgm_select_cap_06 },
  { STRM_BGM_VS_SKULL		       ,SYS_FLAG_BATTLE_BGM_SELECT_07, battle_bgm_select_cap_07 },
  { STRM_BGM_VS_SKULLELITE	   ,SYS_FLAG_BATTLE_BGM_SELECT_08, battle_bgm_select_cap_08 },
  { STRM_BGM_VS_SKULLBOSS	     ,SYS_FLAG_BATTLE_BGM_SELECT_09, battle_bgm_select_cap_09 },
  { STRM_BGM_MJ_VS07           ,SYS_FLAG_BATTLE_BGM_SELECT_27, battle_bgm_select_cap_26 },
  { STRM_BGM_VS_ETHER		       ,SYS_FLAG_BATTLE_BGM_SELECT_10, battle_bgm_select_cap_10 },
  { STRM_BGM_VS_MOTHER		     ,SYS_FLAG_BATTLE_BGM_SELECT_11, battle_bgm_select_cap_11 },
  { STRM_BGM_VS_BEAST		       ,SYS_FLAG_BATTLE_BGM_SELECT_13, battle_bgm_select_cap_13 },
  { STRM_BGM_MJ_VS08           ,SYS_FLAG_BATTLE_BGM_SELECT_28, battle_bgm_select_cap_27 },
  { STRM_BGM_MJ_VS09           ,SYS_FLAG_BATTLE_BGM_SELECT_29, battle_bgm_select_cap_28 },
  { STRM_BGM_VS_LEGEND02_NJ    ,SYS_FLAG_BATTLE_BGM_SELECT_17, battle_bgm_select_cap_16 },
  { STRM_BGM_VS_BROYAL		     ,SYS_FLAG_BATTLE_BGM_SELECT_19, battle_bgm_select_cap_17 },
  { STRM_BGM_VS_LEGEND01_NJ		 ,SYS_FLAG_BATTLE_BGM_SELECT_16, battle_bgm_select_cap_18 },
  { STRM_BGM_VS_SHITENNO_NJ    ,SYS_FLAG_BATTLE_BGM_SELECT_14, battle_bgm_select_cap_14 },
  { STRM_BGM_GAME_SYNC         ,SYS_FLAG_BATTLE_BGM_SELECT_15, battle_bgm_select_cap_46 },
  { STRM_BGM_VS_CHAMP_NJ		   ,SYS_FLAG_BATTLE_BGM_SELECT_15, battle_bgm_select_cap_15 },
  { STRM_BGM_VS_REDGREEN		   ,SYS_FLAG_BATTLE_BGM_SELECT_18, battle_bgm_select_cap_19 },
  { STRM_BGM_MJ_VS01           ,SYS_FLAG_BATTLE_BGM_SELECT_21, battle_bgm_select_cap_20 },
  { STRM_BGM_MJ_VS02           ,SYS_FLAG_BATTLE_BGM_SELECT_22, battle_bgm_select_cap_21 },
  { STRM_BGM_MJ_VS03           ,SYS_FLAG_BATTLE_BGM_SELECT_23, battle_bgm_select_cap_22 },
  { STRM_BGM_MJ_VS04           ,SYS_FLAG_BATTLE_BGM_SELECT_24, battle_bgm_select_cap_23 },
  { STRM_BGM_MJ_VS05           ,SYS_FLAG_BATTLE_BGM_SELECT_25, battle_bgm_select_cap_24 },
  { STRM_BGM_MJ_VS06           ,SYS_FLAG_BATTLE_BGM_SELECT_26, battle_bgm_select_cap_25 },
  { STRM_BGM_MJ_VSDIVEZONE02   ,SYS_FLAG_BATTLE_BGM_SELECT_31, battle_bgm_select_cap_30 },
  { STRM_BGM_MJ_VSDIVEZONE01   ,SYS_FLAG_BATTLE_BGM_SELECT_30, battle_bgm_select_cap_29 },
  { STRM_BGM_MJ_VSDIVEZONE08   ,SYS_FLAG_BATTLE_BGM_SELECT_37, battle_bgm_select_cap_36 },
  { STRM_BGM_MJ_VSDIVEZONE06   ,SYS_FLAG_BATTLE_BGM_SELECT_35, battle_bgm_select_cap_34 },
  { STRM_BGM_MJ_VSDIVEZONE07   ,SYS_FLAG_BATTLE_BGM_SELECT_36, battle_bgm_select_cap_35 },
  { STRM_BGM_MJ_VSDIVEZONE03   ,SYS_FLAG_BATTLE_BGM_SELECT_32, battle_bgm_select_cap_31 },
  { STRM_BGM_MJ_VSDIVEZONE05   ,SYS_FLAG_BATTLE_BGM_SELECT_34, battle_bgm_select_cap_33 },
  { STRM_BGM_MJ_VSDIVEZONE04   ,SYS_FLAG_BATTLE_BGM_SELECT_33, battle_bgm_select_cap_32 },
  { STRM_BGM_MJ_VSDIVEZONE11   ,SYS_FLAG_BATTLE_BGM_SELECT_40, battle_bgm_select_cap_39 },
  { STRM_BGM_MJ_VSDIVEZONE09   ,SYS_FLAG_BATTLE_BGM_SELECT_38, battle_bgm_select_cap_37 },
  { STRM_BGM_MJ_VSDIVEZONE10   ,SYS_FLAG_BATTLE_BGM_SELECT_39, battle_bgm_select_cap_38 },
  { STRM_BGM_MJ_VSDIVEZONE12   ,SYS_FLAG_BATTLE_BGM_SELECT_41, battle_bgm_select_cap_40 },
  { STRM_BGM_MJ_VSDIVEZONE13   ,SYS_FLAG_BATTLE_BGM_SELECT_42, battle_bgm_select_cap_41 },
  { STRM_BGM_MJ_VSDIVEZONE14   ,SYS_FLAG_BATTLE_BGM_SELECT_43, battle_bgm_select_cap_42 },
  { STRM_BGM_MJ_VSDIVEZONE16   ,SYS_FLAG_BATTLE_BGM_SELECT_45, battle_bgm_select_cap_43 },
};

//! nijiの選べる戦闘曲テーブル
static const u32 SELECT_BGM_TABLE_NIJI[BGM_TABLE_COUNT_MAX_NIJI][BGM_TABLE_ELEM_NUM] =
{
  // BGMラベル、解放フラグ、メッセージラベル
  { STRM_BGM_VS_TRAINER_NJ     ,0                            , battle_bgm_select_cap_01 },//はじめから開放
  { STRM_BGM_VS_NORAPOKE_NJ    ,SYS_FLAG_BATTLE_BGM_SELECT_12, battle_bgm_select_cap_02 },// @fix MMCat661 クリア後開放に変更
  { STRM_BGM_VS_NORAPOKE_02    ,SYS_FLAG_BATTLE_BGM_SELECT_01, battle_bgm_select_cap_03 },
  { STRM_BGM_VS_GIVER		       ,SYS_FLAG_BATTLE_BGM_SELECT_02, battle_bgm_select_cap_04 },
  { STRM_BGM_VS_FRIEND_NJ		   ,SYS_FLAG_BATTLE_BGM_SELECT_05, battle_bgm_select_cap_05 }, 
  { STRM_BGM_VS_RIVAL_NJ		   ,SYS_FLAG_BATTLE_BGM_SELECT_06, battle_bgm_select_cap_06 },
  { STRM_BGM_VS_SKULL		       ,SYS_FLAG_BATTLE_BGM_SELECT_07, battle_bgm_select_cap_07 },
  { STRM_BGM_VS_SKULLELITE	   ,SYS_FLAG_BATTLE_BGM_SELECT_08, battle_bgm_select_cap_08 },
  { STRM_BGM_VS_SKULLBOSS	     ,SYS_FLAG_BATTLE_BGM_SELECT_09, battle_bgm_select_cap_09 },
  { STRM_BGM_VS_ETHER		       ,SYS_FLAG_BATTLE_BGM_SELECT_10, battle_bgm_select_cap_10 },
  { STRM_BGM_VS_MOTHER		     ,SYS_FLAG_BATTLE_BGM_SELECT_11, battle_bgm_select_cap_11 },
  { STRM_BGM_VS_MOTHERBEAST    ,SYS_FLAG_BATTLE_BGM_SELECT_12, battle_bgm_select_cap_12 },
  { STRM_BGM_VS_BEAST		       ,SYS_FLAG_BATTLE_BGM_SELECT_13, battle_bgm_select_cap_13 },
  { STRM_BGM_VS_SHITENNO_NJ    ,SYS_FLAG_BATTLE_BGM_SELECT_14, battle_bgm_select_cap_14 },
  { STRM_BGM_VS_CHAMP_NJ		   ,SYS_FLAG_BATTLE_BGM_SELECT_15, battle_bgm_select_cap_15 },
  { STRM_BGM_VS_LEGEND02_NJ    ,SYS_FLAG_BATTLE_BGM_SELECT_17, battle_bgm_select_cap_16 },
  { STRM_BGM_VS_BROYAL		     ,SYS_FLAG_BATTLE_BGM_SELECT_19, battle_bgm_select_cap_17 },
  { STRM_BGM_VS_LEGEND01_NJ		 ,SYS_FLAG_BATTLE_BGM_SELECT_16, battle_bgm_select_cap_18 },
  { STRM_BGM_VS_REDGREEN		   ,SYS_FLAG_BATTLE_BGM_SELECT_18, battle_bgm_select_cap_19 }
};


//,SYS_FLAG_BATTLE_BGM_SELECT_01 STRM_BGM_VS_NORAPOKE_02	戦闘！ぬしポケモン
//,SYS_FLAG_BATTLE_BGM_SELECT_02	STRM_BGM_VS_GIVER		    戦闘！授ける者
//,SYS_FLAG_BATTLE_BGM_SELECT_05	STRM_BGM_VS_FRIEND_NJ		戦闘！おとなりさん
//,SYS_FLAG_BATTLE_BGM_SELECT_06	STRM_BGM_VS_RIVAL_NJ		戦闘！少女の兄
//,SYS_FLAG_BATTLE_BGM_SELECT_07	STRM_BGM_VS_SKULL		    戦闘！スカル団
//,SYS_FLAG_BATTLE_BGM_SELECT_08	STRM_BGM_VS_SKULLELITE	戦闘！スカル団幹部
//,SYS_FLAG_BATTLE_BGM_SELECT_09	STRM_BGM_VS_SKULLBOSS	  戦闘！スカル団ボス
//,SYS_FLAG_BATTLE_BGM_SELECT_10	STRM_BGM_VS_ETHER		    戦闘！エーテル財団トレーナー　（幹部なども含んで良い）
//,SYS_FLAG_BATTLE_BGM_SELECT_11	STRM_BGM_VS_MOTHER		  戦闘！エーテル財団ボス　（少女の母）通常
//,SYS_FLAG_BATTLE_BGM_SELECT_12	STRM_BGM_VS_MOTHERBEAST	戦闘！エーテル財団ボス　（少女の母）ビースト化
//,SYS_FLAG_BATTLE_BGM_SELECT_13	STRM_BGM_VS_BEAST		    戦闘！ビースト
//,SYS_FLAG_BATTLE_BGM_SELECT_14	STRM_BGM_VS_SHITENNO_NJ	戦闘！四天王　　（余力があれば、４バージョン別アレンジで）
//,SYS_FLAG_BATTLE_BGM_SELECT_15	STRM_BGM_VS_CHAMP_NJ		戦闘！チャンピオン
//,SYS_FLAG_BATTLE_BGM_SELECT_17	STRM_BGM_VS_LEGEND02_NJ	戦闘！伝説ポケモン
//,SYS_FLAG_BATTLE_BGM_SELECT_19	STRM_BGM_VS_BROYAL		  戦闘！バトルロイヤル
//,SYS_FLAG_BATTLE_BGM_SELECT_20	空き

class NetworkBattleBgmSelect
{
private:



  NetworkBattleBgmSelect();
  ~NetworkBattleBgmSelect();


  // BGMのラベル取得
  static u32 GetBgmTableBgmID( u8 index, const bool isNiji )
  {
    return isNiji ? SELECT_BGM_TABLE_NIJI[index][0] : SELECT_BGM_TABLE[index][0];
  }
  // BGM解放フラグ取得
  static u32 GetBgmTableFlag( u8 index, const bool isNiji )
  {
    return isNiji ? SELECT_BGM_TABLE_NIJI[index][1] : SELECT_BGM_TABLE[index][1];
  }
  // BGM曲名のメッセージラベル取得
  static u32 GetBgmTableTextLabel( u8 index, const bool isNiji )
  {
    return isNiji ? SELECT_BGM_TABLE_NIJI[index][2] : SELECT_BGM_TABLE[index][2];
  }
  // BGMテーブルの最大値取得
  static u32 GetBgmTableCountMax( const bool isNiji )
  {
    return isNiji ? BGM_TABLE_COUNT_MAX_NIJI : BGM_TABLE_COUNT_MAX;
  }

public:
 
  //! BGMのラベルID取得
  static u32 GetBgmID( u8 index, const bool isNiji = false )
  {
    const u32 bgmTableCountMax = GetBgmTableCountMax( isNiji );

    if( index >= bgmTableCountMax )
    {
      GFL_ASSERT( 0 ); //@fix
      return STRM_BGM_VS_TRAINER_NJ;
    }

    u8 totalIndex = GetTotalBgmIndex( index, isNiji );
    return GetBgmTableBgmID( totalIndex, isNiji );
  };

  //! BGM曲名のメッセージラベルID取得
  static u32 GetBgmTextLabel( u8 index, const bool isNiji = false )
  {
    const u32 bgmTableCountMax = GetBgmTableCountMax( isNiji );

    if( index >= bgmTableCountMax )
    {
      GFL_ASSERT( 0 ); //@fix
      return battle_bgm_select_cap_01;
    }

    u8 totalIndex = GetTotalBgmIndex( index, isNiji );
    return GetBgmTableTextLabel( totalIndex, isNiji );
  };


  //! BGMテーブルのMAXインデックス値を取得
  //  解放条件も内部で加味している
  static u8 GetMaxBgmIndex( const bool isNiji = false )
  {
    u32 bgmCount = 0;
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

    const u32 bgmTableCountMax = GetBgmTableCountMax( isNiji );
    for( u32 index = 0 ; index < bgmTableCountMax ; ++index )
    {
      u32 flagNo = GetBgmTableFlag( index, isNiji );
      if( index <= FIXED_BGM_INDEX_MAX || pEventWork->CheckEventFlag( flagNo ) )
      {
        bgmCount++;
      }
    }

    return bgmCount - 1;
  };

  //! 指定したインデックス値を、BGMテーブルの中でのインデックス値（BGMテーブル参照する用）を計算して取得
  static u8 GetTotalBgmIndex( u8 index, const bool isNiji = false )
  {
    u32 maxBgmIndex = GetMaxBgmIndex( isNiji );
    if( index > maxBgmIndex )
    {
      GFL_ASSERT( 0 ); //@fix
      return 0;
    }

    u32 bgmCount = 0;
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

    const u32 bgmTableCountMax = GetBgmTableCountMax( isNiji );
    for( u32 totalIndex = 0 ; totalIndex < bgmTableCountMax ; ++totalIndex )
    {
      u32 flagNo = GetBgmTableFlag( totalIndex, isNiji );
      if( totalIndex <= FIXED_BGM_INDEX_MAX || pEventWork->CheckEventFlag( flagNo ) )
      {
        if( bgmCount == index )
        {
          return totalIndex;
        }

        bgmCount++;
      }
    }

    return maxBgmIndex;
  };
};


GFL_NAMESPACE_END(BgmSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __NETWORKBATTLEBGMSELECT_H_INCLUDED__
