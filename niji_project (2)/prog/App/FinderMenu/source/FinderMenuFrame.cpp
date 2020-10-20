//======================================================================
/**
 * @file    FinderMenuFrame.cpp
 * @date    2015/11/02 17:42:44
 * @author  fukushima_yuya
 * @brief   ファインダーメニューFrame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuFrame.h"
#include "FinderMenuDrawerBase.h"
#include "FinderMenuUpperDraw.h"
#include "FinderMenuPhotoSelectDraw.h"
#include "FinderMenuCommentProductDraw.h"
#include "FinderMenuCommentListDraw.h"
#include "FinderMenuSelectDraw.h"
#include "FinderMenuScoreDraw.h"
#include "FinderMenuBGDraw.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSet.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/finder_menu.gaix>
#include <arc_index/FinderGrpFont.gaix>
#include <arc_index/MenuWindow.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum{
    MM_SEQ_NONE,
    MM_SEQ_END
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuFrame::FinderMenuFrame( void )
  : m_pAppHeap(NULL)
  , m_pAppRenderingManager(NULL)
  , m_pMsgData(NULL)
  , m_pWordSet(NULL)
  , m_pCursor(NULL)
  , m_pAppParam(NULL)
  , m_pParentView(NULL)
  , m_pUpperDraw(NULL)
  , m_pPhotoSelectDraw(NULL)
  , m_pCommentProductDraw(NULL)
  , m_pCommentListDraw(NULL)
  , m_pSelectDraw(NULL)
  , m_pScoreDraw(NULL)
  , m_pBGDraw(NULL)
  , m_pGrpFontReadBuff(NULL)
  , m_pMsgReadBuff(NULL)
  , m_LytCount(0)
  , m_LytLoadSeq(0)
  , m_GrpFontLoadSeq(0)
  , m_MsgLoadSeq(0)
  , m_CursorSeq(0)
  , m_MsgWndSeq(0)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief    PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result FinderMenuFrame::InitFunc( void )
{
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // メニューカーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;

  // グラフィックフォントデータの読み込み
  if( !LoadGrpFont() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  
  void* finderMenuArcBuff = m_pArcReadBuff[ARC_FINDER_MENU];

  // 上画面
  m_pUpperDraw = GFL_NEW(devHeap) FinderMenuUpperDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pUpperDraw );

    m_pUpperDraw->SetTitleViewUIListener( this );
    m_pUpperDraw->Init( m_pAppParam, finderMenuArcBuff, m_pMsgData, m_pWordSet );

    m_pParentView = m_pUpperDraw;
  }

  // 写真選択画面
  {
    FinderMenuPhotoSelectDraw::CreateParam param;
    {
      param.heap        = m_pAppHeap;
      param.renderMan   = m_pAppRenderingManager;
      param.cursor      = m_pCursor;
    }

    m_pPhotoSelectDraw = GFL_NEW(devHeap) FinderMenuPhotoSelectDraw( param );
    {
      GFL_ASSERT( m_pPhotoSelectDraw );

      m_pPhotoSelectDraw->SetTitleViewUIListener( this );
      m_pPhotoSelectDraw->Init( m_pAppParam, finderMenuArcBuff, m_pMsgData, m_pWordSet );
    }
  }

  // 評価演出画面
  m_pCommentProductDraw = GFL_NEW(devHeap) FinderMenuCommentProductDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pCommentProductDraw );

    m_pCommentProductDraw->SetTitleViewUIListener( this );
    m_pCommentProductDraw->Init( m_pAppParam, finderMenuArcBuff, m_pMsgData, m_pWordSet );
  }

  // 評価リスト画面
  m_pCommentListDraw = GFL_NEW(devHeap) FinderMenuCommentListDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pCommentListDraw );

    m_pCommentListDraw->SetTitleViewUIListener( this );
    m_pCommentListDraw->Init( m_pAppParam, finderMenuArcBuff, m_pMsgData, m_pWordSet );
  }

  // 得点表示
  {
    FinderMenuScoreDraw::CreateParam param;
    {
      param.heap        = m_pAppHeap;
      param.renderMan   = m_pAppRenderingManager;
    }

    m_pScoreDraw = GFL_NEW(devHeap) FinderMenuScoreDraw( param );
    {
      GFL_ASSERT( m_pScoreDraw );

      m_pScoreDraw->SetTitleViewUIListener( this );
      m_pScoreDraw->Init( m_pAppParam, finderMenuArcBuff, m_pGrpFontReadBuff, m_pMsgData, m_pWordSet );
    }
  }

  // 選択画面
  {
    FinderMenuSelectDraw::CreateParam param;
    {
      param.heap        = m_pAppHeap;
      param.renderMan   = m_pAppRenderingManager;
      param.cursor      = m_pCursor;
    }

    m_pSelectDraw = GFL_NEW(devHeap) FinderMenuSelectDraw( param );
    {
      GFL_ASSERT( m_pSelectDraw );

      m_pSelectDraw->SetTitleViewUIListener( this );
      m_pSelectDraw->Init( m_pAppParam, finderMenuArcBuff, m_pMsgData, m_pWordSet );
    }
  }

  // 背景
  {
    FinderMenuBGDraw::CreateParam param;
    {
      param.heap      = m_pAppHeap;
      param.renderMan = m_pAppRenderingManager;
    }

    m_pBGDraw = GFL_NEW(devHeap) FinderMenuBGDraw( param );
    {
      GFL_ASSERT( m_pBGDraw );

      m_pBGDraw->Init( finderMenuArcBuff );
    }
  }

  //if( m_pParentView )   // @fix cov_ctr[11259]
  {
    m_pParentView->AddSubView( m_pBGDraw );
    m_pParentView->AddSubView( m_pPhotoSelectDraw );
    m_pParentView->AddSubView( m_pCommentProductDraw );
    m_pParentView->AddSubView( m_pCommentListDraw );
    m_pParentView->AddSubView( m_pScoreDraw );
    m_pParentView->AddSubView( m_pSelectDraw );
  }

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result FinderMenuFrame::EndFunc( void )
{
  if( m_pBGDraw )
  {
    if( !m_pBGDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pBGDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pBGDraw );
  }
  if( m_pSelectDraw )
  {
    if( !m_pSelectDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pSelectDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pSelectDraw );
  }
  if( m_pScoreDraw )
  {
    if( !m_pScoreDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pScoreDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pScoreDraw );
  }
  if( m_pCommentListDraw )
  {
    if( !m_pCommentListDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pCommentListDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pCommentListDraw );
  }
  if( m_pCommentProductDraw )
  {
    if( !m_pCommentProductDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pCommentProductDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pCommentProductDraw );
  }
  if( m_pPhotoSelectDraw )
  {
    if( !m_pPhotoSelectDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pPhotoSelectDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pPhotoSelectDraw );
  }
  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->End() ) { return applib::frame::RES_CONTINUE; }

    GFL_SAFE_DELETE( m_pUpperDraw );
  }
  if( m_pCursor )
  {
    if( !m_pCursor->DeleteMenuCursor() ) { return applib::frame::RES_CONTINUE; }

    GFL_SAFE_DELETE( m_pCursor );
  }

  for( u32 i=0; i<ARC_NUM; ++i )
  {
    GflHeapFreeMemory( m_pArcReadBuff[i] );
  }
  
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

  GflHeapFreeMemory( m_pMsgReadBuff );
  GflHeapFreeMemory( m_pGrpFontReadBuff );

  m_pParentView = NULL;

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result FinderMenuFrame::UpdateFunc( void )
{
  if( m_pUpperDraw ) m_pUpperDraw->UpdateTree();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      gfl2::Fade::FadeManager* fadeMan = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
      {
        if( fadeMan->IsEnd(gfl2::Fade::DISP_UPPER) && fadeMan->IsEnd(gfl2::Fade::DISP_LOWER) )
        {
          result = applib::frame::RES_FINISH;
        }
        else {
          result = applib::frame::RES_CONTINUE;
        }
      }
    } break;

  default:
    {
      result = applib::frame::RES_CONTINUE;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pUpperDraw ) m_pUpperDraw->DrawTree( no );
}



//------------------------------------------------------------------------------
/**
 * @brief    レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool FinderMenuFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };
  static const u32 c_ArcID[] = {
    ARCID_FINDER_MENU,
    ARCID_MENU_WINDOW,
  };
  static const u32 c_DataID[] = {
    GARC_finder_menu_finder_menu_applyt_COMP,
    GARC_MenuWindow_MenuWindow_applyt_COMP,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID  = c_ArcID[m_LytCount];
  u32 dataID = c_DataID[m_LytCount];

  switch( m_LytLoadSeq )
  {
  case LYT_SEQ_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_LOAD;
    } // no break;

  case LYT_SEQ_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff[m_LytCount],
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_LytLoadSeq = LYT_SEQ_WAIT;
    } // no break;

  case LYT_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff[m_LytCount] ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_CLOSE;
    } // no break;

  case LYT_SEQ_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_LytCount++;

      if( m_LytCount == ARC_NUM )
      {
        m_LytLoadSeq = LYT_SEQ_END;
      }
      else {
        m_LytLoadSeq = LYT_SEQ_OPEN;
      }
    } break;

  case LYT_SEQ_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   グラフィックフォントデータの読み込み
 */
