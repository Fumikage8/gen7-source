

#if PM_DEBUG

#if !defined(GIFTDEBUG_H_INCLUDED)
#define GIFTDEBUG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftDebug.h
 * @date   2016/03/25 Fri. 11:32:18
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>
#include  <system/include/Date/gfl2_Date.h>

#include  <NetStatic/NetAppLib/include/Debug/NetAppDebugTextDrawer.h>


namespace NetApp    {
namespace Gift  {
  class GiftWork;

namespace Debug {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  */
//==============================================================================
class GiftDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(GiftDebug);
public:
  GiftDebug(GiftWork* pWork);
  virtual ~GiftDebug();

  void  ApplyServerDate(const gfl2::system::Date& rServerDate);
  void  OnUpdate(void);
  void  OnDraw(const gfl2::gfx::CtrDisplayNo displayNo);

  bool  GetEshopTransitionCheckValue(void) const  {return (m_ViewItemInfos[VALITEM_IsEShopTransitionCheck].value != 0);}

#if defined( GF_PLATFORM_CTR )
  void  OverwriteServerTimeByLocalTimeIfEnable(nn::nex::DateTime & rNexDateTime);
#endif
  
protected:
  static void   FullFill(               void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void   DeleteAll(              void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void   NumFill(                void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void   NumDelete(              void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void   NumReceive(             void* _pThis, gfl2::debug::DebugWinItem* pItem);

  static void   SetSerialPenaltyCount(  void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void   ClearSerialPenalty(     void* _pThis, gfl2::debug::DebugWinItem* pItem);


  static void     Update_ServerTimeVal(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t* Draw_ServerTimeVal( void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  static void     Update_LocalTimeVal(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t* Draw_LocalTimeVal( void* _pThis, gfl2::debug::DebugWinItem* pWinItem);


  void      OnUpdate_TimeVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode);
  wchar_t*  OnDraw_TimeVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode);

  /*  receiveOnce  */
  static void     ClearAllReceiveOnceFlag(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_ReceiveOnceFlag(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t* Draw_ReceiveOnceFlag( void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  /*  randomDeliv  */
  static void     ClearAllRandomDelivFlag(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  static void     Update_RandomDelivFlagVal_00(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_01(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_02(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_03(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_04(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_05(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_06(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_07(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_08(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void     Update_RandomDelivFlagVal_09(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  static wchar_t*  Draw_RandomDelivFlagVal_00(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_01(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_02(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_03(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_04(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_05(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_06(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_07(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_08(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_RandomDelivFlagVal_09(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  void            OnUpdate_RandomDelivFlagVal (gfl2::debug::DebugWinItem* pWinItem, const u32 flagIndex);
  wchar_t*        OnDraw_RandomDelivFlagVal   (gfl2::debug::DebugWinItem* pWinItem, const u32 flagIndex);


  /*  onceADay  */
  static void  ClearAllReceiveOnceADayFlag(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  static void  Update_ReceiveOnceADayVal_00(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_01(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_02(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_03(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_04(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_05(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_06(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_07(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_08(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static void  Update_ReceiveOnceADayVal_09(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  static wchar_t*  Draw_ReceiveOnceADayVal_00(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_01(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_02(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_03(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_04(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_05(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_06(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_07(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_08(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);
  static wchar_t*  Draw_ReceiveOnceADayVal_09(void* _pThis, gfl2::debug::DebugWinItem* pWinItem);

  void      OnUpdate_ReceiveOnceADayVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode);
  wchar_t*  OnDraw_ReceiveOnceADayVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode);



  enum  GroupID
  {
    GROUP_ALBUM,
    GROUP_GIFTFLAG_ReceiveOnce,
    GROUP_GIFTFLAG_RandomDeliv,
    GROUP_GIFTFLAG_ReceiveOnceADay,
    
    GROUP_MAX
  };


  GiftWork*                       m_pWork;
  gfl2::debug::DebugWinGroup*     m_pGroup_Root;
  gfl2::debug::DebugWinGroup*     m_pGroups[GROUP_MAX];

  /*  items  */
  enum  {
    VALITEM_FillNum,
    VALITEM_DeleteNum,
    VALITEM_ReceiveNum,

    VALITEM_NGCount,
    /*  text  */
    VALITEM_IsDispPenaltyInfo,
    VALITEM_IsEShopTransitionCheck,
    
    /*  receiveOnce  */
    VALITEM_ReceiveOnceFlag_TargetEventID,    /**<  一度のみ受信フラグ：EventID  */
    
    /*  randomDeliv  */
    VALITEM_RandomDelivFlag_Index,            /**<  ランダム配信フラグ：Index  */
    
    VALITEM_OverwriteServerTimeByLocalTime,   /**<  認証Helperで取得したサーバ時刻を、ローカル時刻で上書きする  */
    
    VALITEM_MAX
  };

  struct  ItemInfo
  {
    GiftDebug*                              pThis;
    gfl2::debug::DebugWinItemValueU32<u32>* pViewItem;
    u32                                     value;
  };

  ItemInfo  m_ViewItemInfos[VALITEM_MAX];


  gfl2::system::Date    m_ServerDate;
  gfl2::system::Date    m_LocalDate;
  u32                   m_LocalDateTarget;

  NetAppLib::Debug::NetAppDebugTextDrawer m_Text;
};



} /*  namespace Debug     */
} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(GIFTDEBUG_H_INCLUDED)  */

#endif  /*  #if PM_DEMUG  */
