//==============================================================================
/**
 * @file   LowerView_QR.cpp
 * @date   2015/10/09 Fri. 20:12:32
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
#include  "../BattleSpotResource.h"
#include  "../System/BattleSpotWork.h"

#include  "./LowerView_QR.h"


// =============================================================================
/**/
//==============================================================================


namespace
{

}


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_QR::LowerView_QR(BattleSpotWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pEventHandler(NULL)
  , m_ViewMode(VIEWMODE_INVALID)

{
  //gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();


  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_BATTLESPOT_BATTLESPOT_QRC_LOW_000_BFLYT,
    LA_BATTLESPOT_BATTLESPOT_QRC_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    m_pWork->GetMessageUtility(),
    BATTLESPOT_MESSAGE_ARCDAT
  );

  {
    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &GetG2DUtil()->GetLayoutResourceID(0));

    m_pQRPicture  = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_QRC_LOW_000_PANE_QR_DUMMY);

    /*  QRView側のBackButtonは未使用  */ 
    helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_QRC_LOW_000_PANE_BACKBUTTON)->SetVisible(false);
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
LowerView_QR::~LowerView_QR()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool LowerView_QR::IsDrawing(void) const
{
  return  (
    NetAppLib::UI::NetAppEmptyView::IsDrawing()
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_QR::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_QR::SetViewMode(const ViewMode viewMode)
{
  m_ViewMode  = viewMode;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::lyt::LytPicture* LowerView_QR::GetQRPicture(void)
{
  return  m_pQRPicture;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_QR::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->LowerView_QR_OnEvent(this, eventCode);
  }
}

} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
