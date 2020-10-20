
#if !defined(NETAPPMESSAGEMENU_H_INCLUDED)
#define NETAPPMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppMessageMenu.h
  *  @brief  汎用メッセージウインドウ&リストメニュー
  *  @date   2015.06.15 Mon.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Tool/app_tool_MessageMenuView.h>
#include  "../../include/Message/NetAppMessageDataUtility.h"



namespace NetAppLib {
namespace System  {
  class ApplicationWorkBase;
}
}






/*    */
namespace NetAppLib  {
namespace UI {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class   NetAppMessageMenu
  *  @brief   NetAppMessageWindowとapp::tool::MenuWindowを持つView
  *  @detail  3つのViewで構成される
  *           View階層は次の通り
  *           [NetAppMessageMenu(EmptyView)]
  *             |
  *             +--[NetAppMessageWindow]
  *                   |
  *                   +--[app::tool::MenuWindow]
  *
  *  @date    2015.06.15 Mon.
  */
//==============================================================================
class NetAppMessageMenu
  : public App::Tool::MessageMenuView
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppMessageMenu );
public:


  //------------------------------------------------------------------
  /**
    *  @func     NetAppMessageMenu
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  NetAppMessageMenu(NetAppLib::System::ApplicationWorkBase* pWorkBase, IEventHandler* pHandler = NULL);


  //------------------------------------------------------------------
  /**
    *  @func     ~NetAppMessageMenu
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~NetAppMessageMenu();


  //------------------------------------------------------------------
  /**
    *  @func     Setup
    *  @brief    使用準備
    *  @param    layoutResourceID [in]  ApplicationWorkBase(ResourceManager)から(ARCID_MENU_WINDOW,  GARC_MenuWindow_MenuWindow_applyt_COMP)を取得するためのResourceID
    *  @param    pMessageUtility  [in]  IDでメッセージを設定する場合は必須
    *  @note     内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う
    */
  //------------------------------------------------------------------
  void  Setup(const u32 layoutResourceID, NetAppLib::Message::MessageUtility* pMessageUtility = NULL);
  void  Setup(const u32 layoutResourceID, const u32 cursorLayoutResourceID, NetAppLib::Message::MessageUtility* pMessageUtility = NULL);

  //--------------------------------------------------------------------------------------------
  /**
    * @brief    メッセージのみの表示
    * @param    isImmediateMessage  [in]  即時表示するか（false:時間をかけてストリーム表示）
    * @param    isShowTimerIcon     [in]  TimerIconを表示するか
    * @param    isShowBG            [in]  背景の暗転板を表示するか
    */
  //--------------------------------------------------------------------------------------------
  void  ShowMessage(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);

  //--------------------------------------------------------------------------------------------
  /**
    * @brief     メッセージ表示後、リストメニューを表示する
    * @detail    FinishMode_Noneの場合はEOM(EndOfMessage)で、それ以外はFinishイベントでリストメニューが表示される
    * @note      immediate表示のときはFinishが通知されないようなので注意
    */
  //--------------------------------------------------------------------------------------------
  void  ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::fs::ArcFile::ARCDATID             windowMessageGARCID,
    const u32                                     windowMessageID,
    const bool                                    isImmediateMessage,
    /*  リスト設定  */
    const gfl2::fs::ArcFile::ARCDATID             listMessageGARCID,
    const u32                                     listMessageID_0,
    const u32                                     listMessageID_1,
    const u32                                     listMessageID_2 = UINT_MAX,
    const u32                                     listMessageID_3 = UINT_MAX,
    const u32                                     listMessageID_4 = UINT_MAX,
    const u32                                     listMessageID_5 = UINT_MAX
  );

  void  ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::str::StrBuf*                      pWindowMessage,
    const bool                                    isImmediateMessage,
    /*  リスト設定  */
    const gfl2::str::StrBuf*                      pListMessage_0,
    const gfl2::str::StrBuf*                      pListMessage_1,
    const gfl2::str::StrBuf*                      pListMessage_2 = NULL,
    const gfl2::str::StrBuf*                      pListMessage_3 = NULL,
    const gfl2::str::StrBuf*                      pListMessage_4 = NULL,
    const gfl2::str::StrBuf*                      pListMessage_5 = NULL
  );

  void  ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::fs::ArcFile::ARCDATID             windowMessageGARCID,
    const u32                                     windowMessageID,
    const bool                                    isImmediateMessage,
    IAddItemHandler*                              pAddItemHandler
  );

protected:
  /*
    members
  */
  NetAppLib::System::ApplicationWorkBase*          m_pWorkBase;
  NetAppLib::Message::MessageUtility*               m_pMessageUtility;
};




} /*  namespace UI          */
} /*  namespace NetAppLib   */
#endif  /*  #if !defined(NETAPPMESSAGEMENU_H_INCLUDED)  */
