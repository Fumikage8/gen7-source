#if !defined(JOINFESTADATATABLE_H_INCLUDED)
#define JOINFESTADATATABLE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaDataTable.h
 * @date   2015/10/28 Wed. 19:52:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <stdio.h>  //NULL
#include  <limits.h>

/*
  gfl2
*/
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
//#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>
#include  <debug/include/gfl2_DebugPrint.h>
#include  <debug/include/gfl2_Assert.h>


/*
  infos
*/
#include  "./Info/JoinFestaLotInfo.h"
#include  "./Info/JoinFestaShopItemInfo.h"
#include  "./Info/JoinFestaShopServiceInfo.h"
#include  "./Info/JoinFestaAffinityAttractionInfo.h"
#include  "./Info/JoinFestaPokeTypeAttractionInfo.h"
#include  "./Info/JoinFestaKisekaePatternInfo.h"


// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
struct  ShopItemInfoTable
{
  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  enum  ShopItemID
  {
    /*  くじびき  */
    SHOPITEM_LOTTERY_00,
    SHOPITEM_LOTTERY_01,
    SHOPITEM_LOTTERY_02,
    SHOPITEM_LOTTERY_03,
    SHOPITEM_LOTTERY_04,
    SHOPITEM_LOTTERY_05,
    SHOPITEM_LOTTERY_06,
    SHOPITEM_LOTTERY_07,
    SHOPITEM_LOTTERY_08,
    SHOPITEM_LOTTERY_09,
    SHOPITEM_LOTTERY_10,
    SHOPITEM_LOTTERY_11,
    SHOPITEM_LOTTERY_12,
    SHOPITEM_LOTTERY_13,
    SHOPITEM_LOTTERY_14,
    /*  ビックリハウス  */
    SHOPITEM_HAPPENING_00,
    SHOPITEM_HAPPENING_01,
    SHOPITEM_HAPPENING_02,
    SHOPITEM_HAPPENING_03,
    SHOPITEM_HAPPENING_04,
    SHOPITEM_HAPPENING_05,
    SHOPITEM_HAPPENING_06,
    SHOPITEM_HAPPENING_07,
    SHOPITEM_HAPPENING_08,
    SHOPITEM_HAPPENING_09,
    SHOPITEM_HAPPENING_10,
    SHOPITEM_HAPPENING_11,
    SHOPITEM_HAPPENING_12,
    SHOPITEM_HAPPENING_13,
    SHOPITEM_HAPPENING_14,
    /*  ギフトショップ（おみやげ）  */
    SHOPITEM_GIFT_A_00,
    SHOPITEM_GIFT_A_01,
    SHOPITEM_GIFT_A_02,
    SHOPITEM_GIFT_A_03,
    SHOPITEM_GIFT_A_04,
    SHOPITEM_GIFT_A_05,
    SHOPITEM_GIFT_A_06,
    SHOPITEM_GIFT_A_07,
    SHOPITEM_GIFT_B_00,
    SHOPITEM_GIFT_B_01,
    SHOPITEM_GIFT_B_02,
    SHOPITEM_GIFT_B_03,
    SHOPITEM_GIFT_B_04,
    SHOPITEM_GIFT_B_05,
    SHOPITEM_GIFT_B_06,
    SHOPITEM_GIFT_B_07,
    SHOPITEM_GIFT_B_08,

    /*  屋台  */
    SHOPITEM_KITCHEN_A_00,
    SHOPITEM_KITCHEN_A_01,
    SHOPITEM_KITCHEN_A_02,
    SHOPITEM_KITCHEN_A_03,
    SHOPITEM_KITCHEN_A_04,
    SHOPITEM_KITCHEN_A_05,
    SHOPITEM_KITCHEN_A_06,
    SHOPITEM_KITCHEN_A_07,
    SHOPITEM_KITCHEN_B_00,
    SHOPITEM_KITCHEN_B_01,
    SHOPITEM_KITCHEN_B_02,
    SHOPITEM_KITCHEN_B_03,
    SHOPITEM_KITCHEN_B_04,
    SHOPITEM_KITCHEN_B_05,
    SHOPITEM_KITCHEN_B_06,
    SHOPITEM_KITCHEN_B_07,
    SHOPITEM_KITCHEN_B_08,
    SHOPITEM_KITCHEN_B_09,
    /*  エア遊具  */
    SHOPITEM_AIR_A_00,
    SHOPITEM_AIR_A_01,
    SHOPITEM_AIR_A_02,
    SHOPITEM_AIR_A_03,
    SHOPITEM_AIR_A_04,
    SHOPITEM_AIR_A_05,
    SHOPITEM_AIR_A_06,
    SHOPITEM_AIR_A_07,
    SHOPITEM_AIR_A_08,
    SHOPITEM_AIR_A_09,
    SHOPITEM_AIR_B_00,
    SHOPITEM_AIR_B_01,
    SHOPITEM_AIR_B_02,
    SHOPITEM_AIR_B_03,
    SHOPITEM_AIR_B_04,
    /*  占い  */
    SHOPITEM_FORTUNETELLING_RGB_00,
    SHOPITEM_FORTUNETELLING_RGB_01,
    SHOPITEM_FORTUNETELLING_RGB_02,
    SHOPITEM_FORTUNETELLING_GS_00,
    SHOPITEM_FORTUNETELLING_GS_01,
    SHOPITEM_FORTUNETELLING_GS_02,
    SHOPITEM_FORTUNETELLING_RS_00,
    SHOPITEM_FORTUNETELLING_RS_01,
    SHOPITEM_FORTUNETELLING_RS_02,
    SHOPITEM_FORTUNETELLING_DP_00,
    SHOPITEM_FORTUNETELLING_DP_01,
    SHOPITEM_FORTUNETELLING_DP_02,
    SHOPITEM_FORTUNETELLING_BW_00,
    SHOPITEM_FORTUNETELLING_BW_01,
    SHOPITEM_FORTUNETELLING_BW_02,
    SHOPITEM_FORTUNETELLING_XY_00,
    SHOPITEM_FORTUNETELLING_XY_01,
    SHOPITEM_FORTUNETELLING_XY_02,
    SHOPITEM_FORTUNETELLING_ALL_00,
    SHOPITEM_FORTUNETELLING_ALL_01,
    SHOPITEM_FORTUNETELLING_ALL_02,
    /*  染物  */
    SHOPITEM_DYE_RED_00,
    SHOPITEM_DYE_RED_01,
    SHOPITEM_DYE_RED_02,
    SHOPITEM_DYE_YELLOW_00,
    SHOPITEM_DYE_YELLOW_01,
    SHOPITEM_DYE_YELLOW_02,
    SHOPITEM_DYE_GREEN_00,
    SHOPITEM_DYE_GREEN_01,
    SHOPITEM_DYE_GREEN_02,
    SHOPITEM_DYE_BLUE_00,
    SHOPITEM_DYE_BLUE_01,
    SHOPITEM_DYE_BLUE_02,
    SHOPITEM_DYE_ORANGE_00,
    SHOPITEM_DYE_ORANGE_01,
    SHOPITEM_DYE_ORANGE_02,
    SHOPITEM_DYE_NAVY_00,
    SHOPITEM_DYE_NAVY_01,
    SHOPITEM_DYE_NAVY_02,
    SHOPITEM_DYE_PURPLE_00,
    SHOPITEM_DYE_PURPLE_01,
    SHOPITEM_DYE_PURPLE_02,
    SHOPITEM_DYE_PINK_00,
    SHOPITEM_DYE_PINK_01,
    SHOPITEM_DYE_PINK_02,

    // momiji追加
    /* こうかんじょ */
    SHOPITEM_BFES_00,
    SHOPITEM_BFES_01,
    SHOPITEM_BFES_02,
    
    SHOPITEM_MAX,
    SHOPITEM_FORCED_UINT = UINT_MAX
  };  /*  enum  ShopItemID  */


