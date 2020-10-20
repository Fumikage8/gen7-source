
#if !defined(GIFTMESSAGEMENU_H_INCLUDED)
#define GIFTMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftMessageMenu.h
 * @date   2015/08/04 Tue. 11:17:15
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h>

#include  "../Common/AuthenticationHelper.h"

namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  GiftMessageMenu
  *  @brief  ふしぎなおくりものに特化したMessageMenu
  *  @date   2015/07/01 Wed. 15:36:35
  */
//==============================================================================
class GiftMessageMenu
  : public    NetAppLib::UI::NetAppMessageMenu
  , protected app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(GiftMessageMenu);
public:
  //==============================================================================
  /**
    *  @class  
    *  @brief  
    *  @date   2016/03/09 Wed. 20:08:08
    */
  //==============================================================================
  class IValueProvider
  {
  public:
    virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)  = 0;       /**<  あとn件       */
    virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender)  = 0;   /**<  受信Progress  */
    
    virtual ~IValueProvider(){}
  };
  void  SetValueProvider(IValueProvider* pValueProvider) {m_pValueProvider = pValueProvider;}

  

  enum  Mode
  {
    MODE_None,

    /*  common  */
    MODE_Searching,                 /**<  おくりものを探していますn%          */
    MODE_Connecting,                /**<  接続しています                      */
    MODE_Saving,                    /**<  おくりものをうけとっています        */
    MODE_NotFound,                  /**<  みつかりませんでした                */
    MODE_FoundButCannotReceive,     /**<  見つけましたがうけとれません        */
    MODE_Failed,                    /**<  しっぱいしました                    */
    MODE_WirelessSwitchOff,         /**<  むせんOFFです                       */
    MODE_ReceiveCancelConfirm,      /**<  うけとるのをやめますか Y/N          */
    MODE_RemainCancelConfirm,       /**<  残っているけどやめますか Y/N        */
    MODE_InternetConnectionConfirm, /**<  インターネットに接続しますか Y/N    */
    MODE_GiftNotThere,              /**<  おくりものがありませんでした        */
    MODE_UpdateFound,               /**<  追加データが見つかりました eshopにいきますか？ Y/N  */
    MODE_InternetNotAvailable,      /**<  インターネットに接続できないため終了                */
    MODE_CanReceiveOnceAtDay,       /**<  一日一回だけ受け取れます            */
    MODE_EshopNotAvailable,         /**<  eshopを使用できません 本体を更新してください  */
    MODE_Finalizing,                /**<  終了しています                      */

    /*  info  */
    MODE_AlbumFull,                 /**<  いっぱいでうけとれない              */
    MODE_AlbumEmpty,                /**<  うけとっている贈り物がありません    */
    MODE_AmountSpace,               /**<  あとn件うけとれます                 */
    MODE_SelectFunction,            /**<  うけとりかたをえらんで              */

    MODE_NG_AlreadyReceived,        /**<  すでにうけとっているのでNG          */
    MODE_NG_OnceAtDay,              /**<  一日一回なのでNG                    */
    MODE_NG_OnceAtDay_HistoryFull,  /**<  一日一回系履歴領域Full              */

    MODE_AlbumCleanupConfirm,       /**<  アルバムを整理しますか？            */
    MODE_AlbumCleanup,              /**<  いらないカードを捨ててください      */
    MODE_AlbumCleanupExitConfirm,   /**<  捨てないと貰えません宜しいですか？  */


    /*  Ir  */
    MODE_StartIr,                   /**<  向かい合わせてください Y/N          */

    /*  Serial  */
    MODE_Serial_1stConfirm,           /**<  ご注意！～シリアルコードで受け取りますか？ Y/N      */
    MODE_Serial_InputSerial,          /**<  お手持ちのシリアルコードを入力してください          */
    MODE_Serial_Authentication,       /**<  シリアルコードを確認しています                      */
    MODE_Serial_RetryConfirm,         /**<  もう一度シリアルコードを入れますか？ Y/N            */

    MODE_Serial_Error_SerialNG,       /**<  シリアルコードが間違っています                      */
    MODE_Serial_Error_TimeoutPenalty, /**<  続けて失敗したためしばらくたってから                */

    MODE_Serial_Error_MismatchROM,          /**<  お手持ちのタイトルでは受け取れません  */
    MODE_Serial_Error_MismatchRegion,       /**<  お手持ちのソフトでは受け取れません  */
    MODE_Serial_Error_AlreadyDownloaded,    /**<  シリアルコードは使用済みです        */
    MODE_Serial_Error_CampaignDownloaded,   /**<  このコードは既に受け取っています  */
    MODE_Serial_Error_LockedOther,          /**<  このコードは使用中です            */
    MODE_Serial_Error_CampaignNotStarted,   /**<  このコードはまだ受け取れません  */
    MODE_Serial_Error_CampaignEnd,          /**<  有効期限が過ぎています  */
    MODE_Serial_Error_SerialUpdateNG,       /**<  シリアルコードの更新に失敗（Commit失敗）  */

    /*  album  */
    MODE_Album_ChooseOperation_FrontFace,   /**<  カードをどうしますか（表面）                */
    MODE_Album_ChooseOperation_BackFace,    /**<  カードをどうしますか（裏面）                */
    MODE_Album_DeleteCardYN,                /**<  捨ててもよろしいですか？                    */
    MODE_Album_CannotDeleteCard,            /**<  うけとっていないのですてられません          */
    MODE_Album_DeletedCard,                 /**<  カードを捨てました                          */
    MODE_Album_DeletedCard_NoHide,          /**<  カードを捨てました                          */
    MODE_Album_Saving,                      /**<  アルバムを保存しています                    */
    MODE_Album_NoCard,                      /**<  カードがなくなりました                      */

    MODE_MAX
  };


  void  Show(const Mode mode);                                                        /**<  汎用（パラメータなし）                  */

  /*  autherror  */
  void  ShowAuthError(const AuthenticationHelper::InternetConnectResult     authResult);
  void  ShowAuthError(const AuthenticationHelper::FriendServerConnectResult authResult);
  void  ShowAuthError(const AuthenticationHelper::PGLSerialAuthResult       authResult);
  void  ShowAuthError(const AuthenticationHelper::CommitSaveResult          authResult);



  GiftMessageMenu(GiftWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler = NULL);
  virtual ~GiftMessageMenu();

  Mode  GetMode(void) const {return m_Mode;}
  void  SetCardSwapEnable(const bool isEnable)  {m_IsEnableCardSwap = isEnable;}

   /*  UIVIew  */
   virtual void Update(void);
protected:
  virtual void  CommonMessageWindowView_OnEvent(const App::Tool::CommonMessageWindowView::IEventHandler::EventCode event);   /*  override  */

  void          PreShow(void);                                          /**<  showの前に共通でやっておくこと  */
  void          ShowYN(const u32 messageID, const bool isImmediate);    /**<  Y/N  */



  GiftWork*       m_pWork;
  Mode            m_Mode;
  IValueProvider* m_pValueProvider;
  bool            m_IsAutoHide;
  bool            m_IsEnableCardSwap;
  u32             m_AutoHideDelay;
};



} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTMESSAGEMENU_H_INCLUDED)  */
