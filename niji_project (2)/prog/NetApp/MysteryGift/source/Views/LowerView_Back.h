#if !defined(LOWERVIEW_BACK_H_INCLUDED)
#define LOWERVIEW_BACK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Back.h
 * @date   2015/08/04 Tue. 11:26:36
 * @author muto_yu
 * @brief  ふしぎなおくりもの 下画面 戻るボタンのみ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"

#include  "../Common/AuthenticationHelper.h"
#include  "./GiftMessageMenu.h"


namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  Backボタンのみの下画面View
  *  @date   2015/07/13 Mon. 12:07:54
  */
//==============================================================================
class LowerView_Back
  : public NetAppLib::UI::NetAppCursorView
  , public GiftMessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Back );
public:

  /*
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      /*  common  */
      EVENT_Exit,
      EVENT_MessageFinish,
      EVENT_ReceiveCancelConfirm_Yes,
      EVENT_ReceiveCancelConfirm_No,
      EVENT_InternetConfirm_Yes,
      EVENT_InternetConfirm_No,

      /*  Ir  */
      EVENT_IrConfirm_Yes,
      EVENT_IrConfirm_No,

      /*  serial  */
      EVENT_1stConfirm_Yes,
      EVENT_1stConfirm_No,
      EVENT_TransitToEshopConfirm_Yes,
      EVENT_TransitToEshopConfirm_No,
      EVENT_SerialRetryConfirm_Yes,
      EVENT_SerialRetryConfirm_No,

      EVENT_
    };

    virtual void  LowerView_Back_OnEvent(const EventCode event) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)    {m_pHandler = pHandler;}


  void  SetMessageMenuValueProvider(GiftMessageMenu::IValueProvider* pObject) {m_pMessageMenu->SetValueProvider(pObject);}






  /*
  */
  enum  ViewMode
  {
    VIEWMODE_None,

    /*  common  */
    VIEWMODE_Searching,               /**<  おくりものを探しています      */
    VIEWMODE_ReceiveCancelConfirm,    /**<  うけとるのをやめますか Y/N    */
    VIEWMODE_Finalizing,              /**<  終了しています                */

    /*  error  */
    VIEWMODE_Error_ReceiveFailed,           /**<  受け取りに失敗した        */
    VIEWMODE_Error_NotFound,                /**<  見つからなかった          */
    VIEWMODE_Error_FoundButCannotReceive,   /**<  見つけたけど受け取れない  */
    VIEWMODE_Error_NoData,                  /**<  ありませんでした          */
    VIEWMODE_Error_WirelessOff,             /**<  無線OFFです               */
    VIEWMODE_Error_TimeoutPenalty,          /**<  続けて失敗したため不可    */

    /*  Ir  */
    VIEWMODE_IrConfirm,               /**<  赤外線を向かい合わせて Y/N    */

    /*  Serial  */
    VIEWMODE_Serial_1stConfirm,           /**<  ご注意！～シリアルコードで受け取りますか？ Y/N      */
    VIEWMODE_Serial_InternetConfirm,      /**<  インターネットに接続します Y/N                      */
    VIEWMODE_Serial_Connecting,           /**<  接続しています                                      */
    VIEWMODE_Serial_UpdateFound,          /**<  追加データが見つかりました eshopにいきますか？ Y/N  */
    VIEWMODE_Serial_InternetNotAvailable, /**<  インターネットに接続できないため終了                */
    VIEWMODE_Serial_EshopNotAvailable,    /**<  eshopがないので更新してください                     */
    VIEWMODE_Serial_InputSerial,          /**<  お手持ちのシリアルコードを入力してください          */
    VIEWMODE_Serial_Authentication,       /**<  シリアルコードを確認しています                      */
    VIEWMODE_Serial_RetryConfirm,         /**<  もう一度シリアルコードを入れますか？ Y/N            */


    VIEWMODE_
  };
  void  SetViewMode(const ViewMode viewMode);

  /*  autherror  */
  void  ShowAuthError(const AuthenticationHelper::InternetConnectResult     authResult) {m_pMessageMenu->ShowAuthError(authResult);}
  void  ShowAuthError(const AuthenticationHelper::FriendServerConnectResult authResult) {m_pMessageMenu->ShowAuthError(authResult);}
  void  ShowAuthError(const AuthenticationHelper::PGLSerialAuthResult       authResult) {m_pMessageMenu->ShowAuthError(authResult);}
  void  ShowAuthError(const AuthenticationHelper::CommitSaveResult          authResult) {m_pMessageMenu->ShowAuthError(authResult);}



  
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LowerView_Back(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~LowerView_Back();

  /*
    UIView
  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  /*
    end UIView
  */

protected:
  /*
    UIInputListener
  */
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
  /*
    end UIInputListener
  */

  /*  implement GiftMessageMenu::IEventHandler  */
  virtual void  MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode event);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Event(const IEventHandler::EventCode event);



  GiftWork*         m_pWork;
  IEventHandler*    m_pHandler;
  GiftMessageMenu*  m_pMessageMenu;

  ViewMode          m_ViewMode;
};



} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_BACK_H_INCLUDED)  */
