//==============================================================================
/**
  *  @file   NetAppMessageWindow.cpp
  *  @brief  
  *  @date   2015.06.15 Mon.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <niji_conv_header/app/common_menu/MenuWindow.h>
#include  <niji_conv_header/app/common_menu/MenuWindow_anim_list.h>
#include  <niji_conv_header/app/common_menu/MenuWindow_pane.h>

#include  "AppLib/include/Tool/app_tool_MenuWindow.h"

#include  "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppMessageWindow.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  {
    LAYOUTWORK_MessageWindow,

    LAYOUTWORK_MAX
  };
}


namespace NetAppLib {
namespace UI  {
// =============================================================================
/**/
//==============================================================================







//------------------------------------------------------------------
/**
  *  @func     NetAppMessageWindow
  *  @brief    ctor
  */
//------------------------------------------------------------------
NetAppMessageWindow::NetAppMessageWindow(NetAppLib::System::ApplicationWorkBase* pWorkBase, IEventHandler* pHandler)
  : App::Tool::CommonMessageWindowView(App::Tool::CommonMessageWindowView::PARAM(pWorkBase->GetAppHeap(), pWorkBase->GetAppRenderingManager()), pHandler)
  , m_pWorkBase(pWorkBase)
  , m_MessageWindowManipulator()
  , m_pMessageUtility(NULL)
{
  GFL_ASSERT(m_pWorkBase);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    dtor
  */
//------------------------------------------------------------------
NetAppMessageWindow::~NetAppMessageWindow()
{
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppMessageWindow::Setup(const u32 layoutResourceID, NetAppLib::Message::MessageUtility* pMessageUtility)
{
  App::Tool::CommonMessageWindowView::Setup(m_pWorkBase->GetResourceManager()->GetResourceBuffer(layoutResourceID));

  m_pMessageUtility   = pMessageUtility;
}


//--------------------------------------------------------------------------------------------
/**
  * @brief     メッセージ表示
  */
//--------------------------------------------------------------------------------------------
void  NetAppMessageWindow::ShowMessage(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID, const bool isImmediateMessage, const bool isShowTimerIcon, const bool isShowBG)
{
  if(!m_pMessageUtility)  return;

  App::Tool::CommonMessageWindowView::ShowMessage(&m_pMessageUtility->GetString(garcID, messageID), isImmediateMessage, isShowTimerIcon, isShowBG);
}




} /*  namespace UI        */
} /*  namespace NetAppLib */

