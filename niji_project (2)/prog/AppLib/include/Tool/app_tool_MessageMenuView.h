#if !defined(APP_TOOL_MESSAGEMENUVIEW_H_INCLUDED)
#define APP_TOOL_MESSAGEMENUVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   app_tool_MessageMenuView.h
 * @date   2015/07/03 Fri. 12:05:39
 * @author muto_yu
 * @brief  汎用メッセージウインドウ&リストメニュー
 * @detail メッセージウインドウ、メニューウインドウを簡便に扱うためのView
 *         基本的に、インスタンスを生成した後はShowMessage～をコールするだけで出現するポップアップメニューのように使う
 *         メッセージ表示後に選択肢を表示することができる
 * @note   NetAppLibより移動
 * @note   CommonMessageWindowView及びMenuWindowを使用しているため、
 *         Menu_MIN_LOW_000.flyt、Menu_PLT_LOW_000.flyt(ARCID_MENU_WINDOW,  GARC_MenuWindow_MenuWindow_applyt_COMP)に依存する
 *         ※CommonMessageWindowViewはSetupをOverrideすることで別離ソースに対応させることが可能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include  <AppLib/include/Ui/UIView.h>
#include  <Savedata/include/ConfigSave.h>
#include "Sound/include/Sound.h"

#include  "./CommonMessageWindowView.h"

/*
  forward declare
*/
namespace app {
namespace tool {
  class MenuWindow;
  class MenuWindowListener;
}
}