  /*  members  */
  ShopItemInfo  infos[SHOPITEM_MAX];


  static bool IsValidIndex(const u32 index)   {return (index < SHOPITEM_MAX);}

  static JoinFestaScript::FacilitiesType GetFacilitiesType( const u32 index )
  {
    /*  くじびき  */
    if( SHOPITEM_LOTTERY_00 <= index && index <= SHOPITEM_LOTTERY_14 )
    {
      return JoinFestaScript::FACILITIES_TYPE_LOTTERY;
    }
    /*  ビックリハウス  */
    else if( SHOPITEM_HAPPENING_00 <= index && index <= SHOPITEM_HAPPENING_14 )
    {
      return JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE;
    }
    /*  ギフトショップ（おみやげ）  */
    else if( SHOPITEM_GIFT_A_00 <= index && index <= SHOPITEM_GIFT_B_08 )
    {
      return JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP;
    }
    /*  屋台  */
    else if( SHOPITEM_KITCHEN_A_00 <= index && index <= SHOPITEM_KITCHEN_B_09 )
    {
      return JoinFestaScript::FACILITIES_TYPE_STALL;
    }
    /*  エア遊具  */
    else if( SHOPITEM_AIR_A_00 <= index && index <= SHOPITEM_AIR_B_04 )
    {
      return JoinFestaScript::FACILITIES_TYPE_AIR_TOY;
    }
    /*  占い  */
    else if( SHOPITEM_FORTUNETELLING_RGB_00 <= index && index <= SHOPITEM_FORTUNETELLING_ALL_02 )
    {
      return JoinFestaScript::FACILITIES_TYPE_FORTUNE;
    }
    /*  染物  */
    else if( SHOPITEM_DYE_RED_00 <= index && index <= SHOPITEM_DYE_PINK_02 )
    {
      return JoinFestaScript::FACILITIES_TYPE_DYEING;
    }
    /* こうかんじょ */
    else if( SHOPITEM_BFES_00 <= index && index <= SHOPITEM_BFES_02 )
    {
      return JoinFestaScript::FACILITIES_TYPE_TRADE;
    }

    GFL_ASSERT_MSG(0,"index%d\n",index);
    return JoinFestaScript::FACILITIES_TYPE_LOTTERY;
  }


  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
struct  ShopServiceInfoTable
{
  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  enum  ServiceID
  {
    /*  pokepara  */
    SERVICE_POKEPARA_00,
    SERVICE_POKEPARA_01,
    SERVICE_POKEPARA_02,
    SERVICE_POKEPARA_03,
    SERVICE_POKEPARA_04,
    SERVICE_POKEPARA_05,
    SERVICE_POKEPARA_06,
    SERVICE_POKEPARA_07,
    SERVICE_POKEPARA_08,
    SERVICE_POKEPARA_09,
    SERVICE_POKEPARA_10,
    SERVICE_POKEPARA_11,
    SERVICE_POKEPARA_12,
    SERVICE_POKEPARA_13,
    SERVICE_POKEPARA_14,
    SERVICE_POKEPARA_15,
    SERVICE_POKEPARA_16,
    SERVICE_POKEPARA_17,
    SERVICE_POKEPARA_18,
    SERVICE_POKEPARA_19,
    SERVICE_POKEPARA_20,
    SERVICE_POKEPARA_21,
    SERVICE_POKEPARA_22,
    SERVICE_POKEPARA_23,
    SERVICE_POKEPARA_24,
    SERVICE_POKEPARA_25,
    SERVICE_POKEPARA_26,
    SERVICE_POKEPARA_27,
    SERVICE_POKEPARA_28,
    SERVICE_POKEPARA_29,
    SERVICE_POKEPARA_30,
    SERVICE_POKEPARA_31,
    SERVICE_POKEPARA_32,
    SERVICE_POKEPARA_33,
    SERVICE_POKEPARA_34,
    SERVICE_POKEPARA_35,
    SERVICE_POKEPARA_36,
    SERVICE_POKEPARA_37,
    SERVICE_POKEPARA_38,
    SERVICE_POKEPARA_39,
    SERVICE_POKEPARA_40,
    SERVICE_POKEPARA_41,
    SERVICE_POKEPARA_42,
    SERVICE_POKEPARA_43,
    SERVICE_POKEPARA_44,
    SERVICE_POKEPARA_45,
    SERVICE_POKEPARA_46,
    SERVICE_POKEPARA_47,
    SERVICE_POKEPARA_48,
    SERVICE_POKEPARA_49,
    SERVICE_POKEPARA_50,
    SERVICE_POKEPARA_51,
    SERVICE_POKEPARA_52,
    SERVICE_POKEPARA_53,
    SERVICE_POKEPARA_54,
    SERVICE_POKEPARA_55,

    /*  item  */
    SERVICE_ITEM_00,
    SERVICE_ITEM_01,
    SERVICE_ITEM_02,
    SERVICE_ITEM_03,
    SERVICE_ITEM_04,
    SERVICE_ITEM_05,
    SERVICE_ITEM_06,
    SERVICE_ITEM_07,
    SERVICE_ITEM_08,
    SERVICE_ITEM_09,
    SERVICE_ITEM_10,
    SERVICE_ITEM_11,
    SERVICE_ITEM_12,
    SERVICE_ITEM_13,
    SERVICE_ITEM_14,
    SERVICE_ITEM_15,
    SERVICE_ITEM_16,
    SERVICE_ITEM_17,
    SERVICE_ITEM_18,
    SERVICE_ITEM_19,
    SERVICE_ITEM_20,
    SERVICE_ITEM_21,
    SERVICE_ITEM_22,
    SERVICE_ITEM_23,
    SERVICE_ITEM_24,
    SERVICE_ITEM_25,
    SERVICE_ITEM_26,
    SERVICE_ITEM_27,
    SERVICE_ITEM_28,
    SERVICE_ITEM_29,
    SERVICE_ITEM_30,
    SERVICE_ITEM_31,
    SERVICE_ITEM_32,
    SERVICE_ITEM_33,
    SERVICE_ITEM_34,
    SERVICE_ITEM_35,
    SERVICE_ITEM_36,
    SERVICE_ITEM_37,
    SERVICE_ITEM_38,
    SERVICE_ITEM_39,
    SERVICE_ITEM_40,
    SERVICE_ITEM_41,
    SERVICE_ITEM_42,
    SERVICE_ITEM_43,
    SERVICE_ITEM_44,
    SERVICE_ITEM_45,
    SERVICE_ITEM_46,
    SERVICE_ITEM_47,
    SERVICE_ITEM_48,
    SERVICE_ITEM_49,

    /*  ビックリハウス  */
    SERVICE_HAPPENING_00,
    SERVICE_HAPPENING_01,
    SERVICE_HAPPENING_02,
    SERVICE_HAPPENING_03,
    SERVICE_HAPPENING_04,
    SERVICE_HAPPENING_05,
    SERVICE_HAPPENING_06,
    SERVICE_HAPPENING_07,
    SERVICE_HAPPENING_08,
    SERVICE_HAPPENING_09,
    SERVICE_HAPPENING_10,
    SERVICE_HAPPENING_11,
    SERVICE_HAPPENING_12,
    SERVICE_HAPPENING_13,
    SERVICE_HAPPENING_14,
    SERVICE_HAPPENING_15,
    SERVICE_HAPPENING_16,
    SERVICE_HAPPENING_17,
    SERVICE_HAPPENING_18,
    SERVICE_HAPPENING_19,
    SERVICE_HAPPENING_20,
    SERVICE_HAPPENING_21,
    SERVICE_HAPPENING_22,
    SERVICE_HAPPENING_23,
    SERVICE_HAPPENING_24,
    SERVICE_HAPPENING_25,
    SERVICE_HAPPENING_26,
    SERVICE_HAPPENING_27,
    SERVICE_HAPPENING_28,
    SERVICE_HAPPENING_29,
    SERVICE_HAPPENING_30,
    SERVICE_HAPPENING_31,
    SERVICE_HAPPENING_32,
    SERVICE_HAPPENING_33,
    SERVICE_HAPPENING_34,
    SERVICE_HAPPENING_35,
    SERVICE_HAPPENING_36,
    SERVICE_HAPPENING_37,
    SERVICE_HAPPENING_38,
    SERVICE_HAPPENING_39,

    /*  くじびき  */
    SERVICE_LOT_00,
    SERVICE_LOT_01,
    SERVICE_LOT_02,
    SERVICE_LOT_03,
    SERVICE_LOT_04,
    SERVICE_LOT_05,
    SERVICE_LOT_06,
    SERVICE_LOT_07,
    SERVICE_LOT_08,
    SERVICE_LOT_09,
    SERVICE_LOT_10,
    SERVICE_LOT_11,
    SERVICE_LOT_12,
    SERVICE_LOT_13,
    SERVICE_LOT_14,
    SERVICE_LOT_15,

    /*  そめもの  */
    SERVICE_DYE_00,
    SERVICE_DYE_01,
    SERVICE_DYE_02,
    SERVICE_DYE_03,
    SERVICE_DYE_04,
    SERVICE_DYE_05,
    SERVICE_DYE_06,
    SERVICE_DYE_07,
    SERVICE_DYE_08,
    SERVICE_DYE_09,
    SERVICE_DYE_10,
    SERVICE_DYE_11,
    SERVICE_DYE_12,
    SERVICE_DYE_13,
    SERVICE_DYE_14,
    SERVICE_DYE_15,

    /*  占い  */
    SERVICE_FORTUNE_00,
    SERVICE_FORTUNE_01,
    SERVICE_FORTUNE_02,
    SERVICE_FORTUNE_03,
    SERVICE_FORTUNE_04,
    SERVICE_FORTUNE_05,
    SERVICE_FORTUNE_06,
    SERVICE_FORTUNE_07,
    SERVICE_FORTUNE_08,
    SERVICE_FORTUNE_09,
    SERVICE_FORTUNE_10,
    SERVICE_FORTUNE_11,
    SERVICE_FORTUNE_12,
    SERVICE_FORTUNE_13,
    SERVICE_FORTUNE_14,
    SERVICE_FORTUNE_15,


    SERVICE_MAX,
    SERVICE_FORCED_INT = INT_MAX
  };  /*  enum  ServiceID  */