//------------------------------------------------------------------------------
bool FinderMenuFrame::LoadGrpFont( void )
{
  enum {
    GRPFONT_SEQ_OPEN,
    GRPFONT_SEQ_LOAD,
    GRPFONT_SEQ_WAIT,
    GRPFONT_SEQ_CLOSE,
    GRPFONT_SEQ_END,
  };

  u32 arcID  = ARCID_FINDER_GRPFONT;

  switch( m_GrpFontLoadSeq )
  {
  case GRPFONT_SEQ_OPEN:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileOpen( arcID, heap->GetLowerHandle() );
      }
      m_GrpFontLoadSeq = GRPFONT_SEQ_LOAD;
    } // no break;

  case GRPFONT_SEQ_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileLoad(
          arcID,
          GARC_FinderGrpFont_FinderGrpFont_applyt_COMP,
          &m_pGrpFontReadBuff,
          heap,
          true,
          nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
      }

      m_GrpFontLoadSeq = GRPFONT_SEQ_WAIT;
    } // no break;

  case GRPFONT_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pGrpFontReadBuff ) ) break;

      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileClose( arcID, heap->GetLowerHandle() );
      }
      m_GrpFontLoadSeq = GRPFONT_SEQ_CLOSE;
    } // no break;

  case GRPFONT_SEQ_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_GrpFontLoadSeq = GRPFONT_SEQ_END;
    } // no break;

  case GRPFONT_SEQ_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool FinderMenuFrame::LoadMsgData( void )
{
  enum {
    MSG_LOAD_OPEN,
    MSG_LOAD_LOAD,
    MSG_LOAD_WAIT,
    MSG_LOAD_CLOSE,
    MSG_LOAD_CREATE,
    MSG_LOAD_END,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = print::GetMessageArcId();
  u32 dataID = GARC_message_pokefinder_DAT;

  switch( m_MsgLoadSeq )
  {
  case MSG_LOAD_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_MsgLoadSeq = MSG_LOAD_LOAD;
    } // no break;

  case MSG_LOAD_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pMsgReadBuff,
        devHeap,
        false,
        4 );

      m_MsgLoadSeq = MSG_LOAD_WAIT;
    } // no break;

  case MSG_LOAD_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pMsgReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_MsgLoadSeq = MSG_LOAD_CLOSE;
    } // no break;

  case MSG_LOAD_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;
      
      m_MsgLoadSeq = MSG_LOAD_CREATE;
    } // no break;
    
  case MSG_LOAD_CREATE:
    {
      m_pMsgData = GFL_NEW(devHeap) gfl2::str::MsgData( m_pMsgReadBuff, devHeap );
      GFL_ASSERT( m_pMsgData );
      
      m_pWordSet = GFL_NEW(devHeap) print::WordSet( devHeap );
      GFL_ASSERT( m_pWordSet );
      
      m_MsgLoadSeq = MSG_LOAD_END;
    } // no break;

  case MSG_LOAD_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの生成
 */
