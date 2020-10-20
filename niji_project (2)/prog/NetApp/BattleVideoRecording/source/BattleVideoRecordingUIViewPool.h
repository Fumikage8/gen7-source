#if !defined(NIJI_PROJECT_BATTLE_VIDEO_RECORDING_UIVIEW_POOL_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_RECORDING_UIVIEW_POOL_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoRecordingUIViewPool.h
 @date    2015.09.15
 @note
          アプリで使うUIVIEWすべて生成して管理
          ここでアプリが表示するViewを一括で生成して破棄の管理をする
          またView生成関数を呼んだ時にRootViewのAddやRemoveなどを呼ぶことをする
          これで使う側がAdd/Removeについて気にすることはない
 */
//==============================================================================

//  UIView関連のヘッダーをインクルード

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>
#include "AppLib/include/Tool/app_tool_MessageMenuView.h"
#include "AppLib/include/Tool/GeneralView/GeneralLowerView.h"

//  Viewのインクルード
#include "NetApp/BattleVideoRecording/source/View/BattleVideoRecordingLowRootView.h"
#include "NetApp/BattleVideoRecording/source/View/BattleVideoRecordingUppRootView.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

  class MsgData;

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;
  class AppRenderingManager;
  class AppUtilFileControlState;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)

  class UIView;

GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  //==============================================================================
  /**
   @class     UIViewPool
   */
  //==============================================================================
  class UIViewPool :
  public App::Tool::MessageMenuView::IEventHandler
  {
    GFL_FORBID_COPY_AND_ASSIGN(UIViewPool);

  public:
    enum MENU_WINDWOW_SELECT
    {
      MENU_WINDWOW_SELECT_YES = 0,
      MENU_WINDWOW_SELECT_NO,

      MENU_WINDWOW_SELECT_MAX
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pUtilHeap             = NULL;
        pLytBinData           = NULL;
        pRenderManager        = NULL;
        pAppMsgData           = NULL;
        pWordSet              = NULL;
        pMenuCursorLytBinData = NULL;
      }

      app::util::Heap*                  pUtilHeap;
      void*                             pLytBinData;
      void*                             pMenuCursorLytBinData;
      app::util::AppRenderingManager*   pRenderManager;
      gfl2::str::MsgData*               pAppMsgData;
      print::WordSet*                   pWordSet;
    };

    UIViewPool( const struct INIT_PARAM& rInitParam );
    virtual ~UIViewPool();

    //------------------------------------------------------------------
    /**
      * @brief      初期化
      * @return     初期化完了: true / 初期化中: false
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief  終了
      * @return 終了完了: true / 終了中: false
      */
    //------------------------------------------------------------------
    bool EndFunc();

    //  上画面のルートView取得
    UppRootView*  GetUppRootView() const { return m_pUppRootView; }
    //  下画面のルートView取得
    LowRootView*  GetLowRootView() const { return m_pLowRootView; }
    //  下画面の２択View取得
    app::tool::GeneralLowerView*  GetLowGeneralButtonsView() const { return m_pButtonsView; }

    //  初期パラメータ取得
    const struct INIT_PARAM&  GetInitParam() const { return m_initParam; }

    //  メッセージ表示開始
    App::Tool::MessageMenuView* ShowSystemMessageLower( gfl2::str::MsgData* pMsgData, const u32 strId, app::ui::UIView* pAttachUIView, const bool bImmediateMessage, const bool bUserInput, const bool bShowTimerIcon, const bool bShowBG );

    //  メッセージ表示しているか
    bool IsMessageShow() const { return m_bMessegeShow; }

    //  メッセージメニューViewのコールバック
    virtual void  MessageMenuView_OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode eventCode );

  private:
    void _Clear()
    {
      m_initSeq = 0;
      m_endSeq  = 0;

      m_pLowRootView  = NULL;
      m_pUppRootView  = NULL;
      m_pButtonsView  = NULL;

      m_pLowMessageMenuView = NULL;
      m_pMesssageMenuViewFileCtrl = NULL;

      m_bMessageUserInput = false;
      m_bMessegeShow      = false;
    }

    INIT_PARAM                      m_initParam;

    LowRootView*                    m_pLowRootView;
    UppRootView*                    m_pUppRootView;

    App::Tool::MessageMenuView*     m_pLowMessageMenuView;
    app::util::AppUtilFileControlState*  m_pMesssageMenuViewFileCtrl;

    //  選択のView
    app::tool::GeneralLowerView*    m_pButtonsView;

    u32 m_initSeq;
    u32 m_endSeq;

    bool  m_bMessageUserInput;
    bool  m_bMessegeShow;
  };

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetApp)

#endif  // NIJI_PROJECT_BATTLE_VIDEO_RECORDING_UIVIEW_POOL_H_INCLUDED

