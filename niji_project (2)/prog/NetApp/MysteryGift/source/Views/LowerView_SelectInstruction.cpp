//==============================================================================
/**
 * @file   LowerView_SelectInstruction.cpp
 * @date   2016/04/04 Mon. 20:36:22
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../GiftResource.h"
#include  "../ApplicationSystem/GiftWork.h"

#include  "./LowerView_SelectInstruction.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_SelectInstruction::LowerView_SelectInstruction(GiftWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pHandler(NULL)
  , m_pGeneralLowerView()
{
  gfl2::heap::HeapBase* pHeapBase  = m_pWork->GetDeviceHeap();
  
  m_pGeneralLowerView = GFL_NEW(pHeapBase) app::tool::GeneralLowerView(m_pWork->GetAppHeap(), m_pWork->GetAppRenderingManager());
  m_pGeneralLowerView->SetResourceProvider(this);
  m_pGeneralLowerView->ObserveSetup();
  
  m_pGeneralLowerView->SetBGVisibility(false);
  m_pGeneralLowerView->SetLayoutMode(app::tool::GeneralLowerView::LAYOUTMODE_3Buttons);

  m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button0, m_pWork->GetString(GIFT_MESSAGE_GRAC_ID, msg_mystery_sel_12));
  m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button1, m_pWork->GetString(GIFT_MESSAGE_GRAC_ID, msg_mystery_sel_10));
  m_pGeneralLowerView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button2, m_pWork->GetString(GIFT_MESSAGE_GRAC_ID, msg_mystery_sel_11));

  m_pGeneralLowerView->SetEventHandler(this);

  AddSubView(m_pGeneralLowerView);
  m_pGeneralLowerView->SetModal(true);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_SelectInstruction::~LowerView_SelectInstruction()
{
  if(m_pGeneralLowerView) m_pGeneralLowerView->RemoveFromSuperView();
  GFL_SAFE_DELETE(m_pGeneralLowerView);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void    LowerView_SelectInstruction::GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0:    Event(IEventHandler::EVENT_AboutGift);      break;    /**<  ボタン0が押された     */
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1:    Event(IEventHandler::EVENT_AboutAlbum);     break;    /**<  ボタン1が押された     */
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton2:    Event(IEventHandler::EVENT_AboutAlbumFull); break;    /**<  ボタン2が押された     */
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButtonBack: Event(IEventHandler::EVENT_Exit);           break;    /**<  戻るボタンが押された  */
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectInstruction::Reset(void)
{
  m_pGeneralLowerView->CursorMoveTo(0);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void*   LowerView_SelectInstruction::IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
{
  return m_pWork->GetResourceManager()->GetResourceBuffer(arcID, arcDatID);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectInstruction::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_SelectInstruction_OnEvent(eventCode);
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
void LowerView_SelectInstruction::OnSetInputEnabled( bool isEnable )
{
  m_pGeneralLowerView->SetInputEnabled(isEnable);
}


} /*  namespace Gift    */
} /*  namespace NetApp  */
