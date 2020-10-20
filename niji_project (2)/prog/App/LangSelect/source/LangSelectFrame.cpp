//======================================================================
/**
 * @file    LangSelectFrame.cpp
 * @date    2015/07/23 15:13:30
 * @author  fukushima_yuya
 * @brief   言語選択：Frame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/LangSelect/include/LangSelectFrame.h>

#include "LangSelectUpperDraw.h"
#include "LangSelectLowerDraw.h"
#include "LangSelectDrawerBase.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSetLoader.h>

#include <ExtSavedata/include/NijiExtSaveSystem.h>
#include <fs/include/CTR/gfl2_FsCtrExtSaveSystem.h>
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/lang_select.gaix>
#include <arc_index/message.gaix>
#include <arc_index/fieldwindow.gaix>

// DynamicHeader
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>
#include <System/include/ArcIdGetter.h>

#define NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH "arc_index/message.gaix"
#define NIJI_LANG_SELECT_LYT_HEADER_FILE_PATH "arc_def_index/arc_def.h"

#define NIJI_LANG_SELECT_MSG_TABLE_CAP_NUM 256
#define NIJI_LANG_SELECT_LYT_TABLE_CAP_NUM 2048

#include GFL_DYNAMIC_HEADER(NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH)
#include GFL_DYNAMIC_HEADER(NIJI_LANG_SELECT_LYT_HEADER_FILE_PATH)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum{
    MM_SEQ_NONE,
    MM_SEQ_READY,
    MM_SEQ_FORMAT,
    MM_SEQ_REPORT_BROKEN,
    MM_SEQ_REPORT_REPAIR,
    MM_SEQ_EX_SAVE_INIT,
    MM_SEQ_EX_SAVE_FORMAT,
    MM_SEQ_EX_SAVE_BROKEN,
    MM_SEQ_EX_SAVE_ERROR,
    MM_SEQ_LANGSELECT_INIT,
    MM_SEQ_LANGSELECT_MAIN,
    MM_SEQ_END
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
LangSelectFrame::LangSelectFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pArcReadBuff( NULL )
  , m_pMsgReadBuff( NULL )
  , m_pWinResBuff( NULL )
  , m_DynaLoadSeq( 0 )
  , m_LytLoadSeq( 0 )
  , m_MsgLoadSeq( 0 )
  , m_CursorSeq( 0 )
  , m_winResLoadSeq( 0 )
  , m_SaveSeq( 0 )
  , m_SaveDelete( false )
  , m_bInputDecide(false)
  , m_pAppParam( NULL )
  , m_pUpperDraw( NULL )
  , m_pLowerDraw( NULL )
  , m_pCursor( NULL )
  , m_pGameData( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData() )
  , m_pGameManager( GFL_SINGLETON_INSTANCE(GameSys::GameManager) )
  , m_pMsgHeap( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
LangSelectFrame::~LangSelectFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result LangSelectFrame::InitFunc( void )
{
  switch( GetSubSeq() )
  {
  case 0:
    {
      gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
    }
    CreateMessageHeap();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    // ダイナミックヘッダーの読み込み
    if( !LoadDynamicHeader() ) return applib::frame::RES_CONTINUE;

    // メッセージデータの読み込み
    if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

    // メニューカーソルの生成
    if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;

    // レイアウトデータの読み込み
    if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

    // TalkWindowデータの読み込み
    if( !LoadTalkWindowResource() ) return applib::frame::RES_CONTINUE;

    gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  
    // Drawの生成
    m_pUpperDraw = GFL_NEW(devHeap) LangSelectUpperDraw( m_pAppHeap, m_pAppRenderingManager, m_pCursor );
    m_pUpperDraw->Init( m_pAppParam, m_pArcReadBuff, m_pWinResBuff, m_pMsgData, m_pWordSet );
    m_pUpperDraw->SetTitleViewUIListener( this );

    m_pLowerDraw = GFL_NEW(devHeap) LangSelectLowerDraw( m_pAppHeap, m_pAppRenderingManager );
    m_pLowerDraw->Init( m_pAppParam, m_pArcReadBuff, m_pMsgData, m_pWordSet );

    m_pUpperDraw->AddSubView( m_pLowerDraw );
  }

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result LangSelectFrame::UpdateFunc( void )
{
  if( m_pUpperDraw ) m_pUpperDraw->UpdateTree();

  applib::frame::Result result = applib::frame::RES_CONTINUE;

  switch( GetSubSeq() )
  {
  case MM_SEQ_NONE:
    {
      SetSubSeq( MM_SEQ_READY );
    } 
  case MM_SEQ_READY:
    {
      if( !m_pUpperDraw->IsReady() ) break;
      if( !m_pLowerDraw->IsReady() ) break;

      // セーブ状態シーケンス
      Sequence_SaveStatus();
    } break;

  case MM_SEQ_FORMAT:
    {
      // フォーマットシーケンス
      Sequence_Format();
    } break;

  case MM_SEQ_REPORT_BROKEN:
    {
      // データ破損シーケンス
      Sequence_ReportBroken();
    } break;

  case MM_SEQ_REPORT_REPAIR:
    {
      // データ修復シーケンス
      Sequence_ReportRepair();
    } break;

  case MM_SEQ_EX_SAVE_INIT:
    {
      Sequence_ExsaveStatus();
    } break;

  case MM_SEQ_EX_SAVE_FORMAT:
    {
      Sequence_ExsaveFormat();
    } break;

  case MM_SEQ_EX_SAVE_BROKEN:
    {
      Sequence_ExsaveBroken();
    } break;

  case MM_SEQ_EX_SAVE_ERROR:
    {
      Sequence_ExsaveError();
    } break;

  case MM_SEQ_LANGSELECT_INIT:
    {
      m_pUpperDraw->StartLangSelect();
      SetDispMode( DISP_MODE_LANG_SELECT );
      SetSubSeq( MM_SEQ_LANGSELECT_MAIN );
    } break;

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
void LangSelectFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pUpperDraw ) m_pUpperDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result LangSelectFrame::EndFunc( void )
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

  GflHeapFreeMemory( m_pWinResBuff );

  GflHeapFreeMemory( m_pArcReadBuff );

  GflHeapFreeMemory( m_pMsgReadBuff );

  DeleteMessageHeap();

  GFL_UNLOAD_HEADER_HASH( NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH );
  GFL_UNLOAD_HEADER_HASH( NIJI_LANG_SELECT_LYT_HEADER_FILE_PATH );

  return applib::frame::RES_FINISH;
}



//------------------------------------------------------------------
/**
 * @brief   ダイナミックヘッダーの読み込み
 */
