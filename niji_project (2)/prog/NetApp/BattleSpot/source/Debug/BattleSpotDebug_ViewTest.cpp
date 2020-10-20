
#if PM_DEBUG

//==============================================================================
/**
 * @file   BattleSpotDebug_ViewTest.cpp
 * @date   2015/10/08 Thu. 12:05:59
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/BattleSpotWork.h"
#include  "./BattleSpotDebug_ViewTest.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

namespace NetApp      {
namespace BattleSpot  {
namespace Debug       {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ViewTest::ViewTest(BattleSpotWork* pWork, IEventHandler* pEventHandler)
  : m_pWork(pWork)
  , m_pEventHandler(pEventHandler)
  , m_pGroup_Root(NULL)
{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(m_pWork->GetDeviceHeap(), pMsgData, msg_dmenu_battlespot_top);   // "BattleSpot::Debug::ViewTest"


  /*  win  */
  {
    gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();
    
    /*  scene  */
    m_SceneItemInfo.pThis     = this;
    m_SceneItemInfo.pViewItem = gfl2::debug::DebugWin_AddItemValueU32(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_battlespot_scene, &m_SceneItemInfo.value, 0, BattleSpotViewContainer::SCENE_MAX-1);   // "scene"
    m_SceneItemInfo.value     = 0;
    m_SceneItemInfo.pViewItem->SetChangedValueFunc(OnChangeScene, &m_SceneItemInfo);

    /*  views  */
#define VIEWDEF(_ItemID, _CaptionID, _min, _max)   do{ \
                                                    m_ViewItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ViewItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueU32(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, &m_ViewItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ViewItemInfos[(_ItemID)].value      = 0; \
                                                    m_ViewItemInfos[(_ItemID)].pViewItem->SetChangedValueFunc(OnChangeViewMode, &m_ViewItemInfos[(_ItemID)]); \
                                                  }while(false)

    VIEWDEF(VALITEM_UpperView_Information_ViewMode, msg_dmenu_battlespot_upper_info,    0, UpperView_Information::VIEWMODE_MAX-1); // "UpperView_Information"
    VIEWDEF(VALITEM_LowerView_Selection_ViewMode,   msg_dmenu_battlespot_lower_select,  0, LowerView_Selection::VIEWMODE_MAX-1);   // "LowerView_Selection"
    VIEWDEF(VALITEM_LowerView_Card_ViewMode,        msg_dmenu_battlespot_lower_card,    0, LowerView_Card::VIEWMODE_MAX-1);        // "LowerView_Card"

#undef  VIEWDEF

    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_battlespot_exit,     this, DoExit);     // "<Exit>"
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_battlespot_mem_dump, this, DoMemDump);  // "<MemDump>"
  }

  /*  resetview  */
  {
    BattleSpotViewContainer*  pContainer  = m_pWork->m_pViewContainer;

    pContainer->SetScene(static_cast<BattleSpotViewContainer::SceneID>(0));
    pContainer->m_pUpperView_Information->SetViewMode(static_cast<UpperView_Information::ViewMode>(0));
    pContainer->m_pLowerView_Selection->SetViewMode(static_cast<LowerView_Selection::ViewMode>(0));
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ViewTest::~ViewTest()
{
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void ViewTest::OnChangeScene(void* pArg)
{
  ItemInfo*                 pItemInfo   = static_cast<ItemInfo*>(pArg);
  ViewTest*                 pThis       = pItemInfo->pThis;
  BattleSpotViewContainer*  pContainer  = pThis->m_pWork->m_pViewContainer;


  pContainer->SetScene(static_cast<BattleSpotViewContainer::SceneID>(pItemInfo->value));

}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void ViewTest::OnChangeViewMode(void* pArg)
{
  ItemInfo*                 pItemInfo   = static_cast<ItemInfo*>(pArg);
  ViewTest*                 pThis       = pItemInfo->pThis;
  u32                       index;
  BattleSpotViewContainer*  pContainer  = pThis->m_pWork->m_pViewContainer;

  for(index = 0; index < GFL_NELEMS(pThis->m_ViewItemInfos); ++index)
  {
    if(pItemInfo  == &pThis->m_ViewItemInfos[index])
    {
      break;
    }
  }


  switch(index)
  {
    case VALITEM_UpperView_Information_ViewMode:  pContainer->m_pUpperView_Information->SetViewMode(static_cast<UpperView_Information::ViewMode>(pItemInfo->value));  break;
    case VALITEM_LowerView_Selection_ViewMode:    pContainer->m_pLowerView_Selection->SetViewMode(static_cast<LowerView_Selection::ViewMode>(pItemInfo->value));      break;
    case VALITEM_LowerView_Card_ViewMode:         pContainer->m_pLowerView_Card->SetViewMode(static_cast<LowerView_Card::ViewMode>(pItemInfo->value));                break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void ViewTest::DoExit(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    ViewTest*  pThis = static_cast<ViewTest*>(_pThis);

    GFL_PRINT("<%s>\n", __FUNCTION__);
    pThis->Event(IEventHandler::EVENT_Exit);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void ViewTest::DoMemDump(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    ViewTest*  pThis = static_cast<ViewTest*>(_pThis);

    GFL_PRINT("<%s>\n", __FUNCTION__);
    pThis->m_pWork->GetDeviceHeap()->Dump();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  ViewTest::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->ViewTest_OnEvent(this, eventCode);
  }
}



} /*  namespace Debug     */
} /*  namespace Evolution */
} /*  namespace NetApp    */


#endif