  /*  members  */
  ShopServiceInfo infos[SERVICE_MAX];



  static bool IsValidIndex(const u32 index)   {return (index < SERVICE_MAX);}


  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
struct  LotInfoTable
{
  //------------------------------------------------------------------
  /**
    *   抽選
    */
  //------------------------------------------------------------------
  enum  LotTableID
  {
    /*  ビックリハウス  */
    LOTID_HAPPENING_POKE_00,
    LOTID_HAPPENING_POKE_01,
    LOTID_HAPPENING_POKE_02,
    LOTID_HAPPENING_POKE_03,
    LOTID_HAPPENING_POKE_04,
    LOTID_HAPPENING_POKE_05,
    LOTID_HAPPENING_POKE_06,
    LOTID_HAPPENING_POKE_07,
    LOTID_HAPPENING_POKE_08,
    LOTID_HAPPENING_POKE_09,
    LOTID_HAPPENING_POKE_10,
    LOTID_HAPPENING_POKE_11,
    LOTID_HAPPENING_POKE_12,
    LOTID_HAPPENING_POKE_13,
    LOTID_HAPPENING_POKE_14,
    LOTID_HAPPENING_POKE_15,
    LOTID_HAPPENING_POKE_16,
    LOTID_HAPPENING_POKE_17,
    LOTID_HAPPENING_POKE_18,
    LOTID_HAPPENING_POKE_19,
    LOTID_HAPPENING_POKE_20,
    LOTID_HAPPENING_PLAYER_00,
    LOTID_HAPPENING_PLAYER_01,
    LOTID_HAPPENING_PLAYER_02,
    LOTID_HAPPENING_PLAYER_03,
    LOTID_HAPPENING_PLAYER_04,
    LOTID_HAPPENING_PLAYER_05,
    LOTID_HAPPENING_PLAYER_06,
    LOTID_HAPPENING_PLAYER_07,
    LOTID_HAPPENING_PLAYER_08,
    LOTID_HAPPENING_PLAYER_09,
    LOTID_HAPPENING_PLAYER_10,
    LOTID_HAPPENING_PLAYER_11,
    LOTID_HAPPENING_PLAYER_12,
    LOTID_HAPPENING_PLAYER_13,
    LOTID_HAPPENING_PLAYER_14,
    LOTID_HAPPENING_PLAYER_15,
    LOTID_HAPPENING_PLAYER_16,
    LOTID_HAPPENING_PLAYER_17,
    LOTID_HAPPENING_PLAYER_18,
    LOTID_HAPPENING_PLAYER_19,
    LOTID_HAPPENING_PLAYER_20,
    /*  くじ屋  */
    LOTID_LOTTERY_00,
    LOTID_LOTTERY_01,
    LOTID_LOTTERY_02,
    LOTID_LOTTERY_03,
    LOTID_LOTTERY_04,
    LOTID_LOTTERY_05,
    LOTID_LOTTERY_06,
    LOTID_LOTTERY_07,
    LOTID_LOTTERY_08,
    LOTID_LOTTERY_09,
    LOTID_LOTTERY_10,
    LOTID_LOTTERY_11,
    LOTID_LOTTERY_12,
    LOTID_LOTTERY_13,
    LOTID_LOTTERY_14,
    LOTID_LOTTERY_15,
    LOTID_LOTTERY_16,
    LOTID_LOTTERY_17,
    LOTID_LOTTERY_18,
    LOTID_LOTTERY_19,
    /*  占いや  */
    LOTID_WORDI_00,
    LOTID_WORDI_01,
    LOTID_WORDI_02,
    LOTID_WORDI_03,
    LOTID_WORDI_04,
    LOTID_WORDI_05,
    LOTID_WORDI_06,
    LOTID_WORDI_07,
    LOTID_WORDI_08,
    LOTID_WORDI_09,
    LOTID_WORDI_10,
    LOTID_WORDI_11,
    LOTID_WORDI_12,
    LOTID_WORDI_13,
    LOTID_WORDI_14,
    LOTID_WORDI_15,
    LOTID_WORDI_16,
    LOTID_WORDI_17,
    LOTID_WORDI_18,
    LOTID_WORDI_19,
    LOTID_WORDI_20,


