//==============================================================================
/**
 @file    BattleVideoPlayerBaseProc.h
 @date    2015.09.24
 @note
          バトルビデオ再生の基本プロセス
 */
//==============================================================================
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerBaseProc.h"

//  gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>

//  niji
#include "System/include/HeapDefine.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "Applib/include/Util/AppUtilFileControlState.h"
#include "Applib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "AppLib/include/Tool/AppToolBaseGrpFont.h"
#include "AppLib/include/Tool/PokeIconTexPool.h"
#include "AppLib/include/Ui/UIViewManager.h"

#include "System/include/nijiAllocator.h"
#include "System/include/ArcIdGetter.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/BattleVideoPlayerLytData.gaix"
#include "arc_index/message.gaix"

#include "GameSys/include/GameData.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  BaseProc::BaseProc( PROC_APP_PARAM* pAppParam ) : GameSys::GameProc()
  {
    _Clear();

    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

    m_pVideoDataManager = m_pAppParam->pVideoDataManager;
    GFL_ASSERT( m_pVideoDataManager );
  }

  BaseProc::~BaseProc()
  {
  }

  gfl2::proc::Result BaseProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    //  アプリ内で使用するメモリ量
    static const s32 HEAP_MEM_SIZE = 1024 * 1024 * 2;
    static const s32 DEV_MEM_SIZE  = 1024 * 1024 * 25;

    static const s32 LOCAL_HEAP_SIZE = 1024 * 1024 * 1;
    static const s32 LOCAL_DEVICE_HEAP_SIZE  = 1024 * 1024 * 24;

    enum
    {
      SEQ_INIT  = 0,
      SEQ_LOAD,
      SEQ_OBJ_CREATE,
      SEQ_WAIT_OBJ_CREATE,
      SEQ_END
    };

    int seq = GameProc::GetSubSeq();

    //  初期化( メモリなど必要なのを作成 )
    if( seq == SEQ_INIT )
    {
      // @fix GFNMCat[1699] 
      //  ペインリストとそれ以外のボタンを同時に検知できないようにするために必要
      app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager );
      m_bOldInputExclusiveKey = pUIViewMan->GetInputExclusiveKeyByButtonAnimation();
      pUIViewMan->SetInputExclusiveKeyByButtonAnimation( true );

      //  全体が使用するメモリを確保
      {
        gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pHeapMem = GFL_CREATE_LOCAL_HEAP( parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

        gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pDevMem = GFL_CREATE_LOCAL_HEAP( parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
      }

      // ローカルヒープ生成
      {
        m_pUtilheap = GFL_NEW( m_pDevMem ) app::util::Heap();
        m_pUtilheap->LocalHeapCreate( m_pHeapMem, m_pDevMem, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
      }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc0\n" );
      }
#endif
      // パイプライン生成
      {
        app::util::AppRenderingManager::Description renderingMgrDesc;
        //  画面のレンダー設定
        {
          //  下画面
          renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

          //  上画面はレイアウト->モデル->レイアウト描画
          renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder  = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

          //  トレーナーアイコンを表示するためのターゲットレンダーテクスチャを扱うための設定
          renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN].renderOrder     = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
        }

        
        m_pNijiAllocatorForEdgeMapTexture = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) System::nijiAllocator( m_pUtilheap->GetDeviceHeap() );

        m_pRenderingManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppRenderingManager();
        //  非同期で作成
        m_pRenderingManager->StartAsyncCreate(
            renderingMgrDesc,
            m_pUtilheap,
            m_pNijiAllocatorForEdgeMapTexture,
            GameSys::GameProc::GetGameManager()->GetAsyncFileManager()
        );
      }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc1\n" );
      }
#endif
      //  フレーム管理生成
      {
        // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
        m_pFrameManagerHeap = GFL_CREATE_LOCAL_HEAP( m_pUtilheap->GetDeviceHeap(), 4096, gfl2::heap::HEAP_TYPE_EXP, false );

        m_pFrameManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) applib::frame::Manager( m_pFrameManagerHeap );
      }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc2\n" );
      }
