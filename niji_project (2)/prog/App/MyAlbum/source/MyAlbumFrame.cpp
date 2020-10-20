//=============================================================================
/**
 * @file    MyAlbumFrame.cpp
 * @brief   マイアルバム画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2017.01.25
 */
//=============================================================================

// module
#include "App/MyAlbum/include/MyAlbumAppParam.h"
#include "MyAlbumFrame.h"
#include "MyAlbumPhotoData.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// momiji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowDataManager.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"
#include "AppLib/include/Ui/UIViewManager.h"
#include "AppLib/include/Sort/StringMonsName.h"
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioHeapDefine.h"
#include "App/FinderStudioViewer/include/FinderStudioViewer.h"
#include "ExtSavedata/include/FinderStudioExtSaveData.h"
#include "Savedata/include/FinderStudioSave.h"
#include "System/include/HomeSleepAndPowerSystem.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/MyAlbumGraphic.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_fs_album.h"

#if PM_DEBUG
#include "Debug/MyAlbumDebug.h"
#endif


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

//! メインシーケンス
enum MainSeq
{
  SEQ_START = 0,        //!< 開始
  SEQ_MAIN,             //!< メイン
  SEQ_PHOTO_MENU,       //!< 写真メニュー
  SEQ_WAIT_MESSAGE,     //!< メッセージ終了待ち
  SEQ_PHOTO_DELETE,     //!< 写真削除
  SEQ_PHOTO_SAVE,       //!< 写真保存
  SEQ_THUMBNAIL_ERROR,  //!< サムネイル読み込みエラー
  SEQ_END,              //!< 終了
};

//! ビューワ用シーケンス
enum
{
  VIEWER_SEQ_THROUGH = 0,   //!< 初期状態
  VIEWER_SEQ_CREATE,        //!< クラス生成
  VIEWER_SEQ_INIT,          //!< 初期化
  VIEWER_SEQ_WAIT,          //!< 待機中
  VIEWER_SEQ_LOAD,          //!< 読み込み
  VIEWER_SEQ_LOAD_WAIT,     //!< 読み込み中
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
MyAlbumFrame::MyAlbumFrame(void)
  : m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_misc( NULL )
  , m_finderStudioSave( NULL )
  , m_pViewerHeap( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pBgMsgBuff( NULL )
  , m_pInitialMsgBuf( NULL )
  , m_isUIViewInputExclusiveKey( false )
  , m_pDraw( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_pMsgMenuView( NULL )
  , m_isEndMessage( false )
  , m_isEnableWindow( false )
  , m_isUgcButton( false )
  , m_pViewer( NULL )
  , m_pPhotoData( NULL )
  , m_pInfoWinManager( NULL )
  , m_pInfoWinBook( NULL )
  , m_pInfoWinStrBuf( NULL )
  , m_pListTop( NULL )
  , m_pListCore( NULL )
  , m_pSort( NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
{
  gfl2::std::MemClear( m_menuItemID, sizeof(MenuItemID)*PHOTO_MENU_MAX );
  gfl2::std::MemClear( &m_viewerWork, sizeof(VIEWER_WORK) );
  gfl2::std::MemClear( &m_copySortData, sizeof(SORT_DATA) );
  GetInputExculusiveKeyMode();
  SetInputExculusiveKeyMode( true );
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
MyAlbumFrame::~MyAlbumFrame()
{
  SetInputExculusiveKeyMode( m_isUIViewInputExclusiveKey );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2017.02.09
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_misc = gd->GetMisc();
  m_finderStudioSave = gd->GetFinderStudioSave();
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2017.01.25
 *
 * @param   param       外部設定パラメータ
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_appParam = param;
  m_heap = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2017.01.25
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result MyAlbumFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    InitFade();
    CreateLocalHeap();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 2:
    if( LoadMessage() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 3:
    if( CreateMenuCursor() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 4:
    if( CreateDraw() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 5:
    if( CreateMsgMenuView() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 6:
    if( CreatePhotoData() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2017.01.25
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result MyAlbumFrame::EndFunc(void)
{
  if( DeleteStudioViewer() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteMsgMenuView() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteMenuCursor() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  UnloadMessage();
  UnloadLayoutResource();

  if( DeletePhotoData() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  DeleteLocalHeap();

  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2017.01.25
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result MyAlbumFrame::UpdateFunc(void)
{
  UpdateViewer();
  m_pPhotoData->Update();
  m_pDraw->UpdateTree();

  CheckLoadThumbnail();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    Update_SeqStart();
    break;

  case SEQ_MAIN:
    break;

  case SEQ_PHOTO_MENU:
    break;

  case SEQ_WAIT_MESSAGE:
    Update_SeqWaitMessage();
    break;

  case SEQ_PHOTO_DELETE:
    Update_SeqPhotoDelete();
    break;

  case SEQ_PHOTO_SAVE:
    Update_SeqPhotoSave();
    break;

  case SEQ_THUMBNAIL_ERROR:
    Update_SeqThumbnailError();
    break;

  case SEQ_END:
    if( Update_SeqEnd() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2017.01.25
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pViewer != NULL && m_viewerWork.seq >= VIEWER_SEQ_WAIT )
  {
    m_pViewer->Draw( no );
  }
  m_pDraw->DrawTree( no );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetInputExculusiveKeyMode
 * @brief   UIViewのボタンアニメ中の処理モードを取得
 * @date    2017.02.01
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::GetInputExculusiveKeyMode(void)
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  m_isUIViewInputExclusiveKey = man->GetInputExclusiveKeyByButtonAnimation();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputExculusiveKeyMode
 * @brief   UIViewのボタンアニメ中の処理モードを設定
 * @date    2017.02.01
 *
 * @param   flg   true = 排他制御ON
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetInputExculusiveKeyMode( bool flg )
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  man->SetInputExclusiveKeyByButtonAnimation( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLocalHeap
 * @brief   ローカルヒープ生成
 * @date    2017.02.07
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::CreateLocalHeap(void)
{
  m_pViewerHeap = GFL_CREATE_LOCAL_HEAP( m_heap->GetDeviceHeap(), App::FinderStudio::Static::HeapDefine::VIEWER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteLocalHeap
 * @brief   ローカルヒープ削除
 * @date    2017.02.07
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::DeleteLocalHeap(void)
{
  GFL_DELETE_HEAP( m_pViewerHeap );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePhotoData
 * @brief   写真データクラス生成
 * @date    2017.02.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::CreatePhotoData(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pPhotoData = GFL_NEW(m_heap->GetSystemHeap()) MyAlbumPhotoData( m_heap );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPhotoData->Initialize() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePhotoData
 * @brief   写真データクラス生成
 * @date    2017.02.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DeletePhotoData(void)
{
  if( m_pPhotoData != NULL )
  {
    switch( m_subSeq )
    {
    case 0:
      if( m_pPhotoData->UnloadData() == false )
      {
        break;
      }
      m_subSeq++;
      /* FALL THROUGH */
    case 1:
      if( m_pPhotoData->Terminate() == false )
      {
        break;
      }
      m_subSeq = 0;
      GFL_SAFE_DELETE( m_pPhotoData );
      return true;
    }
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadPhotoData
 * @brief   写真データ読み込み
 * @date    2017.03.13
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumFrame::LoadPhotoData(void)
{
  MyAlbumPhotoData::SDCardAccessResult result = m_pPhotoData->LoadData();

  if( result != MyAlbumPhotoData::SDC_ACCESS_RESULT_OK && result != MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_BROKEN )
  {
    return result;
  }

  if( result != MyAlbumPhotoData::SDC_ACCESS_RESULT_WAIT )
  {
    // 絞込みを反映
    m_pPhotoData->CreateSortData( &m_appParam->sort_data );
    // 初期設定
    if( m_appParam->init_share == MYALBUM_INIT_SHARE )
    {
      m_appParam->init_share = m_pPhotoData->GetShareIndex();

      for( u32 i=0; i<m_pPhotoData->GetDataCountOrigin(); i++ )
      {
        if( m_pPhotoData->IsFavoriteOrigin(i) == false )
        {
          continue;
        }
        u32 index = m_pPhotoData->GetIndexOrigin( i );
        m_appParam->init_favorite[index/8] |= (1<<(index%8));
      }
    }
    // 画面復帰
    if( m_appParam->end_mode == END_MODE_DECO )
    {
      SetRecoverData();
    }
  }

  return result;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2017.01.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::CreateDraw(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) MyAlbumDraw( m_appParam, m_heap, m_renderingManager, this );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->Init( m_pLytResBuff, m_pMsgBuff, m_pBgMsgBuff, m_pInitialMsgBuf ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2017.01.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteStudioViewer
 * @brief   上画面クラス削除
 * @date    2017.02.07
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DeleteStudioViewer(void)
{
  if( m_pViewer != NULL )
  {
    if( m_pViewer->EndFunc() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pViewer );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   メニューカーソル生成
 * @date    2017.01.27
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::CreateMenuCursor(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorBuffer );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuCursor->LoadResource() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   メニューカーソル削除
 * @date    2017.01.27
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DeleteMenuCursor(void)
{
  if( m_pMenuCursor != NULL )
  {
    if( m_pMenuCursor->IsModuleFree() == false )
    {
      return false;
    }
    m_pMenuCursor->DeleteResource();
    GFL_SAFE_DELETE( m_pMenuCursor );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMsgMenuView
 * @brief   メッセージメニュー生成
 * @date    2017.01.27
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::CreateMsgMenuView(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pMsgMenuView = GFL_NEW(m_heap->GetSystemHeap()) App::FinderStudio::Static::MessageMenuView( m_heap, m_renderingManager, m_pDraw->GetMsgData(), m_pMenuCursorBuffer );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMsgMenuView->InitFunc() == false )
    {
      break;
    }
    m_pMsgMenuView->SetListener( this );
    m_pDraw->AddSubView( m_pMsgMenuView->GetView() );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMsgMenuView
 * @brief   メッセージメニュー削除
 * @date    2017.01.27
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DeleteMsgMenuView(void)
{
  if( m_pMsgMenuView != NULL )
  {
    switch( m_subSeq )
    {
    case 0:
      m_pMsgMenuView->GetView()->RemoveFromSuperView();
      m_subSeq++;
      /* FALL THROUGH */
    case 1:
      if( m_pMsgMenuView->EndFunc() == false )
      {
        return false;
      }
      GFL_SAFE_DELETE( m_pMsgMenuView );
      m_subSeq = 0;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateInfoWindow
 * @brief   インフォウィンドウ生成
 * @date    2017.02.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::CreateInfoWindow(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pInfoWinManager = GFL_NEW(m_heap->GetSystemHeap()) app::tool::InfoWindowDataManager();
    m_pInfoWinManager->StartAsyncCreate( m_heap, app::tool::InfoWindowDataManager::CREATE_TYPE_BOOK );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    m_pInfoWinManager->UpdateFunc();
    if( m_pInfoWinManager->IsCreated() == false )
    {
      break;
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    m_pInfoWinStrBuf = GFL_NEW(m_heap->GetSystemHeap()) gfl2::str::StrBuf( INFOWIN_STR_LEN, m_heap->GetDeviceHeap() );
    m_pInfoWinBook = GFL_NEW(m_heap->GetSystemHeap()) app::tool::InfoWindowBookType( m_heap );
    m_pInfoWinBook->StartAsyncCreate( m_heap, m_renderingManager, INFOWIN_PAGE_MAX, m_pInfoWinManager );
    m_pDraw->AddSubView( m_pInfoWinBook );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( m_pInfoWinBook->IsCreated() == false )
    {
      break;
    }
    m_pInfoWinBook->ResetContents( INFOWIN_PAGE_MAX );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteInfoWindow
 * @brief   インフォウィンドウ削除
 * @date    2017.02.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DeleteInfoWindow(void)
{
  if( m_pInfoWinBook != NULL )
  {
    if( m_pInfoWinBook->EndFunc() == false )
    {
      return false;
    }
    m_pInfoWinBook->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pInfoWinBook );
    GFL_SAFE_DELETE( m_pInfoWinStrBuf );
  }

  if( m_pInfoWinBook == NULL )
  {
    if( m_pInfoWinManager != NULL )
    {
      if( m_pInfoWinManager->EndFunc() == false )
      {
        return false;
      }
      GFL_SAFE_DELETE( m_pInfoWinManager );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2017.01.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_MYALBUM_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_MYALBUM_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_MYALBUM_GRA,
      GARC_MyAlbumGraphic_MyAlbum_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_MYALBUM_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_MYALBUM_GRA ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::UnloadLayoutResource(void)
{
  GflHeapSafeFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2017.01.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_fs_album_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_fs_photo_DAT,
      &m_pBgMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileLoad( &m_pBgMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_initial_DAT,
      &m_pInitialMsgBuf,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:
    if( app::util::FileAccessor::IsFileLoad( &m_pInitialMsgBuf ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 5:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージ削除
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::UnloadMessage(void)
{
  GflHeapSafeFreeMemory( m_pInitialMsgBuf );
  GflHeapSafeFreeMemory( m_pBgMsgBuff );
  GflHeapSafeFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2017.01.25
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::IsFadeEnd(void)
{
  gfl2::Fade::FadeManager * man = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  if( man->IsEnd(gfl2::Fade::DISP_UPPER) == false || man->IsEnd(gfl2::Fade::DISP_LOWER) == false )
  {
    return false;
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    ChangeSequence
 * @brief   シーケンス切り替え
 * @date    2017.01.25
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeEndSequence
 * @brief   終了シーケンスへ切り替え
 * @date    2017.02.17
 *
 * @param   mode  終了モード
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::ChangeEndSequence( EndMode mode )
{
  m_appParam->end_mode = mode;
  ChangeSequence( SEQ_END );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::Update_SeqStart(void)
{
  enum
  {
    START_SEQ_CREATEINFO_WINDOW = 0,  // インフォウィンドウ作成
    START_SEQ_FADEIN,                 // フェードイン
    START_SEQ_MSG_WAIT,               // 初期メッセージウェイト
    START_SEQ_IMGDB_CHECK,            // イメージDBチェック
    START_SEQ_LOAD_PHOTO,             // 写真読み込み
    START_SEQ_CHECK_NUM,              // 残り保存可能枚数チェック
    START_SEQ_INIT_DISP,              // 画面初期設定
    START_SEQ_CREATE_SORT,            // ソートデータ生成
    START_SEQ_START,                  // 開始
    START_SEQ_ERR_IMGDB,              // エラー：イメージDB関連
    START_SEQ_ERR_SDC_SAFE,           // エラー：SDカード関連
    START_SEQ_ERR_NUM,                // エラー：保存可能枚数関連
    START_SEQ_ERR_APP_END,            // エラー：アプリ終了
  };

  switch( m_mainSeq )
  {
  case START_SEQ_CREATEINFO_WINDOW:   // インフォウィンドウ作成
    // @note UIViewのUpdateが回らないとダメみたいなのでここで。
    if( CreateInfoWindow() == false )
    {
      break;
    }
    if( m_appParam->end_mode != END_MODE_DECO )
    {
      m_pMsgMenuView->ShowMessage_Id( fs_album_win_01, true, true );
      m_isEnableWindow = true;
    }
    SetFadeIn();
    m_mainSeq = START_SEQ_FADEIN;
    /* FALL THROUGH */
  case START_SEQ_FADEIN:              // フェードイン
    if( IsFadeEnd() == false )
    {
      break;
    }
    if( m_appParam->end_mode != END_MODE_DECO )
    {
      m_subSeq = 0;
      m_mainSeq = START_SEQ_MSG_WAIT;
    }
    else
    {
      m_mainSeq = START_SEQ_IMGDB_CHECK;
    }
    break;

  case START_SEQ_MSG_WAIT:            // 初期メッセージウェイト
    if( m_subSeq != SDCARD_INIT_MSG_WAIT )
    {
      m_subSeq++;
    }
    else
    {
      m_subSeq = 0;
      m_mainSeq = START_SEQ_IMGDB_CHECK;
    }
    break;

  case START_SEQ_IMGDB_CHECK:         // イメージDBチェック
    {
      MyAlbumPhotoData::SDCardAccessResult result = m_pPhotoData->CheckImageDBError();
      if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_FATAL )
      {
        ChangeEndSequence( END_MODE_FATAL_ERR );
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_LACK_OF_SPACE )
      {
        SetStreamingMessage( fs_album_win_02 );
        m_mainSeq = START_SEQ_ERR_IMGDB;
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_OVER_IMAGE )
      {
        SetStreamingMessage( fs_album_win_03 );
        m_mainSeq = START_SEQ_ERR_IMGDB;
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_ETC )
      {
        SetStreamingMessage( fs_album_win_04 );
        m_mainSeq = START_SEQ_ERR_APP_END;
        break;
      }
    }
    m_mainSeq = START_SEQ_LOAD_PHOTO;
    break;

  case START_SEQ_LOAD_PHOTO:        // 写真読み込み
    {
      MyAlbumPhotoData::SDCardAccessResult result = LoadPhotoData();
      if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_WAIT )
      {
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_FATAL )
      {
        ChangeEndSequence( END_MODE_FATAL_ERR );
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_BROKEN )
      {
        SetStreamingMessage( fs_album_win_05 );
        m_mainSeq = START_SEQ_ERR_SDC_SAFE;
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_LACK_OF_SPACE )
      {
        SetStreamingMessage( fs_album_win_06 );
        m_mainSeq = START_SEQ_ERR_SDC_SAFE;
        break;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_ETC )
      {
        GFL_ASSERT( 0 );
        m_isEndMessage = false;
        m_mainSeq = START_SEQ_ERR_APP_END;
        break;
      }
    }
    m_mainSeq = START_SEQ_CHECK_NUM;
    /* FALL THROUGH */
  case START_SEQ_CHECK_NUM:   // 残り保存可能枚数チェック
    {
      MyAlbumPhotoData::ResultPhotoNum result = m_pPhotoData->CheckPhotoNum();
      if( result == MyAlbumPhotoData::RESULT_PHOTO_NUM_WARNING )
      {
        SetStreamingMessageExpand(
          fs_album_win_07,
          MyAlbumPhotoData::PHOTO_DATA_MAX - m_pPhotoData->GetDataCountOrigin(),
          1 );
        m_mainSeq = START_SEQ_ERR_NUM;
        break;
      }
      else if( result == MyAlbumPhotoData::RESULT_PHOTO_NUM_MAX )
      {
        SetStreamingMessage( fs_album_win_08 );
        m_mainSeq = START_SEQ_ERR_NUM;
        break;
      }
    }
    m_mainSeq = START_SEQ_INIT_DISP;
    /* FALL THROUGH */
  case START_SEQ_INIT_DISP:   // 画面初期設定
    InitViewerRequest();
    m_pDraw->Setup( m_pPhotoData, m_pMenuCursor, m_appParam->select_pos );
    CreatePaneListView();
    m_pMenuCursor->SetVisible( false );   // @note ペインリストがONにしてしまうため
    m_mainSeq = START_SEQ_CREATE_SORT;
    /* FALL THROUGH */
  case START_SEQ_CREATE_SORT:   // ソートデータ生成
    if( CreateSortBinary() == false )
    {
      break;
    }
    m_pDraw->Start();
    HideMessageWindow();
    m_mainSeq = START_SEQ_START;
    /* FALL THROUGH */
  case START_SEQ_START:   // 開始
    if( m_pDraw->IsStart() == false )
    {
      break;
    }
    if( m_finderStudioSave->IsTutorialState( Savedata::FinderStudioSave::TUTORIAL_STATE_MYALBUM_INFO ) == false )
    {
      m_finderStudioSave->SetTutorialState( Savedata::FinderStudioSave::TUTORIAL_STATE_MYALBUM_INFO );
      StartInfoWindow();
    }
    ChangeSequence( SEQ_MAIN );
    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
    break;

  case START_SEQ_ERR_IMGDB:     // エラー：イメージDB関連
    if( m_isEndMessage == false )
    {
      break;
    }
    if( m_appParam->end_mode != END_MODE_DECO )
    {
      m_pMsgMenuView->ShowMessage_Id( fs_album_win_01, true, true );
      m_isEnableWindow = true;
    }
    m_mainSeq = START_SEQ_LOAD_PHOTO;
    break;

  case START_SEQ_ERR_SDC_SAFE:  // エラー：SDカード関連
    if( m_isEndMessage == false )
    {
      break;
    }
    m_mainSeq = START_SEQ_CHECK_NUM;
    break;

  case START_SEQ_ERR_NUM:       // エラー：保存可能枚数関連
    if( m_isEndMessage == false )
    {
      break;
    }
    m_mainSeq = START_SEQ_INIT_DISP;
    break;

  case START_SEQ_ERR_APP_END:   // エラー：アプリ終了
    if( m_isEndMessage == false )
    {
      break;
    }
    ChangeEndSequence( END_MODE_EXIT );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqWaitMessage
 * @brief   アップデート：メッセージ終了待ち
 * @date    2017.01.30
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::Update_SeqWaitMessage(void)
{
  if( m_isEndMessage == false )
  {
    return;
  }
  m_pDraw->SetInputEnabled( true );
  HideMessageWindow();
  ChangeSequence( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqPhotoDelete
 * @brief   アップデート：写真削除
 * @date    2017.01.30
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::Update_SeqPhotoDelete(void)
{
  switch( m_mainSeq )
  {
  case 0:
    break;

  case 1:
    m_pPhotoData->InitDataBrokenFlag();
    m_pMsgMenuView->ShowMessage_Id( fs_album_win_14, true, true );
    m_isEnableWindow = true;
    m_subSeq = 0;
    m_mainSeq++;
    break;

  case 2:
    if( m_subSeq != SDCARD_INIT_MSG_WAIT )
    {
      m_subSeq++;
    }
    else
    {
      m_subSeq = 0;
      m_mainSeq++;
    }
    break;

  case 3:
    {
      MyAlbumPhotoData::SDCardAccessResult result = m_pDraw->DeletePhotoData();
      if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_OK )
      {
        SetStreamingMessage( fs_album_win_15 );
        m_mainSeq++;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_FATAL )
      {
        ChangeEndSequence( END_MODE_FATAL_ERR );
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_BROKEN )
      {
        SetStreamingMessage( fs_album_win_05 );
        m_mainSeq++;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_ETC )
      {
        SetStreamingMessage( fs_album_win_04 );
        m_mainSeq = 5;
      }
    }
    break;

  case 4:
    if( m_isEndMessage == false )
    {
      break;
    }
    m_pDraw->SetInputEnabled( true );
    HideMessageWindow();
    if( m_pDraw->GetDispMode() == MyAlbumDraw::DISP_MODE_DELETE && m_pPhotoData->GetDataCount() == 0 )
    {
      EndMultiDelete();
      m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SELECT );
    }
    ChangeSequence( SEQ_MAIN );
    break;

  case 5:
    if( m_isEndMessage == false )
    {
      break;
    }
    ChangeEndSequence( END_MODE_EXIT );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqPhotoSave
 * @brief   アップデート：写真保存
 * @date    2017.02.14
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::Update_SeqPhotoSave(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pPhotoData->InitDataBrokenFlag();
    m_pMsgMenuView->ShowMessage_Id( fs_album_win_20, true, true );
    m_isEnableWindow = true;
    m_subSeq = 0;
    m_mainSeq = 1;
    break;

  case 1:
    if( m_subSeq != SDCARD_INIT_MSG_WAIT )
    {
      m_subSeq++;
    }
    else
    {
      // @fix GFMMCat[471]: ホームボタン/スリープ禁止
      System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_IMAGEDB_ACC );
      m_subSeq = 0;
      m_mainSeq = 2;
    }
    break;

  case 2:
    {
      u32 max = m_pPhotoData->GetDataCountOrigin();
      if( m_subSeq >= max )
      {
        m_mainSeq = 4;
        break;
      }
      else
      {
        for( ; m_subSeq<max; m_subSeq++ )
        {
          if( IsSave(m_subSeq) != false )
          {
            m_mainSeq = 3;
            break;
          }
        }
        if( m_subSeq >= max )
        {
          m_mainSeq = 4;
          break;
        }
      }
    }
    /* FALL THROUGH */
  case 3:
    {
      MyAlbumPhotoData::SDCardAccessResult result = m_pPhotoData->SaveData( m_pPhotoData->GetIndexOrigin(m_subSeq) );
      if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_OK || result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_BROKEN )
      {
        m_subSeq++;
        m_mainSeq = 2;
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_FATAL )
      {
        m_subSeq = 0;
        ChangeEndSequence( END_MODE_FATAL_ERR );
      }
      else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_LACK_OF_SPACE )
      {
        // @fix GFMMCat[471]: ホームボタン/スリープ禁止解除
        System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_IMAGEDB_ACC );
        SetStreamingMessage( fs_album_win_12 );
        m_mainSeq = 5;
      }
    }
    break;

  case 4:
    // @fix GFMMCat[471]: ホームボタン/スリープ禁止解除
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_IMAGEDB_ACC );
    if( m_pPhotoData->IsDataBroken() == false )
    {
      SetStreamingMessage( fs_album_win_21 );
    }
    else
    {
      SetStreamingMessage( fs_album_win_05 );
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 5:
    if( m_isEndMessage == false )
    {
      break;
    }
    m_subSeq = 0;
    ChangeEndSequence( END_MODE_EXIT );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqThumbnailError
 * @brief   アップデート：サムネイル読み込みエラー
 * @date    2017.02.17
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::Update_SeqThumbnailError(void)
{
  if( m_isEndMessage != false )
  {
    m_subSeq = 0;
    ChangeEndSequence( END_MODE_ERROR );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqEnd
 * @brief   アップデート：終了処理
 * @date    2017.01.25
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::Update_SeqEnd(void)
{
  switch( m_mainSeq )
  {
  case 0:
    // 通信用のデータに入れる
    if( m_appParam->end_mode == END_MODE_EXIT )
    {
      u32 share = m_pPhotoData->GetShareIndex();
      if( share == MyAlbumPhotoData::PHOTO_DATA_MAX )
      {
        m_finderStudioSave->RemoveSendPhotoData();  // 解除
      }
      else
      {
        App::FinderStudio::Static::IntegratedData * data = m_pPhotoData->GetDataIndex( share );
        m_finderStudioSave->SetSendPhotoData( *data );  // 追加
      }
    }
    SetFadeOut();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsFadeEnd() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pPhotoData->IsTerminate() == false )
    {
      break;
    }
    if( IsViewerTerminate() == false )
    {
      break;
    }
    // @note UIViewのUpdateが回らないとダメみたいなのでここで。
    if( DeleteInfoWindow() == false )
    {
      break;
    }
    DeletePaneListView();
    DeleteSortBinary();
    m_mainSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPhotoMenu
 * @brief   写真メニュー表示
 * @date    2017.01.27
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::PutPhotoMenu(void)
{
  static const u32 tbl[] =
  {
    fs_album_cap_41,  // へんしゅうする
    fs_album_cap_42,  // すてる
    fs_album_cap_43,  // おきにいりする
    fs_album_cap_48,  // みんなにみせる
    fs_album_cap_45,  // やめる
  };

  u32 item[PHOTO_MENU_MAX];

  for( u32 i=0; i<PHOTO_MENU_MAX; i++ )
  {
    item[i] = tbl[i];
    m_menuItemID[i] = static_cast<MenuItemID>( i );
  }
  if( m_pDraw->IsPhotoFavorite() != false )
  {
    item[MENU_PHOTO_FAVORITE] = fs_album_cap_44;  // はずす
  }
  if( m_pDraw->IsPhotoShare() != false )
  {
    item[MENU_PHOTO_SHARE] = fs_album_cap_49;  // みせない
  }

  m_pMsgMenuView->GetView()->SetYNListMode( false );
  m_pMsgMenuView->ShowMessageWithListMenu( fs_album_win_22, item, PHOTO_MENU_MAX, PHOTO_MENU_MAX-1, true, true );   // @fix GFMMCat[537]: 戻るマーク設定追加

  ChangeSequence( SEQ_PHOTO_MENU );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutYesNoMenu
 * @brief   "はい・いいえ"メニュー表示
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::PutYesNoMenu( u32 msg )
{
  static const u32 item[] =
  {
    fs_album_cap_46,
    fs_album_cap_47,
  };

  u32 size = GFL_NELEMS(item);

  m_pMsgMenuView->GetView()->SetYNListMode( true );
  m_pMsgMenuView->ShowMessageWithListMenu( msg, item, size, size-1, false, false );   // @fix GFMMCat[537]: 戻るマーク設定追加
}

//-----------------------------------------------------------------------------
/**
 * @func    PutDeleteMenu
 * @brief   写真削除メニュー表示
 * @date    2017.01.30
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::PutDeleteMenu(void)
{
  m_menuItemID[0] = MENU_DELETE_YES;
  m_menuItemID[1] = MENU_DELETE_NO;
  PutYesNoMenu( fs_album_win_10 );
  ChangeSequence( SEQ_PHOTO_DELETE );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutShareMenu
 * @brief   写真共有メニュー表示
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::PutShareMenu(void)
{
  m_menuItemID[0] = MENU_SHARE_YES;
  m_menuItemID[1] = MENU_SHARE_NO;
  PutYesNoMenu( fs_album_win_24 );
  ChangeSequence( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutShareUgcMenu
 * @brief   写真共有UGCメニュー表示
 * @date    2017.02.09
 *
 * @param   is_button   true = 共有ボタンから
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::PutShareUgcMenu( bool is_button )
{
  m_menuItemID[0] = MENU_SHARE_UGC_YES;
  m_menuItemID[1] = MENU_SHARE_UGC_NO;
  PutYesNoMenu( fs_album_guideline_00 );
  m_misc->SetMyAlbumUGCEnable();
  m_isUgcButton = is_button;
  ChangeSequence( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetStreamingMessage
 * @brief   送りメッセージ開始
 * @date    2017.04.14
 *
 * @param   msg   メッセージID
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetStreamingMessage( u32 msg )
{
  m_pMsgMenuView->ShowMessage_Id( msg );
  m_isEndMessage = false;
  m_isEnableWindow = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetStreamingMessageExpand
 * @brief   送りメッセージ開始（展開版）
 * @date    2017.04.14
 *
 * @param   msg   メッセージID
 * @param   val   Register[0]に入れる値
 * @param   keta  valの桁
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetStreamingMessageExpand( u32 msg, u32 val, u32 keta )
{
  gfl2::str::StrBuf * str = m_pDraw->GetExpandMessage( msg, val, keta );
  m_pMsgMenuView->ShowMessage_Str( str );
  m_isEndMessage = false;
  m_isEnableWindow = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartMessage
 * @brief   メッセージ開始
 * @date    2017.01.30
 *
 * @param   msg   メッセージID
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::StartMessage( u32 msg )
{
  SetStreamingMessage( msg );
  ChangeSequence( SEQ_WAIT_MESSAGE );
}

//-----------------------------------------------------------------------------
/**
 * @func    HideMessageWindow
 * @brief   メッセージウィンドウを非表示にする
 * @date    2017.04.14
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::HideMessageWindow(void)
{
  if( m_isEnableWindow != false )
  {
    m_pMsgMenuView->HideMessage();
    m_isEnableWindow = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    EndMultiDelete
 * @brief   複数削除モード終了
 * @date    2017.01.31
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::EndMultiDelete(void)
{
  m_pDraw->InitDeletePhotoBuffer();
  for( u32 i=0; i<MyAlbumDraw::PHOTO_PUT_MAX; i++ )
  {
    m_pDraw->SetVisiblePhotoSelectMark( i, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DecideDelete
 * @brief   削除決定
 * @date    2017.02.06
 *
 * @retval  true  = 削除可能
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::DecideDelete(void)
{
  switch( m_pDraw->CheckSelectDeletePhoto() )
  {
  case MyAlbumDraw::DEL_RESULT_OK:        // 削除可能
    PutDeleteMenu();
    return true;

  case MyAlbumDraw::DEL_RESULT_NG_SELECT: // 選択されていない
    StartMessage( fs_album_win_18 );
    break;

  case MyAlbumDraw::DEL_RESULT_NG_FAVORITE: // お気に入りに登録されている
  case MyAlbumDraw::DEL_RESULT_NG_SHARE:    // 共有されている
    StartMessage( fs_album_win_23 );
    break;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsSave
 * @brief   セーブするか（全体チェック）
 * @date    2017.02.14
 *
 * @retval  true  = する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::IsSave(void)
{
  for( u32 i=0; i<m_pPhotoData->GetDataCountOrigin(); i++ )
  {
    if( IsSave(i) != false )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsSave
 * @brief   セーブするか（個別チェック）
 * @date    2017.02.14
 *
 * @param   index   バッファのインデックス
 *
 * @retval  true  = する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::IsSave( u32 index )
{
  u32 sd_index = m_pPhotoData->GetIndexOrigin( index );

  { // 共有
    u32 new_share = m_pPhotoData->GetShareIndex();
    if( m_appParam->init_share != new_share )
    {
      if( m_appParam->init_share == sd_index || new_share == sd_index )
      {
        return true;
      }
    }
  }

  { // お気に入り
    bool init_favorite = false;
    if( ( m_appParam->init_favorite[sd_index/8] & (1<<(sd_index%8)) ) != 0 )
    {
      init_favorite = true;
    }
    if( m_pPhotoData->IsFavoriteOrigin(index) != init_favorite )
    {
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRecoverData
 * @brief   画面復帰用データ生成
 * @date    2017.02.14
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::CreateRecoverData(void)
{
  m_appParam->rcv_share = m_pPhotoData->GetShareIndex();

  for( u32 i=0; i<MYALBUM_PHOTO_FLAG_SIZE; i++ )
  {
    m_appParam->rcv_favorite[i] = 0;
  }

  for( u32 i=0; i<m_pPhotoData->GetDataCountOrigin(); i++ )
  {
    if( m_pPhotoData->IsFavoriteOrigin(i) == false )
    {
      continue;
    }
    u32 index = m_pPhotoData->GetIndexOrigin( i );
    m_appParam->rcv_favorite[index/8] |= (1<<(index%8));
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRecoverData
 * @brief   画面復帰データ設定
 * @date    2017.02.14
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetRecoverData(void)
{
  for( u32 i=0; i<m_pPhotoData->GetDataCountOrigin(); i++ )
  {
    u32 index = m_pPhotoData->GetIndexOrigin( i );
    // 共有
    if( m_appParam->rcv_share == index )
    {
      m_pPhotoData->SetShareOrigin( i, true );
    }
    else
    {
      m_pPhotoData->SetShareOrigin( i, false );
    }
    // お気に入り
    if( ( m_appParam->rcv_favorite[index/8] & (1<<(index%8)) ) != 0 )
    {
      m_pPhotoData->SetFavoriteOrigin( i, true );
    }
    else
    {
      m_pPhotoData->SetFavoriteOrigin( i, false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitViewerRequest
 * @brief   上画面ビューワリクエストデータ初期設定
 * @date    2017.02.15
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::InitViewerRequest(void)
{
  m_viewerWork.seq  = VIEWER_SEQ_CREATE;
  m_viewerWork.req  = VIEWER_REQUEST_NONE;
  m_viewerWork.data = VIEWER_REQUEST_NONE;

  u32 max = m_pPhotoData->GetDataCount();
  if( max != 0 )
  {
    if( m_appParam->select_pos >= max )
    {
      m_appParam->select_pos = 0;
    }
    m_viewerWork.req = m_pPhotoData->GetIndex( m_appParam->select_pos );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetViewerRequest
 * @brief   上画面ビューワリクエスト
 * @date    2017.02.15
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetViewerRequest( u32 pos )
{
  if( pos < m_pPhotoData->GetDataCount() )
  {
    u32 index = m_pPhotoData->GetIndex( pos );
    if( m_viewerWork.data != index || ( m_viewerWork.req != VIEWER_REQUEST_NONE && m_viewerWork.req != index ) )
    {
      m_viewerWork.req = index;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateViewer
 * @brief   上画面ビューワ更新
 * @date    2017.02.15
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::UpdateViewer(void)
{
  switch( m_viewerWork.seq )
  {
  case VIEWER_SEQ_THROUGH:
    break;

  case VIEWER_SEQ_CREATE:
    if( m_pPhotoData->GetDataCount() == 0 )
    {
      m_viewerWork.seq = VIEWER_SEQ_WAIT;
      return;
    }
    {
      App::FinderStudio::Static::IntegratedData * data = m_pPhotoData->GetDataIndex( m_viewerWork.req );
      m_pViewer = GFL_NEW(m_heap->GetSystemHeap()) App::FinderStudio::Viewer::FinderStudioViewer(
                                                    m_pViewerHeap,
                                                    m_renderingManager,
                                                    data->photo_data.photo_param,
                                                    data->photo_data.deco_param );
#if PM_DEBUG
      DebugClass::SetFinderStudioViewer( m_pViewer );
      DebugClass::SetReadPhotoParam( data );
#endif
    }
    m_viewerWork.data = m_viewerWork.req;
    m_viewerWork.seq = VIEWER_SEQ_INIT;
    /* FALL THROUGH */
  case VIEWER_SEQ_INIT:
    if( m_pViewer->InitFunc() != false )
    {
      if( m_viewerWork.data == m_viewerWork.req )
      {
        m_viewerWork.req = VIEWER_REQUEST_NONE;
        if( IsUpperViewerOn() != false )
        {
          SetVisibleViewer( false );
        }
      }
      m_viewerWork.seq = VIEWER_SEQ_WAIT;
    }
    break;

  case VIEWER_SEQ_WAIT:
#if PM_DEBUG
    if( m_viewerWork.req == VIEWER_REQUEST_NONE && !DebugClass::IsPhotoReproduct() )
    {
      break;
    }
#else
    if( m_viewerWork.req == VIEWER_REQUEST_NONE )
    {
      break;
    }
#endif
    if( m_pViewer == NULL )
    {
      m_viewerWork.seq = VIEWER_SEQ_CREATE;
      break;
    }
    SetVisibleViewer( true );
    m_viewerWork.data = m_viewerWork.req;
    m_viewerWork.seq = VIEWER_SEQ_LOAD;
    /* FALL THROUGH */
  case VIEWER_SEQ_LOAD:
    if( m_pViewer->IsCanStartPhotoProduct() == false )
    {
      break;
    }
    {
      App::FinderStudio::Static::IntegratedData * data = m_pPhotoData->GetDataIndex( m_viewerWork.data );

#if PM_DEBUG
      if( DebugClass::IsPhotoReproduct() )
      {
        /// 不正データを反映
        data = DebugClass::GetIllegalParameter();
        /// データ反映フラグをリセット
        DebugClass::ResetPhotoReproduct();
      }
      else {
        /// 正規データを設定
        DebugClass::SetReadPhotoParam( data );
      }
#endif
      if( data == NULL )
      {
        GFL_ASSERT( 0 );
        m_viewerWork.req = VIEWER_REQUEST_NONE;
        m_viewerWork.seq = VIEWER_SEQ_WAIT;
        break;
      }
      m_pViewer->StartPhotoReproduct( data->photo_data.photo_param, &data->photo_data.deco_param );
    }
    m_viewerWork.seq = VIEWER_SEQ_LOAD_WAIT;
    /* FALL THROUGH */
  case VIEWER_SEQ_LOAD_WAIT:
    if( m_pViewer->IsFinishPhotoReproduct() == false )
    {
      break;
    }
    if( m_viewerWork.req == m_viewerWork.data )
    {
      m_viewerWork.req = VIEWER_REQUEST_NONE;
      if( IsUpperViewerOn() != false )
      {
        SetVisibleViewer( false );
      }
    }
    m_viewerWork.seq = VIEWER_SEQ_WAIT;
    break;
  }

  if( m_pViewer != NULL && m_viewerWork.seq >= VIEWER_SEQ_WAIT )
  {
    m_pViewer->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsViewerTerminate
 * @brief   上画面ビューワを削除可能か
 * @date    2017.02.16
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::IsViewerTerminate(void)
{
  if( m_viewerWork.seq == VIEWER_SEQ_THROUGH ||
      ( m_viewerWork.seq == VIEWER_SEQ_WAIT && m_viewerWork.req == VIEWER_REQUEST_NONE ) )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleViewer
 * @brief   上画面ビューワ表示切り替え
 * @date    2017.03.17
 *
 * @param   flg   true = 非表示, false = 表示
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetVisibleViewer( bool flg )
{
  if( GetSubSeq() != SEQ_THUMBNAIL_ERROR )
  {
    m_pDraw->SetVisibleUpperBg( flg );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckLoadThumbnail
 * @brief   サムネイル読み込み中にエラーが発生したかをチェック
 * @date    2017.02.17
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::CheckLoadThumbnail(void)
{
  if( m_pPhotoData == NULL )
  {
    return;
  }

  MyAlbumPhotoData::SDCardAccessResult result = m_pPhotoData->CheckThumbnailLoadError();
  
  if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_OK )
  {
    return;
  }

  int seq = GetSubSeq();
  if( seq != SEQ_START && seq != SEQ_END && seq != SEQ_THUMBNAIL_ERROR )
  {
    m_subSeq = 0;
    m_pDraw->SetInputEnabled( false );
    if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_FATAL )
    {
      m_pMsgMenuView->GetView()->SetInputEnabled( false );
      ChangeEndSequence( END_MODE_FATAL_ERR );
    }
    else
    {
      SetVisibleViewer( true );
      m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_ERROR );
      m_pMsgMenuView->GetView()->ClearMenu();
      SetStreamingMessage( fs_album_win_04 );
      ChangeSequence( SEQ_THUMBNAIL_ERROR );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartInfoWindow
 * @brief   インフォウィンドウ開始
 * @date    2017.02.20
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::StartInfoWindow(void)
{
  m_pDraw->SetInputEnabled( false );
  SetVisibleViewer( true );
  {
    gfl2::math::Vector3 vec( 0.0f, 0.0f, 0.0f );
    m_pDraw->GetInfoPos( &vec );
    m_pInfoWinBook->StartOpen( this, vec );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneListView
 * @brief   ペインリストビュー生成
 * @date    2017.02.20
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::CreatePaneListView(void)
{
  m_pListTop = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::PaneListView( m_heap, true );
  m_pListTop->SetListener( this );
  m_pDraw->InitListTop( m_pListTop );
  SetListTopInputEnabled( false );

  m_pListCore = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::PaneListView( m_heap, true );
  m_pListCore->SetListener( this );
  m_pDraw->InitListCore( m_pListCore );
  SetListCoreInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePaneListView
 * @brief   ペインリストビュー削除
 * @date    2017.02.20
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::DeletePaneListView(void)
{
  if( m_pListCore != NULL )
  {
    m_pDraw->EndListCore();
    GFL_SAFE_DELETE( m_pListCore );
  }
  if( m_pListTop != NULL )
  {
    m_pDraw->EndListTop();
    GFL_SAFE_DELETE( m_pListTop );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListTopInputEnabled
 * @brief   ソートトップリストの入力切り替え
 * @date    2017.02.20
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetListTopInputEnabled( bool flg )
{
  m_pListTop->SetInputEnabled( flg );
  m_pListTop->GetPaneList()->SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListCoreInputEnabled
 * @brief   ソート項目別リストの入力切り替え
 * @date    2017.02.20
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SetListCoreInputEnabled( bool flg )
{
  m_pListCore->SetInputEnabled( flg );
  m_pListCore->GetPaneList()->SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    ReturnTopList
 * @brief   項目別リストからトップリストへ戻る
 * @date    2017.02.21
 *
 * @param   is_cancel   true = キャンセル
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::ReturnTopList( bool is_cancel )
{
  m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SEARCH_TYPE );
  if( is_cancel == false )
  {
    m_pDraw->ResetTopList();
  }
  else
  {
    m_pDraw->SetListType( MyAlbumDraw::LIST_TYPE_TOP );
  }
  m_pListTop->GetPaneList()->PutCursor();
  SetListTopInputEnabled( true );
  SetListCoreInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateSortBinary
 * @brief   ソートデータ生成
 * @date    2017.02.22
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::CreateSortBinary(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pSort = GFL_NEW(m_heap->GetSystemHeap()) app::sort::StringMonsName();
    m_pSort->LoadSetup( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pSort->LoadMain() == false )
    {
      break;
    }
    m_pDraw->SetPokemonSortData( m_pSort );
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteSortBinary
 * @brief   ソートデータ削除
 * @date    2017.02.22
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::DeleteSortBinary(void)
{
  GFL_SAFE_DELETE( m_pSort );
}

//-----------------------------------------------------------------------------
/**
 * @func    CopySortData
 * @brief   ソートデータを退避
 * @date    2017.02.24
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::CopySortData(void)
{
  gfl2::std::MemCopy( &m_appParam->sort_data, &m_copySortData, sizeof(SORT_DATA) );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsSetSortData
 * @brief   ソートデータが設定されているか
 * @date    2017.02.24
 *
 * @retval  true  = 設定されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::IsSetSortData(void)
{
  if( m_appParam->sort_data.mons_no == 0 &&
      m_appParam->sort_data.bg_no == 0 &&
      m_appParam->sort_data.low == 0 &&
      m_appParam->sort_data.deco == 0 &&
      m_appParam->sort_data.favorite == 0 &&
      m_appParam->sort_data.photo == 0 &&
      m_appParam->sort_data.share == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsChangeSortData
 * @brief   ソートデータが変更されたか
 * @date    2017.02.24
 *
 * @retval  true  = 変更された
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
/*
bool MyAlbumFrame::IsChangeSortData(void)
{
  if( m_appParam->sort_data.mons_no == m_copySortData.mons_no &&
      m_appParam->sort_data.bg_no == m_copySortData.bg_no &&
      m_appParam->sort_data.low == m_copySortData.low &&
      m_appParam->sort_data.deco == m_copySortData.deco &&
      m_appParam->sort_data.favorite == m_copySortData.favorite &&
      m_appParam->sort_data.photo == m_copySortData.photo &&
      m_appParam->sort_data.share == m_copySortData.share )
  {
    return false;
  }
  return true;
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    IsUpperViewerOn
 * @brief   上画面の写真ビューワを表示できるか
 * @date    2017.02.23
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MyAlbumFrame::IsUpperViewerOn(void)
{
  MyAlbumDraw::DispMode mode = m_pDraw->GetDispMode();

  if( m_pInfoWinBook->IsOpen() == false &&            // インフォウィンドウ非表示
      mode != MyAlbumDraw::DISP_MODE_SEARCH_TYPE &&   // 非検索トップ画面
      mode != MyAlbumDraw::DISP_MODE_SEARCH_CORE )    // 非検索条件画面
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpperViewerOn
 * @brief   上画面の写真ビューワを表示
 * @date    2017.02.23
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::UpperViewerOn(void)
{
  if( m_viewerWork.req == VIEWER_REQUEST_NONE && m_pPhotoData->GetDataCount() != 0 )
  {
    SetVisibleViewer( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectEdit
 * @brief   "編集する"選択処理
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::SelectEdit(void)
{
  u32 pos = m_pDraw->GetCursorPosIndex();

  if( pos >= m_pPhotoData->GetDataCount() )
  {
    GFL_ASSERT( 0 );
    StartMessage( fs_album_cap_53 );
  }
  else if( m_pPhotoData->IsDeco(pos) == false )
  {
    // @fix MMCat442]: 容量オーバーの場合はデコに行けないようにする
    MyAlbumPhotoData::SDCardAccessResult result = m_pPhotoData->CheckImageDBError();
    if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_FATAL )
    {
      ChangeEndSequence( END_MODE_FATAL_ERR );
    }
    else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_LACK_OF_SPACE )
    {
      StartMessage( fs_album_win_02 );
    }
    else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_OVER_IMAGE )
    {
      StartMessage( fs_album_win_03 );
    }
    else if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_ETC )
    {
      StartMessage( fs_album_win_04 );
    }
    else
    {
      m_pDraw->SetInputEnabled( false );
      m_appParam->select_pos = pos;
      gfl2::std::MemCopy( m_pPhotoData->GetData(m_appParam->select_pos), &m_appParam->photo_data, sizeof(App::FinderStudio::Static::IntegratedData) );
      m_appParam->select_index = m_pPhotoData->GetIndex( m_appParam->select_pos );
      m_appParam->photo_count  = m_pPhotoData->GetDataCountOrigin();
      CreateRecoverData();
      ChangeEndSequence( END_MODE_DECO );
    }
  }
  else
  {
    StartMessage( fs_album_cap_53 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力時のリスナー
 * @date    2017.01.25
 *
 * @param   id    動作ID
 *
 * @note  override MyAlbumDraw::Listener
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::OnAction( MyAlbumDraw::Listener::ActionID id )
{
  switch( id )
  {
  case MyAlbumDraw::Listener::ACTION_ID_RETURN:   // 戻る
    m_pDraw->SetInputEnabled( false );
    if( IsSave() != false )
    {
      ChangeSequence( SEQ_PHOTO_SAVE );
    }
    else
    {
      ChangeEndSequence( END_MODE_EXIT );
    }
    break;
  case MyAlbumDraw::Listener::ACTION_ID_SEARCH:   // 検索
    CopySortData();
    SetVisibleViewer( true );
    m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SEARCH_TYPE );
    m_pDraw->ResetTopList();
    SetListTopInputEnabled( true );
    break;
  case MyAlbumDraw::Listener::ACTION_ID_DELETE:   // 削除
    m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_DELETE );
    break;

  case MyAlbumDraw::Listener::ACTION_ID_PHOTO:    // 写真選択
    m_pDraw->SetInputEnabled( false );
    PutPhotoMenu();
    break;

  case MyAlbumDraw::Listener::ACTION_ID_FAVORITE: // お気に入り
    m_pDraw->SetPhotoFavoriteFlag();
    break;

  case MyAlbumDraw::Listener::ACTION_ID_SHARE:    // 共有
    // @fix MMCat[637]: 登録済みの場合はUGC表示をしない
    if( m_misc->IsMyAlbumUGCEnable() != false || m_pDraw->IsPhotoShare() != false )
    {
      m_pDraw->SetPhotoShareFlag();
    }
    else
    {
      PutShareUgcMenu( true );
    }
    break;

  case MyAlbumDraw::Listener::ACTION_ID_INFO:   // インフォボタン
    StartInfoWindow();
    break;

  case MyAlbumDraw::Listener::ACTION_ID_END_DELETE:   // 削除モード終了
    EndMultiDelete();
    m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SELECT );
    break;

  case MyAlbumDraw::Listener::ACTION_ID_SELECT_DELETE:  // 削除する写真を選択
    {
      u32 index = m_pDraw->GetCursorPosIndex();
      m_pDraw->SetDeletePhotoBit( index );
      m_pDraw->SetVisiblePhotoSelectMark( m_pDraw->GetCursorPos(), m_pDraw->IsDeletePhotoBit(index) );
    }
    break;

  case MyAlbumDraw::Listener::ACTION_ID_DECIDE_DELETE:  // 削除決定
    m_pDraw->SetInputEnabled( false );
    DecideDelete();
    break;

  case MyAlbumDraw::Listener::ACTION_ID_CURSOR_MOVE:  // カーソル移動
    {
      u32 pos = m_pDraw->GetCursorPosIndex();
      if( pos < m_pPhotoData->GetDataCount() )
      {
        SetViewerRequest( pos );
      }
      else
      {
        SetVisibleViewer( true );
      }
    }
    break;

  case MyAlbumDraw::Listener::ACTION_ID_SEARCH_TYPE_RESET:  // 検索：リセット
    gfl2::std::MemClear( &m_appParam->sort_data, sizeof(App::MyAlbum::SORT_DATA) );
    m_pDraw->ResetTopList();
    m_pListTop->GetPaneList()->PutCursor();
    break;
  case MyAlbumDraw::Listener::ACTION_ID_SEARCH_TYPE_CANCEL: // 検索：キャンセル
    gfl2::std::MemCopy( &m_copySortData, &m_appParam->sort_data, sizeof(SORT_DATA) );
    SetListTopInputEnabled( false );
    m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SELECT );
    UpperViewerOn();
    break;
  case MyAlbumDraw::Listener::ACTION_ID_SEARCH_TYPE_DECIDE: // 検索：実行
    SetListTopInputEnabled( false );
    m_appParam->is_sort_mode = IsSetSortData();
    {
      u32 old_index = MyAlbumPhotoData::PHOTO_DATA_NONE;
      {
        u32 pos = m_pDraw->GetCursorPosIndex();
        if( pos < m_pPhotoData->GetDataCount() )
        {
          old_index = m_pPhotoData->GetIndex( pos );
        }
      }
//      if( IsChangeSortData() != false )
      {
        m_pDraw->StartSort();
        SetViewerRequest( m_pDraw->GetCursorPosIndex() );
      }
      u32 new_index = MyAlbumPhotoData::PHOTO_DATA_NONE;
      {
        u32 pos = m_pDraw->GetCursorPosIndex();
        if( pos < m_pPhotoData->GetDataCount() )
        {
          new_index = m_pPhotoData->GetIndex( pos );
        }
      }
      if( old_index != MyAlbumPhotoData::PHOTO_DATA_NONE && new_index == old_index )
      {
        UpperViewerOn();
      }
    }
    m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SELECT );
    break;

  case MyAlbumDraw::Listener::ACTION_ID_SEARCH_CORE_CANCEL: // 検索条件：キャンセル
    {
      MyAlbumDraw::ListType type = m_pDraw->GetListType();
      if( type == MyAlbumDraw::LIST_TYPE_MONS_NAME )
      {
        if( System::GetLang() == POKEMON_LANG_JAPAN )
        {
          m_pDraw->ResetCoreList( MyAlbumDraw::LIST_TYPE_VOWEL, m_pDraw->GetVowelListMax(), false );
          m_pListCore->GetPaneList()->PutCursor();
        }
        else
        {
          m_pDraw->ResetCoreList( MyAlbumDraw::LIST_TYPE_CONSONANT, m_pDraw->GetLangListMax(), false );
          m_pListCore->GetPaneList()->PutCursor();
        }
      }
      else if( type == MyAlbumDraw::LIST_TYPE_VOWEL )
      {
        m_pDraw->ResetCoreList( MyAlbumDraw::LIST_TYPE_CONSONANT, m_pDraw->GetLangListMax(), false );
        m_pListCore->GetPaneList()->PutCursor();
      }
      else
      {
        ReturnTopList( true );
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnEvent
 * @brief   メッセージ表示リスナー
 * @date    2017.01.27
 *
 * @param   event   イベントコード
 *
 * @note  override App::FinderStudio::Static::MessageMenuViewListener
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    m_isEndMessage = true;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnSelectMenu
 * @brief   メニュー選択時のリスナー
 * @date    2017.01.27
 *
 * @param   index   項目番号
 *
 * @note  override App::FinderStudio::Static::MessageMenuViewListener
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::OnSelectMenu( const u32 index )
{
  switch( m_menuItemID[index] )
  {
  // 写真メニュー
  case MENU_PHOTO_EDIT:     // 編集する
    SelectEdit();
    break;

  case MENU_PHOTO_DELETE:   // 捨てる
    m_pDraw->SetDeletePhotoBit( m_pDraw->GetCursorPosIndex() );
    if( DecideDelete() == false )
    {
      m_pDraw->SetDeletePhotoBit( m_pDraw->GetCursorPosIndex() );   // 消せない場合は非選択にする
    }
    break;

  case MENU_PHOTO_FAVORITE: // お気に入り
    StartMessage( m_pDraw->SetPhotoFavoriteFlag() );
    break;

  case MENU_PHOTO_SHARE:    // 共有
    if( m_pDraw->IsPhotoShare() == false && m_pDraw->IsPhotoShareOther() != false )
    {
      PutShareMenu();
    }
    // @fix MMCat[637]: 登録済みの場合はUGC表示をしない
    else if( m_misc->IsMyAlbumUGCEnable() == false && m_pDraw->IsPhotoShare() == false )
    {
      PutShareUgcMenu( false );
    }
    else
    {
      StartMessage( m_pDraw->SetPhotoShareFlag() );
    }
    break;

  case MENU_PHOTO_CANCEL:   // やめる
    m_pDraw->SetInputEnabled( true );
    ChangeSequence( SEQ_MAIN );
    break;

  // 削除"はい・いいえ"
  case MENU_DELETE_YES:   // はい
    m_mainSeq = 1;
    break;

  case MENU_DELETE_NO:   // いいえ
    if( m_pDraw->GetDispMode() == MyAlbumDraw::DISP_MODE_SELECT )
    {
      m_pDraw->SetDeletePhotoBit( m_pDraw->GetCursorPosIndex() );
    }
    m_pDraw->SetInputEnabled( true );
    ChangeSequence( SEQ_MAIN );
    break;

  // 共有"はい・いいえ"
  case MENU_SHARE_YES:  // はい
    if( m_misc->IsMyAlbumUGCEnable() != false )
    {
      StartMessage( m_pDraw->SetPhotoShareFlag() );
    }
    else
    {
      PutShareUgcMenu( false );
    }
    break;

  case MENU_SHARE_NO:   // いいえ
    m_pDraw->SetInputEnabled( true );
    ChangeSequence( SEQ_MAIN );
    break;

  // 共有UGC"はい・いいえ"
  case MENU_SHARE_UGC_YES:  // はい
    if( m_isUgcButton == false )
    {
      StartMessage( m_pDraw->SetPhotoShareFlag() );
    }
    else
    {
      m_pDraw->SetPhotoShareFlag();
      m_pDraw->SetInputEnabled( true );
      ChangeSequence( SEQ_MAIN );
    }
    break;

  case MENU_SHARE_UGC_NO:   // いいえ
    m_pDraw->SetInputEnabled( true );
    ChangeSequence( SEQ_MAIN );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnClose
 * @brief   インフォウィンドウを閉じたときのリスナー
 * @date    2017.02.17
 *
 * @note  override app::tool::InfoWindowBookTypeListener
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::OnClose(void)
{
  m_pDraw->SetInputEnabled( true );
  UpperViewerOn();
}

//-----------------------------------------------------------------------------
/**
 * @func    OnChangePage
 * @brief   インフォウィンドウのページ切り替え時のリスナー
 * @date    2017.02.17
 *
 * @param   pageNo  これから表示するページ番号
 *
 * @note  override app::tool::InfoWindowBookTypeListener
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::OnChangePage( u32 pageNo )
{
  static const u32 tbl[] =
  {
    fs_album_info_01,
    fs_album_info_02,
  };
  m_pDraw->GetMsgData()->GetString( tbl[pageNo], *m_pInfoWinStrBuf );
  m_pInfoWinBook->SetTextContent( pageNo, m_pInfoWinStrBuf, NULL );
}

//-----------------------------------------------------------------------------
/**
 * @func    PaneListItemSelectListener
 * @brief   ペインリスト決定時に呼ばれるリスナー
 * @date    2017.02.20
 *
 * @param   pos   カーソル位置
 *
 * @note  override app::tool::PaneListView::Listener
 */
//-----------------------------------------------------------------------------
void MyAlbumFrame::PaneListItemSelectListener( u32 pos )
{
  switch( m_pDraw->GetListType() )
  {
  case MyAlbumDraw::LIST_TYPE_TOP:          // トップ
    {
      static const MyAlbumDraw::ListType type_tbl[] =
      {
        MyAlbumDraw::LIST_TYPE_LOW,         // 並び
        MyAlbumDraw::LIST_TYPE_CONSONANT,   // ポケモン：子音
        MyAlbumDraw::LIST_TYPE_FAVORITE,    // お気に入り
        MyAlbumDraw::LIST_TYPE_DECO,        // 飾りつけ
        MyAlbumDraw::LIST_TYPE_PHOTO,       // 撮り方
        MyAlbumDraw::LIST_TYPE_BG,          // 背景
        MyAlbumDraw::LIST_TYPE_SHARE,       // 共有
      };
      static const u32 max_tbl[] =
      {
        MyAlbumDraw::CORE_LIST_LOW_MAX,       // 並び
        MyAlbumDraw::CORE_LIST_CONSONANT_MAX, // ポケモン：子音
        MyAlbumDraw::CORE_LIST_FAVORITE_MAX,  // お気に入り
        MyAlbumDraw::CORE_LIST_DECO_MAX,      // 飾りつけ
        MyAlbumDraw::CORE_LIST_PHOTO_MAX,     // 撮り方
        MyAlbumDraw::CORE_LIST_BG_MAX,        // 背景
        MyAlbumDraw::CORE_LIST_SHARE_MAX,     // 共有
      };
      if( pos >= GFL_NELEMS(type_tbl) )
      {
        GFL_ASSERT( 0 );
        return;
      }
      m_pDraw->ChangeDispMode( MyAlbumDraw::DISP_MODE_SEARCH_CORE );
      u32 max = max_tbl[pos];
      if( type_tbl[pos] == MyAlbumDraw::LIST_TYPE_CONSONANT )
      {
        max = m_pDraw->GetLangListMax();
      }
      m_pDraw->ResetCoreList( type_tbl[pos], max, false );
    }
    m_pListCore->GetPaneList()->PutCursor();
    SetListTopInputEnabled( false );
    SetListCoreInputEnabled( true );
    break;

  case MyAlbumDraw::LIST_TYPE_LOW:         // 並び
    m_appParam->sort_data.low = pos;
    ReturnTopList( false );
    break;
  case MyAlbumDraw::LIST_TYPE_CONSONANT:   // ポケモン：子音
    if( pos == 0 )
    {
      m_appParam->sort_data.mons_no = 0;
      ReturnTopList( false );
    }
    else
    {
      if( System::GetLang() == POKEMON_LANG_JAPAN )
      {
        m_pDraw->SetSelectVowelData( pos-1 );
        m_pDraw->ResetCoreList( MyAlbumDraw::LIST_TYPE_VOWEL, m_pDraw->GetVowelListMax(), true );
        m_pListCore->GetPaneList()->PutCursor();
      }
      else
      {
        u32 count = m_pDraw->CreatePokemonInitialData( pos );
        if( count != 0 )
        {
          m_pDraw->ResetCoreList( MyAlbumDraw::LIST_TYPE_MONS_NAME, count, true );
          m_pListCore->GetPaneList()->PutCursor();
        }
      }
    }
    break;
  case MyAlbumDraw::LIST_TYPE_VOWEL:      // ポケモン：母音
    {
      u32 count = m_pDraw->CreatePokemonInitialData( pos );
      if( count != 0 )
      {
        m_pDraw->ResetCoreList( MyAlbumDraw::LIST_TYPE_MONS_NAME, count, true );
        m_pListCore->GetPaneList()->PutCursor();
      }
    }
    break;
  case MyAlbumDraw::LIST_TYPE_MONS_NAME:   // ポケモン名
    m_appParam->sort_data.mons_no = m_pDraw->GetPokemonInitialData(pos);
    ReturnTopList( false );
    break;
  case MyAlbumDraw::LIST_TYPE_FAVORITE:    // お気に入り
    m_appParam->sort_data.favorite = pos;
    ReturnTopList( false );
    break;
  case MyAlbumDraw::LIST_TYPE_DECO:        // 飾りつけ
    m_appParam->sort_data.deco = pos;
    ReturnTopList( false );
    break;
  case MyAlbumDraw::LIST_TYPE_PHOTO:       // 撮り方
    m_appParam->sort_data.photo = pos;
    ReturnTopList( false );
    break;
  case MyAlbumDraw::LIST_TYPE_BG:          // 背景
    m_appParam->sort_data.bg_no = pos;
    ReturnTopList( false );
    break;
  case MyAlbumDraw::LIST_TYPE_SHARE:       // 共有
    m_appParam->sort_data.share = pos;
    ReturnTopList( false );
    break;
  }
}



GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)