//------------------------------------------------------------------
bool LangSelectFrame::LoadDynamicHeader( void )
{
  enum {
    DYNA_HEADER_LOAD,
    DYNA_HEADER_END,
  };

  switch( m_DynaLoadSeq )
  {
  case DYNA_HEADER_LOAD:
    {
      GFL_LOAD_HEADER_HASH( NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH, NIJI_LANG_SELECT_MSG_TABLE_CAP_NUM );
      GFL_LOAD_HEADER_HASH( NIJI_LANG_SELECT_LYT_HEADER_FILE_PATH, NIJI_LANG_SELECT_LYT_TABLE_CAP_NUM );

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
 * @brief   レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool LangSelectFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
#if PM_DEBUG
  u32 arcID = NIJI_ARCID_DEFINE_IN_HEADER(ARCID_LANG_SELECT);
#else
  u32 arcID = NIJI_ARCID_NUM(ARCID_LANG_SELECT);
#endif // PM_DEBUG
  u32 dataID = GARC_lang_select_lang_select_applyt_COMP;

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
bool LangSelectFrame::LoadMsgData( void )
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
  u32 dataID = GFL_VALUE_DEFINE_IN_HEADER(GARC_message_language_select_DAT);

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
        m_pMsgHeap,
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
      m_pMsgData = GFL_NEW(m_pMsgHeap) gfl2::str::MsgData( m_pMsgReadBuff, m_pMsgHeap );
      GFL_ASSERT( m_pMsgData );
      
      m_pWordSet = GFL_NEW(m_pMsgHeap) print::WordSet( m_pMsgHeap );
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
bool LangSelectFrame::CreateMenuCursor( void )
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
void LangSelectFrame::Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager )
{
  m_pAppHeap = pHeap;
  m_pAppRenderingManager = pRenderingManager;
  m_pAppParam = pParam;
}


//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：初期化
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_SaveStatus( void )
{
  SAVE_STATUS status = m_pAppParam->saveStatus;

  // セーブデータの状態ごとに遷移する
  switch( status )
  {
  case SAVE_STATUS_NEED_FORMAT:
    {
      m_pLowerDraw->SetSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_FORMAT );
    } break;

  case SAVE_STATUS_ERROR:
    {
      m_pLowerDraw->SetSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_REPORT_BROKEN );
    } break;

  case SAVE_STATUS_REWIND_ERROR:
    {
      m_pLowerDraw->SetSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_REPORT_BROKEN );
    } break;

  case SAVE_STATUS_REWIND_WARNING:
    {
      m_pLowerDraw->SetSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_REPORT_REPAIR );
    } break;

  default:
    {
      SetSubSeq( MM_SEQ_EX_SAVE_INIT );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：フォーマット
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_Format( void )
{
  enum
  {
    SAVE_SEQ_FORMAT_START = 0,
    SAVE_SEQ_FORMAT_WAIT,
  };

  switch( m_SaveSeq )
  {
  case SAVE_SEQ_FORMAT_START:
    {
      m_pGameData->FormatStart( m_pAppHeap->GetDeviceHeap() );
      m_SaveSeq++;
    }
  break;
  case SAVE_SEQ_FORMAT_WAIT:
    {
      gfl2::fs::FsCtrBackupSystem::Result res;
      if( m_pGameData->FormatMain( &res ) == false ) break;
      if( res == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        m_SaveSeq = 0;
        SetSubSeq( MM_SEQ_EX_SAVE_INIT );
        break;
      }
      //セーブデータのフォーマット失敗は致命的
      GFL_PRINT("Format Result = %d\n", res );
      GFL_ASSERT_STOP( 0 );
    }
  break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：データ破損
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_ReportBroken( void )
{
  if( m_SaveDelete )
  {
    m_pAppParam->selectLang = m_pAppParam->defaultLang;
    m_pAppParam->requestFormatSaveData = true;

    if( m_pUpperDraw ) m_pUpperDraw->LangSelectClose();   // @fix cov_ctr[11059]

    SetSubSeq( MM_SEQ_LANGSELECT_MAIN );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：データ修復
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_ReportRepair( void )
{
  //修復したらリセットする
  m_pGameData->RepairCTRSecureValue();

  System::GflUse::SetSoftResetRequest();

  //SetSubSeq( MM_SEQ_EX_SAVE_INIT );
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：拡張セーブ状態
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_ExsaveStatus( void )
{
  EX_SAVE_STATUS status = m_pAppParam->exSaveStatus;

  switch( status )
  {
  case EX_SAVE_STATUS_TRUE:
    {
      SetSubSeq( MM_SEQ_LANGSELECT_INIT );
    } break;

  case EX_SAVE_STATUS_FALSE:
    {
      m_pLowerDraw->SetExSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_EX_SAVE_FORMAT );
    } break;

  case EX_SAVE_STATUS_BROKEN_DATA:
    {
      m_pLowerDraw->SetExSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_EX_SAVE_BROKEN );
    } break;

  case EX_SAVE_STATUS_NOT_FOUND:
    {
      m_pLowerDraw->SetExSaveDataStatusMessage( status );
      SetSubSeq( MM_SEQ_EX_SAVE_ERROR );
    } break;

  default:
    {
      GFL_ASSERT( 0 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：拡張セーブフォーマット
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_ExsaveFormat( void )
{
  enum
  {
    EXT_SAVE_SEQ_FORMAT_START = 0,
    EXT_SAVE_SEQ_FORMAT_WAIT,
  };

  ExtSavedata::NijiExtSaveSystem * p_extsys = m_pGameManager->GetNijiExtSaveSystem();

  switch( m_SaveSeq )
  {
  case EXT_SAVE_SEQ_FORMAT_START:
    {
      p_extsys->StartBootProcess( m_pAppHeap->GetDeviceHeap() );
      m_SaveSeq++;
    }
  break;
  case EXT_SAVE_SEQ_FORMAT_WAIT:
    {
      if( p_extsys->WaitBootProcess() == false )
      {
        break;
      }
    
      switch ( p_extsys->GetBootResult() )
      {
      case gfl2::fs::ExtSaveSystem::RESULT_OK:
        break;
      case gfl2::fs::ExtSaveSystem::RESULT_NG_NOT_ENOUGH_SPACE:
        //ガイドライン5.2.7　SDカードの空き容量が足りない場合
      //subWork->pGraWork->PutSubDispInfo( GraphicWork::SUB_INFO_ID_SD_NOT_ENOUGH );
        m_pLowerDraw->SetExSaveDataErrorMessage_Shortage();
        SetSubSeq( MM_SEQ_EX_SAVE_ERROR );
        return;
      default:
        GFL_ASSERT( 0 );//@fix
      case gfl2::fs::ExtSaveSystem::RESULT_NG_MEDIA_NOT_FOUND: //5.2.1
      case gfl2::fs::ExtSaveSystem::RESULT_NG_ERROR:           //5.2.2
      case gfl2::fs::ExtSaveSystem::RESULT_NG_VERIFICATION_FAILED: //5.2.3 or 5.2.4
      case gfl2::fs::ExtSaveSystem::RESULT_NG_NOT_FORMATTED: //5.2.3 or 5.2.4
      case gfl2::fs::ExtSaveSystem::RESULT_NG_BAD_FORMAT:       //5.2.5
      case gfl2::fs::ExtSaveSystem::RESULT_NG_WRITE_PROTECTED:  //5.2.6
        //ガイドライン5.2.1～5.2.6 SDカードが認識しませんでした
      //subWork->pGraWork->PutSubDispInfo( GraphicWork::SUB_INFO_ID_SD_NOT_FOUND );
        m_pLowerDraw->SetExSaveDataErrorMessage_Basic();
        SetSubSeq( MM_SEQ_EX_SAVE_ERROR );
        return;
      }

      if( m_pAppParam->saveStatus != SAVE_STATUS_TRUE )
      {//言語選択へ
        SetSubSeq( MM_SEQ_LANGSELECT_INIT );
      }
      else
      {//終了へ
        m_pAppParam->selectLang = System::GetLang();
        m_pAppParam->kanjiFlg = m_pGameData->GetConfig()->IsKanjiMode();
        m_pUpperDraw->LangSelectClose();
        m_pLowerDraw->LangSelectClose();
        SetSubSeq( MM_SEQ_LANGSELECT_MAIN );
      }
    }
  break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：拡張セーブ破損
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_ExsaveBroken( void )
{
#if 0
  //エラーになったら画面にエラーを表示して、停止する?
  GFL_ASSERT( 0 );
  SetSubSeq( MM_SEQ_LANGSELECT_INIT );
#else
  // 破損の場合はフォーマットへ遷移
  if( m_bInputDecide )
  {
    m_pLowerDraw->SetExSaveDataStatusMessage( EX_SAVE_STATUS_FALSE );
    SetSubSeq( MM_SEQ_EX_SAVE_FORMAT );
  }
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンス：拡張セーブエラー
 */
//------------------------------------------------------------------------------
void LangSelectFrame::Sequence_ExsaveError( void )
{
#if 0
  //エラーになったら画面にエラーを表示して、停止する?
  GFL_ASSERT( 0 );

  if( m_pAppParam->saveStatus != SAVE_STATUS_TRUE )
  {//言語選択へ
    SetSubSeq( MM_SEQ_LANGSELECT_INIT );
  }
  else
  {//終了へ
    m_pAppParam->selectLang = System::GetLang();
    m_pAppParam->kanjiFlg = m_pGameData->GetConfig()->IsKanjiMode();
    m_pUpperDraw->LangSelectClose();
    m_pLowerDraw->LangSelectClose();
    SetSubSeq( MM_SEQ_LANGSELECT_MAIN );
  }
#endif
  // 継続不能、無限ループ
}

//------------------------------------------------------------------------------
/**
 * @brief   言語選択の開始
 */
//------------------------------------------------------------------------------
void LangSelectFrame::StartLangSelect( void )
{
  m_pUpperDraw->LangSelectOpen();

  SetDispMode( DISP_MODE_LANG_SELECT );
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードの設定
 * @param   dispMode    画面モード
 */
//------------------------------------------------------------------------------
void LangSelectFrame::SetDispMode( DispMode dispMode )
{
  if( dispMode == DISP_MODE_LANG_SELECT )
  {
    m_pLowerDraw->SetLangSelectMessage();
  }
  else if( dispMode == DISP_MODE_CONFIRM )
  {
    m_pLowerDraw->SetLangDecideConfirmMessage();
  }
  else if( dispMode != DISP_MODE_END_SETUP && dispMode != DISP_MODE_TELL )
  {
    m_pLowerDraw->SetLangDecideJpnConfirmMessage();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   言語選択画面の終了通知
 */
//------------------------------------------------------------------------------
void LangSelectFrame::EndLangSelect( void )
{
  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   選択した言語
 * @param   langID    言語ID
 * @param   kanji     漢字フラグ
 */
//------------------------------------------------------------------------------
void LangSelectFrame::SetLangID( u8 langID, bool kanji )
{
  m_pAppParam->selectLang = langID;
  m_pAppParam->kanjiFlg = kanji;

  ReloadLangData();
}

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータ削除コマンドの入力
 */
//------------------------------------------------------------------------------
void LangSelectFrame::InputSavedataDeleteCommand( void )
{
  m_SaveDelete = true;
}

//------------------------------------------------------------------------------
/**
 * @brief   Aボタン入力
 */
//------------------------------------------------------------------------------
void LangSelectFrame::InputDecide( bool input )
{
  m_bInputDecide = input;
}


//-----------------------------------------------------------------------------
/**
 * @brief   TalkWindow用リソース読み込み
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool LangSelectFrame::LoadTalkWindowResource(void)
{
  switch( m_winResLoadSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_FIELDWINDOW, m_pAppHeap->GetSystemHeap() );
    m_winResLoadSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen(ARCID_FIELDWINDOW ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_FIELDWINDOW,
      GARC_fieldwindow_fieldwindow_applyt_COMP,
      &m_pWinResBuff,
      m_pAppHeap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_winResLoadSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad(&m_pWinResBuff) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_FIELDWINDOW, m_pAppHeap->GetSystemHeap() );
    m_winResLoadSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose(ARCID_FIELDWINDOW ) == false )
    {
      break;
    }
    m_winResLoadSeq++;
    /* FALL THROUGH */
  case 4:
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   言語切り替え時のデータ再設定
 * @caution   これ以降、メッセージウィンドウ以外への文字描画は禁止！
 */
//-----------------------------------------------------------------------------
void LangSelectFrame::ReloadLangData(void)
{
  gfl2::heap::HeapBase * dev_heap = m_pAppHeap->GetDeviceHeap();

  // 選択した言語をセット
  System::SetLang( m_pAppParam->selectLang, dev_heap, m_pAppParam->kanjiFlg );

  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  gd->ClearSaveData( dev_heap );  // @note sangoはタイトル後にセーブデータをクリアしていたが、nijiはイントロに遷移するのでここでクリアする
  gd->GetConfig()->SetKanjiMode( m_pAppParam->kanjiFlg );

  if( m_pAppParam->debugLangSelect != false )
  {
    return;
  }

  // 言語選択画面のリソース再生成
  { // メッセージ
    GFL_SAFE_DELETE( m_pWordSet );
    GFL_SAFE_DELETE( m_pMsgData );
    GflHeapSafeFreeMemory( m_pMsgReadBuff );

    u32 arc_id = print::GetMessageArcId();
    u32 dat_id = GFL_VALUE_DEFINE_IN_HEADER(GARC_message_language_select_DAT);
    app::util::FileAccessor::FileOpenSync( arc_id, dev_heap );
    app::util::FileAccessor::FileLoadSync( arc_id, dat_id, &m_pMsgReadBuff, m_pMsgHeap, false, 4 );
    app::util::FileAccessor::FileCloseSync( arc_id );

    m_pMsgData = GFL_NEW(m_pMsgHeap) gfl2::str::MsgData( m_pMsgReadBuff, m_pMsgHeap );
    m_pWordSet = GFL_NEW(m_pMsgHeap) print::WordSet( m_pMsgHeap );

    m_pUpperDraw->ResetMessage( m_pMsgData, m_pWordSet );
    m_pLowerDraw->ResetMessage( m_pMsgData, m_pWordSet );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   メッセージ関連専用のローカルヒープを生成
 */
//-----------------------------------------------------------------------------
void LangSelectFrame::CreateMessageHeap(void)
{
  gfl2::heap::HeapBase * dev_heap = m_pAppHeap->GetDeviceHeap();
  m_pMsgHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MESSAGE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   メッセージ関連専用のローカルヒープを削除
 */
//-----------------------------------------------------------------------------
void LangSelectFrame::DeleteMessageHeap(void)
{
  GFL_DELETE_HEAP( m_pMsgHeap );
}


GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )
