
#if !defined(COMMONMESSAGEWINDOWMANIPULATOR_H_INCLUDED)
#define COMMONMESSAGEWINDOWMANIPULATOR_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   CommonMessageWindowManipulator.h
 * @date   2015/07/03 Fri. 12:30:19
 * @author muto_yu
 * @brief  NetApp用共通メッセージウインドウ操作クラス
 *         リソースは持たず、外部で構築され提供されたリソースに対する操作のみを行う
 * @note   NetAppLibより移動
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#include  <Layout/include/gfl2_Layout.h>
#include  <Str/include/gfl2_Str.h>
#include  <Savedata/include/ConfigSave.h>


// =============================================================================
/**/
//==============================================================================
namespace app {
namespace util  {
  class  Heap;
}

namespace ui  {
  class   UIView;
}
}

namespace App {
namespace Tool  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  CommonMessageWindowManipulator
  *  @brief  共通メッセージウインドウ操作クラス
  *          リソースは持たず、外部で構築され提供されたリソースに対する操作のみを行う
  *          （つまり、SetupInfo.pTargetView->GetG2DUtil()に対する操作のHelper）
  *  @note   メッセージウインドウは次の構造の部品Paneとして作成されている
  *          [Root]
  *             +- All
  *               +- BG_00
  *               +- Null_Pos
  *                 +- TextBox_2      <- immediate用
  *                 +- Null_Text1
  *                   +- TextBox_1a   <- stream用1of2
  *                   +- TextBox_1b   <- stream用2of2
  *          必要に応じて、ここにTimerIconやMessageCursorを追加する
  *
  *  @date   2015.06.05 Fri.
  */
//==============================================================================
class CommonMessageWindowManipulator
{
  GFL_FORBID_COPY_AND_ASSIGN( CommonMessageWindowManipulator );

public:
  /*    */
  enum  EventID
  {
    EVENT_OnEOM,      /**<  メッセージの終端まで表示された（RES_DONE相当、ShowMessage後の一度のみ発生）  */
    EVENT_OnFinish,   /**<  設定されたフィニッシュ待ちまで終了（RES_FINISH相当）                        */
    
    EVENT_
  };

