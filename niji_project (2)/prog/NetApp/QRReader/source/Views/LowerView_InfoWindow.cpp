//==============================================================================
/**
 * @file   LowerView_InfoWindow.cpp
 * @date   2016/02/15 Mon. 16:26:34
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include  "../System/QRReaderWork.h"
#include  "../QRReaderResource.h"
#include  "./LowerView_InfoWindow.h"
// =============================================================================
/**/
//==============================================================================
namespace
{
  static const u32  PAGE_CAPACITY = 6;


  /*    */
  static const u32  instructionMessageTable_All[]  =
  {
    qrscan_info_01_01,
    qrscan_info_01_02,
    qrscan_info_01_03,
    qrscan_info_01_04,
    /*
      2016/03/23  Yu Muto
      寺地さんの要望により、説明ページを6→4に削減
    */
//    qrscan_info_01_05,
//    qrscan_info_01_06,
  };
  static const u32  instructionPages_All = GFL_NELEMS(instructionMessageTable_All);
}





namespace NetApp    {
namespace QRReader  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_InfoWindow::LowerView_InfoWindow(QRReaderWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , app::tool::InfoWindowBookTypeListener()
  , m_pWork(pWork)
  , m_pHandler(NULL)
  , m_ViewMode(VIEWMODE_NONE)
  , m_pInfoWindow(NULL)
  , m_pMessageIDs(NULL)
  , m_MessagePages(0)

{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetDeviceHeap();

  /*  infowindow  */
  {
    m_pInfoWindow = GFL_NEW(pHeapBase) app::tool::InfoWindowBookType(pWork->GetAppHeap());
    m_pInfoWindow->SyncCreate(pWork->GetAppHeap(), m_pWork->GetAppRenderingManager(), PAGE_CAPACITY, NULL);

    AddSubView(m_pInfoWindow);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
LowerView_InfoWindow::~LowerView_InfoWindow()
{
  if(m_pInfoWindow) m_pInfoWindow->RemoveFromSuperView();
  /*  infowindow  */
  GFL_SAFE_DELETE(m_pInfoWindow);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  LowerView_InfoWindow::ExitQuery(void)
{
  return  m_pInfoWindow->EndFunc();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_InfoWindow::SetViewMode(const ViewMode mode, IInfoWindowOpenPositionProvider* pPosProvider)
{
  m_pMessageIDs   = NULL;
  m_MessagePages  = 0;
  m_ViewMode      = mode;

  switch(m_ViewMode)
  {
    case  VIEWMODE_Instruction_All:
      m_pMessageIDs   = instructionMessageTable_All;
      m_MessagePages  = instructionPages_All;
      break;
  };


  if(m_pMessageIDs)
  {
    gfl2::math::Vector3   openPos(0,0,0);
    
    m_pInfoWindow->ResetContents(m_MessagePages);
    if(pPosProvider)
    {
      pPosProvider->IInfoWindowOpenPositionProvider_OnQueryInfoWindowPosition(openPos);
    }
    m_pInfoWindow->StartOpen(this, openPos, app::tool::InfoWindowBookType::OPEN_TYPE_SCALE_ANIM);
  }
}


/*
  implement app::tool::InfoWindowBookTypeListener I/F
*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void LowerView_InfoWindow::OnClose(void)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_InfoWindow_OnEvent(IEventHandler::EVENT_Exit);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void LowerView_InfoWindow::OnChangePage(u32 page)
{
  if(m_pMessageIDs && (page < m_MessagePages))
  {
    m_pInfoWindow->SetTextContent(page, &(m_pWork->GetMessageUtility()->GetString(QRREADER_MESSAGE_GRAC_ID, m_pMessageIDs[page])), NULL);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void LowerView_InfoWindow::OnCommand(u32 page)
{
}


} /*  namespace Gift    */
} /*  namespace NetApp  */
