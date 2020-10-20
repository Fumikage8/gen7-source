#if !defined(JOINFESTASHOPSERVICEINFO_H_INCLUDED)
#define JOINFESTASHOPSERVICEINFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaShopServiceInfo.h
 * @date   2015/10/29 Thu. 15:19:53
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <limits.h>
/*
  gfl2
*/
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
//#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>


namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  * @brief  商品定義
  * @note   JC_DATA1
  */
//------------------------------------------------------------------
struct  ShopServiceInfo
{
  static const u32  SERVICE_ARG_SLOT_MAX  = 2;        /**<  サービス引数の枠数  */
  static const u32  TALKMESSAGE_SLOT_MAX  = 2;        /**<  演出メッセージ枠数  */
  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  enum  ServiceType
  {
    /*
      パラメータ増分
      arg[0]  増分0
      arg[1]  増分1
    */
    SERVICETYPE_Familiarity,        /**<  なつき          */
    SERVICETYPE_Familiarity_HP,     /**<  なつき  HP      */
    SERVICETYPE_Familiarity_Atk,    /**<  なつき  攻撃    */
    SERVICETYPE_Familiarity_Def,    /**<  なつき  防御    */
    SERVICETYPE_Familiarity_SpAtk,  /**<  なつき  特攻    */
    SERVICETYPE_Familiarity_SpDef,  /**<  なつき  特防    */
    SERVICETYPE_Familiarity_Agi,    /**<  なつき  素早    */
    SERVICETYPE_LV,                 /**<  LV              */
    SERVICETYPE_EffortPowerHP,      /**<  努力値HP        */
    SERVICETYPE_EffortPowerAtk,     /**<  努力値攻撃      */
    SERVICETYPE_EffortPowerDef,     /**<  努力値防御      */
    SERVICETYPE_EffortPowerSpAtk,   /**<  努力値特攻      */
    SERVICETYPE_EffortPowerSpDef,   /**<  努力値特防      */
    SERVICETYPE_EffortPowerAgi,     /**<  努力値素早      */

    /*
      アイテム
      arg[0]  ItemID
      arg[1]  個数
    */
    SERVICETYPE_Item,               /**<  道具            */

    /*
      ビックリハウス
      arg[0]  LotteryTableのIndex
    */
    SERVICETYPE_Happening,          /**<  ビックリハウス  */

    /*
      くじ抽選
      arg[0]  LotteryTableのIndex
    */
    SERVICETYPE_Lottery,            /**<  くじ抽選        */


    SERVICETYPE_MAX,
    SERVICETYPE_FORCED_INT = INT_MAX
  };


  /*
    members
  */
  s32           isHallOfFame;                         /**<  殿堂入り後登場      */
  s32           messageID_ServiceName;                /**<  サービス名          */
  s32           coinPrice;                            /**<  コイン              */
  s32           messageID_Talk[TALKMESSAGE_SLOT_MAX]; /**<  演出メッセージ                                        */
  /*  サービス定義  */
  ServiceType   serviceType;                          /**<  サービス種別                                          */
  s32           serviceArgs[SERVICE_ARG_SLOT_MAX];    /**<  サービス引数（内容はServiceTypeによって解釈が異なる） */


  /*
    funcs
  */
  s32 GetArg(const u32 slot) const  {return ((slot < GFL_NELEMS(serviceArgs) ? serviceArgs[slot] : -1));}
  s32 GetPrice(void) const          {return coinPrice;}


  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};





} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  #if !defined(JOINFESTASHOPSERVICEINFO_H_INCLUDED)  */
