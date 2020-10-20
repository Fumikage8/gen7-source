//==============================================================================
/**
 * @file    FinderFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   ポケファインダーUIフレーム
 */
//==============================================================================

// niji
#include <App/Finder/include/FinderFrame.h>

#include "FinderDrawListener.h"
#include "FinderUpperDraw.h"
#include "FinderLowerDraw.h"
#include "FinderDrawerBase.h"
#include "FinderRotomFadeDraw.h"

#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/PokeFinderSave.h>

#include <System/include/GflUse.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/finder.gaix>
#include <arc_index/FinderGrpFont.gaix>
#include <arc_index/fieldwindow.gaix>
#include <arc_index/rotom_fade.gaix>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_pokefinder_camera.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
enum{
  MM_SEQ_NONE,
  MM_SEQ_IDLE,
  MM_SEQ_FADEIN,
  MM_SEQ_TUTORIAL,
  MM_SEQ_START,
  MM_SEQ_MAIN,
  MM_SEQ_END
};

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
 //------------------------------------------------------------------------------
FinderFrame::FinderFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pFinderSave( NULL )
  , m_pGrpFontReadBuff(NULL)
  , m_pMsgReadBuff( NULL )
  , m_LytCount( 0 )
  , m_LytLoadSeq( 0 )
  , m_GrpFontLoadSeq(0)
  , m_MsgLoadSeq( 0 )
  , m_CursorSeq( 0 )
  , m_TalkWndSeq( 0 )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam( NULL )
  , m_pUpperDraw( NULL )
  , m_pLowerDraw( NULL )
  , m_pRotomFade( NULL )
  , m_pCursor( NULL )
  , m_pTalkWindow( NULL )
  , m_Result( applib::frame::RES_CONTINUE )
  , m_pParentView( NULL )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void FinderFrame::Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager, bool debugFlg )
{
  {
    m_pAppParam            = pParam;
    m_pAppHeap             = pHeap;
    m_pAppRenderingManager = pRenderingManager;
  }

  GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  {
    GameSys::GameData* gd = gm->GetGameData();
    {
      // ポケファインダーセーブデータ
      m_pFinderSave = gd->GetPokeFinderSave();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   終了コール
 */
//------------------------------------------------------------------------------
void FinderFrame::EndFinder( void )
{
  m_Result = applib::frame::RES_FINISH;
}


//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result FinderFrame::InitFunc( void )
{
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // カーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;

  // グラフィックフォントデータの読み込み
  if( !LoadGrpFont() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  // 会話ウィンドウの生成
  if( !CreateTalkWindow() ) return applib::frame::RES_CONTINUE;

  // 上画面の生成
  m_pUpperDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) FinderUpperDraw( m_pAppHeap, m_pAppRenderingManager, m_pFinderSave );
  {
    GFL_ASSERT( m_pUpperDraw );

    m_pUpperDraw->SetUIListener( this );
    m_pUpperDraw->Init( m_pAppParam, m_pArcReadBuff[ARC_FINDER], m_pMsgData, m_pWordSet );
  }

  // 下画面の生成
  m_pLowerDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) FinderLowerDraw( m_pAppHeap, m_pAppRenderingManager, m_pFinderSave );
  {
    GFL_ASSERT( m_pLowerDraw );

    m_pLowerDraw->SetTitleViewUIListener( this );
    m_pLowerDraw->Init( m_pAppParam, m_pArcReadBuff[ARC_FINDER], m_pGrpFontReadBuff, m_pMsgData, m_pWordSet );

    m_pParentView = m_pLowerDraw;
  }

  // ロトムフェード
  m_pRotomFade = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) FinderRotomFade( m_pAppHeap, m_pAppRenderingManager );
  {
    GFL_ASSERT( m_pRotomFade );

    m_pRotomFade->Init( m_pAppParam, m_pArcReadBuff[ARC_ROTOM] );
  }

  //if( m_pParentView )   // @fix cov_ctr[11258]
  {
    m_pParentView->AddSubView( m_pUpperDraw );
    m_pParentView->AddSubView( m_pRotomFade );
    m_pParentView->AddSubView( m_pTalkWindow );
  }

  this->SetSubSeq( MM_SEQ_IDLE );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result FinderFrame::UpdateFunc( void )
{
  if( m_pParentView ) m_pParentView->UpdateTree();

  switch( this->GetSubSeq() )
  {
  case MM_SEQ_FADEIN:
    {
      // ロトムフェードアニメの終了待ち
      if( m_pAppParam->fade_skip || m_pRotomFade->IsEndRotomFade() )
      {
        if( m_pAppParam->tutorial_flg > 0 )
        {
          FinderStart( true );
          SetTutorialMsg();
          this->SetSubSeq( MM_SEQ_TUTORIAL );
        }
        else {
          this->SetSubSeq( MM_SEQ_START );
        }
      }
    } break;

  case MM_SEQ_TUTORIAL:
    {
      if( m_pTalkWindow->GetStatus() != App::Tool::TalkWindow::WIN_STATUS_WIN_EXIT )
      {
        break;
      }

      // 会話ウィンドウを閉じる
      m_pTalkWindow->CloseMsgWin();
      // チュートリアルフラグを更新
      m_pFinderSave->SetTutorialFlags( m_pAppParam->tutorial_flg, true );

      this->SetSubSeq( MM_SEQ_START );
    } break;

  case MM_SEQ_START:
    {
      // UI画面の入力を有効に
      FinderStart();

      // 外部入力有効を通知
      m_pAppParam->ext_input_disable = false;

      this->SetSubSeq( MM_SEQ_MAIN );
    } break;
  }

  return m_Result;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void FinderFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pParentView ) m_pParentView->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result FinderFrame::EndFunc( void )
{
  if( m_pTalkWindow )
  {
    m_pTalkWindow->RemoveFromSuperView();
    m_pTalkWindow->End();
    GFL_SAFE_DELETE( m_pTalkWindow );
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
  if( m_pRotomFade )
  {
    if( !m_pRotomFade->End() ) return applib::frame::RES_CONTINUE;

    GFL_SAFE_DELETE( m_pRotomFade );
  }
  if( m_pCursor )
  {
    if( !m_pCursor->DeleteMenuCursor() ) { return applib::frame::RES_CONTINUE; }

    GFL_SAFE_DELETE( m_pCursor );
  }

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
  GflHeapFreeMemory( m_pMsgReadBuff );
  GflHeapFreeMemory( m_pGrpFontReadBuff );

  for( u32 i=0; i<ARC_NUM; ++i )
  {
    GflHeapFreeMemory( m_pArcReadBuff[i] );
  }

  if( m_pAppParam )
  {
    m_pAppParam->fade_skip = false;
    m_pAppParam = NULL;
  }
  m_pParentView = NULL;

  return applib::frame::RES_FINISH;
}


//------------------------------------------------------------------------------
/**
 * @brief   レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool FinderFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  static const u32 c_ArcID[] = {
    ARCID_FINDER,
    ARCID_FIELDWINDOW,
    ARCID_ROTOM_FADE,
  };
  static const u32 c_DataID[] = {
    GARC_finder_finder_applyt_COMP,
    GARC_fieldwindow_fieldwindow_applyt_COMP,
    GARC_rotom_fade_rotom_fade_applyt_COMP,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID  = c_ArcID[m_LytCount];
  u32 dataID = c_DataID[m_LytCount];

  switch( m_LytLoadSeq )
  {
  case LYT_SEQ_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap->GetLowerHandle() );
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

      app::util::FileAccessor::FileClose( arcID, devHeap->GetLowerHandle() );
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
bool FinderFrame::LoadGrpFont( void )
{
  enum {
    GRPFONT_SEQ_OPEN,
    GRPFONT_SEQ_LOAD,
    GRPFONT_SEQ_WAIT,
    GRPFONT_SEQ_CLOSE,
    GRPFONT_SEQ_END,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID  = ARCID_FINDER_GRPFONT;
  u32 dataID = GARC_FinderGrpFont_FinderGrpFont_applyt_COMP;

  switch( m_GrpFontLoadSeq )
  {
  case GRPFONT_SEQ_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap->GetLowerHandle() );
      m_GrpFontLoadSeq = GRPFONT_SEQ_LOAD;
    } // no break;

  case GRPFONT_SEQ_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pGrpFontReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_GrpFontLoadSeq = GRPFONT_SEQ_WAIT;
    } // no break;

  case GRPFONT_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pGrpFontReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap->GetLowerHandle() );
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
 * @brief   メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool FinderFrame::LoadMsgData( void )
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
  u32 dataID = GARC_message_pokefinder_camera_DAT;

  switch( m_MsgLoadSeq )
  {
  case MSG_LOAD_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap->GetLowerHandle() );
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

      app::util::FileAccessor::FileClose( arcID, devHeap->GetLowerHandle() );
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
bool FinderFrame::CreateMenuCursor( void )
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
 * @brief   会話ウィンドウの生成
 */
//------------------------------------------------------------------------------
bool FinderFrame::CreateTalkWindow( void )
{
  enum {
    WINDOW_CREATE,
    WINDOW_END,
  };
  enum{
    FINDER_FRAME_MSG_BUFLEN = (18*16)*3  //18文字*16行 x3はmstxtバイナリ化時点でローカライズ対策向けの文字列長補正分
  };

  switch( m_TalkWndSeq )
  {
  case WINDOW_CREATE:
    {
      gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();

      m_pTalkWindow = GFL_NEW(heap) App::Tool::TalkWindow( m_pAppHeap, m_pAppRenderingManager );
      GFL_ASSERT( m_pTalkWindow );

      m_pTalkWindow->SetWindowType( App::Tool::WIN_TYPE_SYS_DOWN );
      m_pTalkWindow->Setup( m_pArcReadBuff[ARC_FLD_WND], FINDER_FRAME_MSG_BUFLEN );
      m_pTalkWindow->InitMsgData( m_pMsgData );
      m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );

      m_TalkWndSeq++;
    } break;

  case WINDOW_END:
    {
      return true;
    } // no break;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   チュートリアルメッセージのセット
 */
//------------------------------------------------------------------------------
void FinderFrame::SetTutorialMsg( void )
{
  u16 tutorial = m_pAppParam->tutorial_flg;
  u32 msgID    = 0;

  if( tutorial & Savedata::PokeFinderSave::FINDER_TUTORIAL_STARTUP )
  {
    msgID = msg_pokefinder_tutorial_01_01;
  }
  else if( tutorial & Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM1 )
  {
    msgID = msg_pokefinder_tutorial_04_01;
  }
  else if( tutorial & Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM2 )
  {
    msgID = msg_pokefinder_tutorial_05_01;
  }
  else if( tutorial & Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM3 )
  {
    msgID = msg_pokefinder_tutorial_06_01;
  }
  else if( tutorial & Savedata::PokeFinderSave::FINDER_TUTORIAL_CALL )
  {
    msgID = msg_pokefinder_tutorial_06_02;
  }

  if( m_pTalkWindow )
  {
    m_pTalkWindow->SetMessage( msgID );
    m_pTalkWindow->CallMsgWin();
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   終了モードの設定
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void FinderFrame::SetEndMode( EndMode endMode )
{
  m_pAppParam->endMode = endMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケファインダーUI画面の操作開始
 *
 * @param   tutorial    チュートリアルフラグ(trueならチュートリアル)
 */
//------------------------------------------------------------------------------
void FinderFrame::FinderStart( bool tutorial )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetInputEnabled( true );
  }

  if( m_pLowerDraw )
  {
    if( !tutorial )
    {
      m_pLowerDraw->SetInputEnabled( true );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケファインダーUI画面を閉じる
 */
//------------------------------------------------------------------------------
void FinderFrame::FinderClose( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->Close();
  }

  if( m_pLowerDraw )
  {
    m_pLowerDraw->Close();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シャッターアニメの開始
 */
//------------------------------------------------------------------------------
void FinderFrame::StartShutterAnim( void )
{
  m_pUpperDraw->StartCloseShutter();
  m_pAppParam->shutter_open = false;
}

//------------------------------------------------------------------------------
/**
* @brief   シャッターアニメの終了
*/
//------------------------------------------------------------------------------
void FinderFrame::EndShutterAnim( void )
{
  //m_pAppParam->update_shutter = false;

  // @note ここでは一旦、入力制御の解除をしない
  // @note ↑らしいがどこで解除するのか不明だったので解除してみる fukushima
  if( m_pLowerDraw )
  {
    m_pLowerDraw->SetInputEnabled( true );
  }

  m_pAppParam->shutter_open = false;
}
 
//------------------------------------------------------------------------------
/**
 * @brief   照準の変更
 */
//------------------------------------------------------------------------------
void FinderFrame::ChangeSight( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->ChangeSight();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ピントの変更
 * @param   mode    ピントのモード
 */
//------------------------------------------------------------------------------
void FinderFrame::SetPintMode( u8 mode )
{
  m_pAppParam->pint_type = mode;

  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetPintMode( mode );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   システムメッセージの表示
 * @param   msgID   メッセージID
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void FinderFrame::OpenSystemMessage( u32 msgID, EndMode endMode )
{
  if( m_pTalkWindow )
  {
    m_pTalkWindow->SetMessage( msgID );
    m_pTalkWindow->CallMsgWin();
  }

  if( m_pLowerDraw )
  {
    m_pLowerDraw->StartWaitMode( endMode );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ファインダーの終了
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void FinderFrame::FinishFinder( EndMode endMode )
{
  if( m_pTalkWindow )
  {
    if( m_pTalkWindow->GetStatus() != App::Tool::TalkWindow::WIN_STATUS_WIN_EXIT )
    {
      return;
    }
  }

  // 終了モードの設定
  m_pAppParam->endMode = endMode;

  if( m_pLowerDraw )
  {
    m_pLowerDraw->SetInputEnabled( false );   //!< 入力の無効
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   情報表示の切り替え
 */
//------------------------------------------------------------------------------
void FinderFrame::ChangeVisibleInfo( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->ChangeVisibleInfo();
  }
}





//
// ファインダーイベント向け公開関数
//

//------------------------------------------------------------------------------
/**
* @brief  開始準備が完了しているか取得する
*
* @return true 完了 false 完了していない
*/
//------------------------------------------------------------------------------
b32 FinderFrame::IsStartSetupEnd( void ) const
{
  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->IsOpened() ) return false;
  }
  else {
    return false;
  }

  if( m_pLowerDraw )
  {
    if( !m_pLowerDraw->IsOpened() ) return false;
  }
  else {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
* @brief   タッチパッド、ボタン、アナログキーの入力検出有効設定
*
* @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
*/
//------------------------------------------------------------------------------
void FinderFrame::SetInputEnabled( bool isEnable )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetInputEnabled( isEnable );
  }

  if( m_pLowerDraw )
  {
    m_pLowerDraw->SetInputEnabled( isEnable );
  }
}

//------------------------------------------------------------------------------
/**
* @brief   キャンセルボタンが押されたか取得
*
* @return true 押された false 押されていない
*/
//------------------------------------------------------------------------------
b32 FinderFrame::IsCanceled( void ) const
{
  return m_pAppParam->endMode == END_MODE_CANCEL;
}

//------------------------------------------------------------------------------
/**
* @brief   シャッター動作中か取得
*
* @return true 動作中 false 動作していない
*/
//------------------------------------------------------------------------------
b32 FinderFrame::IsShutteUpdate( void ) const
{
  //return m_pAppParam->update_shutter
  return true;
}

//------------------------------------------------------------------------------
/**
* @brief   ロトムフェードの開始
*/
//------------------------------------------------------------------------------
void FinderFrame::StartRotomFade( void )
{
  if( m_pRotomFade )
  {
    if( !m_pAppParam->fade_skip )
    {
      // ロトムフェードを再生
      m_pRotomFade->StartRotomFade();
      m_pRotomFade->SetVisible( true );
    }
    else {
      // ロトムフェードをスキップし非表示にする
      m_pRotomFade->SetVisible( false );
    }
  }

  this->SetSubSeq( MM_SEQ_FADEIN );
}

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )
