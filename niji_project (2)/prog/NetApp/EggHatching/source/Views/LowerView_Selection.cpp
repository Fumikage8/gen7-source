
//==============================================================================
/**
 * @file   LowerView_Selection.cpp
 * @date   2015/11/24 Tue. 12:58:38
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

#include  "../EggHatchingResource.h"
#include  "../System/EggHatchingWork.h"

#include  "./LowerView_Selection.h"

namespace NetApp    {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Selection::LowerView_Selection(EggHatchingWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pGeneralLowerView(NULL)
  , m_pHandler(NULL)
  , m_ViewMode(VIEWMODE_None)

{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();

  m_pGeneralLowerView = GFL_NEW(pHeapBase) app::tool::GeneralLowerView(m_pWork->GetAppHeap(), m_pWork->GetAppRenderingManager());
  m_pGeneralLowerView->SetEventHandler(this);
  m_pGeneralLowerView->SetResourceProvider(this);
  m_pGeneralLowerView->ObserveSetup();

  /*  se/shortcut  */
//  m_pGeneralLowerView->GetButtonManager()->SetButtonSelectSE(0, SEQ_SE_DECIDE1);
//  m_pGeneralLowerView->GetButtonManager()->SetButtonSelectSE(1, SEQ_SE_CANCEL1);
//  m_pGeneralLowerView->RegisterShortcut_B(app::tool::GeneralLowerView::BUTTON_1);
  m_pGeneralLowerView->SetLayoutMode(app::tool::GeneralLowerView::LAYOUTMODE_2Buttons);


  AddSubView(m_pGeneralLowerView);
  m_pGeneralLowerView->SetVisible(false);
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
  if(m_pGeneralLowerView) m_pGeneralLowerView->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pGeneralLowerView);
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
  m_pGeneralLowerView->SetVisible(true);
  m_pGeneralLowerView->SetModal(true);
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
  m_pGeneralLowerView->SetVisible(false);
  m_pGeneralLowerView->SetModal(false);
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
void  LowerView_Selection::SetViewMode(const ViewMode viewMode)
{

  m_ViewMode  = viewMode;
  m_pGeneralLowerView->SetButtonSEtoDefault();
  m_pGeneralLowerView->RegisterShortcut_B();
  m_pGeneralLowerView->CursorMoveTo(0);

  switch(m_ViewMode)
  {
    case  VIEWMODE_Nickname:    /**<  ニックネーム  */
      m_pGeneralLowerView->GetButtonManager()->SetButtonSelectSE(app::tool::GeneralLowerView::BUTTON_0, SEQ_SE_DECIDE1);
      m_pGeneralLowerView->GetButtonManager()->SetButtonSelectSE(app::tool::GeneralLowerView::BUTTON_1, SEQ_SE_CANCEL1);
      m_pGeneralLowerView->RegisterShortcut_B(app::tool::GeneralLowerView::BUTTON_1);

      m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button0, &m_pWork->GetMessageUtility().GetString(EGGHATCHING_MESSAGE_GRAC_ID, msg_egg_event_01_01));
      m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button1, &m_pWork->GetMessageUtility().GetString(EGGHATCHING_MESSAGE_GRAC_ID, msg_egg_event_01_01));
      break;

    case  VIEWMODE_AddTo:       /**<  手持ち/BOX    */
      m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button0, &m_pWork->GetMessageUtility().GetString(EGGHATCHING_MESSAGE_GRAC_ID, msg_egg_event_01_01));
      m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button1, &m_pWork->GetMessageUtility().GetString(EGGHATCHING_MESSAGE_GRAC_ID, msg_egg_event_01_01));
      break;

    case  VIEWMODE_None:  /*  fallthrough */
    default:
      break;
  }


  GetG2DUtil()->Update2D();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void*  LowerView_Selection::IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
{
  return m_pWork->GetResourceManager()->GetResourceBuffer(arcID, arcDatID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Selection::GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0:
      Event((m_ViewMode == VIEWMODE_Nickname) ? IEventHandler::EVENT_Nickname_Yes : IEventHandler::EVENT_AddTo_Party);
      break;
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1:
      Event((m_ViewMode == VIEWMODE_Nickname) ? IEventHandler::EVENT_Nickname_No : IEventHandler::EVENT_AddTo_Box);
      break;
  }
}



} /*  namespace EggHatching */
} /*  namespace NetApp    */
