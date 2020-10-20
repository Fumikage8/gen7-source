//======================================================================
/**
 * @file HeroSelectFrame.cpp
 * @date 2015/08/26 11:59:46
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "HeroSelectUpperDraw.h"
#include "HeroSelectLowerDraw.h"
#include "HeroSelectDrawerBase.h"

#include <App/HeroSelect/include/HeroSelectFrame.h>

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/PrintSystem.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/hero_select.gaix>
#include <arc_index/fieldwindow.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum{
    MM_SEQ_NONE,
    MM_SEQ_END
  };

  enum{
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
HeroSelectFrame::HeroSelectFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pMsgReadBuff( NULL )
  , m_LytCount( 0 )
  , m_LytLoadSeq( 0 )
  , m_MsgLoadSeq( 0 )
  , m_CursorSeq( 0 )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam( NULL )
  , m_pUpperDraw( NULL )
  , m_pLowerDraw( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
HeroSelectFrame::~HeroSelectFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result HeroSelectFrame::InitFunc( void )
{
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // メニューカーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  
  // Drawの生成
  m_pUpperDraw = GFL_NEW(devHeap) HeroSelectUpperDraw( m_pAppHeap, m_pAppRenderingManager, m_pCursor );
  m_pUpperDraw->Init( m_pAppParam, m_pArcReadBuff[ARC_HERO_SELECT], m_pArcReadBuff[ARC_FIELDWINDOW], m_pMsgData, m_pWordSet );
  m_pUpperDraw->SetTitleViewUIListener( this );

  m_pLowerDraw = GFL_NEW(devHeap) HeroSelectLowerDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pLowerDraw->Init( m_pArcReadBuff[ARC_HERO_SELECT], m_pMsgData, m_pWordSet );
  m_pUpperDraw->AddSubView( m_pLowerDraw ); //!< SubView登録

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result HeroSelectFrame::UpdateFunc( void )
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
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void HeroSelectFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pUpperDraw ) m_pUpperDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result HeroSelectFrame::EndFunc( void )
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

  GFL_SAFE_DELETE( m_pMsgData );
  GFL_SAFE_DELETE( m_pWordSet );

  for( u32 i=0; i<ARC_MAX; ++i )
  {
    GflHeapFreeMemory( m_pArcReadBuff[i] );
  }

  GflHeapFreeMemory( m_pMsgReadBuff );

  return applib::frame::RES_FINISH;
}



//------------------------------------------------------------------------------
/**
 * @brief   レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool HeroSelectFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  static const u32 c_ArcIDList[] = {
    ARCID_HERO_SELECT,
    ARCID_FIELDWINDOW,
  };
  static const u32 c_DataIDList[] = {
    GARC_hero_select_hero_select_applyt_COMP,
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
 * @brief   メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool HeroSelectFrame::LoadMsgData( void )
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
  u32 dataID = GARC_message_introdemo_DAT;

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
bool HeroSelectFrame::CreateMenuCursor( void )
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
 * @brief   動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void HeroSelectFrame::Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager )
{
  m_pAppParam = pParam;
  m_pAppHeap = pHeap;
  m_pAppRenderingManager = pRenderingManager;
}


//------------------------------------------------------------------------------
/**
 * @brief   画面の終了通知
 */
//------------------------------------------------------------------------------
void HeroSelectFrame::EndHeroSelect( void )
{
  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   画面のオープン
 */
//------------------------------------------------------------------------------
void HeroSelectFrame::OpenHeroSelect( void )
{
  m_pLowerDraw->Open();
}

//------------------------------------------------------------------------------
/**
 * @brief   画面のクローズ
 */
//------------------------------------------------------------------------------
void HeroSelectFrame::CloseHeroSelect( void )
{
  m_pLowerDraw->Close();
}

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )
