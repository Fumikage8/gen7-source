#if !defined(NIJI_PROJECT_BATTLE_VIDEO_PLYAER_UIVIEW_POOL_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_PLYAER_UIVIEW_POOL_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoPlayerUIViewPool.h
 @date    2015.09.24
 @note
          アプリで使うUIVIEWすべて生成して管理
          ここでアプリが表示するViewを一括で生成して破棄の管理をする
          またView生成関数を呼んだ時にRootViewのAddやRemoveなどを呼ぶことをする
 */
//==============================================================================

//  UIView関連のヘッダーをインクルード

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "AppLib/include/Tool/app_tool_MessageMenuView.h"
#include "AppLib/include/Tool/GeneralView/GeneralLowerView.h"

//  Viewのインクルード
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowRootView.h"
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowDataCtrlView.h"
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowDataListView.h"

#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerUppRootView.h"
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerUppGameInfoView.h"
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerUppGameResultView.h"

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
GFL_NAMESPACE_BEGIN(tool)

  class  PokeIconTexPool;
  class  AppToolTrainerIconRendering;
  class  AppToolBaseGrpFont;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)

  class UIView;

GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //==============================================================================
  /**
   @class     UIViewPool
   */
  //==============================================================================
  class UIViewPool : public App::Tool::MessageMenuView::IEventHandler
  {
    GFL_FORBID_COPY_AND_ASSIGN(UIViewPool);

  public:
    enum UppInfoViewMode
    {
      UPP_INFO_VIEW_MODE_DETAIL = 0,
      UPP_INFO_VIEW_MODE_RESULT
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
        pTrainerIconRendering = NULL;
        pGrpFont              = NULL;
        pPokeIconTexPool      = NULL;
      }

      app::util::Heap*                  pUtilHeap;
      void*                             pLytBinData;
      void*                             pMenuCursorLytBinData;
      app::util::AppRenderingManager*   pRenderManager;
      gfl2::str::MsgData*               pAppMsgData;
      print::WordSet*                   pWordSet;
      app::tool::AppToolTrainerIconRendering*           pTrainerIconRendering;
      app::tool::AppToolBaseGrpFont*                    pGrpFont;
      App::Tool::PokeIconTexPool*                       pPokeIconTexPool;
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
    UppRootView*        GetUppRootView() const { return m_pUppRootView; }

    //  ※複数のViewがあるので一つの関数でまとめる
    //  上画面のルートViewにインフォ関連のViewを追加
    void AddUppRootInfoView();
    //  上画面のルートViewのインフォ関連のView解除
    void RemoveUppRootInfoView();

    //------------------------------------------------------------------
    /**
      * @brief  上画面のインフォViewにビデオデータを設定
      * @param  pVideoData                上画面に表示するビデオデータアドレス
      * @param  bForceVideoData           上画面に強制表示設定するかどうか
      * @note
      *         pViedeDataで設定したアドレスが保持している。
      *         同じアドレスの場合は設定しないようするため
      *         しかし同じアドレスでも再設定するケース（シリアルＮＯデータ）があったので、bForceVideoDataで設定フラグを用意した。
      */
    //------------------------------------------------------------------
    void SetVideoDataUppInfoView( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData, bool bForceVideoData = false );
    //  上画面のインフォViewのモード設定
    void SetUppInfoViewState( const UppInfoViewMode mode );
    //  上画面のインフォViewのモード切替
    void ChangeUppInfoViewState();
    //  上画面のインフォViewのモード取得
    UppInfoViewMode GetUppInfoViewMode() const { return m_uppInfoViewMode; }
    //  上画面のインフォ表示設定
    void SetVisibleUppInfoView( const bool bFlg );
    //  上画面のインフォのアップロード情報を更新
    void UpdateInfoUploadData();

    //  下画面のルートView取得
    LowRootView*      GetLowRootView() const { return m_pLowRootView; }
    //  下画面のデータリストView取得
    LowDataListView*  GetLowDataListView() const { return m_pLowDataListView; }
    //  下画面のデータ制御View取得
    LowDataCtrlView*  GetLowDataCtrlView() const { return m_pLowDataCtrlView; }

    //  下画面の選択View取得
    app::tool::GeneralLowerView*  GetLowGeneralButtonsView() const { return m_pSelectButtonsView; }

    //  初期パラメータ取得
    const struct INIT_PARAM&  GetInitParam() const { return m_initParam; }

    //  下画面のシステムメッセージ表示
    void ShowLowerSystemMessage( gfl2::str::MsgData* pMsgData, const u32 strId, const bool bImmediateMessage, const bool bShowTimerIcon, const bool bShowBG );

    //------------------------------------------------------------------
    /**
      *  @func     メッセージはStrBuf*だけどメニューリストはid指定版
      *  @brief    
      */
    //------------------------------------------------------------------
    void  ShowMessageWithListMenu(
      /*  ウインドウ設定  */
      const gfl2::str::StrBuf*                      pWindowMessage,
      const bool                                    isImmediateMessage,
      const bool                                    bYesNoMode,
      /*  リスト設定  */
      const gfl2::str::MsgData&                     rListMessageData,
      const u32                                     listMessageID_0,
      const u32                                     listMessageID_1,
      const u32                                     listMessageID_2 = UINT_MAX,
      const u32                                     listMessageID_3 = UINT_MAX,
      const u32                                     listMessageID_4 = UINT_MAX,
      const u32                                     listMessageID_5 = UINT_MAX
    );

    //------------------------------------------------------------------
    /**
      *  @func     メッセージはStrBuf*メニューリストのStrBuf*指定版
      *  @brief
      *  @note     他人のバトルビデオデータの選択
      */
    //------------------------------------------------------------------
    bool  ShowMessageWithListMenuByDual( const NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData );

    //  下画面のシステムメッセージ非表示
    void HideLowerSystemMessage();

    //  下画面のシステムメッセージが表示か
    bool IsEnableLowerSystemMessage();

    //  下画面のシステムメッセージ処理が終了しているか
    bool IsEndProcLowerSystemMessage();
    
    //  下画面のシステムメッセージコード取得
    App::Tool::MessageMenuView::IEventHandler::EventCode  GetLowerSystemMessageCode() const { return m_lowerSystemMessageEventCode; }

    //  下画面のメニューウィンドウで押したボタンidxを取得
    u32 GetLowerSystemMessageMenuSelectIdx();

    //  MessageMenuWindowのイベント
    virtual void  MessageMenuView_OnEvent(const EventCode event);

  private:
    void _Clear()
    {
      m_initSeq = 0;
      m_endSeq  = 0;

      m_pVideoData        = NULL;
      m_pLowRootView      = NULL;
      m_pLowDataListView  = NULL;
      m_pLowDataCtrlView  = NULL;

      m_pUppRootView          = NULL;
      m_pUppGameInfoView      = NULL;
      m_pUppGameResultView    = NULL;

      m_pLowMessageMenuView       = NULL;
      m_pMesssageMenuViewFileCtrl = NULL;
      m_pSelectButtonsView        = NULL;

      m_uppInfoViewMode = UPP_INFO_VIEW_MODE_DETAIL;

      m_lowerSystemMessageEventCode = App::Tool::MessageMenuView::IEventHandler::EVENT_;
    }

    INIT_PARAM                      m_initParam;

    LowRootView*                    m_pLowRootView;
    LowDataListView*                m_pLowDataListView;
    LowDataCtrlView*                m_pLowDataCtrlView;

    UppRootView*                    m_pUppRootView;
    UppGameInfoView*                m_pUppGameInfoView;
    UppGameResultView*              m_pUppGameResultView;

    //  選択のView
    app::tool::GeneralLowerView*      m_pSelectButtonsView;

    App::Tool::MessageMenuView*          m_pLowMessageMenuView;
    app::util::AppUtilFileControlState*  m_pMesssageMenuViewFileCtrl;

    UppInfoViewMode  m_uppInfoViewMode;

    NetAppLib::BattleVideoPlayer::VIDEO_DATA*     m_pVideoData;

    App::Tool::MessageMenuView::IEventHandler::EventCode m_lowerSystemMessageEventCode;

    u32 m_initSeq;
    u32 m_endSeq;
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif  // NIJI_PROJECT_BATTLE_VIDEO_PLYAER_UIVIEW_POOL_H_INCLUDED