namespace App {
namespace Tool  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class   MessageMenuView
  *  @brief   CommonMessageWindowとMenuWindowを持つView
  *  @detail  3つのViewで構成される
  *           View階層は次の通り
  *           [MessageMenu(Empty)]
  *             |
  *             +--[CommonMessageWindow]
  *                   |
  *                   +--[MenuWindow]
  *  @note    MenuWindow（選択肢）はUpdateを回すことでシーケンスが進み内部でリソースを読む実装であるため、
  *           表示時にAddSubViewするのではなく、非表示状態で初期化時から常にAddSubViewしておいてバックグラウンドでシーケンスを進めておくのが良い
  *  @note    CommonMessageWindowView（メッセージウインドウ）は、リソースの提供は外部より受ける実装となっている
  *           CommonMessageWindowViewはSetupとDrawをOverrideしてCreate2Dすることで別リソースに対応させることが可能
  *           使用するCommonMessageWindowViewを変える場合は本クラスのCreateMessageWindowをOverrideする
  *  @code
  *     //  基本的な使い方
  *     //  単体でも扱えるが、Frameなどのコントロールクラスや、親となるViewによって取り回す使用を想定
  *     void ControlClass::Create(void)
  *     {
  *       //  何らかの方法でメッセージウインドウ用のMenu_PLT_LOW_000.flyt(ARCID_MENU_WINDOW,  GARC_MenuWindow_MenuWindow_applyt_COMP)を用意
  *       void*                             pMessageWindowLayoutResource  = GetResource();
  *       app::util::Heap*                  pHeap                         = GetHeap();
  *       app::util::AppRenderingManager*   pRenderingManager             = GetRenderingManager();
  *       MessageMenuView::IEventHandler*   pHandler                      = this;             //  Frameなど、本クラスを扱うクラスなどでIEventHandler::MessageMenuView_OnEvent()を実装し、通知を受け取る
  *       
  *       
  *       //  インスタンス生成
  *       //  MessageMenuViewは MessageMenuView - CommonMessageWindow - MenuWindow の3つのViewのツリーで構成される
  *       //  初期状態は非表示
  *       
  *       m_pMessageMenuView  = GFL_NEW(pHeap->GetDeviceHeap()) App::Tool::MessageMenuView(
  *                                                               App::Tool::PARAM(pHeap. pRenderingManager),
  *                                                               pHandler
  *                                                             );
  *       //  Setup（下位クラスでCreate2D()）
  *       m_pMessageMenuView->Setup(pMessageWindowLayoutResource);
  *
  *       //  必要に応じて親ViewにAdd
  *       m_pParentView->AddSubView(m_pMessageMenuView);
  *     }
  *
  *     //  更新処理
  *     void ControlClass::Update(void)
  *     {
  *       m_pParentView->UpdateTree();    //  親ViewにAddSubした場合は、ルートとなるViewのUpdateTree経由で更新する
  *       
  *       //  直接取り回す場合は、下位のViewの更新も必要となるのでUpdate()ではなくUpdateTree()で更新する
  *       //  Update()をコールするだけでは、最上位の空のViewの更新のみ行われるため、何も動作しない
  *       //  m_pMessageMenuView->UpdateTree();
  *
  *       //  Message表示
  *       //  Visibilityは内部で設定される
  *       if(needShowMessage)
  *       {
  *         switch(mode)
  *         {
  *           //  通常のストリーム表示
  *           case MODE_NORMAL:
  *             m_pMessageMenuView->ShowMessage(rWindowMessageData, messageID);
  *             break;
  *         
  *           //  即時表示
  *           case MODE_IMMEDIATE:
  *             m_pMessageMenuView->ShowMessage(rWindowMessageData, messageID, true);
  *             break;
  *         
  *           //  4択：ストリーム表示
  *           case MODE_IMMEDIATE:
  *             //  ストリーム表示時、ページ送りで選択肢が出現しないようFinishModeをUserInputとし、
  *             //  メッセージ終端でのユーザ入力待ち後に選択肢を表示するように設定する
  *             m_pMessageMenuView->SetFinishMode_UserInput();
  *
  *             //  選択肢表示の場合、自動的にisShowBG = true、isShowTimerIcon = false、SetModal(true)となる
  *             m_pMessageMenuView->ShowMessageWithListMenu(
  *               rWindowMessageData,
  *               messageID,
  *               false,
  *               rListMessageData,
  *               listMessageID_0,
  *               listMessageID_1,
  *               listMessageID_2,
  *               listMessageID_3
  *             );
  *             break;
  *         
  *           //  はい/いいえ
  *           case MODE_IMMEDIATE:
  *             //  はい/いいえの2択は専用の表示方法となるため、Showの前にフラグをセットする
  *             m_pMessageMenuView->SetYNListMode(true);
  *
  *             //  Yes/Noの場合、選択項目は最初の二つ以外は無視される
  *             m_pMessageMenuView->ShowMessageWithListMenu(
  *               rWindowMessageData,
  *               messageID,
  *               false,
  *               rListMessageData,
  *               listMessageID_0,
  *               listMessageID_1
  *             );
  *             break;
  *         }
  *       }
  *     }
  *
  *     //  描画
  *     void ControlClass::Draw(void)
  *     {
  *       //  Updateと同様、DrawTree()で描画する
  *       n_pParentView->DrawTree();
  *     }
  *
  *     //  イベントハンドラ
  *     void ControlClass::MessageMenuView_OnEvent(const App::Tool::MessageMenuView::IEventHandler::EventCode event)
  *     {
  *       switch(event)
  *       {
  *         //  選択肢が選択された（Yes/No時はこのEventではなく、専用のEventとして通知される）
  *         case  App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedItem:
  *           //  do something
  *           break;
  *
  *         //  Yes/NoモードでYesが選択された
  *         case  App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes:
  *           //  do something
  *           break;
  *         
  *         //  Yes/NoモードでNoが選択された
  *         case  App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo:
  *           //  do something
  *           break;
  *       }
  *     }
  *  @endcode
  *  @date   2015/07/03 Fri. 12:07:12
  */
//==============================================================================
class MessageMenuView
  : public    app::ui::UIView
  , protected app::tool::MenuWindowListener
  , protected CommonMessageWindowView::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( MessageMenuView );

public:
  /**
    @brief  動作に必要なパラメータ群
    @note   内部でshallowCopyされる
  */
  typedef struct  PARAM
  {
    app::util::Heap*                  pHeap;
    app::util::AppRenderingManager*   pRenderingManager;

    /*  ctor  */
    PARAM(app::util::Heap* _pHeap, app::util::AppRenderingManager* _pRenderingManager)
      : pHeap(_pHeap)
      , pRenderingManager(_pRenderingManager)
    {}
  }PARAM;



