//==============================================================================
/**
 * @file   LowerView_InfoWindow.cpp
 * @date   2015/08/20 Thu. 17:42:55
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

#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"
#include  "./LowerView_InfoWindow.h"
// =============================================================================
/**/
//==============================================================================
namespace
{
  static const u32  PAGE_CAPACITY = 5;


  /*  gift  */
  static const u32  instructionMessageTable_Gift[]  =
  {
    msg_mystery_exp_08,
    msg_mystery_exp_09,
  };
  static const u32  instructionPages_Gift = GFL_NELEMS(instructionMessageTable_Gift);

  /*  album  */
  static const u32  instructionMessageTable_Album[]  =
  {
    msg_mystery_exp_02,
    msg_mystery_exp_03,
  };
  static const u32  instructionPages_Album = GFL_NELEMS(instructionMessageTable_Album);

  /*  albumFull  */
  static const u32  instructionMessageTable_AlbumFull[]  =
  {
    msg_mystery_exp_05,
    msg_mystery_exp_06,
  };
  static const u32  instructionPages_AlbumFull = GFL_NELEMS(instructionMessageTable_AlbumFull);
}





namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_InfoWindow::LowerView_InfoWindow(GiftWork* pWork)
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
void  LowerView_InfoWindow::SetViewMode(const ViewMode mode)
{
  m_pMessageIDs   = NULL;
  m_MessagePages  = 0;
  m_ViewMode      = mode;

  switch(m_ViewMode)
  {
    case  VIEWMODE_Instruction_Gift:
      m_pMessageIDs   = instructionMessageTable_Gift;
      m_MessagePages  = instructionPages_Gift;
      break;

    case  VIEWMODE_Instruction_Album:
      m_pMessageIDs   = instructionMessageTable_Album;
      m_MessagePages  = instructionPages_Album;
      break;

    case  VIEWMODE_Instruction_AlbumFull:
      m_pMessageIDs   = instructionMessageTable_AlbumFull;
      m_MessagePages  = instructionPages_AlbumFull;
      break;
  };


  if(m_pMessageIDs)
  {
    m_pInfoWindow->ResetContents(m_MessagePages);
    m_pInfoWindow->StartOpen(this, gfl2::math::Vector3(0,0,0), app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM);
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
    m_pInfoWindow->SetTextContent(page, m_pWork->GetString(GIFT_MESSAGE_GRAC_ID, m_pMessageIDs[page]), NULL);
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
