#if !defined(JOINFESTALOTINFO_H_INCLUDED)
#define JOINFESTALOTINFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaLotInfo.h
 * @date   2015/10/29 Thu. 15:21:27
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
  * @brief  抽選テーブル
  *         SERVICETYPE_Happening, SERVICETYPE_Lottery
  * @note   JC_DATA3
  */
//------------------------------------------------------------------
struct  LotInfo
{
  static const u32  SLOT_MAX  = 10;

  struct Item
  {
    s32   probabilityWeight;  /**<  確率  */
    u32   itemID;             /**<  アイテム  */
  }item[SLOT_MAX];


  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};




} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  #if !defined(JOINFESTALOTINFO_H_INCLUDED)  */