  /**   */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      EVENT_OnEOM,            /**<  メッセージ表示が終端に到達                                  */
      EVENT_OnFinish,         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
      EVENT_OnSelectedItem,   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */

      EVENT_OnSelectedYes,    /**<  Yes/NoモードでYesが選択された                               */
      EVENT_OnSelectedNo,     /**<  Yes/NoモードでNoが選択された                                */
      
      EVENT_,
    };

    virtual void  MessageMenuView_OnEvent(const EventCode event) = 0;
  };


  class IAddItemHandler
  {
  public:
    virtual ~IAddItemHandler(){}

    virtual void MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow ) = 0;
  };

  //------------------------------------------------------------------
  /**
    *  @func     MessageMenuView
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  MessageMenuView(const PARAM& rParam, IEventHandler* pHandler = NULL);


  //------------------------------------------------------------------
  /**
    *  @func     ~MessageMenuView
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~MessageMenuView();


  //------------------------------------------------------------------
  /**
    *  @func     SetEventHandler
    *  @brief    本クラスからの通知を受け取るハンドラの設定(登録)を行います
    *  @param    [in]  pHandler  通知を受け取るハンドラ
    */
  //------------------------------------------------------------------
  void  SetEventHandler(IEventHandler* pHandler){ m_pHandler = pHandler; }

  //------------------------------------------------------------------
  /**
    *  @func     RemoveEventHandler
    *  @brief    本クラスからの通知を受け取るハンドラの登録解除を行います
    */
  //------------------------------------------------------------------
  void  RemoveEventHandler(){ m_pHandler = NULL; }


  //------------------------------------------------------------------
  /**
    *  @func     Setup
    *  @brief    使用準備、内部でCreate2Dなど行う
    *  @param    [in]  pLayoutResourceData  layout(Menu_PLT_LOW_000.flyt(ARCID_MENU_WINDOW,  GARC_MenuWindow_MenuWindow_applyt_COMP))のリソースデータ
    */
  //------------------------------------------------------------------
  void  Setup(void* pLayoutResourceData);
  void  Setup(void* pLayoutResourceData, void* pCursorResourceData);


  //------------------------------------------------------------------
  /**
    *  @func     IsSetup
    *  @brief    使用準備が完了したか
    *  @retval   true:完了
    *  @retval   false:未完了
    */
  //------------------------------------------------------------------
  bool  IsSetup();


  //------------------------------------------------------------------
  /**
    *  @func     SetYNListMode
    *  @brief    ListMenuをYesNoモードで表示するか
    *            trueに設定しておくと、ShowMessageWithListMenuでYes/No選択専用モードで表示され、EventHandlerでは...Selected{Yes|No}が通知される
    *  @param    [in] negativeIndex   否定系として扱う選択肢番号（{0|1}）、Bボタンでのショートカットに割り当てたりする
    */
  //------------------------------------------------------------------
  void  SetYNListMode(const bool isEnable, const u32 negativeIndex = 1);

  //------------------------------------------------------------------
  /**
    *  @func     SetMessageSpeed
    *  @brief    メッセージスピードを設定する
    */
  //------------------------------------------------------------------
  void  SetMessageSpeed(Savedata::ConfigSave::MSG_SPEED spd);

  //------------------------------------------------------------------
  /**
    *  @brief    メッセージ時の自動送りフラグを設定
    */
  //------------------------------------------------------------------
  void SetMessageAutoFlg( const bool bFlg );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief    メッセージのみの表示
    * @param    [in]  isImmediateMessage  即時表示するか（false:時間をかけてストリーム表示）
    * @param    [in]  isShowTimerIcon     TimerIconを表示するか
    * @param    [in]  isShowBG            背景の暗転板を表示するか
    * @note     isShowBG = trueとした場合、Modalとして動作する。Modelessとしたい場合は直後にSetModal(false)する。
    */
  //--------------------------------------------------------------------------------------------
  void  ShowMessage(const gfl2::str::MsgData& rWindowMessageData, const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);
  void  ShowMessage(const gfl2::str::StrBuf* pStr, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);


  //--------------------------------------------------------------------------------------------
  /**
    * @brief     メッセージ表示後、リストメニューを表示する
    * @detail    FinishMode_Noneの場合はEOM(EndOfMessage)で、それ以外はFinishイベントでリストメニューが表示される
    * @note      immediate表示のときはFinishが通知されないようなので注意
    */
  //--------------------------------------------------------------------------------------------
  void  ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::str::MsgData&                     rWindowMessageData,
    const u32                                     windowMessageID,
    const bool                                    isImmediateMessage,
    /*  リスト設定  */
    const gfl2::str::MsgData&                     rListMessageData,
    const u32                                     listMessageID_0,
    const u32                                     listMessageID_1,
    const u32                                     listMessageID_2 = UINT_MAX,
    const u32                                     listMessageID_3 = UINT_MAX,
    const u32                                     listMessageID_4 = UINT_MAX,
    const u32                                     listMessageID_5 = UINT_MAX
  );

  /*
  */
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

  //  メッセージはStrBuf*だけどメニューリストはid指定版
  void  ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::str::StrBuf*                      pWindowMessage,
    const bool                                    isImmediateMessage,
    /*  リスト設定  */
    const gfl2::str::MsgData&                     rListMessageData,
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
    IAddItemHandler*                              pAddItemHandler
  );

  //------------------------------------------------------------------
  /**
    *  @func     HideMessage
    *  @brief    非表示とする
    *  @note     List表示時は、項目選択時に自動的にコールされる
    */
  //------------------------------------------------------------------
  void  HideMessage(void);


  //------------------------------------------------------------------
  /**
    *  @func     選択項目の追加設定
    *  @param[in]   pMsg           メッセージデータ
    *  @param[in]   id            文字列ID
    *  @param[in]   isCancel     true = Bボタンを割り当てる
    *  @param[in]   isMark       true = 戻るマークを表示する
    *  @param[in]   selectSeId  選択時のSEID(0だとデフォルトのSEになる)
    *  @note
    *            ShowMessageWithListMenu()関数ではAddItem()での設定が自由にできないので使う側が個別で設定できるように関数を用意
    *  @caution AddItemした後、ClearMenuせずにメッセージを上書きするとAddItemした選択肢が残るので注意
    *             ClearMenu()は明示的にコールするか、選択肢を選択するか、HideMessageでコールされる
    */
  //------------------------------------------------------------------
  void AddItem( gfl2::str::MsgData* pMsg, u32 id, bool isCancel, bool isMark, u32 selectSeId );
  void AddItem( gfl2::str::StrBuf& rStrBuf, bool isCancel, bool isMark, u32 selectSeId );

  //------------------------------------------------------------------
  /**
    *  @func     選択項目の追加設定
    *  @param[in]   str          文字列
    *  @param[in]   isCancel     true = Bボタンを割り当てる
    *  @param[in]   isMark       true = 戻るマークを表示する
    *  @param[in]   selectSeId  選択時のSEID(0だとデフォルトのSEになる)
    *  @note
    *            ShowMessageWithListMenu()関数ではAddItem()での設定が自由にできないので使う側が個別で設定できるように関数を用意
    */
  //------------------------------------------------------------------
  void MessageMenuView::AddItem( const gfl2::str::StrBuf & str, bool isCancel=false, bool isMark=false, u32 selectSeId=SMID_NULL );


  //------------------------------------------------------------------
  /**
    *  @brief    AddItemした選択肢の強制クリア  
    *  @note     内部で m_pCommonMenu->StartYesNoMenu(0);  m_pCommonMenu->EndMenu(true); としている
    *  @note     HideMessageからもコールされる
    */
  //------------------------------------------------------------------
  void  ClearMenu(void);


  //------------------------------------------------------------------
  /**
    *  @func     GetSelectedItemIndex
    *  @brief    EVENT_OnSelectedItem時に選択されたIDを取得する
    */
  //------------------------------------------------------------------
  u32   GetSelectedItemIndex(void) const  {return m_SelectedIndex;}

  /*  messageWindowのfinishEvent種別  */
  void  SetFinishMode_None(void)                              {m_IsShowListOnFinishEvent = false; m_pMessageWindowView->SetFinishMode_None();}                          /**<  Finishなし              */
  void  SetFinishMode_UserInput(void)                         {m_IsShowListOnFinishEvent = true;  m_pMessageWindowView->SetFinishMode_UserInput();}                     /**<  Userの入力によって終了  */
  void  SetFinishMode_Timer(const u8 timeoutFrame)            {m_IsShowListOnFinishEvent = true;  m_pMessageWindowView->SetFinishMode_Timer(timeoutFrame);}             /**<  一定時間後に終了        */
  void  SetFinishMode_UserInputOrTimer(const u8 timeoutFrame) {m_IsShowListOnFinishEvent = true;  m_pMessageWindowView->SetFinishMode_UserInputOrTimer(timeoutFrame);}  /**<  User/TimerのHybrid      */

  /* -------------------------------------------------------------------------*/
  /**
   * @brief キー入力可否設定
   * @note  trueで一時キー入力無効
   */
  /* -------------------------------------------------------------------------*/
  void SetSuspendKey( bool flag );


  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){};
  /*  end implement UIView I/F  */


  /*  implement MenuWindowListener I/F  */
  virtual void  SelectMenu(u32 selectedIndex);
  /*  end implement MenuWindowListener I/F  */


