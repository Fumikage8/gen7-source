
#if !defined(NETAPPMESSAGEWINDOW_H_INCLUDED)
#define NETAPPMESSAGEWINDOW_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppMessageWindow.h
  *  @brief  
  *  @date   2015.06.15 Mon.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <Print/include/WordSet.h>
#include  <AppLib/include/Tool/CommonMessageWindowView.h>

#include  "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"
#include  "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"





namespace NetAppLib {
namespace System  {
  class ApplicationWorkBase;
}
}




namespace NetAppLib {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  NetAppMessageWindow
  *  @brief  共通メッセージウインドウのLayoutを使用したメッセージウインドウ
  *  @date   2015.06.15 Mon.
  */
//==============================================================================
class NetAppMessageWindow
  : public App::Tool::CommonMessageWindowView
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppMessageWindow );

public:
  //------------------------------------------------------------------
  /**
    *  @func     NetAppMessageWindow
    *  @brief    ctor
    *  @param    pLayoutBuffer  [in]  Layoutリソースデータへのポインタ。通常ResourceManagerから取得する。
    */
  //------------------------------------------------------------------
  NetAppMessageWindow(NetAppLib::System::ApplicationWorkBase* pWorkBase, IEventHandler* pHandler = NULL);


  //--------------------------------------------------------------------------------------------
  /**
    * @brief    dtor
    */
  //--------------------------------------------------------------------------------------------
  virtual ~NetAppMessageWindow();



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @note     内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う
    *  @note     messageUtilityを渡さない場合はShowMessage(garcID, messageID)及びregister/expand系（Set...Numberなど）が使えない
    */
  //------------------------------------------------------------------
  void  Setup(const u32 layoutResourceID, NetAppLib::Message::MessageUtility* pMessageUtility = NULL);


  //--------------------------------------------------------------------------------------------
  /**
    * @brief     メッセージ表示
    */
  //--------------------------------------------------------------------------------------------
  void  ShowMessage(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);


protected:
  NetAppLib::System::ApplicationWorkBase*              m_pWorkBase;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator   m_MessageWindowManipulator;
  NetAppLib::Message::MessageUtility*                   m_pMessageUtility;
};




} /*  namespace UI        */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPMESSAGEWINDOW_H_INCLUDED)  */