  /**
    MessageWindowで発生した各種Eventを受け取るハンドラインターフェイス
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}
    virtual void  CommonMessageWindowManipulator_OnEvent(const EventID event) = 0;
  };

  /**
      @brief  設定用構造体
      @note   省略（無効値の設定）可能な要素は次の通り
                - pHandler
                - animID_TimerIcon
                - animID_MsgCursor
                - paneID_TimerIcon
                - paneID_MessageCursor
  */
  typedef struct _SSetupInfo
  {
    /*  system  */
    app::util::Heap*          pHeap;
    /*  layout  */
    app::ui::UIView*          pTargetView;            /**<  各種リソースを持つUIView。G2DUtilを使うのでCreate2D済みであること */
    u32                       layoutWorkID;           /**<  リソースが含まれるLayoutWorkのID                                  */
    IEventHandler*            pHandler;               /**<  各種イベントハンドラ、不要な場合はNULL                            */

    /*  anim  */
    u32                       animID_TimerIcon;       /**<  タイマアイコンのアニメーション                                    */
    u32                       animID_TimerIcon_In;    /**<  タイマアイコンのアニメーション(表示)                              */
    u32                       animID_TimerIcon_Out;   /**<  タイマアイコンのアニメーション(非表示)                            */
    u32                       animID_MsgCursor;       /**<  メッセージカーソルのアニメーション                                */

    /*  parts  */
    gfl2::lyt::LytPaneIndex   paneID_WindowParts;     /**<  メッセージウインドウ部品のIndex                                   */
    /*  immediate  */
    gfl2::lyt::LytPaneIndex   paneID_TextBox2;        /**<  即時表示用TextBoxPane                                             */
    /*  stream  */
    gfl2::lyt::LytPaneIndex   paneID_NullText1;       /**<  ストリーム表示TextBoxPaneがまとめられているPaneのID、即時表示との切り替えに使う  */
    gfl2::lyt::LytPaneIndex   paneID_TextBox1a;       /**<  ストリーム表示TextBoxPane1/2                                      */
    gfl2::lyt::LytPaneIndex   paneID_TextBox2a;       /**<  ストリーム表示TextBoxPane2/2                                      */
    /*  timerIcon  */
    gfl2::lyt::LytPaneIndex   paneID_TimerIcon;       /**<  タイマアイコンのPaneID、不要な場合は gfl2::lyt::LYTSYS_PANEINDEX_ERROR  */
    /*  messageCursor */
    gfl2::lyt::LytPaneIndex   paneID_MsgCursor;       /**<  （ユーザ入力待ちなどの）メッセージカーソルのPaneID                */


    /**
      ctor
    */
    _SSetupInfo(void)
      : pHeap(NULL)
      , pTargetView(NULL)
      , layoutWorkID(UINT_MAX)
      , pHandler(NULL)
      , animID_TimerIcon(UINT_MAX)
      , animID_TimerIcon_In(UINT_MAX)
      , animID_TimerIcon_Out(UINT_MAX)
      , animID_MsgCursor(UINT_MAX)
      , paneID_WindowParts(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      , paneID_TextBox2(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      , paneID_NullText1(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      , paneID_TextBox1a(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      , paneID_TextBox2a(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      , paneID_TimerIcon(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      , paneID_MsgCursor(gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
    {}
  }SSetupInfo;


  //------------------------------------------------------------------
  /**
    *  @func     NetAppCommonMessageWindowManipulator
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  CommonMessageWindowManipulator(void);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~CommonMessageWindowManipulator();



  //------------------------------------------------------------------
  /**
    *  @func     Setup
    *  @brief    初期設定
    *  @note     内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う。Delete～は外部に依存する（通常UIView::Delete2D()で解体される）。
    */
  //------------------------------------------------------------------
  void  Setup(const SSetupInfo& rSetupInfo);

  //------------------------------------------------------------------
  /**
    *  @func     Setup
    *  @brief    初期設定
    *  @brief    ウィンドウのパーツペインを指定
    *  @note     内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う。Delete～は外部に依存する（通常UIView::Delete2D()で解体される）。
    *            Setup処理内で rSeetupInfo.paneID_WindowParts 変数は使わないで変わりに pWindowsPartsPane を使う
    */
  //------------------------------------------------------------------
  void  Setup(const SSetupInfo& rSetupInfo, gfl2::lyt::LytParts* pWindowsPartsPane );

  //------------------------------------------------------------------
  /**
    *  @func     UpdateWindow
    *  @brief    状態更新
    *  @note     フレーム毎にコールする
    */
  //------------------------------------------------------------------
  void  UpdateWindow(void);


  //------------------------------------------------------------------
  /**
    *  @func     SetMessageSpeed
    *  @brief    メッセージスピードを設定する
    */
  //------------------------------------------------------------------
  void  SetMessageSpeed(Savedata::ConfigSave::MSG_SPEED spd);


  //------------------------------------------------------------------
  /**
    *  @func     ShowMessage
    *  @brief    文字列を設定してWindowを表示する
    *  @param    isImmediateMessage  [in]   true:即時表示 / false:ストリーム（タイピング）表示
    *  @param    isShowTimerIcon     [in]   タイマーアイコンを表示する（SetupでpaneID_TimerIconを指定していること）
    */
  //------------------------------------------------------------------
  void  ShowMessage(const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false);
  void  ShowMessage(const gfl2::str::StrBuf* pStr, const bool isImmediateMessage = false, const bool isShowTimerIcon = false);


  //------------------------------------------------------------------
  /**
    *  @func     HideMessage
    *  @brief    Windowを非表示にする
    */
  //------------------------------------------------------------------
  void  HideMessage(void);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief キー入力可否設定
   * @note  trueで一時キー入力無効
   */
  /* -------------------------------------------------------------------------*/
  void SetSuspendKey( bool flag );


  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  void  SetFinishMode_None(void);                                 /**<  Finishなし              */
  void  SetFinishMode_UserInput(void);                            /**<  Userの入力によって終了  */
  void  SetFinishMode_Timer(const u8 timeoutFrame);               /**<  一定時間後に終了        */
  void  SetFinishMode_UserInputOrTimer(const u8 timeoutFrame);    /**<  User/TimerのHybrid      */


  //------------------------------------------------------------------
  /**
    *  @brief    SetupInfo.pTargetView->GetG2DUtil()を返す
    */
  //------------------------------------------------------------------
  app::util::G2DUtil* GetG2DUtil(void) const;


protected:
  SSetupInfo              m_SetupInfo;
  /*  汎用メッセージwindow  */
  gfl2::lyt::LytPane*     m_pPane_Window;               /**<  メッセージウインドウ（表示制御用）*/
  gfl2::lyt::LytTextBox*  m_pPane_TextBoxImmediate;     /**<  即時表示用TextBox                 */
  gfl2::lyt::LytPane*     m_pPane_Stream;               /**<  Stream表示用TextBox制御           */
  gfl2::lyt::LytParts*    m_pParts_TimerIcon;           /**<  タイマーアイコン                  */

  bool                    m_IsNoticedEOM;               /**<  EndOfMessageを通知済みであるか    */
  bool                    m_IsNoticedFinish;            /**<  Finishを通知済みであるか          */
  
};




} /*  namespace Tool  */
} /*  namespace App   */
#endif  /*  #if !defined(COMMONMESSAGEWINDOWMANIPULATOR_H_INCLUDED)  */
