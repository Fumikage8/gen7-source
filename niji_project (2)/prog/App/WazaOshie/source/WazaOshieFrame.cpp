//======================================================================
/**
 * @file WazaOshieFrame.cpp
 * @date 2015/07/01 14:59:18
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/WazaOshie/include/WazaOshieFrame.h>

#include "WazaOshieUpperDraw.h"
#include "WazaOshieLowerDraw.h"
#include "WazaOshieDrawerBase.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSetLoader.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_status.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

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
WazaOshieFrame::WazaOshieFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pAppParam( NULL )
  , m_pMsgReadBuff( NULL )
  , m_LytLoadSeq( 0 )
  , m_MsgLoadSeq( 0 )
  , m_WordSetSeq( 0 )
  , m_CursorSeq( 0 )
  , m_pWordSetLoader( NULL )
  , m_pWazaOshieUpperDraw( NULL )
  , m_pWazaOshieLowerDraw( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
WazaOshieFrame::~WazaOshieFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result WazaOshieFrame::InitFunc( void )
{
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // ワードセットローダーの生成
  if( !CreateWordSetLoader() ) return applib::frame::RES_CONTINUE;

  // メニューカーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // 下画面
  m_pWazaOshieLowerDraw = GFL_NEW(devHeap) WazaOshieLowerDraw( m_pAppHeap, m_pAppRenderingManager, m_pCursor );
  {
    GFL_ASSERT( m_pWazaOshieLowerDraw );

    // 初期化パラメータの設定
    WazaOshieLowerDraw::InitParam param;
    {
      param.appParam      = m_pAppParam;
      param.arcBuff       = m_pArcReadBuff;
      param.msgData       = m_pMsgData;
      param.wordSet       = m_pWordSet;
      param.fadeMan       = m_SetupParam.fadeMan;
      param.partyData     = m_SetupParam.partyData;
      param.cmnIcon       = m_SetupParam.cmnIcon;
      param.createNumFont = m_SetupParam.createNumFont;
    }

    m_pWazaOshieLowerDraw->SetTitleViewUIListener( this );
    m_pWazaOshieLowerDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pWazaOshieLowerDraw->Init( param );
  }

  if( m_pAppParam->upper_draw > 0 )
  {
    // 上画面
    m_pWazaOshieUpperDraw = GFL_NEW(devHeap) WazaOshieUpperDraw( m_pAppHeap, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pWazaOshieUpperDraw );

      m_pWazaOshieUpperDraw->Init( m_pAppParam, m_pArcReadBuff, m_SetupParam.fadeMan );
    }

    // サブビューへの追加
    m_pWazaOshieLowerDraw->AddSubView( m_pWazaOshieUpperDraw );
  }

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result WazaOshieFrame::UpdateFunc( void )
{
  if( m_pWazaOshieLowerDraw ) m_pWazaOshieLowerDraw->UpdateTree();

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
void WazaOshieFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pWazaOshieLowerDraw ) m_pWazaOshieLowerDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result WazaOshieFrame::EndFunc( void )
{
  if( m_pWazaOshieUpperDraw )
  {
    if( !m_pWazaOshieUpperDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pWazaOshieUpperDraw->RemoveFromSuperView();   // @fix GFNMCat[4752]

    GFL_SAFE_DELETE( m_pWazaOshieUpperDraw );
  }

  if( m_pWazaOshieLowerDraw )
  {
    if( !m_pWazaOshieLowerDraw->End() ) return applib::frame::RES_CONTINUE;
    
    GFL_SAFE_DELETE( m_pWazaOshieLowerDraw );
  }

  if( m_pCursor )
  {
    if( !m_pCursor->DeleteMenuCursor() ) { return applib::frame::RES_CONTINUE; }

    GFL_SAFE_DELETE( m_pCursor );
  }

  GflHeapFreeMemory( m_pArcReadBuff );

  GFL_SAFE_DELETE( m_pWordSetLoader );
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

  GflHeapFreeMemory( m_pMsgReadBuff );

  return applib::frame::RES_FINISH;
}



//------------------------------------------------------------------------------
/**
 * @brief   レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool WazaOshieFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_COMMON_STATUS;
  u32 dataID = GARC_common_status_common_status_applyt_COMP;

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
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_LytLoadSeq = LYT_SEQ_WAIT;
    } // no break;

  case LYT_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_CLOSE;
    } // no break;

  case LYT_SEQ_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_LytLoadSeq = LYT_SEQ_END;
    } // no break;

  case LYT_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool WazaOshieFrame::LoadMsgData( void )
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
  u32 dataID = GARC_message_btl_app_DAT;

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

      // Drawの生成

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
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   ワードセットローダーの生成
 */
//------------------------------------------------------------------------------
bool WazaOshieFrame::CreateWordSetLoader( void )
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

      if( m_pAppParam->mode == MODE_WAZA_OBOE )
      {
        id = PRELOAD_WAZANAME;
      }
      else {
        id = PRELOAD_WAZANAME | PRELOAD_ITEMNAME | PRELOAD_TOKUSEI;
      }

      m_pWordSetLoader = GFL_NEW(devHeap) WordSetLoader( devHeap, id );
      m_pWordSetLoader->StartLoad( devHeap );

      m_WordSetSeq++;
    } // no break;

  case WORDSET_WAIT:
    {
      if( !m_pWordSetLoader->WaitLoad() ) break;

      m_WordSetSeq = WORDSET_END;
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
bool WazaOshieFrame::CreateMenuCursor( void )
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
void WazaOshieFrame::Setup( APP_PARAM* appParam, SetupParam& param )
{
  m_pAppParam             = appParam;
  m_pAppHeap              = param.heap;
  m_pAppRenderingManager  = param.renderMan;
  m_SetupParam            = param;
}

//------------------------------------------------------------------------------
/**
 * @brief   技教え・技忘れ画面の終了通知
 */
//------------------------------------------------------------------------------
void WazaOshieFrame::EndWazaOshie( void )
{
  SetSubSeq( MM_SEQ_END );
}

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )
