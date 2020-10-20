//======================================================================
/**
 * @file    FinderStudioDecorationProc.cpp
 * @date    2016/11/17 18:17:05
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：プロック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioDecoration/include/FinderStudioDecorationProc.h>

#include "FinderStudioDecorationHeapDefine.h"
#include "FinderStudioDecorationDecoItemManager.h"
#include "Frame/FinderStudioDecorationFrameManager.h"
#include "View/FinderStudioDecorationViewDefine.h"
#include "View/FinderStudioDecorationUpperView.h"
#include "View/FinderStudioDecorationLowerBGView.h"
#include "View/FinderStudioDecorationLowerMainView.h"
#include "View/FinderStudioDecorationLowerFrameListView.h"
#include "View/FinderStudioDecorationLowerStampListView.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioAlbumWork.h>
#include <App/FinderStudioStatic/include/FinderStudioParentView.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioViewer/include/FinderStudioViewer.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/SystemFont.h>
#include <print/include/WordSet.h>
#include <print/include/WordSetLoader.h>
#include <System/include/HeapDefine.h>

#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/FinderStudioDeco.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

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
  , m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pViewer( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pFrameManager( NULL )
  , m_pAlbumWork( NULL )
  , m_pViewList( NULL )
  , m_pLytDatBuff( NULL )
  , m_pParentView( NULL )
  , m_pMessageMenuView( NULL )
  , m_pUpperView( NULL )
  , m_pBGView( NULL )
  , m_pLowerMainView( NULL )
  , m_pFrameListView( NULL )
  , m_pStampListView( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuf( NULL )
  , m_pDecoItemManager( NULL )
  , m_InitSeq( 0 )
  , m_EndSeq( 0 )
  , m_MainSeq( 0 )
  , m_SubSeq( 0 )
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
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* manager )
{
  INIT_END_FUNC INIT_FUNC_TABLE[] = {
    &Proc::CreateHeap,
    &Proc::CreateViewList,
    &Proc::CreateFrameManager,
    &Proc::CreateAppRenderingManager,
    &Proc::CreateFont,
    &Proc::LoadLayoutData,
    &Proc::CreateMessageData,
    &Proc::CreateMenuCursor,
    &Proc::CreateView,
    &Proc::CreateMessageMenuView,
    &Proc::CreateDecoItemManager,
    &Proc::Create3DViewer,
    &Proc::CreatePhotoData,
  };

  for( u8 i = m_InitSeq; i < GFL_NELEMS( INIT_FUNC_TABLE ); ++i )
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
      m_pFrameManager->StartFrame( Frame::FRAME_ID_DECORATION );
    }
  }

  // 初期化終了
  return gfl2::proc::RES_FINISH;
}

/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* manager )
{
  INIT_END_FUNC END_FUNC_TABLE[] = {
    &Proc::DeletePhotoData,
    &Proc::Delete3DViewer,
    &Proc::DeleteDecoItemManager,
    &Proc::DeleteMessageMenuView,
    &Proc::DeleteMenuCursor,
    &Proc::DeleteView,
    &Proc::DeleteMessageData,
    &Proc::UnloadLayoutData,
    &Proc::DeleteFont,
    &Proc::DeleteAppRenderingManager,
    &Proc::DeleteFrameManager,
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
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* manager )
{
  m_pParentView->UpdateTree();
  m_pViewer->Update();
  // インフォウィンドウの更新
  Static::InformationWindow::Update();
  // 背景アニメの更新   @fix MMCat[255]
  _UpdateBackGroundAnim();

  // シーケンス：フェードイン
  if( m_MainSeq == SEQ_FADE_IN )
  {
    if( _UpdateFadeIn() == RES_NEXT_SEQ )
    {
      m_MainSeq = SEQ_MAIN;
    }
  }
  // シーケンス：メイン
  if( m_MainSeq == SEQ_MAIN )
  {
    if( _UpdateMain() == RES_NEXT_SEQ )
    {
      m_MainSeq = SEQ_FADE_OUT;
    }
  }
  // シーケンス：フェードアウト
  if( m_MainSeq == SEQ_FADE_OUT )
  {
    if( _UpdateFadeOut() == RES_NEXT_SEQ )
    {
      m_MainSeq = SEQ_END;
    }
  }
  // シーケンス：終了
  if( m_MainSeq == SEQ_END )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief   更新：フェードイン
 */
