//======================================================================
/**
 * @file    FinderStudioCaptureProc.cpp
 * @date    2016/08/25
 * @author  harada_takumi
 * @brief   ファインダースタジオ「撮影」プロック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/include/FinderStudioCaptureProc.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioParentView.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>

#include "FinderStudioCaptureViewerManager.h"
#include "FinderStudioCaptureHeapDefine.h"
#include "Frame/FinderStudioCaptureFrameManager.h"
#include "View/FinderStudioCaptureViewDefine.h"
#include "View/FinderStudioCaptureUpperView.h"
#include "View/FinderStudioCaptureBGView.h"
#include "View/FinderStudioCapturePokeListView.h"
#include "View/FinderStudioCaptureBGListView.h"
#include "View/FinderStudioCaptureDressupListView.h"
#include "View/FinderStudioCaptureCompositionView.h"
#include "View/FinderStudioCaptureSelectButtonView.h"
#include "View/FinderStudioCaptureShootView.h"
#include "View/FinderStudioCaptureZMotionView.h"
#include "View/FinderStudioCaptureCameraView.h"
#include "View/FinderStudioCapturePhotoListView.h"
#include "View/FinderStudioCapturePrepareSelectView.h"
#include "View/FinderStudioCapturePrepareCommonView.h"
#include "View/FinderStudioCaptureShootCommonView.h"
#include "View/FinderStudioCaptureShootLayoutView.h"

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Tool/PokeIcon.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <print/include/WordSet.h>
#include <print/include/SystemFont.h>
#include <Savedata/include/FinderStudioSave.h>
#include <System/include/HeapDefine.h>
#include <System/include/nijiAllocator.h>

#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/FinderStudioCapture.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 関数ポインタ begin ]---

typedef bool( Proc::*INIT_END_FUNC )( void );

/// ---[ 関数ポインタ end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
Proc::Proc( void )
  : m_pRootHeap( NULL )
  , m_pProcHeap( NULL )
  , m_pManagerHeap( NULL )
  , m_pImgdbHeap( NULL )
  , m_pViewerHeap( NULL )
  , m_pPhotoTextureHeap( NULL )
  , m_pAppHeap( NULL )
  , m_pViewerManager( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pFrameManager( NULL )
  , m_pWork( NULL )
  , m_pViewList( NULL )
  , m_pLytDatBuff( NULL )
  , m_pParentView( NULL )
  , m_pUpperView( NULL )
  , m_pBGView( NULL )
  , m_pPokeListView( NULL )
  , m_pBGListView( NULL )
  , m_pDressupListView( NULL )
  , m_pCompositionView( NULL )
  , m_pSelectButtonView( NULL )
  , m_pShootView( NULL )
  , m_pZMotionView( NULL )
  , m_pCameraView( NULL )
  , m_pPhotoListView( NULL )
  , m_pPrepareSelectView( NULL )
  , m_pPrepareCommonView( NULL )
  , m_pShootCommonView( NULL )
  , m_pMessageMenuView( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuf( NULL )
  , m_pCommonIcon( NULL )
  , m_pPokeIcon( NULL )
  , m_InitSeq( 0 )
  , m_EndSeq(0)
  , m_MainSeq(SEQ_FADE_IN)
  , m_SubSeq(0)
  , m_ViewerSeq(0)
  , m_bShowViewer( true )
{
#if PM_DEBUG
  HeapDefine::DebugPrint_AllHeapSize();
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   PROCの初期化を行う関数
 * @retval  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* manager )
{
  INIT_END_FUNC INIT_FUNC_TABLE[] = {
    &Proc::CreateHeap,
    &Proc::CreateViewList,
    &Proc::Create3DViewer,
    &Proc::CreateFrameManager,
    &Proc::CreateAppRenderingManager,
    &Proc::LoadLayoutData,
    &Proc::CreateMessageData,
    &Proc::CreateView,
    &Proc::CreateMenuCursor,
    &Proc::CreateCommonIcon,
    &Proc::CreatePokeIcon,
    &Proc::CreatePrepareView,
    &Proc::CreateShootMainView,
    &Proc::CreateMessageMenuView,
    &Proc::CreatePhotoTexture,
  };

  for( u8 i = m_InitSeq; i < GFL_NELEMS(INIT_FUNC_TABLE); ++i )
  {
    // 初期化関数
    if( ( this->*INIT_FUNC_TABLE[i] )( ) == false )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    m_SubSeq = 0;
    m_InitSeq++;

    if( m_InitSeq == GFL_NELEMS( INIT_FUNC_TABLE ) )
    {
      // フレームの開始
      m_pFrameManager->StartFrame( m_pWork->app.start_frame_id );
    }
  }

  // 初期化終了
  return gfl2::proc::RES_FINISH;
}

/**
 * @brief   PROCの終了を行う関数
 * @retval  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* manager )
{
  INIT_END_FUNC END_FUNC_TABLE[] = {
    &Proc::DeletePhotoTexture,
    &Proc::DeleteMessageMenuView,
    &Proc::DeleteShootMainView,
    &Proc::DeletePrepareView,
    &Proc::DeletePokeIcon,
    &Proc::DeleteCommonIcon,
    &Proc::DeleteMenuCursor,
    &Proc::DeleteView,
    &Proc::DeleteMessageData,
    &Proc::UnloadLayoutData,
    &Proc::DeleteAppRenderingManager,
    &Proc::DeleteFrameManager,
    &Proc::Delete3DViewer,
    &Proc::DeleteViewList,
    &Proc::DeleteHeap,
  };

  for( u8 i = m_EndSeq; i < GFL_NELEMS( END_FUNC_TABLE ); ++i )
  {
    // 初期化関数
    if( ( this->*END_FUNC_TABLE[i] )( ) == false )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    m_SubSeq = 0;
    m_EndSeq++;
  }

  // 初期化終了
  return gfl2::proc::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   PROCのメインを行う関数
 * @retval  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* manager )
{
  if( m_bShowViewer )
  {
    m_pViewerManager->Update();
  }
  m_pParentView->UpdateTree();
  Static::InformationWindow::Update();

  // ビューワーへの反映
  _UpdateViewer();

  if( m_MainSeq == SEQ_FADE_IN )
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );
    m_MainSeq++;
  }
  if( m_MainSeq == SEQ_FADE_IN_WAIT )
  {
    if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_UPPER ) &&
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_LOWER ) )
    {
      m_MainSeq++;
    }
  }
  if( m_MainSeq == SEQ_MAIN )
  {
    applib::frame::MainStatus frameStatus = m_pFrameManager->Main();

    if( frameStatus == applib::frame::MAINSTATUS_NULL )
    {
      if( !m_pFrameManager->NextFrame( frameStatus ) )
      {
        m_MainSeq++;
      }
    }
  }
  if( m_MainSeq == SEQ_FADE_OUT )
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

    m_MainSeq++;
  }
  else if( m_MainSeq == SEQ_FADE_OUT_WAIT )
  {
    m_MainSeq++;
  }
  else if( m_MainSeq == SEQ_END )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief   ビューワーの更新
 */