protected:
  //------------------------------------------------------------------
  /**
  *  @brief     CommonMessageWindowViewを生成する
  *  @note      コンストラクタから呼ばれる
  *  @note      OverrideすることでメッセージウインドウをCommonMessageWindowViewを継承したクラスに変更し、別のリソースに対応することなどが可能
    */
  //------------------------------------------------------------------
  virtual CommonMessageWindowView*  CreateCommonMessageWindowView(const CommonMessageWindowView::PARAM& rParam, CommonMessageWindowView::IEventHandler* pHandler);

  void  ShowListMenu(const u32 defaultSelectedIndex = 0);
  /*  implement CommonMessageWindowView::IEventHandler I/F  */
  virtual void  CommonMessageWindowView_OnEvent(const CommonMessageWindowView::IEventHandler::EventCode event);
  /*  end implement CommonMessageWindowView::IEventHandler I/F  */


private:
  /* -------------------------------------------------------------------------*/
  /**
   * @brief  テキストボックスペインのアルファをリセット(不透明に)する
   */
  /* -------------------------------------------------------------------------*/
  void resetMessageWindowTextBoxPaneAlpha(void);


protected:
  /*
    members
  */
  PARAM                       m_Param;                        /**<  起動パラメータ（shallowCopy）                         */
  IEventHandler*              m_pHandler;                     /**<  本クラスからの通知を受け取るハンドラ                  */

  /*  汎用メッセージwindow  */
  CommonMessageWindowView*    m_pMessageWindowView;

  /*  汎用メニュー（選択肢）  */
  app::tool::MenuWindow*      m_pCommonMenu;                  /**<  選択肢  */
  u32                         m_SelectedIndex;                /**<  app::tool::MenuWindowで選択された項目Index            */
  u32                         m_YNListNegativeIndex;          /**<  Yes/No専用モードでどちらを否定として扱うか            */
  bool                        m_YNListMode;                   /**<  Yes/No専用モードで選択肢を出す必要があるか            */
  bool                        m_IsNeedShowListMenu;           /**<  選択肢を出す必要があるか                              */
  bool                        m_IsShowListOnFinishEvent;      /**<  Finishイベントでリストメニューを表示する（またはEOM） */
};


} /*  namespace Tool  */
} /*  namespace App   */
#endif  /*  #if !defined(APP_TOOL_MESSAGEMENUVIEW_H_INCLUDED)  */

