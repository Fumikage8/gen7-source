//==============================================================================
/**
 * @file   LowerView_Selection.cpp
 * @date   2015/09/01 Tue. 15:24:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "Sound/include/Sound.h"

#include  "../EvolutionResource.h"
#include  "../System/EvolutionWork.h"

#include  "./LowerView_Selection.h"

namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Selection::LowerView_Selection(EvolutionWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_p2ButtonsView(NULL)
  , m_pHandler(NULL)

{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();

  m_p2ButtonsView = GFL_NEW(pHeapBase) app::tool::TwoButtonsView(m_pWork->GetAppHeap(), m_pWork->GetAppRenderingManager(), false);
  m_p2ButtonsView->Setup(m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_LayoutMenuWindow2), m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_LayoutCommonMenuCursor));
  m_p2ButtonsView->SetEventHandler(this);
  m_p2ButtonsView->SetTextBoxString(app::tool::TwoButtonsView::TEXTBOX_Button1, &m_pWork->GetMessageUtility().GetString(EVOLUTION_MESSAGE_GRAC_ID, msg_shinka_10_01));
  m_p2ButtonsView->SetTextBoxString(app::tool::TwoButtonsView::TEXTBOX_Button2, &m_pWork->GetMessageUtility().GetString(EVOLUTION_MESSAGE_GRAC_ID, msg_shinka_10_02));
  /*  se/shortcut  */
  m_p2ButtonsView->GetButtonManager()->SetButtonSelectSE(0, SEQ_SE_DECIDE1);
  m_p2ButtonsView->GetButtonManager()->SetButtonSelectSE(1, SEQ_SE_CANCEL1);
//  m_p2ButtonsView->RegisterShortcut_B(app::tool::TwoButtonsView::BUTTON_2of2);

  AddSubView(m_p2ButtonsView);
  m_p2ButtonsView->SetVisible(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
LowerView_Selection::~LowerView_Selection()
{
  if(m_p2ButtonsView) m_p2ButtonsView->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_p2ButtonsView);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Selection::ShowChoice(void)
{
  m_p2ButtonsView->CursorMoveTo(0);
  m_p2ButtonsView->SetVisible(true);
  m_p2ButtonsView->SetModal(true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Selection::HideChoice(void)
{
  m_p2ButtonsView->SetVisible(false);
  m_p2ButtonsView->SetModal(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*
void  LowerView_Selection::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}
*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_Selection_OnEvent(eventCode);
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
void  LowerView_Selection::TwoButtonsView_OnEvent(const app::tool::TwoButtonsView::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  app::tool::TwoButtonsView::IEventHandler::EVENT_OnButton1Click:
      Event(IEventHandler::EVENT_ForgetWaza);
      break;
    case  app::tool::TwoButtonsView::IEventHandler::EVENT_OnButton2Click:
      Event(IEventHandler::EVENT_KeepWaza);
      break;
  }
}



} /*  namespace Evolution */
} /*  namespace NetApp    */