void Proc::_UpdateViewer( void )
{
  enum {
    SEQ_UPDATE_IDLE,
    SEQ_UPDATE_CURTAIN_CLOSE,
    SEQ_UPDATE_CURTAIN_CLOSE_WAIT,
    SEQ_UPDATE_VIEWER_UPDATE_START,
    SEQ_UPDATE_VIEWER_UPDATE_WAIT,
    SEQ_UPDATE_CURTAIN_OPEN,
    SEQ_UPDATE_CURTAIN_OPEN_WAIT,
    SEQ_UPDATE_NUM,
  };

  /// 待機
  if( m_ViewerSeq == SEQ_UPDATE_IDLE )
  {
    /// Viewer更新リクエストがあるかどうか
    if( m_pViewerManager->IsRequestUpdateViewer() )
    {
      FUKUSHIMA_PRINT( "StartCurtainOpen\n" );
      m_ViewerSeq = SEQ_UPDATE_CURTAIN_CLOSE;
    }
  }
  /// カーテンを閉じる：開始
  if( m_ViewerSeq == SEQ_UPDATE_CURTAIN_CLOSE )
  {
    m_pUpperView->CurtainClose();
    m_ViewerSeq = SEQ_UPDATE_CURTAIN_CLOSE_WAIT;
  }
  /// カーテンを閉じる：完了待ち
  else if( m_ViewerSeq == SEQ_UPDATE_CURTAIN_CLOSE_WAIT )
  {
    if( m_pUpperView->IsCurtainClose() )
    {
      FUKUSHIMA_PRINT( "FinishCurtainOpen\n" );
      m_ViewerSeq = SEQ_UPDATE_VIEWER_UPDATE_START;
    }
  }
  /// Viewer更新の開始
  else if( m_ViewerSeq == SEQ_UPDATE_VIEWER_UPDATE_START )
  {
    FUKUSHIMA_PRINT( "StartUpdateViewer\n" );
    m_pViewerManager->StartUpdateViewer();
    m_ViewerSeq = SEQ_UPDATE_VIEWER_UPDATE_WAIT;
  }
  else if( m_ViewerSeq == SEQ_UPDATE_VIEWER_UPDATE_WAIT )
  {
    /// Viewer更新の終了待ち
    if( m_pViewerManager->IsFinishUpdadteViewer() )
    {
      FUKUSHIMA_PRINT( "FinishUpdadteViewer\n" );

      /// 最新の構図に更新
      _UpdateCompositionID();

      m_ViewerSeq = SEQ_UPDATE_CURTAIN_OPEN;
    }
  }
  /// カーテンを開く：開始
  else if( m_ViewerSeq == SEQ_UPDATE_CURTAIN_OPEN )
  {
    FUKUSHIMA_PRINT( "StartCurtainClose\n" );
    m_pUpperView->CurtainOpen();
    m_ViewerSeq = SEQ_UPDATE_CURTAIN_OPEN_WAIT;
  }
  /// カーテンを開く：完了待ち
  else if( m_ViewerSeq == SEQ_UPDATE_CURTAIN_OPEN_WAIT )
  {
    if( m_pUpperView->IsCurtainOpen() )
    {
      FUKUSHIMA_PRINT( "FinishCurtainClose\n" );
      m_ViewerSeq = SEQ_UPDATE_IDLE;
    }
  }
}

/**
 * @brief   構図の更新
 */
