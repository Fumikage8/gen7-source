#if PM_DEBUG

#if !defined(BATTLESPOTDEBUG_VIEWTEST_H_INCLUDED)
#define BATTLESPOTDEBUG_VIEWTEST_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotDebug_ViewTest.h
 * @date   2015/10/08 Thu. 12:04:06
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

//#include  "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include  "App/AppDemo/include/AppDemoUtility.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
namespace Debug       {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  */
//==============================================================================
class ViewTest
{
  GFL_FORBID_COPY_AND_ASSIGN(ViewTest);
public:
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_Exit,
      
      EVENT_
    };

    virtual void  ViewTest_OnEvent(ViewTest* pSender, const EventCode eventCode) = 0;
    virtual ~IEventHandler()  {}
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler  = pHandler;}


  ViewTest(BattleSpotWork* pWork, IEventHandler* pHandler);
  virtual ~ViewTest();


protected:
  void  Event(const IEventHandler::EventCode eventCode);

  typedef void(*ChangedValueFunc)( void *work );

  /*  changedValueCallback  */
  static void OnChangeScene(void* pArg);
  static void OnChangeViewMode(void* pArg);

  /*  command  */
  static void DoExit(void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void DoMemDump(void* _pThis, gfl2::debug::DebugWinItem* pItem);


  enum  {
    VALITEM_UpperView_Information_ViewMode,
    VALITEM_LowerView_Selection_ViewMode,
    VALITEM_LowerView_Card_ViewMode,
    VALITEM_LowerView_QR_ViewMode,
    
    VALITEM_MAX
  };

  BattleSpotWork*                           m_pWork;
  IEventHandler*                            m_pEventHandler;
  gfl2::debug::DebugWinGroup*               m_pGroup_Root;

  /*  items  */
  struct  ItemInfo
  {
    ViewTest*                               pThis;
    gfl2::debug::DebugWinItemValueU32<u32>* pViewItem;
    u32                                     value;
  };
  
  ItemInfo  m_SceneItemInfo;
  ItemInfo  m_ViewItemInfos[VALITEM_MAX];
};



} /*  namespace Debug     */
} /*  namespace BattleSpot */
} /*  namespace NetApp    */
#endif  /*  #if !defined(BATTLESPOTDEBUG_VIEWTEST_H_INCLUDED)  */

#endif