//------------------------------------------------------------------------------
bool FinderMenuFrame::CreateMenuCursor( void )
{
  enum {
    CURSOR_INIT,
    CURSOR_CREATE,
    CURSOR_END,
  };

  switch( m_CursorSeq )
  {
  case CURSOR_INIT:
    {
      gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

      m_pCursor = GFL_NEW(devHeap) MenuCursor( m_pAppHeap, m_pAppRenderingManager );
      GFL_ASSERT( m_pCursor );

      m_CursorSeq++;
    } break;

  case CURSOR_CREATE:
    {
      if( m_pCursor->CreateMenuCursor() )
      {
        m_CursorSeq++;
      }
    } break;

  case CURSOR_END:
    {
      return true;
    } // no break;
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief    動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::Setup( APP_PARAM* pAppParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager )
{
  m_pAppParam = pAppParam;
  m_pAppHeap  = pHeap;
  m_pAppRenderingManager = pRenderingManager;
}



//------------------------------------------------------------------------------
/**
 * @brief   ファインダーメニュー画面の終了通知
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::EndFinderMenu( void )
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   ファインダーを継続する設定
 *
 * @param   flag    継続フラグ
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::SetFinderContinue( bool flag )
{
  m_pAppParam->finder_continue = flag;
}


//------------------------------------------------------------------------------
/**
 * @brief   写真選択位置の設定
 * @param   index   選択位置
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::SetSelectPhoto( u32 index )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetSelectPhoto( index );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コメント演出を開く
 * @param   photoIdx    写真インデックス
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::OpenCommentProduct( u32 photoIdx )
{
  // 写真選択画面を閉じる
  if( m_pPhotoSelectDraw )
  {
    m_pPhotoSelectDraw->Close();
  }

  // 写真評価画面を開く
  if( m_pCommentProductDraw )
  {
    m_pCommentProductDraw->Open( photoIdx );
  }

  // 写真評価リスト画面のセットアップ
  if( m_pCommentListDraw )
  {
    m_pCommentListDraw->Setup( photoIdx );
  }

  // 得点画面のセットアップ
  if( m_pScoreDraw )
  {
    m_pScoreDraw->Setup( photoIdx );
  }

  // コマンド選択画面のセットアップ
  if( m_pSelectDraw )
  {
    m_pSelectDraw->SetupPhotoData( photoIdx );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コメントリストを開く
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::OpenCommentList( void )
{
  if( m_pCommentProductDraw )
  {
    m_pCommentProductDraw->Close();
  }

  if( m_pCommentListDraw )
  {
    m_pCommentListDraw->Open();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   得点表示を開く
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::OpenScore( void )
{
  if( m_pCommentListDraw )
  {
    m_pCommentListDraw->Close();
  }

  if( m_pScoreDraw )
  {
    m_pScoreDraw->Open();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コマンド選択画面を開く
 */
//------------------------------------------------------------------------------
void FinderMenuFrame::OpenCmdSelect( void )
{
  if( m_pScoreDraw )
  {
    m_pScoreDraw->Close();
  }

  if( m_pSelectDraw )
  {
    m_pSelectDraw->Open();
  }
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