void Proc::_UpdateCompositionID( void )
{
  /// 最新の構図をViewer側から取得する
  m_pWork->prepare.composition_id = m_pViewerManager->GetComposition();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief	PROCの描画処理(下準備)
 * @param pManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
void Proc::PreDrawFunc( gfl2::proc::Manager* manager )
{
  m_pAppRenderingManager->PreDraw();
}

/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
void Proc::DrawFunc(gfl2::proc::Manager* manager,gfl2::gfx::CtrDisplayNo no)
{
  if( m_bShowViewer )
  {
    m_pViewerManager->Draw( no );
  }

  if( m_MainSeq < SEQ_END )
  {
    m_pParentView->DrawTree( no );
  }

  m_pAppRenderingManager->Update();
  Static::InformationWindow::Draw( no );

  m_pFrameManager->Draw( no );
  m_pAppRenderingManager->Draw( no );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    生成 破棄 関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ヒープ 生成・破棄
 */
bool Proc::CreateHeap( void )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  m_pRootHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap, Static::HeapDefine::ROOT_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSCaptureRootHeap" );
  m_pProcHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::PROC_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSCaptureCaptureProcHeap" );
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSCaptureManagerHeap" );
  m_pImgdbHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::IMAGEDB_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSCaptureImageDBHeap" );
  m_pViewerHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::VIEWER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSCaptureViewerHeap" );
  m_pPhotoTextureHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, HeapDefine::PHOTO_TEX_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSCapturePhotoTextureHeap" );

  m_pAppHeap = GFL_NEW( m_pProcHeap ) app::util::Heap();
  m_pAppHeap->LocalHeapCreate( m_pRootHeap, m_pRootHeap, HeapDefine::APP_SYS_HEAP_SIZE, HeapDefine::APP_DEV_HEAP_SIZE );

#if defined(GF_PLATFORM_CTR)
  Static::ImageDBUtil::SetImageDBHeap( m_pImgdbHeap );
#endif

#if PM_DEBUG
  m_Debug.Initialize( m_pRootHeap );
#endif
  return true;
}

bool Proc::DeleteHeap( void )
{
#if PM_DEBUG
  m_Debug.Terminate();
#endif

#if defined(GF_PLATFORM_CTR)
  Static::ImageDBUtil::SetImageDBHeap( NULL );
#endif

  GFL_SAFE_DELETE( m_pAppHeap );
  GFL_DELETE_HEAP( m_pPhotoTextureHeap );
  GFL_DELETE_HEAP( m_pViewerHeap );
  GFL_DELETE_HEAP( m_pImgdbHeap );
  GFL_DELETE_HEAP( m_pManagerHeap );
  GFL_DELETE_HEAP( m_pProcHeap );
  GFL_DELETE_HEAP( m_pRootHeap );

  return true;
}



/**
* @brief   Viewer 生成・破棄
*/
bool Proc::CreateViewList( void )
{
  m_pViewList = GFL_NEW( m_pProcHeap ) Static::ViewList( m_pProcHeap, VIEW_ID_NUM );
  return true;
}

bool Proc::DeleteViewList( void )
{
  GFL_SAFE_DELETE( m_pViewList );
  return true;
}



/**
* @brief   FrameManager 生成・破棄
*/
bool Proc::CreateFrameManager( void )
{
  m_pFrameManager = GFL_NEW( m_pProcHeap ) FrameManager( m_pManagerHeap, m_pAppHeap, m_pWork, m_pViewList, m_pViewerManager );
  // フレームリスナーの設定
  m_pFrameManager->SetFrameListener( this );
  return true;
}

bool Proc::DeleteFrameManager( void )
{
  GFL_SAFE_DELETE( m_pFrameManager );
  return true;
}



/**
 * @brief   Viewer 生成・破棄
 */
bool Proc::Create3DViewer( void )
{
  enum {
    SEQ_VIEWER_INIT,
    SEQ_VIEWER_CREATE_NORMAL,
    SEQ_VIEWER_CREATE_RESTORATION,
    SEQ_VIEWER_SET_COMPOSITION,
    SEQ_VIEWER_END,
  };

  // ViewerManagerを生成
  if( m_SubSeq == SEQ_VIEWER_INIT )
  {
    m_pViewerManager = GFL_NEW( m_pProcHeap ) ViewerManager( m_pViewerHeap );

    if( !m_pWork->app.call_kisekae )
    {
      m_SubSeq = SEQ_VIEWER_CREATE_NORMAL;   //!< 通常生成へ
    }
    else {
      m_SubSeq = SEQ_VIEWER_CREATE_RESTORATION;   //!< アプリ復帰専用生成へ
    }
  }
  /// 通常生成
  if( m_SubSeq == SEQ_VIEWER_CREATE_NORMAL )
  {
    if( m_pViewerManager->CreateViewer( m_pWork->viewer.simple_param, m_pWork->viewer.dressup_param ) )
    {
      m_SubSeq = SEQ_VIEWER_SET_COMPOSITION;
    }
  }
  /// アプリ復帰専用生成
  if( m_SubSeq == SEQ_VIEWER_CREATE_RESTORATION )
  {
    if( m_pViewerManager->CreateViewer( m_pWork->viewer.rest_viewer_param, Viewer::VIEWER_MODE_NORMAL ) )
    {
      m_pWork->app.call_kisekae = false;

      m_SubSeq = SEQ_VIEWER_SET_COMPOSITION;
    }
  }
  if( m_SubSeq == SEQ_VIEWER_SET_COMPOSITION )
  {
    /// 最新の構図に更新
    _UpdateCompositionID();
    m_SubSeq = SEQ_VIEWER_END;
  }
  if( m_SubSeq == SEQ_VIEWER_END )
  {
    return true;
  }

  return false;
}

bool Proc::Delete3DViewer( void )
{
  if( m_SubSeq == 0 )
  {
    /// 更新
    m_pViewerManager->Update();

    /// 読込み終了チェック
    if( m_pViewerManager->IsFinishUpdadteViewer() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 1 )
  {
    // 写真復帰パラメータを取得
    m_pViewerManager->GetPhotoParam( &m_pWork->viewer.rest_viewer_param );
    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
  {
    if( m_pViewerManager->DeleteViewer() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 3 )
  {
    GFL_SAFE_DELETE( m_pViewerManager );
    m_SubSeq++;
  }
  if( m_SubSeq == 4 )
  {
    return true;
  }

  return false;
}



/**
 * @brief   レンダリングマネージャ 生成・破棄
 */
bool Proc::CreateAppRenderingManager( void )
{
  if( m_SubSeq == 0 )
  {
    m_pAppRenderingManager = GFL_NEW( m_pProcHeap ) app::util::AppRenderingManager();

    gfl2::fs::AsyncFileManager* afm = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
    app::util::AppRenderingManager::Description desc;
    {
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    }
    m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, NULL, afm );

    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    m_pAppRenderingManager->Update();

    if( m_pAppRenderingManager->IsCreated() )
    {
      return true;
    }
  }

  return false;
}

bool Proc::DeleteAppRenderingManager( void )
{
  if( m_SubSeq == 0 )
  {
    if( !m_pAppRenderingManager->CanDestroy() ) { return false; }
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    GFL_DELETE( m_pAppRenderingManager );
    return true;
  }

  return false;
}



/**
 * @brief   レイアウトデータ 読込み・破棄
 */
bool Proc::LoadLayoutData( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetSystemHeap();
  u32 arcID = ARCID_FINDER_STUDIO_CAPTURE;

  if( m_SubSeq == 0 )
  {
    app::util::FileAccessor::FileOpen( arcID, heap->GetLowerHandle() );
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( !app::util::FileAccessor::IsFileOpen( arcID ) ) return false;

    app::util::FileAccessor::FileLoad(
      arcID,
      GARC_FinderStudioCapture_FinderStudioCapture_applyt_COMP,
      &m_pLytDatBuff,
      heap->GetLowerHandle(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
  {
    if( !app::util::FileAccessor::IsFileLoad( &m_pLytDatBuff ) ) return false;

    app::util::FileAccessor::FileClose( arcID, heap->GetLowerHandle() );
    m_SubSeq++;
  }
  if( m_SubSeq == 3 )
  {
    if( !app::util::FileAccessor::IsFileClose( arcID ) ) return false;

    m_SubSeq++;
  }
  if( m_SubSeq == 4 )
  {
    return true;
  }

  return false;
}

bool Proc::UnloadLayoutData( void )
{
  GflHeapSafeFreeMemory( m_pLytDatBuff );
  return true;
}



/**
 * @brief   メッセージデータ 生成・破棄
 */
bool Proc::CreateMessageData( void )
{
  m_pMsgData = GFL_NEW( m_pProcHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_fs_photo_DAT, m_pAppHeap->GetSystemHeap(), gfl2::str::MsgData::LOAD_FULL );
  m_pWordSet = GFL_NEW( m_pProcHeap ) print::WordSet( m_pAppHeap->GetSystemHeap() );

  return true;
}

bool Proc::DeleteMessageData( void )
{
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

  return true;
}



/**
 * @brief   View 生成・破棄
 */
bool Proc::CreateView( void )
{
  // 親ビュー
  m_pParentView = GFL_NEW( m_pProcHeap ) Static::ParentView( m_pAppHeap, m_pAppRenderingManager );

  // 上画面ビュー
  m_pUpperView = GFL_NEW( m_pProcHeap ) UpperView( m_pAppHeap, m_pAppRenderingManager );
  m_pUpperView->SetViewListener( this );
  m_pUpperView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pUpperView->CurtainCloseKeep( false );
  m_pViewList->AddView( VIEW_ID_UPPER, m_pUpperView );

  // 背景ビュー
  m_pBGView = GFL_NEW( m_pProcHeap ) BGView( m_pAppHeap, m_pAppRenderingManager );
  m_pBGView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_BG, m_pBGView );

  // サブビュー登録
  m_pParentView->AddSubView( m_pUpperView );
  m_pParentView->AddSubView( m_pBGView );

  return true;
}

bool Proc::DeleteView( void )
{
  if( !_DeleteView( m_pBGView ) ) { return false; }
  if( !_DeleteView( m_pUpperView ) ) { return false; }

  GFL_SAFE_DELETE( m_pParentView );

  return true;
}

/**
 * @brief   準備画面View 生成・破棄
 */
bool Proc::CreatePrepareView( void )
{
  // 準備選択ビュー
  m_pPrepareSelectView = GFL_NEW( m_pProcHeap ) PrepareSelectView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pPrepareSelectView->SetWork( m_pWork );
  m_pPrepareSelectView->SetViewListener( this );
  m_pPrepareSelectView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_PREPARE_SELECT, m_pPrepareSelectView );

  // ポケモンリストビュー
  m_pPokeListView = GFL_NEW( m_pProcHeap ) PokeListView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pPokeListView->SetWork( m_pWork );
  m_pPokeListView->SetViewListener( this );
  m_pPokeListView->SetCommonIcon( m_pCommonIcon );
  m_pPokeListView->SetPokeIcon( m_pPokeIcon );
  m_pPokeListView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_POKE_LIST, m_pPokeListView );

  // 背景リストビュー
  m_pBGListView = GFL_NEW( m_pProcHeap ) BGListView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pBGListView->SetWork( m_pWork );
  m_pBGListView->SetViewListener( this );
  m_pBGListView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_BG_LIST, m_pBGListView );

  // 衣装リストビュー
  m_pDressupListView = GFL_NEW( m_pProcHeap ) DressupListView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pDressupListView->SetWork( m_pWork );
  m_pDressupListView->SetViewListener( this );
  m_pDressupListView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_DRESSUP_LIST, m_pDressupListView );

  // 構図選択ビュー
  m_pCompositionView = GFL_NEW( m_pProcHeap ) CompositionView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pCompositionView->SetViewListener( this );
  m_pCompositionView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_COMPOSITION, m_pCompositionView );

  // 汎用選択ボタンビュー
  m_pSelectButtonView = GFL_NEW( m_pProcHeap ) SelectButtonView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pSelectButtonView->SetViewListener( this );
  m_pSelectButtonView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_SELECT_BUTTON, m_pSelectButtonView );

  // 準備画面共通ビュー
  m_pPrepareCommonView = GFL_NEW( m_pProcHeap ) PrepareCommonView( m_pAppHeap, m_pAppRenderingManager );
  m_pPrepareCommonView->SetWork( m_pWork );
  m_pPrepareCommonView->SetViewListener( this );
  m_pPrepareCommonView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_PREPARE_COMMON, m_pPrepareCommonView );

  // PrepareCommonViewListenerのセット
  m_pPrepareSelectView->SetPrepareCommonViewListener( m_pPrepareCommonView );
  m_pPokeListView->SetPrepareCommonViewListener( m_pPrepareCommonView );
  m_pBGListView->SetPrepareCommonViewListener( m_pPrepareCommonView );
  m_pDressupListView->SetPrepareCommonViewListener( m_pPrepareCommonView );
  m_pCompositionView->SetPrepareCommonViewListener( m_pPrepareCommonView );
  m_pSelectButtonView->SetPrepareCommonViewListener( m_pPrepareCommonView );

  // サブビュー登録
  m_pParentView->AddSubView( m_pPrepareSelectView );
  m_pParentView->AddSubView( m_pPokeListView );
  m_pParentView->AddSubView( m_pBGListView );
  m_pParentView->AddSubView( m_pDressupListView );
  m_pParentView->AddSubView( m_pCompositionView );
  m_pParentView->AddSubView( m_pSelectButtonView );
  m_pParentView->AddSubView( m_pPrepareCommonView );

  return true;
}

