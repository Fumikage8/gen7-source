
//==============================================================================
/**
  *  @file   NetAppMessageMenu.cpp
  *  @brief  
  *  @date   2015.06.15 Mon.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Tool/app_tool_MenuWindow.h"

#include  "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"


// =============================================================================
/**/
//==============================================================================


namespace NetAppLib {
namespace UI  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     NetAppMessageMenu
  *  @brief    ctor
  */
//------------------------------------------------------------------
NetAppMessageMenu::NetAppMessageMenu(NetAppLib::System::ApplicationWorkBase* pWorkBase, IEventHandler* pHandler)
  : App::Tool::MessageMenuView(App::Tool::MessageMenuView::PARAM(pWorkBase->GetAppHeap(), pWorkBase->GetAppRenderingManager()), pHandler)
  , m_pWorkBase(pWorkBase)
  , m_pMessageUtility(NULL)
{
//  app::util::Heap*  pHeap = m_pWorkBase->GetAppHeap();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    dtor
  */
//------------------------------------------------------------------
NetAppMessageMenu::~NetAppMessageMenu()
{
}


//------------------------------------------------------------------
/**
  *  @func     Setup
  *  @brief    使用準備
  *  @param    layoutResourceID [in]  ApplicationWorkBase(ResourceManager)から(ARCID_MENU_WINDOW,  GARC_MenuWindow_MenuWindow_applyt_COMP)を取得するためのResourceID
  *  @param    pMessageUtility  [in]  IDでメッセージを設定する場合は必須
  *  @note     内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う
  */
//------------------------------------------------------------------
void  NetAppMessageMenu::Setup(const u32 layoutResourceID, NetAppLib::Message::MessageUtility* pMessageUtility)
{
  m_pMessageUtility = pMessageUtility;
  App::Tool::MessageMenuView::Setup(m_pWorkBase->GetResourceManager()->GetResourceBuffer(layoutResourceID));
}


void  NetAppMessageMenu::Setup(const u32 layoutResourceID, const u32 cursorLayoutResourceID, NetAppLib::Message::MessageUtility* pMessageUtility)
{
  m_pMessageUtility = pMessageUtility;
  App::Tool::MessageMenuView::Setup(m_pWorkBase->GetResourceManager()->GetResourceBuffer(layoutResourceID), m_pWorkBase->GetResourceManager()->GetResourceBuffer(cursorLayoutResourceID));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppMessageMenu::ShowMessage(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID, const bool isImmediateMessage, const bool isShowTimerIcon, const bool isShowBG)
{
  if(!m_pMessageUtility)  return;

  App::Tool::MessageMenuView::ShowMessage(&m_pMessageUtility->GetString(garcID, messageID), isImmediateMessage, isShowTimerIcon, isShowBG);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @note    要messageUtil
  */
//------------------------------------------------------------------
void  NetAppMessageMenu::ShowMessageWithListMenu(
  /*  ウインドウ設定  */
  const gfl2::fs::ArcFile::ARCDATID  windowMessageGARCID,
  const u32                          windowMessageID,
  const bool                         isImmediateMessage,
  /*  リスト設定  */
  const gfl2::fs::ArcFile::ARCDATID  listMessageGARCID,
  const u32                          listMessageID_0,
  const u32                          listMessageID_1,
  const u32                          listMessageID_2,
  const u32                          listMessageID_3,
  const u32                          listMessageID_4,
  const u32                          listMessageID_5
)
{
  if(!m_pMessageUtility)  return;


  App::Tool::MessageMenuView::ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    *(m_pMessageUtility->GetData(windowMessageGARCID)),
    windowMessageID,
    isImmediateMessage,
    /*  リスト設定  */
    *(m_pMessageUtility->GetData(listMessageGARCID)),
    listMessageID_0,
    listMessageID_1,
    listMessageID_2,
    listMessageID_3,
    listMessageID_4,
    listMessageID_5
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @note    要messageUtil
  */
//------------------------------------------------------------------
void  NetAppMessageMenu::ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::str::StrBuf*                      pWindowMessage,
    const bool                                    isImmediateMessage,
    /*  リスト設定  */
    const gfl2::str::StrBuf*                      pListMessage_0,
    const gfl2::str::StrBuf*                      pListMessage_1,
    const gfl2::str::StrBuf*                      pListMessage_2,
    const gfl2::str::StrBuf*                      pListMessage_3,
    const gfl2::str::StrBuf*                      pListMessage_4,
    const gfl2::str::StrBuf*                      pListMessage_5
)
{
  if(!m_pMessageUtility)  return;


  App::Tool::MessageMenuView::ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    pWindowMessage,
    isImmediateMessage,
    /*  リスト設定  */
    pListMessage_0,
    pListMessage_1,
    pListMessage_2,
    pListMessage_3,
    pListMessage_4,
    pListMessage_5
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @note    要messageUtil
  */
//------------------------------------------------------------------
void  NetAppMessageMenu::ShowMessageWithListMenu(
  /*  ウインドウ設定  */
  const gfl2::fs::ArcFile::ARCDATID  windowMessageGARCID,
  const u32                          windowMessageID,
  const bool                         isImmediateMessage,
  IAddItemHandler*                   pAddItemHandler
)
{
  if(!m_pMessageUtility)  return;

  gfl2::str::StrBuf& windowMessage = m_pMessageUtility->GetString( windowMessageGARCID, windowMessageID );

  App::Tool::MessageMenuView::ShowMessageWithListMenu(
    &windowMessage,
    isImmediateMessage,
    pAddItemHandler
  );
}


} /*  namespace UI        */
} /*  namespace NetAppLib */
