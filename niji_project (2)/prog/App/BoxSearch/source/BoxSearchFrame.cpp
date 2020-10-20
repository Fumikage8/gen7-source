//======================================================================
/**
 * @file    BoxSearchFrame.cpp
 * @date    2015/07/27 12:58:52
 * @author  fukushima_yuya
 * @brief   ボックス検索：フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchFrame.h"
#include "BoxSearchDrawerBase.h"
#include "BoxSearchBGDraw.h"
#include "BoxSearchUpperDraw.h"
#include "BoxSearchLowerDraw.h"

#include "App/BoxSearch/source/BoxSearchConst.h"
#include "App/BoxSearch/source/parts/BoxSearchUpperMessage.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSetLoader.h>
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

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
BoxSearchFrame::BoxSearchFrame( void )
  : m_pAppHeap(NULL)
  , m_pAppRenderingManager(NULL)
  , m_ArcCount(0)
  , m_LoadArcSeq(0)
  , m_MsgCount(0)
  , m_LoadMsgSeq(0)
  , m_pWordSet(NULL)
  , m_pWordSetLoader(NULL)
  , m_WordSetSeq(0)
  , m_CursorCount(0)
  , m_CursorSeq(0)
  , m_pAppParam(NULL)
  , m_pUpperDraw(NULL)
  , m_pLowerDraw(NULL)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result BoxSearchFrame::EndFunc( void )
{
  if( m_pLowerBGDraw )
  {
    if( !m_pLowerBGDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pLowerBGDraw->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pLowerBGDraw );
  }
  if( m_pUpperBGDraw )
  {
    if( !m_pUpperBGDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pUpperBGDraw->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pUpperBGDraw );
  }
  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pUpperDraw->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pUpperDraw );
  }
  if( m_pLowerDraw )
  {
    if( !m_pLowerDraw->End() ) return applib::frame::RES_CONTINUE;

    GFL_SAFE_DELETE( m_pLowerDraw );
  }

  for( u32 i=0; i<CUR_NUM; ++i )
  {
    if( m_pMenuCursor[i] )
    {
      if( !m_pMenuCursor[i]->DeleteMenuCursor() ) return applib::frame::RES_CONTINUE;

      GFL_SAFE_DELETE( m_pMenuCursor[i] );
    }
  }

  for( u32 i=0; i<ARC_NUM; ++i )
  {
    GflHeapFreeMemory( m_pArcReadBuff[i] );
  }

  GFL_SAFE_DELETE( m_pWordSetLoader );
  GFL_SAFE_DELETE( m_pWordSet );

  for( u32 i=0; i<MSG_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pMsgData[i] );
    GflHeapFreeMemory( m_pMsgReadBuff[i] );
  }

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result BoxSearchFrame::InitFunc( void )
{
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;
  // ワードセットローダーの生成
  if( !CreateWordSetLoader() ) return applib::frame::RES_CONTINUE;
  // メニューカーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;
  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  // 上画面の生成
  CreateUpperDraw();
  // 下画面の生成
  CreateLowerDraw();
  // 上画面背景の生成
  CreateUpperBGDraw();
  // 下画面背景の生成
  CreateLowerBGDraw();

  GFL_ASSERT( m_pParentView );
  {
    m_pParentView->AddSubView( m_pUpperDraw );
    m_pParentView->AddSubView( m_pUpperBGDraw );
    m_pParentView->AddSubView( m_pLowerBGDraw );
  }

  //GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );  // @fix GFNMCat[1491]
  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  上画面の生成
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::CreateUpperDraw( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();

  BoxSearchUpperDraw::InitParam initParam;
  {
    initParam.appParam        = m_pAppParam;
    initParam.lytBuff         = m_pArcReadBuff[ARC_UPPER];
    initParam.msgData_Main    = m_pMsgData[MSG_BOX_SEARCH];
    initParam.msgData_Tokusei = m_pMsgData[MSG_TOKUSEI_INFO];
    initParam.wordSet         = m_pWordSet;
  }

  m_pUpperDraw = GFL_NEW(heap) BoxSearchUpperDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pUpperDraw );

    m_pUpperDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pUpperDraw->Init( initParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  下画面の生成
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::CreateLowerDraw( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();

  BoxSearchLowerDraw::InitParam initParam;
  {
    initParam.appParam        = m_pAppParam;
    initParam.lytBuff         = m_pArcReadBuff[ARC_LOWER];
    initParam.msgData_Main    = m_pMsgData[MSG_BOX_SEARCH];
    initParam.msgData_Initial = m_pMsgData[MSG_INITIAL];
    initParam.wordSet         = m_pWordSet;
    initParam.cursor_item     = m_pMenuCursor[CUR_ITEM_LIST];
    initParam.cursor_cont     = m_pMenuCursor[CUR_CONT_LIST];
  }

  m_pLowerDraw = GFL_NEW(heap) BoxSearchLowerDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pLowerDraw );

    m_pLowerDraw->SetTitleViewUIListener( this );
    m_pLowerDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pLowerDraw->Init( initParam );
  }

  m_pParentView = m_pLowerDraw;
}

//------------------------------------------------------------------------------
/**
 * @brief	  上画面背景の生成
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::CreateUpperBGDraw( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();

  BoxSearchBGDraw::InitParam param;
  {
    param.dispMode = BoxSearchBGDraw::DISP_MODE_UPPER;
    param.lytBuff  = m_pArcReadBuff[ARC_UPPER_BG];
  }

  m_pUpperBGDraw = GFL_NEW(heap) BoxSearchBGDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pUpperBGDraw );

    m_pUpperBGDraw->Init( param );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  下画面背景の生成
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::CreateLowerBGDraw( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();

  BoxSearchBGDraw::InitParam param;
  {
    param.dispMode = BoxSearchBGDraw::DISP_MODE_LOWER;
    param.lytBuff  = m_pArcReadBuff[ARC_LOWER_BG];
  }

  m_pLowerBGDraw = GFL_NEW(heap) BoxSearchBGDraw( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pLowerBGDraw );

    m_pLowerBGDraw->Init( param );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool BoxSearchFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN_START,
    LYT_SEQ_OPEN_END,
    LYT_SEQ_LOAD_START,
    LYT_SEQ_LOAD_END,
    LYT_SEQ_CLOSE_START,
    LYT_SEQ_CLOSE_END,
    LYT_SEQ_END_CHECK,
    LYT_SEQ_END,
  };

  static const u32 c_LytData[] = {
    GARC_boxLyt_srclow_applyt_COMP,
    GARC_boxLyt_bgrlow_applyt_COMP,
    GARC_boxLyt_srcupp_applyt_COMP,
    GARC_boxLyt_bgrupp_applyt_COMP,
  };

  u32 arcID  = ARCID_BOX;
  u32 dataID = c_LytData[m_ArcCount];

  switch( m_LoadArcSeq )
  {
  case LYT_SEQ_OPEN_START:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileOpen( arcID, heap );
      }
      m_LoadArcSeq = LYT_SEQ_OPEN_END;
    } // no break;

  case LYT_SEQ_OPEN_END:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      m_LoadArcSeq = LYT_SEQ_LOAD_START;
    } // no break;

  case LYT_SEQ_LOAD_START:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileLoad(
          arcID,
          dataID,
          &m_pArcReadBuff[m_ArcCount],
          heap,
          true,
          nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
      }
      m_LoadArcSeq = LYT_SEQ_LOAD_END;
    } // no break;

  case LYT_SEQ_LOAD_END:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff[m_ArcCount] ) ) break;

      m_LoadArcSeq = LYT_SEQ_CLOSE_START;
    } // no break;

  case LYT_SEQ_CLOSE_START:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileClose( arcID, heap );
      }
      m_LoadArcSeq = LYT_SEQ_CLOSE_END;
    } // no break;

  case LYT_SEQ_CLOSE_END:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_LoadArcSeq = LYT_SEQ_END_CHECK;
    } // no break;

  case LYT_SEQ_END_CHECK:
    {
      m_ArcCount++;

      if( m_ArcCount < ARC_NUM )
      {
        m_LoadArcSeq = LYT_SEQ_OPEN_START;

        break;
      }
      else {
        m_LoadArcSeq = LYT_SEQ_END;
      }
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
 * @brief	  メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool BoxSearchFrame::LoadMsgData( void )
{
  enum {
    MSG_SEQ_OPEN_START,
    MSG_SEQ_OPEN_END,
    MSG_SEQ_LOAD_START,
    MSG_SEQ_LOAD_END,
    MSG_SEQ_CLOSE_START,
    MSG_SEQ_CLOSE_END,
    MSG_SEQ_CREATE,
    MSG_SEQ_END_CHECK,
    MSG_SEQ_END,
  };

  static const u32 c_MsgDataID[] = {
    GARC_message_box_DAT,
    GARC_message_tokuseiinfo_DAT,
    GARC_message_initial_DAT,
  };

  u32 arcID   = print::GetMessageArcId();   //!< ARC_ID
  u32 dataID  = c_MsgDataID[m_MsgCount];    //!< DATA_ID

  switch( m_LoadMsgSeq )
  {
  case MSG_SEQ_OPEN_START:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileOpen( arcID, heap );
      }
      m_LoadMsgSeq = MSG_SEQ_OPEN_END;
    } // no break;

  case MSG_SEQ_OPEN_END:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      m_LoadMsgSeq = MSG_SEQ_LOAD_START;
    } // no break;

  case MSG_SEQ_LOAD_START:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileLoad(
          arcID,
          dataID,
          &m_pMsgReadBuff[m_MsgCount],
          heap,
          false,
          4 );
      }

      m_LoadMsgSeq = MSG_SEQ_LOAD_END;
    } // no break;

  case MSG_SEQ_LOAD_END:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pMsgReadBuff[m_MsgCount] ) ) break;

      m_LoadMsgSeq = MSG_SEQ_CLOSE_START;
    } // no break;

  case MSG_SEQ_CLOSE_START:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        app::util::FileAccessor::FileClose( arcID, heap );
      }

      m_LoadMsgSeq = MSG_SEQ_CLOSE_END;
    } // no break;

  case MSG_SEQ_CLOSE_END:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_LoadMsgSeq = MSG_SEQ_CREATE;
    } // no break;

  case MSG_SEQ_CREATE:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        m_pMsgData[m_MsgCount] = GFL_NEW(heap) gfl2::str::MsgData( m_pMsgReadBuff[m_MsgCount], heap );
        {
          GFL_ASSERT(m_pMsgData[m_MsgCount]);
        }
      }

      m_LoadMsgSeq = MSG_SEQ_END_CHECK;
    } // no break;

  case MSG_SEQ_END_CHECK:
    {
      m_MsgCount++;

      if( m_MsgCount < MSG_NUM )
      {
        m_LoadMsgSeq = MSG_SEQ_OPEN_START;

        break;
      }
      else {
        m_LoadMsgSeq = MSG_SEQ_END;
      }
    } // no break;

  case MSG_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief	  ワードセットローダーの生成
 */