bool Proc::DeletePrepareView( void )
{
  if( !_DeleteView( m_pPrepareCommonView ) ) { return false; }
  if( !_DeleteView( m_pSelectButtonView ) ) { return false; }
  if( !_DeleteView( m_pCompositionView ) ) { return false; }
  if( !_DeleteView( m_pDressupListView ) ) { return false; }
  if( !_DeleteView( m_pBGListView ) ) { return false; }
  if( !_DeleteView( m_pPokeListView ) ) { return false; }
  if( !_DeleteView( m_pPrepareSelectView ) ) { return false; }

  return true;
}

/**
 * @brief   撮影画面View 生成・破棄
 */
bool Proc::CreateShootMainView( void )
{
  // 撮影メインビュー
  m_pShootView = GFL_NEW( m_pProcHeap ) ShootView( m_pAppHeap, m_pAppRenderingManager );
  m_pShootView->SetWork( m_pWork );
  m_pShootView->SetViewListener( this );
  m_pShootView->SetCommonIcon( m_pCommonIcon );
  m_pShootView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_SHOOT, m_pShootView );

  // カメラ操作ビュー
  m_pCameraView = GFL_NEW( m_pProcHeap ) CameraView( m_pAppHeap, m_pAppRenderingManager );
  m_pCameraView->SetWork( m_pWork );
  m_pCameraView->SetViewListener( this );
  m_pCameraView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_CAMERA, m_pCameraView );

  // 位置調整ビュー
  m_pLayoutView = GFL_NEW( m_pProcHeap ) LayoutView( m_pAppHeap, m_pAppRenderingManager );
  m_pLayoutView->SetWork( m_pWork );
  m_pLayoutView->SetViewListener( this );
  m_pLayoutView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_LAYOUT, m_pLayoutView );

  // 撮影共通ビュー
  m_pShootCommonView = GFL_NEW( m_pProcHeap ) ShootCommonView( m_pAppHeap, m_pAppRenderingManager );
  m_pShootCommonView->SetViewListener( this );
  m_pShootCommonView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_SHOOT_COMMON, m_pShootCommonView );

  // Zモーションビュー
  m_pZMotionView = GFL_NEW( m_pProcHeap ) ZMotionView( m_pAppHeap, m_pAppRenderingManager );
  m_pZMotionView->SetViewListener( this );
  m_pZMotionView->SetCommonIcon( m_pCommonIcon );
  m_pZMotionView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_ZMOTION, m_pZMotionView );

  // 写真リストビュー
  m_pPhotoListView = GFL_NEW( m_pProcHeap ) PhotoListView( m_pAppHeap, m_pAppRenderingManager, m_pMenuCursorBuf );
  m_pPhotoListView->SetWork( m_pWork );
  m_pPhotoListView->SetViewListener( this );
  m_pPhotoListView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_PHOTO_LIST, m_pPhotoListView );

  // 共通Viewのセット
  m_pShootView->SetShootCommonView( m_pShootCommonView );
  m_pLayoutView->SetShootCommonView( m_pShootCommonView );
  m_pCameraView->SetShootCommonView( m_pShootCommonView );

  // サブビュー登録
  m_pParentView->AddSubView( m_pShootView );
  m_pParentView->AddSubView( m_pCameraView );
  m_pParentView->AddSubView( m_pLayoutView );
  m_pParentView->AddSubView( m_pShootCommonView );
  m_pParentView->AddSubView( m_pZMotionView );
  m_pParentView->AddSubView( m_pPhotoListView );

  return true;
}

