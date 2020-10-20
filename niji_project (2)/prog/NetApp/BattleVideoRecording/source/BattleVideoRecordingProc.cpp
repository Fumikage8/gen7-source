//==============================================================================
/**
 @file    BattleVideoRecordingProc.h
 @date    2015.09.15
 @note
          バトルビデオ録画プロセス
 */
//==============================================================================
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingProc.h"

//  view
#include "NetApp/BattleVideoRecording/source/BattleVideoRecordingUIViewPool.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "Applib/include/Util/AppUtilFileControlState.h"
#include "Applib/include/Tool/app_tool_MenuCursor.h"
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "System/include/ArcIdGetter.h"
#include "GameSys/include/GameData.h"

//  リソースのインクルード
#include "arc_def_index/arc_def.h"
#include "arc_index/BattleVideoRecordingLytData.gaix"
#include "arc_index/message.gaix"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  Proc::Proc( APP_PARAM* pAppParam ) : GameSys::GameProc()
  {
    _Clear();

    m_pAppParam = pAppParam;
  }

  Proc::~Proc()
  {
    //  ヒープ確保したのを開放はここで
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
  }

  gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* pManager )
  {
    //  アプリ内で使用するメモリ量
    static const s32 HEAP_MEM_SIZE = 0xf0000;
    static const s32 DEV_MEM_SIZE  = 1024 * 1024 * 30;

    static const s32 LOCAL_HEAP_SIZE = 1024 * 600;
    static const s32 LOCAL_DEVICE_HEAP_SIZE  = 1024 * 1024 * 25;

    enum
    {
      SEQ_INIT  = 0,
      SEQ_LOAD,
      SEQ_OBJ_CREATE,
      SEQ_WAIT_OBJ_CREATE,
      SEQ_SD_CARD_VALIDITY_START,
      SEQ_SD_CARD_VALIDITY_WAIT,
      SEQ_END
    };

    int seq = GameProc::GetSubSeq();

    GFL_ASSERT( m_pAppParam->in.pRecorderSaveData != NULL );

    //  初期化( メモリなど必要なのを作成 )
    if( seq == SEQ_INIT )
    {
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

      // パイプライン生成
      {
        app::util::AppRenderingManager::Description renderingMgrDesc;
        renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        m_pRenderingManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppRenderingManager();
        m_pRenderingManager->SyncCreate( renderingMgrDesc, m_pUtilheap, NULL, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
        GFL_ASSERT( m_pRenderingManager->IsCreated() );  // 同期生成なので m_pAppRenderingMgr->IsCreated() では待たない。
      }

      //  フレーム管理生成
      {
        // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
        m_pFrameManagerHeap = GFL_CREATE_LOCAL_HEAP( m_pUtilheap->GetDeviceHeap(), 4096, gfl2::heap::HEAP_TYPE_EXP, false );

        m_pFrameManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) applib::frame::Manager( m_pFrameManagerHeap );
      }

      //  レイアウトファイルロード処理開始
      {
        gfl2::fs::ArcFile::ARCID arcId = NIJI_ARCID_NUM( ARCID_BTLV_VIDEO_RECORDING_LYT_DATA );
        m_pLayoutFileCtrlState  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppUtilFileControlState( arcId, GARC_BattleVideoRecordingLytData_BattleVideoRecordingLytData_applyt_COMP, m_pUtilheap->GetDeviceHeap(), true, 128 );
        m_pLayoutFileCtrlState->Start();
      }

      //  UI画面で使う共通メッセージ
      //  黒画面中なので同期読みでいいや
      m_pAppMsgData = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_battlevideo_rec_DAT, m_pUtilheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
      GFL_ASSERT( m_pAppMsgData );

      //  UIで使用するwordset
      m_pWordSet = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN , m_pUtilheap->GetDeviceHeap() );
      GFL_ASSERT( m_pWordSet );

      //  メニューカーソルのリソース作成
      m_pMenuCursor = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::tool::MenuCursor( m_pUtilheap, &m_pMenuCursorLytBuffer );
      GFL_ASSERT( m_pMenuCursor );

      ++seq;
    }
    //  ファイルロード( ファイルの読み込みをする )
    if( seq == SEQ_LOAD )
    {
      bool  bLoaded = true;

      m_pLayoutFileCtrlState->Update();
      if( m_pMenuCursor->LoadResource() == false )
      {
        bLoaded = false;
      }

      if( m_pLayoutFileCtrlState->IsEnd() == false )
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
      UIViewPool::INIT_PARAM initParam;
      initParam.pAppMsgData             = m_pAppMsgData;
      initParam.pLytBinData             = m_pLayoutFileCtrlState->GetLoadDataBuffer();
      initParam.pMenuCursorLytBinData   = m_pMenuCursorLytBuffer;
      initParam.pRenderManager          = m_pRenderingManager;
      initParam.pUtilHeap               = m_pUtilheap;
      initParam.pWordSet                = m_pWordSet;

      //  UIView生成ファクトリーを生成
      m_pUIViewPool  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) UIViewPool( initParam );

      ++seq;
    }
    
    if( seq == SEQ_WAIT_OBJ_CREATE )
    {
      bool  bRet  = true;

      if( m_pUIViewPool->InitFunc() == false )
      {
        bRet  = false;
      }
      
      if( bRet == true )
      {
        ++seq;
      }
    }

    if( seq == SEQ_SD_CARD_VALIDITY_START )
    {
      //  データのチェック開始
      m_pAppParam->in.pRecorderSaveData->CheckAllSaveDataValidityContinue( m_pUtilheap->GetDeviceHeap()->GetLowerHandle(), NULL, 1, true );

      ++seq;
    }

    if( seq == SEQ_SD_CARD_VALIDITY_WAIT )
    {
      //  チェック終了まで待つ
      if( m_pAppParam->in.pRecorderSaveData->CheckAllSaveDataValidityContinue( m_pUtilheap->GetDeviceHeap()->GetLowerHandle() ) == false )
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

  gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    //  UpdateFuncのシーケンス
    enum
    {
      //  フェードイン処理
      UPDATE_SEQ_FADE_IN_START = 0,
      UPDATE_SEQ_FADE_IN_WAIT,

      //  データチェック
      //  SDカード、サーバーバージョン
      UPDATE_SEQ_CHK_DATA_START,
      UPDATE_SEQ_CHK_DATA_END,

      //  保存
      UPDATE_SEQ_SAVE_START,
      UPDATE_SEQ_SAVE_END,

      UPDATE_SEQ_END
    };

    int seq = GameProc::GetSubSeq();

    LowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();
    UppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();

    //  フェードイン開始
    if( seq == UPDATE_SEQ_FADE_IN_START )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );

      ++seq;
    }
    //  フェードインの完了を待っている
    if( seq == UPDATE_SEQ_FADE_IN_WAIT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        ++seq;
      }
    }

    if( m_pFrameManager->IsMainHandle() == applib::frame::MAINSTATUS_NULL )
    {
      //  データチェック
      {
        if( seq == UPDATE_SEQ_CHK_DATA_START )
        {
          //  フレーム生成
          _CreateFrame( CREATE_FRAME_DATA_CHK );
          ++seq;
        }
        else
        if( seq == UPDATE_SEQ_CHK_DATA_END )
        {
          //  フレーム終了
          if( m_dataChkFrameParam.out.result == DataChkFrame::RESULT_APP_END )
          {
            //  アプリ終了
            seq = UPDATE_SEQ_END;
          }
          else
          if( m_dataChkFrameParam.out.result == DataChkFrame::RESULT_GO_SAVE )
          {
            seq = UPDATE_SEQ_SAVE_START;
          }
        }
      }

      //  録画開始
      {
        if( seq == UPDATE_SEQ_SAVE_START )
        {
          _CreateFrame( CREATE_FRAME_DATA_SAVE );
          ++seq;
        }
        else
        if( seq == UPDATE_SEQ_SAVE_END )
        {
          if( m_dataSaveFrameParam.out.result == DataSaveFrame::RESULT_SAVE_SUCCESS )
          {
            m_pAppParam->out.procResultType = PROC_RESULT_TYPE_SUCCESS;
          }
          else
          if( m_dataSaveFrameParam.out.result == DataSaveFrame::RESULT_SAVE_CANCEL )
          {
            m_pAppParam->out.procResultType = PROC_RESULT_TYPE_CANCEL;
          }
          else
          if( m_dataSaveFrameParam.out.result == DataSaveFrame::RESULT_SAVE_FATAL )
          {
            m_pAppParam->out.procResultType = PROC_RESULT_TYPE_FATAL;
          }
          else
          {
            m_pAppParam->out.procResultType = PROC_RESULT_TYPE_ERROR;
          }

          seq = UPDATE_SEQ_END;
        }
      }
    }

    //  自身で終了させる
    if( seq == UPDATE_SEQ_END )
    {
      //  フェードアウト開始
      if( m_subSeq == 0 )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

        ++m_subSeq;
      }

      //  フェードアウト終了待ち
      if( m_subSeq == 1 )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
        {
          return gfl2::proc::RES_FINISH;
        }
      }
    }

    m_pFrameManager->Main();

    pUppRootView->UpdateTree();
    pLowRootView->UpdateTree();

    m_pRenderingManager->Update();

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  void Proc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    m_pRenderingManager->PreDraw();
  }

  void Proc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
  {
    //  上下のルートViewを取得
    UppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();
    LowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();

    m_pFrameManager->Draw( displayNo );

    //  上下のルートViewを描画
    pUppRootView->DrawTree( displayNo );
    pLowRootView->DrawTree( displayNo );

    m_pRenderingManager->Draw( displayNo );
  }

  gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    int seq = GameProc::GetSubSeq();

    enum
    {
      SEQ_FADE_OUT  = 0,
      SEQ_FRAME_END,
      SEQ_OBJ_DELETE,
      SEQ_CORE_DELETE,
      SEQ_END
    };

    //  フェードアウト
    if( seq == SEQ_FADE_OUT )
    {
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
    else
    //  UIVIEWを破棄する
    if( seq == SEQ_OBJ_DELETE )
    {
      // 終了処理が完了するまで待つ
      if( m_pUIViewPool->EndFunc() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_CORE_DELETE )
    {
      //  ロードしたフォントを開放
      print::SystemFont_ResetDefaultFontInfo();
      print::SystemFont_DeleteNumFont();

      GFL_SAFE_DELETE( m_pUIViewPool );
      GFL_SAFE_DELETE( m_pLayoutFileCtrlState );

      m_pMenuCursor->DeleteResource();
      
      GFL_ASSERT( m_pRenderingManager->CanDestroy() );  // 同期生成なので m_pRenderingManager->CanDestroy() では待たない。
      GFL_SAFE_DELETE( m_pRenderingManager );

      GFL_SAFE_DELETE( m_pFrameManager );
      GFL_SAFE_DELETE( m_pMenuCursor );
      GFL_SAFE_DELETE( m_pWordSet );
      GFL_SAFE_DELETE( m_pAppMsgData );
      
      return gfl2::proc::RES_FINISH;
    }

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  //  フレーム生成
  void Proc::_CreateFrame( const CreateFrame nowSeq )
  {
    if( m_pFrameManager->IsMainHandle() != applib::frame::MAINSTATUS_NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    //  フレーム作成する
    if( nowSeq == CREATE_FRAME_DATA_CHK )
    {
      DataChkFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, DataChkFrame>( m_pFrameManager );
      {
        m_dataChkFrameParam.in.pUIViewPool    = m_pUIViewPool;
        m_dataChkFrameParam.in.pAppHeap       = m_pUtilheap;
        m_dataChkFrameParam.in.pExtSaveData   = m_pAppParam->in.pRecorderSaveData;
        m_dataChkFrameParam.in.serverVersion  = m_pAppParam->in.saveDataServerVersion;
        m_dataChkFrameParam.in.bRecNG         = m_pAppParam->in.bRecNG;

        pFrame->Initialize( &m_dataChkFrameParam );
      }
    }
    else
    if( nowSeq == CREATE_FRAME_DATA_SAVE )
    {
      DataSaveFrame*  pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, DataSaveFrame>( m_pFrameManager );

      {
        m_dataSaveFrameParam.in.pUIViewPool   = m_pUIViewPool;
        m_dataSaveFrameParam.in.pAppHeap      = m_pUtilheap;
        m_dataSaveFrameParam.in.pExtSaveData  = m_pAppParam->in.pRecorderSaveData;

        pFrame->Initialize( &m_dataSaveFrameParam );
      }
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetApp)