Proc::Result Proc::_UpdateFadeIn( void )
{
  enum {
    SEQ_FADE_IN_BEGIN,
    SEQ_FADE_IN_END,
  };

  // フェードイン
  if( m_SubSeq == SEQ_FADE_IN_BEGIN )
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );
    m_SubSeq = SEQ_FADE_IN_END;
  }
  // フェードイン完了待ち
  if( m_SubSeq == SEQ_FADE_IN_END )
  {
    if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_UPPER ) &&
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_LOWER ) )
    {
      m_SubSeq = 0;
      return RES_NEXT_SEQ;
    }
  }

  return RES_CONTINUE;
}

/**
 * @brief   更新：メイン
 */
Proc::Result Proc::_UpdateMain( void )
{
  applib::frame::MainStatus frameStatus = m_pFrameManager->Main();

  if( frameStatus == applib::frame::MAINSTATUS_NULL )
  {
    if( !m_pFrameManager->NextFrame( frameStatus ) )
    {
      return RES_NEXT_SEQ;
    }
  }

  return RES_CONTINUE;
}

/**
 * @brief   更新：フェードアウト
 */
Proc::Result Proc::_UpdateFadeOut( void )
{
  enum {
    SEQ_FADE_OUT_BEGIN,
    SEQ_FADE_OUT_END,
  };

  // フェードアウト
  if( m_SubSeq == SEQ_FADE_OUT_BEGIN )
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
    m_SubSeq = SEQ_FADE_OUT_END;
  }
  // フェードアウト完了待ち
  else if( m_SubSeq == SEQ_FADE_OUT_END )
  {
    m_SubSeq = 0;
    return RES_NEXT_SEQ;
  }

  return RES_CONTINUE;
}


/**
 * @brief   背景アニメの更新
 */
void Proc::_UpdateBackGroundAnim( void )
{
  /// 現在のフレームの取得
  f32 nowFrame = m_pBGView->GetBackGroundAnimFrame();

  /// フレームの設定
  m_pLowerMainView->SetBackGroundAnimFrame( nowFrame );
  m_pStampListView->SetBackGroundAnimFrame( nowFrame );
  m_pFrameListView->SetBackGroundAnimFrame( nowFrame );
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
void Proc::DrawFunc( gfl2::proc::Manager* manager, gfl2::gfx::CtrDisplayNo no )
{
  m_pViewer->Draw( no );

  if( m_MainSeq < SEQ_END )
  {
    m_pParentView->DrawTree( no );
  }

  m_pAppRenderingManager->Update();

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

  m_pRootHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap, Static::HeapDefine::ROOT_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSDecorationRootHeap" );
  m_pProcHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::PROC_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSDecorationDecorationProcHeap" );
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSDecorationManagerHeap" );
  m_pImgdbHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::IMAGEDB_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSDecorationImageDBHeap" );
  m_pViewerHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pRootHeap, Static::HeapDefine::VIEWER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FSDecorationViewerHeap" );

  m_pAppHeap = GFL_NEW( m_pProcHeap ) app::util::Heap();
  m_pAppHeap->LocalHeapCreate( m_pRootHeap, m_pRootHeap, HeapDefine::APP_SYS_HEAP_SIZE, HeapDefine::APP_DEV_HEAP_SIZE );
  m_DecoWork.app_heap = m_pAppHeap;

#if defined(GF_PLATFORM_CTR)
  Static::ImageDBUtil::SetImageDBHeap( m_pImgdbHeap );
#endif

  return true;
}

bool Proc::DeleteHeap( void )
{
#if defined(GF_PLATFORM_CTR)
  Static::ImageDBUtil::SetImageDBHeap( NULL );
#endif

  GFL_SAFE_DELETE( m_pAppHeap );
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
  m_pFrameManager = GFL_NEW( m_pProcHeap ) FrameManager( m_pManagerHeap, m_pViewList, m_pAlbumWork, &m_DecoWork );
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
  if( m_SubSeq == 0 )
  {
    m_pViewer = GFL_NEW( m_pProcHeap ) Viewer::FinderStudioViewer( m_pViewerHeap, m_pAlbumWork->photo_data.photo_data.photo_param, Viewer::VIEWER_MODE_PHOTO );
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pViewer->InitFunc() )
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

bool Proc::Delete3DViewer( void )
{
  if( m_SubSeq == 0 )
  {
    if( !m_pViewer->EndFunc() ) { return false; }
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    GFL_SAFE_DELETE( m_pViewer );
    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
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

    /// 共通データにセット
    m_DecoWork.render_man = m_pAppRenderingManager;

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
 * @brief   フォント 生成
 */
bool Proc::CreateFont( void )
{
  print::SystemFont_CreateNumFont( m_pAppHeap->GetSystemHeap(), m_pAppHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();

  return true;
}

bool Proc::DeleteFont( void )
{
  print::SystemFont_ResetNumFontInfo();
  print::SystemFont_DeleteNumFont();

  return true;
}



/**
* @brief   レイアウトデータ 読込み
*/
bool Proc::LoadLayoutData( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetSystemHeap();
  u32 arcID = ARCID_FINDER_STUDIO_DECORATION;

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
      GARC_FinderStudioDeco_FinderStudioDeco_applyt_COMP,
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
  m_pMsgData = GFL_NEW( m_pProcHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_fs_deco_DAT, m_pAppHeap->GetSystemHeap(), gfl2::str::MsgData::LOAD_FULL );
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
  // 親View
  m_pParentView = GFL_NEW( m_pProcHeap ) Static::ParentView( m_pAppHeap, m_pAppRenderingManager );

  // 上画面View
  m_pUpperView = GFL_NEW( m_pProcHeap ) UpperView( m_pAppHeap, m_pAppRenderingManager );
  m_pUpperView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_UPP_MAIN, m_pUpperView );

  // 下画面BGView
  m_pBGView = GFL_NEW( m_pProcHeap ) BGView( m_pAppHeap, m_pAppRenderingManager );
  m_pBGView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );

  // 下画面View
  m_pLowerMainView = GFL_NEW( m_pProcHeap ) LowerMainView( m_pAppHeap, m_pAppRenderingManager );
  m_pLowerMainView->SetViewListener( this );
  m_pLowerMainView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_LOW_MAIN, m_pLowerMainView );

  // フレームリストView
  m_pFrameListView = GFL_NEW( m_pProcHeap ) FrameListView( m_pAppHeap, m_pAppRenderingManager );
  m_pFrameListView->SetViewListener( this );
  m_pFrameListView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_LOW_FRAME_LIST, m_pFrameListView );

  // スタンプリストView
  m_pStampListView = GFL_NEW( m_pProcHeap ) StampListView( m_pAppHeap, m_pAppRenderingManager );
  m_pStampListView->SetViewListener( this );
  m_pStampListView->Initialize( m_pLytDatBuff, m_pMsgData, m_pWordSet );
  m_pViewList->AddView( VIEW_ID_LOW_STAMP_LIST, m_pStampListView );

  // サブView登録
  m_pParentView->AddSubView( m_pUpperView );
  m_pParentView->AddSubView( m_pBGView );
  m_pParentView->AddSubView( m_pLowerMainView );
  m_pParentView->AddSubView( m_pFrameListView );
  m_pParentView->AddSubView( m_pStampListView );

  return true;
}

bool Proc::DeleteView( void )
{
  if( !_DeleteView( m_pStampListView ) ) { return false; }
  if( !_DeleteView( m_pFrameListView ) ) { return false; }
  if( !_DeleteView( m_pLowerMainView ) ) { return false; }
  if( !_DeleteView( m_pBGView ) ) { return false; }
  if( !_DeleteView( m_pUpperView ) ) { return false; }

  GFL_SAFE_DELETE( m_pParentView );

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
 * @brief   デコマネージャ 生成・破棄
 */
bool Proc::CreateDecoItemManager( void )
{
  m_pDecoItemManager = GFL_NEW( m_pProcHeap ) DecoItemManager();
  m_DecoWork.deco_man = m_pDecoItemManager;

  return true;
}

bool Proc::DeleteDecoItemManager( void )
{
  GFL_SAFE_DELETE( m_pDecoItemManager );

  return true;
}


/**
 * @brief   写真データ 生成・破棄
 */
bool Proc::CreatePhotoData( void )
{
  m_DecoWork.photo_data = Static::ImageDBUtil::CreateColorBuffer( m_pImgdbHeap, false, Static::DISPLAY_BUFFER_ALIGN );

  return true;
}

bool Proc::DeletePhotoData( void )
{
  Static::ImageDBUtil::DeleteColorBuffer( m_DecoWork.photo_data );

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー：ViewListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ViewListener：新規スタンプの移動開始
 */
void Proc::MoveStartNewStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  if( !m_pDecoItemManager->IsCanNotAdd() )
  {
    FUKUSHIMA_PRINT( "MoveStartNewStamp :" );
    FUKUSHIMA_PRINT( " type=[%d]", type );
    FUKUSHIMA_PRINT( " decoIndex=[%d]", decoIndex );
    FUKUSHIMA_PRINT( " pos x=[%.1f] y=[%.1f]\n", pos.x, pos.y );

    m_pLowerMainView->StartStampMove();
    m_pLowerMainView->CreateStamp( type, decoIndex, pos );
  }
}

/**
 * @brief   ViewListener：新規スタンプの移動終了
 */
void Proc::MoveEndNewStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  FUKUSHIMA_PRINT( "MoveEndNewStamp :" );
  FUKUSHIMA_PRINT( " type=[%d]", type );
  FUKUSHIMA_PRINT( " decoIndex=[%d]", decoIndex );
  FUKUSHIMA_PRINT( " pos x=[%.1f] y=[%.1f]\n", pos.x, pos.y );

  // 座標値を元のスケール値にあった値に戻す
  pos.x /= Static::DECO_SCALE_RATE;
  pos.y /= Static::DECO_SCALE_RATE;

  gfl2::math::VEC3 vecScreen = Static::ConvertUpperVectorLayoutToScreen( pos );

  u8 decoCount = m_pDecoItemManager->GetDecoCount();
  // 上画面に配置
  m_pUpperView->CreateStamp( decoCount, type, decoIndex, vecScreen );

  // データリストに反映
  m_pDecoItemManager->AddDeco( type, decoIndex, vecScreen );
}

/**
 * @brief   ViewListener：配置済みスタンプの移動開始
 */
void Proc::MoveStartOldStamp( u8 index )
{
  FUKUSHIMA_PRINT( "Proc::MoveStartOldStamp : index=[%d]\n", index );

  // 上画面のスタンプを非表示に
  m_pUpperView->InvisibleStamp( index );
}

/**
 * @brief   ViewListener：配置済みスタンプの移動終了
 */
void Proc::MoveEndOldStamp( u8 index, gfl2::math::VEC3 pos )
{
  FUKUSHIMA_PRINT( "Proc::MoveEndOldStamp : index=[%d]  pos x=[%.1f] y=[%.1f]\n", index, pos.x, pos.y );

  // 座標値を元のスケール値にあった値に戻す
  pos.x /= Static::DECO_SCALE_RATE;
  pos.y /= Static::DECO_SCALE_RATE;

  gfl2::math::VEC3 vecScreen = Static::ConvertUpperVectorLayoutToScreen( pos );

  // 上画面のスタンプの位置を変更
  m_pUpperView->SetStampPos( index, vecScreen );
  // 上画面のスタンプを表示
  m_pUpperView->VisibleStamp( index );

  // 配置済みスタンプの位置を変更
  m_pDecoItemManager->SetDecoPos( index, vecScreen );
}

/**
 * @brief   ViewListener：配置済みスタンプの削除
 */
void Proc::DeleteOldStamp( u8 index )
{
  FUKUSHIMA_PRINT( "Proc::DeleteOldStamp : index=[%d]\n", index );

  // 上画面のデコ削除
  m_pUpperView->DeleteStamp( index );
  // デコの削除
  m_pDecoItemManager->RemoveDeco( index );
}

/**
 * @brief   ViewListener：フレームのセット
 */
void Proc::SetFrame( u32 frameIndex )
{
  FUKUSHIMA_PRINT( "Proc::SetFrame : frameIndex=[%d]\n", frameIndex );

  // フレームの設定
  m_pDecoItemManager->SetFrame( frameIndex );

  m_pUpperView->CreateFrame( frameIndex );
  m_pLowerMainView->CreateFrame( frameIndex );
}

/**
 * @brief   ViewListener：スタンプが置けるかどうか
 */
bool Proc::IsCanPutStamp( void )
{
  return !m_pDecoItemManager->IsCanNotAdd();
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