    LOTID_MAX,
    LOTID_FORCED_UINT  = UINT_MAX,
  };  /*  enum  LotTableID  */

  /*  members  */
  LotInfo infos[LOTID_MAX];


  static bool IsValidIndex(const u32 index)   {return (index < LOTID_MAX);}

  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};

//------------------------------------------------------------------
/**
  *  @brief     タイプ相性診断アトラクションに出てくるポケモンデータ構造テーブル
  */
//------------------------------------------------------------------
struct  AffinityAttractionInfoTable
{
  static const int AFFINITY_ID_MAX = 65;

  enum{
    DIFFICULTY_LEVEL_1 = 1, //!< 難易度レベル1
    DIFFICULTY_LEVEL_2,     //!< 難易度レベル2
    DIFFICULTY_LEVEL_3,     //!< 難易度レベル3
  };

  enum{
    DIFFICULTY_LEVEL_1_ID_MAX = 16, //!< 難易度レベル1のデータ件数
    DIFFICULTY_LEVEL_2_ID_MAX = 30, //!< 難易度レベル2のデータ件数
    DIFFICULTY_LEVEL_3_ID_MAX = 19, //!< 難易度レベル3のデータ件数
  };

  /*  members  */
  AffinityAttractionInfo infos[AFFINITY_ID_MAX];

