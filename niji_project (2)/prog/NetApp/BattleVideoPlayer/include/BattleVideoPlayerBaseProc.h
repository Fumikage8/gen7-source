#if !defined(NIJI_PROJECT_BATTLE_VIDEO_PLYAER_BASE_PROC_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_PLYAER_BASE_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoPlayerProc.h
 @date    2015.09.15
 @note
          バトルビデオ録画プロセス
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "Applib/include/Frame/AppFrameManager.h"

//  view
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

//  videoデータ管理
//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

//  frame
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerSDCardChkFrame.h"
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerTutorialFrame.h"

//  フレーム
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataListFrame.h"
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlLocalFrame.h"
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlNetFrame.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class AppUtilFileControlState;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIconToolEx;
  class MenuCursor;
  class AppToolTrainerIconRendering;
  class AppToolBaseGrpFont;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

  class PokeIconTexPool;

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

//  前方宣言
GFL_NAMESPACE_BEGIN(ExtSavedata)

  class BattleRecorderSaveData;

GFL_NAMESPACE_END(ExtSavedata)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //==============================================================================
  /**
   @class     Proc
   */
  //==============================================================================
  class BaseProc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(BaseProc);

  public:
    BaseProc( PROC_APP_PARAM* pAppParam );
    virtual ~BaseProc();

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  protected:
    // heapMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                             m_pHeapMem;
    // devMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                             m_pDevMem;
    //  frameManager用のヒープ
    gfl2::heap::HeapBase*                             m_pFrameManagerHeap;

    app::util::Heap*                                  m_pUtilheap;

    app::util::AppRenderingManager*                   m_pRenderingManager;
    applib::frame::Manager*                           m_pFrameManager;
    applib::frame::MainStatus                         m_frameStatus;

    //  レイアウトファイルロード制御
    app::util::AppUtilFileControlState*               m_pLayoutFileCtrlState;

    //  UIViewインスタンスプール
    UIViewPool*                                       m_pUIViewPool;

    //  メッセージ
    gfl2::str::MsgData*                               m_pAppMsgData;
    //  ワードセット
    print::WordSet*                                   m_pWordSet;

    //  メニューカーソルのリソース
    app::tool::MenuCursor*                            m_pMenuCursor;
    void*                                             m_pMenuCursorLytBuffer;

    //  アプリ側で書き込むためのビデオデータリスト
    //  別procから復帰した場合に必要になる。
    NetAppLib::BattleVideoPlayer::VideoDataManager*   m_pVideoDataManager;

    //  フレーム
    SDCardChkFrame::FRAME_PARAM                       m_sdCardChkFrameParam;
    TutorialFrame::FRAME_PARAM                        m_tutorialFrameParam;

    //  トレーナーアイコン
    app::tool::AppToolTrainerIconRendering*           m_pTrainerIconRendering;

    //  グラフィックフォント
    app::tool::AppToolBaseGrpFont*                    m_pGrpFontData;

    //  外部設定したAppParam
    PROC_APP_PARAM*                                   m_pAppParam;
    
  protected:
    enum
    {
      //  フェードイン(必ず最初はこれが呼ばれる)
      UPDATA_SEQ_ROOT_FADE_IN_START = 0,
      UPDATA_SEQ_ROOT_FADE_IN_WAIT,
      UPDATA_SEQ_ROOT_FADE_IN_END,

      //  チュートリル
      UPDATE_SEQ_ROOT_TUTORIAL_START,
      UPDATE_SEQ_ROOT_TUTORIAL_WAIT,
      UPDATE_SEQ_ROOT_TUTORIAL_END,

      //  SDカード確認
      UPDATE_SEQ_ROOT_SD_CARD_CHK_START,
      UPDATE_SEQ_ROOT_SD_CARD_CHK_WAIT,
      UPDATE_SEQ_ROOT_SD_CARD_CHK_END,

      //  ビデオデータがない画面
      UPDATE_SEQ_ROOT_NON_DATA_START,
      UPDATE_SEQ_ROOT_NON_DATA_WAIT,
      UPDATE_SEQ_ROOT_NON_DATA_END,

      //  データリスト
      UPDATE_SEQ_ROOT_DATA_LIST,
      //  データ操作
      UPDATE_SEQ_ROOT_DATA_CTRL,

      //  再生
      UPDATA_SEQ_ROOT_BTL_PLAY,

      //  模擬戦
      UPDATE_SEQ_ROOT_BTL_AI,

      //  失敗強制終了
      UPDATE_SEQ_ROOT_APP_FATAL,

      //  キャンセルしてアプリ終了
      UPDATA_SEQ_ROOT_APP_CANCEL,

      //  proc終了
      UPDATE_SEQ_ROOT_PROC_END,

      UPDATE_SEQ_ROOT_OTHER,
    };

    enum DataCtrlFrameMode
    {
      DATA_CTRL_FRAME_MODE_LOCAL  = 0,
      DATA_CTRL_FRAME_MODE_NET,
    };

    virtual u32 _StartSeq( const u32 seq );

    //  復帰データからシーケンス開始
    virtual u32 _StartReturnSeq( const PROC_RUN_TYPE_RETURN_DATA& rReturnData );

    //  データ操作シーケンス
    u32 _StartDataCtrlSeq( const u32 dataListIdx, const DataCtrlFrameMode mode );
    u32 _StartDataCtrlSeq( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData, const DataCtrlFrameMode mode );

    DataListFrame::FRAME_PARAM                  m_dataListFrameParam;
    DataCtrlBaseFrame::FRAME_PARAM              m_dataCtrlFrameParam;

    App::Tool::PokeIconTexPool*                 m_pPokeIconTexPool;

    u32                                         m_commonCount;
  private:
    //  フレーム管理のみの制御にするか
    virtual bool _GetFrameMangerProcOnly() { return false; }

    void _Clear()
    {
      m_pAppParam             = NULL;
      m_pHeapMem              = NULL;
      m_pDevMem               = NULL;
      m_pUtilheap             = NULL;
      m_pUIViewPool           = NULL;
      m_pLayoutFileCtrlState  = NULL;
      m_pAppMsgData           = NULL;
      m_pWordSet              = NULL;
      m_pMenuCursor           = NULL;
      m_pMenuCursorLytBuffer  = NULL;
      m_pVideoDataManager     = NULL;
      m_pTrainerIconRendering = NULL;
      m_pPokeIconTexPool      = NULL;
      m_pNijiAllocatorForEdgeMapTexture  = NULL;
      m_pFrameManagerHeap = NULL;
      m_pRenderingManager = NULL;
      m_pFrameManager     = NULL;
      m_frameStatus       = applib::frame::MAINSTATUS_NULL;
      m_pGrpFontData      = NULL;
      m_commonCount       = 0;

      m_updateSeq             = 0;
      m_updateSubSeq          = 0;
      m_bOldInputExclusiveKey = false;
    }

    u32                                         m_updateSeq;
    u32                                         m_updateSubSeq;
    bool                                        m_bOldInputExclusiveKey;

    //  3Dモデル描画用
    System::nijiAllocator*                      m_pNijiAllocatorForEdgeMapTexture;  // VRAM限定。
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)


#endif  // NIJI_PROJECT_BATTLE_VIDEO_PLYAER_BASE_PROC_H_INCLUDED

