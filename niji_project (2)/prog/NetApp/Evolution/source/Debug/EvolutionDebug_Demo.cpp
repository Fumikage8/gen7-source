
#if PM_DEBUG

//==============================================================================
/**
 * @file   EvolutionDebug_Demo.cpp
 * @date   2015/09/07 Mon. 12:04:56
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/EvolutionWork.h"
#include  "./EvolutionDebug_Demo.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

namespace NetApp    {
namespace Evolution {
namespace Debug     {
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
Demo::Demo(EvolutionWork* pWork, App::AppDemo::AppDemoUtility* pDemoUtil)
  : m_pWork(pWork)
  , m_pDemoUtil(pDemoUtil)
  , m_pGroup_Root(NULL)
{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(m_pWork->GetDeviceHeap(), pMsgData, msg_dmenu_evolution_top);  // "Evolution"

  /*  win  */
  {
    gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();
    
//    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_evolution_do_signal, this, DoSignal);   // "<DoSignal>"
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_evolution_do_signal, this, DoMemDump);   // "<DoSignal>"
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
Demo::~Demo()
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
void Demo::DoSignal(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Demo*  pThis = static_cast<Demo*>(_pThis);

    GFL_PRINT("<DoSignal>\n");
    pThis->m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SystemSync);
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
void Demo::DoMemDump(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Demo*  pThis = static_cast<Demo*>(_pThis);

    GFL_PRINT("<%s>\n", __FUNCTION__);
    pThis->m_pWork->GetDeviceHeap()->Dump();
    
    
    GFL_PRINT(">AppDemoUtility<\n");
    pThis->m_pDemoUtil->DumpHeap();
  }
}



} /*  namespace Debug     */
} /*  namespace Evolution */
} /*  namespace NetApp    */

#endif