bool Proc::DeleteShootMainView( void )
{
  if( !_DeleteView( m_pPhotoListView ) ) { return false; }
  if( !_DeleteView( m_pZMotionView ) ) { return false; }
  if( !_DeleteView( m_pShootCommonView ) ) { return false; }
  if( !_DeleteView( m_pLayoutView ) ) { return false; }
  if( !_DeleteView( m_pCameraView ) ) { return false; }
  if( !_DeleteView( m_pShootView ) ) { return false; }

  return true;
}

bool Proc::_DeleteView( Static::ViewBase* view )
{
  if( view )
  {
    if( view->IsDrawing() ) { return false; }
    if( !view->Terminate() ) { return false; }
    view->RemoveFromSuperView();
    GFL_SAFE_DELETE( view );
  }
  return true;
}


/**
 * @brief   メニューカーソル 生成・破棄
 */
bool Proc::CreateMenuCursor( void )
{
  if( m_SubSeq == 0 )
  {
    m_pMenuCursor = GFL_NEW( m_pProcHeap ) app::tool::MenuCursor( m_pAppHeap, &m_pMenuCursorBuf );
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pMenuCursor->LoadResource() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 2 )
  {
    return true;
  }

  return false;
}

bool Proc::DeleteMenuCursor( void )
{
  if( m_SubSeq == 0 )
  {
    // 読込み中に破棄されることはないはずだが・・・一応
    if( m_pMenuCursor->LoadResource() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 1 )
  {
    m_pMenuCursor->DeleteResource();
    GFL_DELETE( m_pMenuCursor );
    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
  {
    return true;
  }

  return false;
}



/**
 * @brief   共通アイコン 生成・破棄
 */
bool Proc::CreateCommonIcon( void )
{
  if( m_SubSeq == 0 )
  {
    m_pCommonIcon = GFL_NEW( m_pProcHeap ) app::tool::AppCommonGrpIconData();
    m_pCommonIcon->Initialize( m_pAppHeap );
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pCommonIcon->IsReady() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 2 )
  {
    return true;
  }

  return false;
}

bool Proc::DeleteCommonIcon( void )
{
  m_pCommonIcon->Terminate();
  GFL_DELETE( m_pCommonIcon );

  return true;
}


/**
 * @brief   ポケモンアイコン 生成・破棄
 */
bool Proc::CreatePokeIcon( void )
{
  if( m_SubSeq == 0 )
  {
    m_pPokeIcon = GFL_NEW( m_pProcHeap ) app::tool::PokeIcon( m_pAppHeap->GetSystemHeap(), m_pAppHeap->GetDeviceHeap(), pml::PokeParty::MAX_MEMBERS );
    m_pPokeIcon->FileOpen( m_pProcHeap->GetLowerHandle() );
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pPokeIcon->IsFileOpen() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 2 )
  {
    return true;
  }

  return false;
}

bool Proc::DeletePokeIcon( void )
{
  if( m_SubSeq == 0 )
  {
    if( m_pPokeIcon->IsFileOpen() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 1 )
  {
    m_pPokeIcon->FileClose( m_pProcHeap->GetLowerHandle() );
    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
  {
    if( m_pPokeIcon->IsFileClose() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 3 )
  {
    if( m_pPokeIcon->IsModuleFree() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 4 )
  {
    GFL_SAFE_DELETE( m_pPokeIcon );
    m_SubSeq++;
  }
  if( m_SubSeq == 5 )
  {
    return true;
  }

  return false;
}


/**
 * @brief   メッセージウィンドウ 生成・破棄
 */
bool Proc::CreateMessageMenuView( void )
{
  if( m_SubSeq == 0 )
  {
    m_pMessageMenuView = GFL_NEW( m_pProcHeap ) Static::MessageMenuView( m_pAppHeap, m_pAppRenderingManager, m_pMsgData, m_pMenuCursorBuf );
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pMessageMenuView->InitFunc() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 2 )
  {
    // メッセージデータとワードセットを設定
    m_pMessageMenuView->SetMessageManagerMsgDataAndWordSet( m_pMsgData, m_pWordSet );

    m_pParentView->AddSubView( m_pMessageMenuView->GetView() );
    m_pViewList->SetMessageMenuView( m_pMessageMenuView );
    m_SubSeq++;
  }
  if( m_SubSeq == 3 )
  {
    return true;
  }

  return false;
}

bool Proc::DeleteMessageMenuView( void )
{
  if( m_SubSeq == 0 )
  {
    // メッセージデータとワードセットを解除
    m_pMessageMenuView->RemoveMessageManagerMsgDataAndWordSet();

    m_pMessageMenuView->GetView()->RemoveFromSuperView();
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pMessageMenuView->EndFunc() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 2 )
  {
    GFL_DELETE( m_pMessageMenuView );
    m_SubSeq++;
  }
  if( m_SubSeq == 3 )
  {
    return true;
  }

  return false;
}


/**
 * @brief   写真テクスチャ 生成・破棄
 */
bool Proc::CreatePhotoTexture( void )
{
  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    m_pWork->viewer.photo_tex[i] = GflHeapAllocMemoryAlign(
      m_pPhotoTextureHeap, Static::PHOTO_BUFFER_SIZE, Static::DISPLAY_BUFFER_ALIGN );
  }

  return true;
}

bool Proc::DeletePhotoTexture( void )
{
  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    GflHeapSafeFreeMemory( m_pWork->viewer.photo_tex[i] );
  }

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 ViewListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ビューワーの更新が可能かどうか
 */
bool Proc::IsUpdateViewer( void )
{
  return m_pUpperView->IsUpdateViewer() && m_ViewerSeq == 0;
}

/**
 * @brief   Viewer更新が終了したか？
 */
bool Proc::IsFinishUpdateViewer( void )
{
  return (m_ViewerSeq == 0);
}

/**
 * @brief   Viewer更新をリクエストしたか？
 */
bool Proc::IsRequestUpdateViewer( void )
{
  return m_pViewerManager->IsRequestUpdateViewer();
}

/**
 * @brief   画面遷移の開始
 */
void Proc::StartScreenTransition( void )
{
  FUKUSHIMA_PRINT( "StartScreenTransition\n" );

  const u32 c_DisableInputViewList[] = {
    VIEW_ID_PREPARE_SELECT,
    VIEW_ID_SELECT_BUTTON,
    VIEW_ID_POKE_LIST,
    VIEW_ID_BG_LIST,
    VIEW_ID_DRESSUP_LIST,
    VIEW_ID_COMPOSITION,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_DisableInputViewList ); ++i )
  {
    Static::ViewBase* view = m_pViewList->GetView( c_DisableInputViewList[i] );
    view->SetInputEnabled( false );
  }
}

/**
 * @brief   ビューワーリセット：カーテン演出あり
 */
void Proc::ViewerReset( void )
{
  m_pViewerManager->RequestViewerUpdate_AllReset();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 PrepareListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影モードの変更
 *
 * @param[in]   mode    撮影モード
 */
void Proc::SetCaptureMode( Viewer::CaptureMode mode )
{
  GFL_ASSERT( m_pWork->viewer.capture_mode == m_pWork->prepare.capture_mode );

  if( m_pWork->prepare.capture_mode != mode )
  {
    m_pWork->prepare.capture_mode = mode;
    m_pWork->viewer.capture_mode = mode;

    /// リクエスト：撮影モード
    m_pViewerManager->RequestViewerUpdate_CaptureMode( mode );
  }
}

/**
 * @brief   撮影モードの取得
 *
 * @retval  撮影モード
 */
Viewer::CaptureMode Proc::GetCaptureMode( void )
{
  GFL_ASSERT( m_pWork->viewer.capture_mode == m_pWork->prepare.capture_mode );

  return m_pWork->viewer.capture_mode;
}

/**
 * @brief   撮影するポケモンの設定
 *
 * @param[in]   pp      ポケモンパラメータ
 * @param[in]   index   メンバーインデックス(手持ち内のインデックス)
 */
void Proc::SetPokemon( pml::pokepara::PokemonParam* pp, u32 index )
{
  // メンバーインデックスの比較
  if( m_pWork->prepare.member_id != index )
  {
    PokeTool::SimpleParam simpleParam;
    PokeTool::GetSimpleParam( &simpleParam, pp );

    m_pWork->prepare.pokemon_param = pp;
    m_pWork->prepare.member_id = index;
    m_pWork->viewer.simple_param = simpleParam;

    /// リクエスト：ポケモン変更
    m_pViewerManager->RequestViewerUpdate_PokeModel( simpleParam );

    FUKUSHIMA_PRINT( "SetPokemon : index=[%d] monsNo=[%d]\n", index, simpleParam.monsNo );
  }
}

/**
 * @brief   撮影する背景の設定
 *
 * @param[in]   index       背景ID
 * @param[in]   timeZone    時間帯
 */
void Proc::SetBackGround( u32 index, Viewer::TimeZone timeZone )
{
  if( m_pWork->prepare.background_id != index )
  {
    m_pWork->prepare.background_id = index;
    m_pWork->viewer.background_id = index;
    m_pWork->viewer.time_zone = timeZone;

    /// リクエスト：背景変更
    m_pViewerManager->RequestViewerUpdate_BackGround( index, timeZone );
  }
}

/**
 * @brief   撮影する背景の設定
 */
void Proc::SetTimeZone( Viewer::TimeZone timeZone )
{
  if( m_pWork->viewer.time_zone != timeZone )
  {
    m_pWork->viewer.time_zone = timeZone;

    /// リクエスト：時間帯変更
    m_pViewerManager->RequestViewerUpdate_TimeZone( timeZone );
  }
}

/**
 * @brief   撮影する背景の設定
 */
Viewer::TimeZone Proc::GetTimeZone( void )
{
  return m_pWork->viewer.time_zone;
}

/**
 * @brief   着せ替えデータを変更するセーブデータ領域のインデックス：０～２
 */
void Proc::SetDressUpSaveIndex( poke_3d::model::DressUpParam dressUpParam, u8 index )
{
  GFL_ASSERT( index < Savedata::FinderStudioSave::DRESSUP_SAVE_MAX );
  m_pWork->app.dressup_save_param = dressUpParam;
  m_pWork->app.dressup_save_index = index;
}

/**
 * @brief   ビューワー側の衣装更新
 */
void Proc::SetDressUp( poke_3d::model::DressUpParam dressUpParam, u8 dressupIndex )
{
  if( m_pWork->prepare.dressup_id != dressupIndex )
  {
    m_pWork->prepare.dressup_id = dressupIndex;
    m_pWork->viewer.dressup_param = dressUpParam;

    /// リクエスト：衣装変更
    m_pViewerManager->RequestViewerUpdate_Dressup( dressUpParam );
  }
}

/**
 * @brief   構図設定
 */
void Proc::SetComposition( Viewer::Composition composition )
{
  if( m_pWork->prepare.composition_id != composition )
  {
    m_pWork->prepare.composition_id = composition;

    /// リクエスト：構図変更
    m_pViewerManager->RequestViewerUpdate_Composition( composition );
  }
}

/**
 * @brief   構図の取得
 */
Viewer::Composition Proc::GetComposition( void )
{
  return m_pWork->prepare.composition_id;
}

/**
 * @brief   使用可能構図
 */
bool Proc::IsCanUseComposition( Viewer::Composition composition )
{
  return m_pViewerManager->IsAvailableComposition( composition );
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 CameraConrtolListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カメラ X軸回転
 */
void Proc::CameraRotateXUp( void )
{
  m_pViewerManager->RequestCameraXAxisRotationUp();
}

/**
 * @brief   カメラ X軸回転
 */
void Proc::CameraRotateXDown( void )
{
  m_pViewerManager->RequestCameraXAxisRotationDown();
}

/**
 * @brief   カメラ Y軸回転
 */
void Proc::CameraRotateYLeft( void )
{
  m_pViewerManager->RequestCameraYAxisRotationLeft();
}

/**
 * @brief   カメラ Y軸回転
 */
void Proc::CameraRotateYRight( void )
{
  m_pViewerManager->RequestCameraYAxisRotationRight();
}

/**
 * @brief   カメラ Y軸回転 取得
 *
 * @retval  角度
 */
f32 Proc::GetCameraRotateY( void )
{
  return m_pViewerManager->GetCameraYAxisRotationDegree();
}

/**
 * @brief   カメラ 高さ
 */
void Proc::CameraMoveYUp( void )
{
  m_pViewerManager->RequestCameraYAxisTranslationUp();
}

/**
 * @brief   カメラ 高さ
 */
void Proc::CameraMoveYDown( void )
{
  m_pViewerManager->RequestCameraYAxisTranslationDown();
}

/**
 * @brief   カメラ 高さ 設定
 *
 * @param[in]   rate    割合：0～1
 */
void Proc::SetCameraHeightRate( f32 rate )
{
  m_pViewerManager->SetCameraYAxisTranslationRate( rate );
}

/**
 * @brief   カメラ 高さ取得
 *
 * @retval  割合：0～1
 */
f32 Proc::GetCameraHeightRate( void )
{
  return m_pViewerManager->GetCameraYAxisTranslationRate();
}

/**
 * @brief   カメラ 注視点の移動
 */
void Proc::CameraLookAtPlayer( void )
{
  m_pViewerManager->RequestCameraFocusTrainer();
}

/**
 * @brief   カメラ 注視点の移動
 */
void Proc::CameraLookAtPokemon( void )
{
  m_pViewerManager->RequestCameraFocusPokemon();
}

/**
 * @brief   カメラ 注視点の度合い 設定
 *
 * @param[in]   rate    割合：0～1
 */
void Proc::SetCameraLookAtRate( f32 rate )
{
  m_pViewerManager->SetCameraFocusRate( rate );
}

/**
 * @brief   カメラ 注視点の度合い取得
 *
 * @retval  割合：0～1
 */
f32 Proc::GetCameraLookAtRate( void )
{
  return m_pViewerManager->GetCameraFocusRate();
}

/**
 * @brief   カメラ ズーム
 */
void Proc::CameraZoomIn( void )
{
  m_pViewerManager->RequestCameraZoomIn();
}

/**
 * @brief   カメラ ズーム
 */
void Proc::CameraZoomOut( void )
{
  m_pViewerManager->RequestCameraZoomOut();
}

/**
 * @brief   カメラ ズーム 設定
 *
 * @param[in]   rate    割合：0～1
 */
void Proc::SetCameraZoomRate( f32 rate )
{
  m_pViewerManager->SetCameraZoomRate( rate );
}

/**
 * @brief   カメラ ズーム取得
 *
 * @retval  割合：0～1
 */
f32 Proc::GetCameraZoomRate( void )
{
  return m_pViewerManager->GetCameraZoomRate();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 TargetControlListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   トレーナー回転：左周り
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::PlayerRotateLeft( void )
{
  return m_pViewerManager->RequestTrainerYAxisRotationLeft();
}

/**
 * @brief   トレーナー回転：右周り
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::PlayerRotateRight( void )
{
  return m_pViewerManager->RequestTrainerYAxisRotationRight();
}

/**
 * @brief   トレーナー回転角の設定
 *
 * @param[in]   degree    角度
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::SetPlayerRotate( f32 degree )
{
  return m_pViewerManager->SetTrainerYAxisRotationDegree( degree );
}

/**
 * @brief   トレーナー回転角の取得：UI上
 *
 * @retval  角度
 */
f32 Proc::GetPlayerRotate( void )
{
  return m_pViewerManager->GetTrainerYAxisRotationDegree();
}

/**
 * @brief   トレーナー回転角の取得：Viewer上
 *
 * @retval  角度
 */
f32 Proc::GetViewerPlayerRotate( void )
{
  return m_pViewerManager->GetTrainerYAxisRotationOriginalDegree();
}


/**
 * @brief   ポケモン回転 左回り
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::PokemonRotateLeft( void )
{
  return m_pViewerManager->RequestPokemonYAxisRotationLeft();
}

/**
 * @brief   ポケモン回転 右回り
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::PokemonRotateRight( void )
{
  return m_pViewerManager->RequestPokemonYAxisRotationRight();
}

/**
 * @brief   ポケモン回転角の設定
 *
 * @param[in]   degree    角度
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::SetPokemonRotate( f32 degree )
{
  return m_pViewerManager->SetPokemonYAxisRotationDegree( degree );
}

/**
 * @brief   ポケモン回転角の取得：UI上
 *
 * @retval  角度
 */
f32 Proc::GetPokemonRotate( void )
{
  return m_pViewerManager->GetPokemonYAxisRotationDegree();
}

/**
 * @brief   ポケモン回転角の取得：Viewer上
 *
 * @retval  角度
 */
f32 Proc::GetViewerPokemonRotate( void )
{
  return m_pViewerManager->GetPokemonYAxisRotationOriginalDegree();
}

/**
 * @brief   ポケモン位置の設定
 *
 * @param[in]   pos       座標値
 * @param[in]   isLift    持ち上げフラグ
 *
 * @retval  "true  == 衝突"
 * @retval  "false == それ以外"
 */
bool Proc::SetPokemonPosition( gfl2::math::Vector3 pos, bool isLift )
{
  return m_pViewerManager->SetPokemonPosition( pos );
}

/**
 * @brief   ポケモン位置の取得：スクリーン座標
 *
 * @retval  座標値
 */
gfl2::math::Vector3 Proc::GetPokemonPosition( void )
{
  return m_pViewerManager->GetPokemonPosition();
}

/**
 * @brief   ポケモン位置の取得：Viewer上
 *
 * @retval  座標値
 */
gfl2::math::Vector3 Proc::GetPokemonViewerPosition( void )
{
  return m_pViewerManager->GetPokemonOriginalPosition();
}

/**
 * @brief   トレーナーAABBの取得
 *
 * @retval  AABB
 */
gfl2::math::AABB Proc::GetPlayerAABB( void )
{
  return m_pViewerManager->GetTrainerAABB();
}

/**
 * @brief   ポケモンAABBの取得
 *
 * @retval  AABB
 */
gfl2::math::AABB Proc::GetPokemonAABB( void )
{
  return m_pViewerManager->GetPokemonAABB();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 MotionControlListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Zモーションタイプの設定
 *
 * @param[in]   type    Zモーションタイプ
 */
void Proc::SetTrainerMotionType( Viewer::ZMotionType type )
{
  if( m_pWork->viewer.z_motion_type != type )
  {
    m_pWork->viewer.z_motion_type = type;
  }
}

/**
 * @brief   Zモーションタイプの取得
 *
 * @retval  Zモーションタイプ
 */
Viewer::ZMotionType Proc::GetTrainerMotionType( void )
{
  return m_pWork->viewer.z_motion_type;
}

/**
 * @brief   モーション再生：プレイヤー
 *
 * @param[in]   type    モーションタイプ
 */
void Proc::TrainerMotionStart( Viewer::ZMotionType type )
{
  Viewer::ZMotionType motionType = ( type != Viewer::Z_MOTION_TYPE_MAX ) ? type : m_pWork->viewer.z_motion_type;

  m_pViewerManager->RequestTrainerMotionPlay( motionType );
}

/**
 * @brief   モーション停止
 */
void Proc::TrainerMotionStop( void )
{
  m_pViewerManager->RequestTrainerMotionStop();
}

/**
 * @brief   モーションリセット
 */
void Proc::TrainerMotionReset( void )
{
  m_pViewerManager->RequestViewerUpdate_TrainerMotionReset();
}

/**
 * @brief   モーション再生中か？
 *
 * @retval  "true"なら再生中
 * @retval  "false"ならそれ以外
 */
bool Proc::IsPlayingTrainerMotion( void )
{
  return m_pViewerManager->IsPlayingTrainerMotion();
}


/**
 * @brief   モーション再生：ポケモン
 */
void Proc::PokemonMotionStart( void )
{
  m_pViewerManager->RequestPokemonMotionPlay();
}

/**
 * @brief   ポケモンモーションタイプの設定
 *
 * @param[in]   bReset    リセットフラグ
 *
 * @retval  設定したモーションタイプ
 */
Viewer::PokeMotionType Proc::ChangePokemonMotionType( bool bReset )
{
  Viewer::PokeMotionType nextMotionType = Viewer::POKE_MOTION_TYPE_BATTLE;

  /// 今のモーションタイプが"バトル"なら"リフレ"に変更
  if( m_pWork->viewer.poke_motion_type == Viewer::POKE_MOTION_TYPE_BATTLE )
  {
    nextMotionType = Viewer::POKE_MOTION_TYPE_KAWAIGARI;
  }

  m_pWork->viewer.poke_motion_type = nextMotionType;

  /// リクエスト：ポケモンモーションタイプ
  m_pViewerManager->RequestViewerUpdate_PokeMotionType( nextMotionType, bReset );

  return nextMotionType;
}

/**
 * @brief   ポケモンモーションタイプの取得
 *
 * @retval  モーションタイプ
 */
Viewer::PokeMotionType Proc::GetPokemonMotionType( void )
{
  return m_pWork->viewer.poke_motion_type;
}

/**
 * @brief   モーションタイプ変更時にリセットが必要か？
 *
 * @retval  "true"なら必要
 * @retval  "false"なら不必要
 */
bool Proc::IsNeedResetPokemonMotionType( void )
{
  Viewer::PokeMotionType nowMotionType = m_pWork->viewer.poke_motion_type;

  return m_pViewerManager->IsNeedResetPokemonMotionType( nowMotionType );
}

/**
 * @brief   ポケモンの再生可能なモーションの割合
 *
 * @retval  再生可能なモーションの割合
 */
f32 Proc::GetPokemonMotionPlayRate( void )
{
  return m_pViewerManager->GetPokemonMotionPlayableRate();
}

/**
 * @brief   ポケモンモーション：自動モーションの切り替え
 *
 * @param[in]   enable    有効フラグ
 */
void Proc::SetPokemonMotionAuto( bool enable )
{
  m_pViewerManager->RequestSetPokemonMotionAutoPlayEnable( enable );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 CaptureListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影開始
 */
void Proc::StartCapture( void )
{
  m_pViewerManager->RequestCaptureStart( m_pWork->viewer.photo_tex, m_pWork->viewer.photo_param );
}

/**
 * @brief   撮影終了
 *
 * @retval  "true"なら終了
 * @retval  "false"ならそれ以外
 */
bool Proc::IsFinishCapture( void )
{
  return m_pViewerManager->IsFinishCapture();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 PhotoListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   上画面に表示する写真の設定
 *
 * @param   index   写真のインデックス
 */
void Proc::SetUpperPhotoImage( u32 index )
{
  if( index >= PHOTO_MAX )
  {
    GFL_ASSERT(0);
    return;
  }

  FUKUSHIMA_PRINT( "UpperPhotoImage=[%d]\n", index );

  // 上画面に写真を表示
  m_pUpperView->RequestPhotoImage( m_pWork->viewer.photo_tex[index] );
}

/**
 * @brief   写真の取得
 *
 * @param   index   写真のインデックス
 *
 * @retval  写真データ
 */
void* Proc::GetPhotoTexture( u32 index )
{
  if( index >= PHOTO_MAX )
  {
    GFL_ASSERT( 0 );
    return NULL;
  }

  return m_pWork->viewer.photo_tex[index];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数群 UpperViewListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   照準を表示
 */
void Proc::VisibleSight( void )
{
  m_pUpperView->VisibleSight();
}

/**
 * @brief   照準を非表示
 */
void Proc::InvisibleSight( void )
{
  m_pUpperView->InvisibleSight();
}

/**
 * @brief   シャッターを開く
 */
void Proc::ShutterOpen( void )
{
  m_pUpperView->ShutterOpen();
}

/**
 * @brief   シャッター開くアニメの終了
 */
bool Proc::IsFinishShutterOpen( void )
{
  return m_pUpperView->IsShutterOpen();
}

/**
* @brief   シャッターを閉じる
*/
void Proc::ShutterClose( void )
{
  m_pUpperView->ShutterClose();
}

/**
* @brief   シャッター閉じるアニメの終了
*/
bool Proc::IsFinishShutterClose( void )
{
  return m_pUpperView->IsShutterClose();
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