  static bool IsValidIndex(const u32 index)   {return (index < AFFINITY_ID_MAX);}

  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};

//------------------------------------------------------------------
/**
  *  @brief     ポケモンタイプ相性アトラクションに出てくるポケモンデータ構造テーブル
  */
//------------------------------------------------------------------
struct  PokeTypeAttractionAlolaInfoTable
{
  static const int POKE_TYPE_ID_MAX = 217;

  /*  members  */
  PokeTypeAttractionInfo infos[POKE_TYPE_ID_MAX];
  
  static bool IsValidIndex(const u32 index)   {return (index < POKE_TYPE_ID_MAX);}

  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};

//------------------------------------------------------------------
/**
  *  @brief     ポケモンタイプ相性アトラクションに出てくるポケモンデータ構造テーブル
  */
//------------------------------------------------------------------
struct  PokeTypeAttractionAllInfoTable
{
  static const int POKE_TYPE_ID_MAX = 285;

  /*  members  */
  PokeTypeAttractionInfo infos[POKE_TYPE_ID_MAX];
  
  static bool IsValidIndex(const u32 index)   {return (index < POKE_TYPE_ID_MAX);}

  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};

//------------------------------------------------------------------
/**
  *  @brief  アトラクションの着せ替えダミーパターンテーブル
  */
//------------------------------------------------------------------
struct  KisekaePatternInfoTable
{
  /**
   *  @enum KisekaePatternID
   *        着せ替えパターンID
   */
  enum KisekaePatternID
  {
    KISEKAE_ID_A_00,  // Ａ	NPC基本データ
    KISEKAE_ID_A_01,  // Ａ	NPC基本データ
    KISEKAE_ID_A_02,  // Ａ	NPC基本データ
    KISEKAE_ID_A_03,  // Ａ	NPC基本データ
    KISEKAE_ID_A_04,  // Ａ	NPC基本データ
    KISEKAE_ID_A_05,  // Ａ	NPC基本データ
    KISEKAE_ID_A_06,  // Ａ	NPC基本データ
    KISEKAE_ID_A_07,  // Ａ	NPC基本データ
    KISEKAE_ID_A_08,  // Ａ	NPC基本データ
    KISEKAE_ID_A_09,  // Ａ	NPC基本データ
    KISEKAE_ID_A_10,  // Ａ	NPC基本データ
    KISEKAE_ID_A_11,  // Ａ	NPC基本データ
    KISEKAE_ID_A_12,  // Ａ	NPC基本データ
    KISEKAE_ID_A_13,  // Ａ	NPC基本データ
    KISEKAE_ID_A_14,  // Ａ	NPC基本データ
    KISEKAE_ID_A_15,  // Ａ	NPC基本データ
    KISEKAE_ID_A_16,  // Ａ	NPC基本データ
    KISEKAE_ID_A_17,  // Ａ	NPC基本データ
    KISEKAE_ID_A_18,  // Ａ	NPC基本データ
    KISEKAE_ID_A_19,  // Ａ	NPC基本データ
    KISEKAE_ID_A_20,  // Ａ	NPC基本データ
    KISEKAE_ID_A_21,  // Ａ	NPC基本データ
    KISEKAE_ID_A_22,  // Ａ	NPC基本データ
    KISEKAE_ID_A_23,  // Ａ	NPC基本データ
    KISEKAE_ID_A_24,  // Ａ	NPC基本データ
    KISEKAE_ID_A_25,  // Ａ	NPC基本データ
    KISEKAE_ID_A_26,  // Ａ	NPC基本データ
    KISEKAE_ID_A_27,  // Ａ	NPC基本データ
    KISEKAE_ID_A_28,  // Ａ	NPC基本データ
    KISEKAE_ID_A_29,  // Ａ	NPC基本データ
    KISEKAE_ID_A_30,  // Ａ	NPC基本データ
    // ↑FEMALE
    // ↓MALE
    KISEKAE_ID_A_31,  // Ａ	NPC基本データ
    KISEKAE_ID_A_32,  // Ａ	NPC基本データ
    KISEKAE_ID_A_33,  // Ａ	NPC基本データ
    KISEKAE_ID_A_34,  // Ａ	NPC基本データ
    KISEKAE_ID_A_35,  // Ａ	NPC基本データ
    KISEKAE_ID_A_36,  // Ａ	NPC基本データ
    KISEKAE_ID_A_37,  // Ａ	NPC基本データ
    KISEKAE_ID_A_38,  // Ａ	NPC基本データ
    KISEKAE_ID_A_39,  // Ａ	NPC基本データ
    KISEKAE_ID_A_40,  // Ａ	NPC基本データ
    KISEKAE_ID_A_41,  // Ａ	NPC基本データ
    KISEKAE_ID_A_42,  // Ａ	NPC基本データ
    KISEKAE_ID_A_43,  // Ａ	NPC基本データ
    KISEKAE_ID_A_44,  // Ａ	NPC基本データ
    KISEKAE_ID_A_45,  // Ａ	NPC基本データ
    KISEKAE_ID_A_46,  // Ａ	NPC基本データ
    KISEKAE_ID_A_47,  // Ａ	NPC基本データ
    KISEKAE_ID_A_48,  // Ａ	NPC基本データ
    KISEKAE_ID_A_49,  // Ａ	NPC基本データ
    KISEKAE_ID_A_50,  // Ａ	NPC基本データ
    KISEKAE_ID_A_51,  // Ａ	NPC基本データ
    KISEKAE_ID_A_52,  // Ａ	NPC基本データ
    KISEKAE_ID_A_53,  // Ａ	NPC基本データ
    KISEKAE_ID_A_54,  // Ａ	NPC基本データ
    KISEKAE_ID_A_55,  // Ａ	NPC基本データ
    KISEKAE_ID_A_56,  // Ａ	NPC基本データ
    KISEKAE_ID_A_57,  // Ａ	NPC基本データ
    KISEKAE_ID_A_58,  // Ａ	NPC基本データ
    KISEKAE_ID_A_59,  // Ａ	NPC基本データ
    KISEKAE_ID_A_60,  // Ａ	NPC基本データ