#endif

      //  レイアウトファイルロード処理開始
      {
        gfl2::fs::ArcFile::ARCID arcId = NIJI_ARCID_NUM( ARCID_BTLV_VIDEO_PLAYER_LYT_DATA );
        m_pLayoutFileCtrlState  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppUtilFileControlState( arcId, GARC_BattleVideoPlayerLytData_BattleVideoPlayerLytData_applyt_COMP, m_pUtilheap->GetDeviceHeap(), true, 128 );
      }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc4\n" );
      }
#endif
      //  UI画面で使う共通メッセージ
      //  黒画面中なので同期読みでいいや
      m_pAppMsgData = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_battlevideo_player_DAT, m_pUtilheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
      GFL_ASSERT( m_pAppMsgData );

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc5\n" );
      }
#endif

      //  UIで使用するwordset
      m_pWordSet = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN , m_pUtilheap->GetDeviceHeap() );
      GFL_ASSERT( m_pWordSet );
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFun6\n" );
      }
#endif
      //  メニューカーソルのリソース作成
      m_pMenuCursor = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::tool::MenuCursor( m_pUtilheap, &m_pMenuCursorLytBuffer );
      GFL_ASSERT( m_pMenuCursor );
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc7\n" );
      }
#endif
      //  グラフィックフォント作成
      {
        m_pGrpFontData  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::tool::AppToolBaseGrpFont();
        m_pGrpFontData->Initialize( m_pUtilheap, app::tool::AppToolBaseGrpFont::FONT_TYPE_BTL_VIDEO_PLAYER );
      }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc8\n" );
      }
#endif
      //  ポケモンアイコン全ロード
      {
        m_pPokeIconTexPool  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) App::Tool::PokeIconTexPool( m_pUtilheap->GetDeviceHeap(), GetGameManager()->GetAsyncFileManager() );
      }

      //  ファイル読み開始
      m_pLayoutFileCtrlState->Start();

      ++seq;
    }
    //  ファイルロード( ファイルの読み込みをする )
    if( seq == SEQ_LOAD )
    {
      bool  bLoaded = true;

      if( m_pGrpFontData->IsReady() == false )
      {
        bLoaded = false;
      }

      m_pRenderingManager->Update();
      if( m_pRenderingManager->IsCreated() == false )
      {
        bLoaded = false;
      }

      m_pLayoutFileCtrlState->Update();
      if( m_pMenuCursor->LoadResource() == false )
      {
        bLoaded = false;
      }

      if( m_pLayoutFileCtrlState->IsEnd() == false )
      {
        bLoaded = false;
      }

      if( m_pPokeIconTexPool->InitFunc() == false )
      {
        bLoaded = false;
      }

      if( bLoaded == true )
      {
        ++seq;
      }
    }

    //  オブジェクトを作成( Viewはすべてここで作成する )
    if( seq == SEQ_OBJ_CREATE )
    {
      //  トレーナーアイコン表示生成
      {
        m_pTrainerIconRendering = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::tool::AppToolTrainerIconRendering();
        m_pTrainerIconRendering->Initialize( m_pRenderingManager, m_pUtilheap, GetGameManager()->GetAsyncFileManager(), 6, app::util::G2DUtil::SETUP_UPPER );
        UCHIDA_PRINT( "VideoPlayer TrainerHeap(%d), TrainerDevHeap(%d) \n", app::tool::AppToolTrainerIconRendering::GetUseSystemHeapSize( 6 ), app::tool::AppToolTrainerIconRendering::GetUseDeviceHeapSize( 6 ) );
      }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      //  残りヒープ数
      {
        m_pUtilheap->GetDeviceHeap()->PrintHeapInfo( "BattleVideoPlayerBaseProc InitFunc3\n" );
      }
#endif

      UIViewPool::INIT_PARAM initParam;
      initParam.pAppMsgData             = m_pAppMsgData;
      initParam.pLytBinData             = m_pLayoutFileCtrlState->GetLoadDataBuffer();
      initParam.pMenuCursorLytBinData   = m_pMenuCursorLytBuffer;
      initParam.pRenderManager          = m_pRenderingManager;
      initParam.pUtilHeap               = m_pUtilheap;
      initParam.pWordSet                = m_pWordSet;
      initParam.pTrainerIconRendering   = m_pTrainerIconRendering;
      initParam.pGrpFont                = m_pGrpFontData;
      initParam.pPokeIconTexPool        = m_pPokeIconTexPool;

      //  UIView生成ファクトリーを生成
      m_pUIViewPool  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) UIViewPool( initParam );

      ++seq;
    }
    
    if( seq == SEQ_WAIT_OBJ_CREATE )
    {
      bool  bRet  = true;

      if( m_pTrainerIconRendering->IsReady() == false )
      {
        bRet = false;
      }

      if( m_pUIViewPool->InitFunc() == false )
      {
        bRet  = false;
      }
      
      if( bRet == true )
      {
        ++seq;
      }
    }

    //  終了
    if( seq == SEQ_END )
    {
      return gfl2::proc::RES_FINISH;
    }

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  gfl2::proc::Result BaseProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    int seq = GameProc::GetSubSeq();

    //  初回フロー
    if( seq == 0 )
    {
      seq = _StartSeq( UPDATA_SEQ_ROOT_FADE_IN_START );
    }

    //  フェードイン開始
    if( seq == UPDATA_SEQ_ROOT_FADE_IN_WAIT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        if( m_pAppParam->in.procRunType == PROC_RUN_TYPE_BEGIN )
        {
          seq = _StartSeq( UPDATE_SEQ_ROOT_TUTORIAL_START );
        }
        else
        if( m_pAppParam->in.procRunType == PROC_RUN_TYPE_RETURN )
        {
          //  復帰データを使ってシーケンスを作成する
          seq = _StartReturnSeq( m_pAppParam->procRunTypeReturnData );
        }
      }
    }
    else
    //  チュートリアル
    if( seq == UPDATE_SEQ_ROOT_TUTORIAL_WAIT )
    {
      if( m_pFrameManager->IsMainHandle() == false )
      {
        //  フレーム終了
        if( m_tutorialFrameParam.out.result == TutorialFrame::RESULT_SELECT_OK )
        {
          // @fix GFNMCat[806] レコーダーを起動を選択したのでチュートリアルフラグを立てる
          GameSys::GameManager* pGameManager  = GameProc::GetGameManager();
          Savedata::Misc* pMiscSaveData = pGameManager->GetGameData()->GetMisc();

          //pMiscSaveData->SetBattleVideoPlayerTutorial();

          //  SDカード確認へ
          seq = _StartSeq( UPDATE_SEQ_ROOT_SD_CARD_CHK_START );
        }
        else
        if( m_tutorialFrameParam.out.result == TutorialFrame::RESULT_SELECT_CANCEL )
        {
          //  終了させる
          seq = _StartSeq( UPDATA_SEQ_ROOT_APP_CANCEL );
        }
      }
    }
    else
    //  SDカードチェック
    if( seq == UPDATE_SEQ_ROOT_SD_CARD_CHK_WAIT )
    {
      if( m_pFrameManager->IsMainHandle() == false )
      {
        if( m_sdCardChkFrameParam.out.result == SDCardChkFrame::RESULT_SUCCESS )
        {
          //  ビデオデータがある
          seq = UPDATE_SEQ_ROOT_SD_CARD_CHK_END;
        }
        else
        if( m_sdCardChkFrameParam.out.result == SDCardChkFrame::RESULT_FATAL )
        {
          //  失敗設定してアプリ終了
          seq = _StartSeq( UPDATE_SEQ_ROOT_APP_FATAL );
        }
        else
        if( m_sdCardChkFrameParam.out.result == SDCardChkFrame::RESULT_ERROR )
        {
          //  エラーの場合は失敗設定しないでアプリ終了
          seq = _StartSeq( UPDATA_SEQ_ROOT_APP_CANCEL );
        }
      }
    }
    else
    //  このシーケンスは表示のみなのでフレーム作成までする必要はない
    if( seq == UPDATE_SEQ_ROOT_NON_DATA_WAIT )
    {
      LowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();
      if( pLowRootView->IsEndInput() == true )
      {
        if( pLowRootView->GetPushBtn() == LowRootView::BUTTON_ID_BACK )
        {
          seq = UPDATE_SEQ_ROOT_NON_DATA_END;
        }
      }
    }
    else
    if( seq == UPDATE_SEQ_ROOT_NON_DATA_END )
    {
    }
    else
    if( seq == UPDATA_SEQ_ROOT_BTL_PLAY )
    {
      m_pAppParam->out.procResultType = PROC_RESULT_TYPE_CALL_BTL_PLAY;

      //  いったんprocは殺す
      seq = _StartSeq( UPDATE_SEQ_ROOT_PROC_END );
    }
    else
    if( seq == UPDATE_SEQ_ROOT_BTL_AI )
    {
      m_pAppParam->out.procResultType = PROC_RESULT_TYPE_CALL_BTL_AI;

      //  いったんprocは殺す
      seq = _StartSeq( UPDATE_SEQ_ROOT_PROC_END );
    }
    else
    if( seq == UPDATA_SEQ_ROOT_APP_CANCEL )
    {
      m_pAppParam->out.procResultType = PROC_RESULT_TYPE_CANCEL;
      seq = _StartSeq( UPDATE_SEQ_ROOT_PROC_END );
    }
    else
    if( seq == UPDATE_SEQ_ROOT_APP_FATAL )
    {
      //  失敗して終了させる

      // FatalErrorEvent呼び出し
      GameSys::GameManager* pGameManager  = GameProc::GetGameManager();
      pGameManager->SetFatalErrorEvent();

      m_pAppParam->out.procResultType = PROC_RESULT_TYPE_FATAL;
      seq = _StartSeq( UPDATE_SEQ_ROOT_PROC_END );
    }
    
    if( seq == UPDATE_SEQ_ROOT_PROC_END )
    {
      if( m_updateSubSeq == 0 )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

        ++m_updateSubSeq;
      }

      if( m_updateSubSeq == 1 )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
        {
          return  gfl2::proc::RES_FINISH;
        }
      }
    }

    m_frameStatus = m_pFrameManager->Main();

    if( _GetFrameMangerProcOnly() == false )
    {
      UppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();
      LowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();

      pLowRootView->UpdateTree();
      pUppRootView->UpdateTree();

      m_pTrainerIconRendering->Update();
    }

    m_pRenderingManager->Update();

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  void BaseProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    m_pRenderingManager->PreDraw();
  }

  void BaseProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
  {
    //  上下のルートViewを取得
    UppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();
    LowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();

    m_pFrameManager->Draw( displayNo );

    if( _GetFrameMangerProcOnly() == false )
    {
      //  上下のルートViewを描画
      pUppRootView->DrawTree( displayNo );
      pLowRootView->DrawTree( displayNo );

      m_pTrainerIconRendering->Draw( displayNo );
    }

    m_pRenderingManager->Draw( displayNo );
  }

  gfl2::proc::Result BaseProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    int seq = GameProc::GetSubSeq();

    enum
    {
      SEQ_FADE_OUT,
      SEQ_FRAME_END,
      SEQ_TRINER_RENDER_END,
      SEQ_TRINER_RENDER_WAIT_END,
      SEQ_OBJ_DELETE,
      SEQ_CORE_DELETE,
      SEQ_CORE_IS_DELETE,
      SEQ_END
    };

    //  フェードアウト
    if( seq == SEQ_FADE_OUT )
    {
      //  復帰データを生成
      {
        if( m_updateSeq == UPDATE_SEQ_ROOT_DATA_CTRL )
        {
          //  復帰のためにシーケンスは保管
          m_pAppParam->procRunTypeReturnData.seq  = m_updateSeq;

          //  選択リストデータからリストデータを生成（復帰時は模擬線やバトル再生の選択の過程で必ず選択リストの値が必ず入っている）
          m_pAppParam->procRunTypeReturnData.pActiveVideoData = m_pAppParam->out.pActiveVideoData;
          GFL_ASSERT( m_pAppParam->procRunTypeReturnData.pActiveVideoData != NULL );

          //  スクロールデータ
          m_pAppParam->procRunTypeReturnData.listScroll     = m_dataListFrameParam.out.listScrollData.listScroll;
          m_pAppParam->procRunTypeReturnData.listCursorPos  = m_dataListFrameParam.out.listScrollData.listPos;

          //  上画面のインフォモード
          m_pAppParam->procRunTypeReturnData.infoDrawMode = (u32)m_pUIViewPool->GetUppInfoViewMode();
        }
      }

      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      ++seq;
    }

    //  フレームを破棄する
    if( seq == SEQ_FRAME_END )
    {
      // 終了処理が完了するまで待つ
      if( m_pFrameManager->End() == false )
      {
        ++seq;
      }
    }

    if( seq == SEQ_TRINER_RENDER_END )
    {
      m_pTrainerIconRendering->Terminate();
      ++seq;
    }

    if( seq == SEQ_TRINER_RENDER_WAIT_END )
    {
      if( m_pTrainerIconRendering->IsDelete() == true )
      {
        ++seq;
      }
    }

    //  UIVIEWを破棄する
    if( seq == SEQ_OBJ_DELETE )
    {
      // 終了処理が完了するまで待つ
      if( m_pUIViewPool->EndFunc() == true )
      {
        ++seq;
      }
    }

    //  破棄開始
    if( seq == SEQ_CORE_DELETE )
    {
      m_pGrpFontData->Terminate();

      ++seq;
    }

    //  破棄終了まで待つ
    if( seq == SEQ_CORE_IS_DELETE )
    {
      bool  bEnd  = true;

      if( m_pPokeIconTexPool->EndFunc() == false )
      {
        bEnd  = false;
      }

      if( m_pRenderingManager->CanDestroy() == false )
      {
        bEnd  = false;
      }

      if( bEnd == true )
      {
        ++seq;
      }
    }

    if( seq == SEQ_END )
    {
      //  ロードしたフォントを開放
      print::SystemFont_ResetDefaultFontInfo();
      print::SystemFont_DeleteNumFont();

      GFL_SAFE_DELETE( m_pUIViewPool );
      GFL_SAFE_DELETE( m_pLayoutFileCtrlState );

      m_pMenuCursor->DeleteResource();
      
      GFL_SAFE_DELETE( m_pRenderingManager );

      GFL_SAFE_DELETE( m_pGrpFontData );
      GFL_SAFE_DELETE( m_pFrameManager );
      GFL_SAFE_DELETE( m_pMenuCursor );
      GFL_SAFE_DELETE( m_pWordSet );
      GFL_SAFE_DELETE( m_pAppMsgData );
      GFL_SAFE_DELETE( m_pTrainerIconRendering );
      GFL_SAFE_DELETE( m_pNijiAllocatorForEdgeMapTexture );
      GFL_SAFE_DELETE( m_pPokeIconTexPool );

      GFL_DELETE_HEAP( m_pFrameManagerHeap );
      
      GFL_SAFE_DELETE( m_pUtilheap );

      if( m_pDevMem != NULL )
      {
        GFL_DELETE_HEAP(m_pDevMem);
        m_pDevMem = NULL;
      }

      if( m_pHeapMem != NULL )
      {
        GFL_DELETE_HEAP(m_pHeapMem);
        m_pHeapMem = NULL;
      }

      // @fix GFNMCat[1699] キー入力の判定処理を元に戻す
      app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager );
      pUIViewMan->SetInputExclusiveKeyByButtonAnimation( m_bOldInputExclusiveKey );

      return gfl2::proc::RES_FINISH;
    }

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  u32 BaseProc::_StartSeq( const u32 seq )
  {
    u32 newSeq  = seq;
    m_updateSubSeq  = 0;
    m_commonCount   = 0;

    if( newSeq == UPDATE_SEQ_ROOT_OTHER )
    {
      //  こちらで管理していないシーケンス
      return  newSeq;
    }

    if( seq == UPDATA_SEQ_ROOT_FADE_IN_START )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );

      newSeq = UPDATA_SEQ_ROOT_FADE_IN_WAIT;
    }
    else
    if( newSeq == UPDATE_SEQ_ROOT_TUTORIAL_START )
    {
      //  チュートリアルフラグがあるとチュートリアルフレーム作成
      GameSys::GameManager* pGameManager  = GameProc::GetGameManager();
      Savedata::Misc* pMiscSaveData = pGameManager->GetGameData()->GetMisc();

      //  チュートリアルをしていないので出す
      if( pMiscSaveData->GetBattleVideoPlayerTutorial() == false )
      //  フレーム生成
      {
        m_tutorialFrameParam.in.pUIViewPool = m_pUIViewPool;
        TutorialFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, TutorialFrame>( m_pFrameManager );
        pFrame->Initialize( &m_tutorialFrameParam );

        newSeq = UPDATE_SEQ_ROOT_TUTORIAL_WAIT;
      }
      else
      {
        //  すでにチュートリアルを見たのでSDカードチェックへ
        newSeq  = _StartSeq( UPDATE_SEQ_ROOT_SD_CARD_CHK_START );
      }
    }
    else
    //  SDカードチェック
    if( seq == UPDATE_SEQ_ROOT_SD_CARD_CHK_START )
    {
      //  フレーム生成
      {
        m_sdCardChkFrameParam.in.pUIViewPool = m_pUIViewPool;
        m_sdCardChkFrameParam.in.pVideoDataManager  = m_pVideoDataManager;

        SDCardChkFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, SDCardChkFrame>( m_pFrameManager );
        pFrame->Initialize( &m_sdCardChkFrameParam );
      }

      newSeq = UPDATE_SEQ_ROOT_SD_CARD_CHK_WAIT;
    }
    //  このシーケンスは表示のみなのでフレーム作成までする必要はない
    else
    if( newSeq == UPDATE_SEQ_ROOT_NON_DATA_START )
    {
      const UIViewPool::INIT_PARAM& rUiViewPoolInitParam = m_pUIViewPool->GetInitParam();

      gfl2::str::MsgData* pAppMsgData = rUiViewPoolInitParam.pAppMsgData;

      //  下
      {
        LowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();
        pLowRootView->SetState( LowRootView::STATE_IDLE );

        pLowRootView->RemoveAllSubView();

        //  Bボタンを表示
        pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, true );
      }

      //  上
      {
        UppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();
        pUppRootView->SetState( UppRootView::STATE_IDLE );

        pUppRootView->RemoveAllSubView();

        gfl2::str::StrBuf*  pTempStrBuf = pUppRootView->GetG2DUtil()->GetTempStrBuf( 0 );

        //  タイトル表記
        {
          pAppMsgData->GetString( msg_bvp_sys_up_01, *pTempStrBuf );
          pUppRootView->ShowMessageUpperOneLine( pTempStrBuf );
        }

        //  エラーメッセージを表示
        {
          pAppMsgData->GetString( msg_bvp_sys_low_02, *pTempStrBuf );

          pUppRootView->ShowMessageCenter( pTempStrBuf );
        }
      }

      newSeq = UPDATE_SEQ_ROOT_NON_DATA_WAIT;
    }
    else
    if( newSeq == UPDATE_SEQ_ROOT_APP_FATAL )
    {
      
    }

    //  ビデオデータリスト
    if( newSeq == UPDATE_SEQ_ROOT_DATA_LIST )
    {
      //  ビデオデータを更新
      m_pVideoDataManager->UpdateTitleName( m_pAppMsgData, m_pWordSet );

      //  フレーム作成
      {
        m_dataListFrameParam.in.pUIViewPool       = m_pUIViewPool;
        m_dataListFrameParam.in.pVideoDataManager = m_pVideoDataManager;
        m_dataListFrameParam.in.listScrollData.listScroll = m_dataListFrameParam.out.listScrollData.listScroll;
        m_dataListFrameParam.in.listScrollData.listPos    = m_dataListFrameParam.out.listScrollData.listPos;

        DataListFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, DataListFrame>( m_pFrameManager );
        pFrame->Initialize( &m_dataListFrameParam );
      }
    }

    return  newSeq;
  }

  //  データ操作シーケンス
  u32 BaseProc::_StartDataCtrlSeq( const u32 dataListIdx, const DataCtrlFrameMode mode )
  {
    NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData  = m_pVideoDataManager->GetListData( dataListIdx );

    return  _StartDataCtrlSeq( pVideoData, mode );
  }

  //  データ操作シーケンス
  u32 BaseProc::_StartDataCtrlSeq( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData, const DataCtrlFrameMode mode )
  {
    GFL_ASSERT( pVideoData != NULL );

    //  ビデオデータを選択した状態にする
    m_pAppParam->out.pActiveVideoData = pVideoData;

    u32 seq = _StartSeq( UPDATE_SEQ_ROOT_DATA_CTRL );

    //  ビデオデータ操作
    if( seq == UPDATE_SEQ_ROOT_DATA_CTRL )
    {
      //  ビデオデータを更新
      m_pVideoDataManager->UpdateTitleName( m_pAppMsgData, m_pWordSet );

      //  フレーム作成
      {
        m_dataCtrlFrameParam.in.pUIViewPool       = m_pUIViewPool;
        m_dataCtrlFrameParam.in.pVideoDataManager = m_pVideoDataManager;
        m_dataCtrlFrameParam.in.pVideoData        = m_pAppParam->out.pActiveVideoData;

        if( mode == DATA_CTRL_FRAME_MODE_LOCAL )
        {
          DataCtrlLocalFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, DataCtrlLocalFrame>( m_pFrameManager );
          pFrame->Initialize( &m_dataCtrlFrameParam );
        }
        else
        if( mode == DATA_CTRL_FRAME_MODE_NET )
        {
          DataCtrlNetFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, DataCtrlNetFrame>( m_pFrameManager );
          pFrame->Initialize( &m_dataCtrlFrameParam );
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }

      m_pAppParam->procRunTypeReturnData.dataCtrlMode = (u32)mode;

      m_updateSeq  = seq;
    }
    else
    {
      //  seqの値は絶対にUPDATE_SEQ_ROOT_DATA_CTRLにしないとアウト
      GFL_ASSERT( 0 );
    }

    return  seq;
  }

  //  復帰データからシーケンス開始
  u32 BaseProc::_StartReturnSeq( const PROC_RUN_TYPE_RETURN_DATA& rReturnData )
  {
    //  今回はデータ操作からの復帰しかないのでそれのみに
    GFL_ASSERT( rReturnData.seq == UPDATE_SEQ_ROOT_DATA_CTRL );
    GFL_ASSERT( rReturnData.pActiveVideoData != NULL );

    //  情報更新して表示
    {
      m_pUIViewPool->SetVideoDataUppInfoView( rReturnData.pActiveVideoData );
      m_pUIViewPool->SetUppInfoViewState( (UIViewPool::UppInfoViewMode)rReturnData.infoDrawMode );
      m_pUIViewPool->SetVisibleUppInfoView( true );
    }

    //  リスト設定
    m_dataListFrameParam.in.listScrollData.listPos    = m_dataListFrameParam.out.listScrollData.listPos     = rReturnData.listCursorPos;
    m_dataListFrameParam.in.listScrollData.listScroll = m_dataListFrameParam.out.listScrollData.listScroll  = rReturnData.listScroll;

    u32 seq = _StartDataCtrlSeq( rReturnData.pActiveVideoData, (DataCtrlFrameMode)rReturnData.dataCtrlMode );
    return  seq;
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
