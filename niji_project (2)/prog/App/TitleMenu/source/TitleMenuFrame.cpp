//======================================================================
/**
 * @file    TitleMenuFrame.cpp
 * @date    2015/07/21 14:52:26
 * @author  fukushima_yuya
 * @brief   タイトルメニューFrame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "TitleMenuLowerDraw.h"
#include "TitleMenuUpperDraw.h"
#include "TitleMenuDrawerBase.h"

#include <App/TitleMenu/include/TitleMenuFrame.h>

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSetLoader.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/title_menu.gaix>
#include <arc_index/fieldwindow.gaix>
#include <arc_index/message.gaix>

// DynamicHeader
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>
#include <System/include/ArcIdGetter.h>

#define NIJI_TITLE_MENU_MSG_HEADER_FILE_PATH "arc_index/message.gaix"
#define NIJI_TITLE_MENU_LYT_HEADER_FILE_PATH "arc_def_index/arc_def.h"

#define NIJI_TITLE_MENU_MSG_TABLE_CAP_NUM 256
#define NIJI_TITLE_MENU_LYT_TABLE_CAP_NUM 2048

#include GFL_DYNAMIC_HEADER(NIJI_TITLE_MENU_MSG_HEADER_FILE_PATH)
#include GFL_DYNAMIC_HEADER(NIJI_TITLE_MENU_LYT_HEADER_FILE_PATH)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

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
TitleMenuFrame::TitleMenuFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pMsgReadBuff( NULL )
  , m_DynaLoadSeq( 0 )
  , m_LytCount( 0 )
  , m_LytLoadSeq( 0 )
  , m_MsgLoadSeq( 0 )
  , m_WordSetSeq( 0 )
  , m_CursorSeq( 0 )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pWordSetLoader( NULL )
  , m_pAppParam( NULL )
  , m_pUpperDraw( NULL )
  , m_pLowerDraw( NULL )
  , m_pCursor( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TitleMenuFrame::~TitleMenuFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief    PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result TitleMenuFrame::InitFunc( void )
{
  // ダイナミックヘッダーの読み込み
  if( !LoadDynamicHeader() ) return applib::frame::RES_CONTINUE;

  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // ワードセットローダーの生成
  if( !CreateWordSetLoader() ) return applib::frame::RES_CONTINUE;

  // メニューカーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  
  // Drawの生成
  m_pUpperDraw = GFL_NEW(devHeap) TitleMenuUpperDraw( m_pAppHeap, m_pAppRenderingManager, m_pCursor );
  m_pUpperDraw->Init( m_pAppParam, m_pArcReadBuff[ARC_TITLE_MENU], m_pArcReadBuff[ARC_FIELDWINDOW], m_pMsgData, m_pWordSet, m_pWordSetLoader );
  m_pUpperDraw->SetTitleViewUIListener( this );

  m_pLowerDraw = GFL_NEW(devHeap) TitleMenuLowerDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pLowerDraw->Init( m_pAppParam, m_pArcReadBuff[ARC_TITLE_MENU], m_pMsgData, m_pWordSet, m_pWordSetLoader );

  m_pUpperDraw->AddSubView( m_pLowerDraw );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result TitleMenuFrame::UpdateFunc( void )
{
  if( m_pUpperDraw ) m_pUpperDraw->UpdateTree();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      result = applib::frame::RES_FINISH;
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
void TitleMenuFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pUpperDraw ) m_pUpperDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result TitleMenuFrame::EndFunc( void )
{
  if( m_pLowerDraw )
  {
    if( !m_pLowerDraw->End() ) { return applib::frame::RES_CONTINUE; }

    m_pLowerDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pLowerDraw );
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

  for( u32 i=0; i<ARC_MAX; ++i )
  {
    GflHeapFreeMemory( m_pArcReadBuff[i] );
  }
  
  GFL_SAFE_DELETE( m_pWordSetLoader );
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

  GflHeapFreeMemory( m_pMsgReadBuff );

  GFL_UNLOAD_HEADER_HASH( NIJI_TITLE_MENU_MSG_HEADER_FILE_PATH );
  GFL_UNLOAD_HEADER_HASH( NIJI_TITLE_MENU_LYT_HEADER_FILE_PATH );

  return applib::frame::RES_FINISH;
}



//------------------------------------------------------------------
/**
 * @brief   ダイナミックヘッダーの読み込み
 */
//------------------------------------------------------------------
bool TitleMenuFrame::LoadDynamicHeader( void )
{
  enum {
    DYNA_HEADER_LOAD,
    DYNA_HEADER_END,
  };

  switch( m_DynaLoadSeq )
  {
  case DYNA_HEADER_LOAD:
    {
      GFL_LOAD_HEADER_HASH( NIJI_TITLE_MENU_MSG_HEADER_FILE_PATH, NIJI_TITLE_MENU_MSG_TABLE_CAP_NUM );
      GFL_LOAD_HEADER_HASH( NIJI_TITLE_MENU_LYT_HEADER_FILE_PATH, NIJI_TITLE_MENU_LYT_TABLE_CAP_NUM );
      m_DynaLoadSeq++;
    } // no break;

  case DYNA_HEADER_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool TitleMenuFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  static const u32 c_ArcIDList[] = {
#if PM_DEBUG
    NIJI_ARCID_DEFINE_IN_HEADER(ARCID_TITLE_MENU),
    NIJI_ARCID_DEFINE_IN_HEADER(ARCID_FIELDWINDOW),
#else
    NIJI_ARCID_NUM(ARCID_TITLE_MENU),
    NIJI_ARCID_NUM(ARCID_FIELDWINDOW),
#endif // PM_DEBUG
  };
  static const u32 c_DataIDList[] = {
    GARC_title_menu_title_menu_applyt_COMP,
    GARC_fieldwindow_fieldwindow_applyt_COMP,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = c_ArcIDList[m_LytCount];
  u32 dataID = c_DataIDList[m_LytCount];

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

      if( m_LytCount < ARC_MAX )
      {
        m_LytLoadSeq = LYT_SEQ_OPEN;
      }
      else {
        m_LytLoadSeq = LYT_SEQ_END;
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
 * @brief    メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool TitleMenuFrame::LoadMsgData( void )
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
  u32 dataID = GFL_VALUE_DEFINE_IN_HEADER(GARC_message_title_menu_DAT);

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
 * @brief    ワードセットローダーの生成
 */
//------------------------------------------------------------------------------
bool TitleMenuFrame::CreateWordSetLoader( void )
{
  enum {
    WORDSET_LOAD,
    WORDSET_WAIT,
    WORDSET_END,
  };

  switch( m_WordSetSeq )
  {
  case WORDSET_LOAD:
    {
      gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
      u32 id = 0;

      id = PRELOAD_PLACENAME;

      m_pWordSetLoader = GFL_NEW(devHeap) WordSetLoader( devHeap, id );
      m_pWordSetLoader->StartLoad( devHeap );

      m_WordSetSeq++;
    } // no break;

  case WORDSET_WAIT:
    {
      if( !m_pWordSetLoader->WaitLoad() ) break;

      m_WordSetSeq++;
    } // no break;

  case WORDSET_END:
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
bool TitleMenuFrame::CreateMenuCursor( void )
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
void TitleMenuFrame::Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager )
{
  m_pAppHeap = pHeap;
  m_pAppRenderingManager = pRenderingManager;
  m_pAppParam = pParam;
}


//------------------------------------------------------------------------------
/**
 * @brief   タイトルメニュー画面の終了通知
 */
//------------------------------------------------------------------------------
void TitleMenuFrame::EndTitleMenu( void )
{
  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   終了モードのセット
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void TitleMenuFrame::SetEndMode( EndMode endMode )
{
  m_pAppParam->endMode = endMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータフォーマット開始を通知
 */
//------------------------------------------------------------------------------
void TitleMenuFrame::OnSavedataFormat( void )
{
  if( m_pLowerDraw )
  {
    m_pLowerDraw->VisibleSavedataFormat();
  }
}

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )
