#if !defined(JOINFESTASHOPITEMINFO_H_INCLUDED)
#define JOINFESTASHOPITEMINFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaShopItemInfo.h
 * @date   2015/10/29 Thu. 15:15:21
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
  * @brief  商品在庫定義
  * @note   JC_Data2
  */
//------------------------------------------------------------------
struct  ShopItemInfo
{
  static const u32    SERVICEID_SLOT_MAX  = 32;
  static const u32    SERVICEID_INVALID   = UINT_MAX;

  s32         rank;                     /**<  ランク                  */
  u32         shopType;                 /**<  店種別(FacilitiesType)  */
  u32         messageID_ShopName;       /**<  店名                    */
  u32         messageID_Description;    /**<  説明                    */
  s32         price;                    /**<  価格                    */
  u32         isAvailableSun;           /**<  Sunで入手可能           */
  u32         isAvailableMoon;          /**<  Moonで入手可能          */

  /*  昼/夜リスト */
  u32         serviceID[SERVICEID_SLOT_MAX];    /**<  ShopServiceInfoTable.infosの添字  */


  /*
    funcs
  */
  static bool  IsValidServiceID(const u32 serviceID)                {return (serviceID != SERVICEID_INVALID);}
  static bool  IsValidSlot(const u32 slot)                          {return (slot < SERVICEID_SLOT_MAX);}

  bool  IsAvailable(const bool isSun) const                         {return (isSun ? isAvailableSun : isAvailableMoon);}

  u32   GetServiceID(const u32 slot) const                          {
                                                                      if(IsValidSlot(slot)) {
                                                                        return (serviceID[slot]);
                                                                      }
                                                                      return SERVICEID_INVALID;
                                                                    }

  u32   CountAvailableServices(void) const                          {
                                                                      u32 count = 0;
                                                                      for(u32 slot = 0; slot < SERVICEID_SLOT_MAX; ++slot)  {
                                                                        if(IsServiceAvailable(slot)) ++count;
                                                                      }
                                                                      return count;
                                                                    }

  /*  任意のSlotに商品が登録されているか  */
  bool  IsServiceAvailable(const u32 slot) const                    {return (IsValidSlot(slot) && IsValidServiceID(GetServiceID(slot)));}


  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};


} /*  namespace NetApp    */
} /*  namespace JoinFesta */
#endif  /*  #if !defined(JOINFESTASHOPITEMINFO_H_INCLUDED)  */