    KISEKAE_ID_B_00,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_01,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_02,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_03,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_04,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_05,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_06,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_07,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_08,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_09,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_10,  // Ｂ	色染め用・赤（正解）
    KISEKAE_ID_B_11,  // Ｂ	色染め用・赤（正解）

    KISEKAE_ID_C_00,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_01,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_02,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_03,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_04,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_05,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_06,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_07,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_08,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_09,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_10,  // Ｃ	色染め用・黄（正解）
    KISEKAE_ID_C_11,  // Ｃ	色染め用・黄（正解）

    KISEKAE_ID_D_00,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_01,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_02,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_03,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_04,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_05,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_06,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_07,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_08,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_09,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_10,  // Ｄ	色染め用・緑（正解）
    KISEKAE_ID_D_11,  // Ｄ	色染め用・緑（正解）

    KISEKAE_ID_E_00,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_01,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_02,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_03,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_04,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_05,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_06,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_07,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_08,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_09,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_10,  // Ｅ	色染め用・青（正解）
    KISEKAE_ID_E_11,  // Ｅ	色染め用・青（正解）

    KISEKAE_ID_F_00,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_01,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_02,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_03,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_04,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_05,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_06,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_07,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_08,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_09,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_10,  // Ｆ	色染め用・橙（正解）
    KISEKAE_ID_F_11,  // Ｆ	色染め用・橙（正解）

    KISEKAE_ID_G_00,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_01,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_02,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_03,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_04,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_05,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_06,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_07,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_08,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_09,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_10,  // Ｇ	色染め用・藍（正解）
    KISEKAE_ID_G_11,  // Ｇ	色染め用・藍（正解）

    KISEKAE_ID_H_00,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_01,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_02,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_03,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_04,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_05,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_06,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_07,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_08,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_09,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_10,  // Ｈ	色染め用・紫（正解）
    KISEKAE_ID_H_11,  // Ｈ	色染め用・紫（正解）

    KISEKAE_ID_I_00,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_01,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_02,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_03,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_04,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_05,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_06,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_07,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_08,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_09,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_10,  // Ｉ	色染め用・桃（正解）
    KISEKAE_ID_I_11,  // Ｉ	色染め用・桃（正解）

    KISEKAE_ID_J_00,  // Ｊ	色染め用・赤（不正解）
    KISEKAE_ID_J_01,  // Ｊ	色染め用・赤（不正解）
    KISEKAE_ID_J_02,  // Ｊ	色染め用・赤（不正解）
    KISEKAE_ID_J_03,  // Ｊ	色染め用・赤（不正解）
    KISEKAE_ID_J_04,  // Ｊ	色染め用・赤（不正解）
    KISEKAE_ID_J_05,  // Ｊ	色染め用・赤（不正解）
    KISEKAE_ID_J_06,  // Ｊ	色染め用・赤（不正解）

    KISEKAE_ID_K_00,  // Ｋ	色染め用・黄（不正解）
    KISEKAE_ID_K_01,  // Ｋ	色染め用・黄（不正解）
    KISEKAE_ID_K_02,  // Ｋ	色染め用・黄（不正解）
    KISEKAE_ID_K_03,  // Ｋ	色染め用・黄（不正解）
    KISEKAE_ID_K_04,  // Ｋ	色染め用・黄（不正解）
    KISEKAE_ID_K_05,  // Ｋ	色染め用・黄（不正解）
    KISEKAE_ID_K_06,  // Ｋ	色染め用・黄（不正解）

    KISEKAE_ID_L_00,  // Ｌ	色染め用・緑（不正解）
    KISEKAE_ID_L_01,  // Ｌ	色染め用・緑（不正解）
    KISEKAE_ID_L_02,  // Ｌ	色染め用・緑（不正解）
    KISEKAE_ID_L_03,  // Ｌ	色染め用・緑（不正解）
    KISEKAE_ID_L_04,  // Ｌ	色染め用・緑（不正解）
    KISEKAE_ID_L_05,  // Ｌ	色染め用・緑（不正解）
    KISEKAE_ID_L_06,  // Ｌ	色染め用・緑（不正解）

    KISEKAE_ID_M_00,  // Ｍ	色染め用・青（不正解）
    KISEKAE_ID_M_01,  // Ｍ	色染め用・青（不正解）
    KISEKAE_ID_M_02,  // Ｍ	色染め用・青（不正解）
    KISEKAE_ID_M_03,  // Ｍ	色染め用・青（不正解）
    KISEKAE_ID_M_04,  // Ｍ	色染め用・青（不正解）
    KISEKAE_ID_M_05,  // Ｍ	色染め用・青（不正解）
    KISEKAE_ID_M_06,  // Ｍ	色染め用・青（不正解）

    KISEKAE_ID_N_00,  // Ｎ	色染め用・橙（不正解）
    KISEKAE_ID_N_01,  // Ｎ	色染め用・橙（不正解）
    KISEKAE_ID_N_02,  // Ｎ	色染め用・橙（不正解）
    KISEKAE_ID_N_03,  // Ｎ	色染め用・橙（不正解）
    KISEKAE_ID_N_04,  // Ｎ	色染め用・橙（不正解）
    KISEKAE_ID_N_05,  // Ｎ	色染め用・橙（不正解）
    KISEKAE_ID_N_06,  // Ｎ	色染め用・橙（不正解）

    KISEKAE_ID_O_00,  // Ｏ	色染め用・藍（不正解）
    KISEKAE_ID_O_01,  // Ｏ	色染め用・藍（不正解）
    KISEKAE_ID_O_02,  // Ｏ	色染め用・藍（不正解）
    KISEKAE_ID_O_03,  // Ｏ	色染め用・藍（不正解）
    KISEKAE_ID_O_04,  // Ｏ	色染め用・藍（不正解）
    KISEKAE_ID_O_05,  // Ｏ	色染め用・藍（不正解）
    KISEKAE_ID_O_06,  // Ｏ	色染め用・藍（不正解）

    KISEKAE_ID_P_00,  // Ｐ	色染め用・紫（不正解）
    KISEKAE_ID_P_01,  // Ｐ	色染め用・紫（不正解）
    KISEKAE_ID_P_02,  // Ｐ	色染め用・紫（不正解）
    KISEKAE_ID_P_03,  // Ｐ	色染め用・紫（不正解）
    KISEKAE_ID_P_04,  // Ｐ	色染め用・紫（不正解）
    KISEKAE_ID_P_05,  // Ｐ	色染め用・紫（不正解）
    KISEKAE_ID_P_06,  // Ｐ	色染め用・紫（不正解）

    KISEKAE_ID_Q_00,  // Ｑ	色染め用・桃（不正解）
    KISEKAE_ID_Q_01,  // Ｑ	色染め用・桃（不正解）
    KISEKAE_ID_Q_02,  // Ｑ	色染め用・桃（不正解）
    KISEKAE_ID_Q_03,  // Ｑ	色染め用・桃（不正解）
    KISEKAE_ID_Q_04,  // Ｑ	色染め用・桃（不正解）
    KISEKAE_ID_Q_05,  // Ｑ	色染め用・桃（不正解）
    KISEKAE_ID_Q_06,  // Ｑ	色染め用・桃（不正解）

    KISEKAE_ID_R_00,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_01,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_02,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_03,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_04,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_05,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_06,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_07,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_08,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_09,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_10,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_11,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_12,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_13,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_14,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_15,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_16,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_17,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_18,  // Ｒ	変装用（帽子×眼鏡）
    KISEKAE_ID_R_19,  // Ｒ	変装用（帽子×眼鏡）

    KISEKAE_ID_S_00,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_01,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_02,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_03,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_04,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_05,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_06,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_07,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_08,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_09,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_10,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_11,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_12,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_13,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_14,  // Ｓ	変装用（帽子）
    KISEKAE_ID_S_15,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_16,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_17,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_18,  // Ｓ	変装用（眼鏡）
    KISEKAE_ID_S_19,  // Ｓ	変装用（眼鏡）

    KISEKA_PATTERN_ID_MAX,
  };

  /*  members  */
  KisekaePatternInfo infos[KISEKA_PATTERN_ID_MAX];

  static bool IsValidIndex(const u32 index)   {return (index < KISEKA_PATTERN_ID_MAX);}

  void  Dump(void) const
#if PM_DEBUG
  {for(u32 index = 0; index < GFL_NELEMS(infos); ++index){GFL_PRINT("{ /*  #%d  */\n", index); infos[index].Dump(); GFL_PRINT("},\n", index); }}
#else
  {}
#endif
};


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
struct  JoinFestaDataTable
{
  ShopItemInfoTable*      pShopItemInfoTable;
  ShopServiceInfoTable*   pShopServiceInfoTable;
  LotInfoTable*           pLotInfoTable;
  AffinityAttractionInfoTable*  pAffinityAttractionInfoTable;
  PokeTypeAttractionAlolaInfoTable*  pPokeTypeAttractionAlolaInfoTable;
  PokeTypeAttractionAllInfoTable*  pPokeTypeAttractionAllInfoTable;
  KisekaePatternInfoTable*      pKisekaePatternInfoTable;
  
  /*  ctor  */
  JoinFestaDataTable(void)
    : pShopItemInfoTable(NULL)
    , pShopServiceInfoTable(NULL)
    , pLotInfoTable(NULL)
    , pAffinityAttractionInfoTable(NULL)
    , pPokeTypeAttractionAlolaInfoTable(NULL)
    , pPokeTypeAttractionAllInfoTable(NULL)
    , pKisekaePatternInfoTable(NULL)
  {}



  const ShopItemInfo*     GetShopItemInfo(const u32 tableIndex) const      {return (ShopItemInfoTable::IsValidIndex(tableIndex) ? (&pShopItemInfoTable->infos[tableIndex]) : NULL);}
  const ShopServiceInfo*  GetShopServiceInfo(const u32 tableIndex) const   {return (ShopServiceInfoTable::IsValidIndex(tableIndex) ? (&pShopServiceInfoTable->infos[tableIndex]) : NULL);}
  const LotInfo*          GetLotInfo(const u32 tableIndex) const           {return (LotInfoTable::IsValidIndex(tableIndex) ? (&pLotInfoTable->infos[tableIndex]) : NULL);}

  const AffinityAttractionInfo* GetAffinityAttractionInfo(const u32 tableIndex) const {return (AffinityAttractionInfoTable::IsValidIndex(tableIndex) ? (&pAffinityAttractionInfoTable->infos[tableIndex]) : NULL);}
  const PokeTypeAttractionInfo* GetPokeTypeAttractionAlolaInfo(const u32 tableIndex) const {return (PokeTypeAttractionAlolaInfoTable::IsValidIndex(tableIndex) ? (&pPokeTypeAttractionAlolaInfoTable->infos[tableIndex]) : NULL);}
  const PokeTypeAttractionInfo* GetPokeTypeAttractionAllInfo(const u32 tableIndex) const {return (PokeTypeAttractionAllInfoTable::IsValidIndex(tableIndex) ? (&pPokeTypeAttractionAllInfoTable->infos[tableIndex]) : NULL);}

  const KisekaePatternInfo* GetKisekaePatternInfo(const u32 tableIndex) const {return (KisekaePatternInfoTable::IsValidIndex(tableIndex) ? (&pKisekaePatternInfoTable->infos[tableIndex]) : NULL);} 











  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  #if !defined(JOINFESTADATATABLE_H_INCLUDED)  */