//------------------------------------------------------------------------------
bool BoxSearchFrame::CreateWordSetLoader( void )
{
  enum {
    WS_SEQ_CREATE,
    WS_SEQ_WAIT,
    WS_SEQ_END
  };

  switch( m_WordSetSeq )
  {
  case WS_SEQ_CREATE:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        // print::WordSet
        m_pWordSet = GFL_NEW(heap) print::WordSet( heap );
        {
          GFL_ASSERT( m_pWordSet );
        }

        // WordSetLoader
        u32 id = 0;
        {
          id = PRELOAD_WAZANAME | PRELOAD_ITEMNAME | PRELOAD_TOKUSEI | PRELOAD_TYPENAME | PRELOAD_SEIKAKU;

          m_pWordSetLoader = GFL_NEW(heap) WordSetLoader( heap, id );
          {
            GFL_ASSERT( m_pWordSetLoader );

            m_pWordSetLoader->StartLoad( heap );
          }
        }
      }

      m_WordSetSeq = WS_SEQ_WAIT;
    } // no break;

  case WS_SEQ_WAIT:
    {
      if( !m_pWordSetLoader->WaitLoad() ) break;

      m_WordSetSeq = WS_SEQ_END;
    } // no break;

  case WS_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの生成
 */
//------------------------------------------------------------------------------
bool BoxSearchFrame::CreateMenuCursor( void )
{
  enum {
    CURSOR_INIT,
    CURSOR_CREATE,
    CURSOR_END_CHECK,
    CURSOR_END,
  };

  switch( m_CursorSeq )
  {
  case CURSOR_INIT:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
      {
        m_pMenuCursor[m_CursorCount] = GFL_NEW(heap) MenuCursor( m_pAppHeap, m_pAppRenderingManager );
        {
          GFL_ASSERT( m_pMenuCursor[m_CursorCount] );
        }
      }
      m_CursorSeq = CURSOR_CREATE;
    } // no break;

  case CURSOR_CREATE:
    {
      if( !m_pMenuCursor[m_CursorCount]->CreateMenuCursor() ) break;

      m_CursorSeq = CURSOR_END_CHECK;
    } // no break;

  case CURSOR_END_CHECK:
    {
      m_CursorCount++;

      if( m_CursorCount < CUR_NUM )
      {
        m_CursorSeq = CURSOR_INIT;
        break;
      }
      else {
        m_CursorSeq = CURSOR_END;
      }
    } // no break;

  case CURSOR_END:
    {
      return true;
    } // no break;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result BoxSearchFrame::UpdateFunc( void )
{
  if( m_pParentView ) m_pParentView->UpdateTree();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
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
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pParentView ) m_pParentView->DrawTree( no );
}



//------------------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager )
{
  m_pAppHeap = pHeap;
  m_pAppRenderingManager = pRenderingManager;
  m_pAppParam = pParam;
}






//------------------------------------------------------------------------------
/**
 * @brief   言語選択画面の終了通知
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::EndBoxSearch( void )
{
  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   終了モードのセット
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetEndMode( EndMode endMode )
{
  m_pAppParam->endMode = endMode;
}



//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージの設定(検索項目リスト用)
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetUpperMessage_ItemList( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetMessage_ItemList();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージの設定(頭文字選択用)
 *
 * @param   listID    リストの種類
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetUpperMessage_Initial( u32 listID )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetMessage_Initial( listID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージの設定(ガイド用)
 *
 * @param   listID    リストの種類
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetUpperMessage_Guide( u32 listID )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetMessage_Guide( listID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージの設定(タイプ用)
 *
 * @param   listID  リストの種類
 * @param   type1   タイプ１
 * @param   type2   タイプ２
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetUpperMessage_Type( u32 listID, u8 type1, u8 type2 )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetMessage_Type( listID, type1, type2 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージの設定(特性用)
 *
 * @param   tokusei   特性No
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetUpperMessage_Tokusei( TokuseiNo tokusei )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetMessage_Tokusei( tokusei );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージの設定(性格用)
 *
 * @param   seikaku   性格ID
 */
//------------------------------------------------------------------------------
void BoxSearchFrame::SetUpperMessage_Seikaku( pml::pokepara::Seikaku seikaku )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetMessage_Seikaku( seikaku );
  }
}

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )
