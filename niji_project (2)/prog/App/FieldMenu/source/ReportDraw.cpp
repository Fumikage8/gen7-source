//=============================================================================
/**
 * @file    ReportDraw.cpp
 * @brief   レポート画面
 * @author  Hiroyuki Nakamura
 * @date    2015.08.17
 */
//=============================================================================

// module
#include "ReportDraw.h"
#include "ReportPlayer.h"
#include "MenuDefine.h"
// niji
#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Tool/app_tool_TimeIcon.h"
#include "Sound/include/Sound.h"
#include "System/include/GflUse.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h" 
#include "Field/FieldRo/include/Fieldmap.h"
#include "SaveData/include/SaveLoadControl.h"
#if   defined(GF_PLATFORM_CTR)
#include "SaveData/include/CTRSaveLoadControl.h"
#else
#include "SaveData/include/Win32SaveLoadControl.h"
#endif
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/ReportRes.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/app/report/ReportRes.h"
#include "niji_conv_header/app/report/ReportRes_pane.h"
#include "niji_conv_header/app/report/ReportRes_anim_list.h"
#include "niji_conv_header/message/msg_report.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

const f32 ReportDraw::MENU_PX = 156.0f;   //!< メニュー表示X座標
const f32 ReportDraw::MENU_PY = 65.0f;    //!< メニュー表示Y座標

const f32 ReportDraw::BEADS_ICON_PX  = -137.0f; //!< ビーズアイコン初期X座標
const f32 ReportDraw::BEADS_ICON_PY1 = -71.0f;  //!< ビーズアイコン初期Y座標（上）
const f32 ReportDraw::BEADS_ICON_PY2 = -80.0f;  //!< ビーズアイコン初期Y座標（下）
const f32 ReportDraw::BEADS_ICON_SX  = 16.0f;   //!< ビーズアイコンX表示間隔

//! メインシーケンス
enum
{
  MAINSEQ_START = 0,            //!< セーブしますか？
  MAINSEQ_START_WAIT,           //!< 開始メッセージ待ち
  MAINSEQ_YESNO_INIT,           //!< "はい/いいえ"セット
  MAINSEQ_YESNO_MAIN,           //!< "はい/いいえ"選択
  MAINSEQ_SAVE_START,           //!< 書き込んでいます
  MAINSEQ_SAVE_MAIN,            //!< セーブ処理
  MAINSEQ_SAVE_COMOLETE,        //!< セーブしました
  MAINSEQ_SAVE_COMOLETE_WAIT,   //!< 完了SE待ち
  MAINSEQ_SAVE_FAILED,          //!< 失敗しました
  MAINSEQ_END_WAIT,             //!< 終了待ち
  MAINSEQ_END                   //!< 終了
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   mode        呼び出しモード
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   cursor      カーソルリソースバッファ
 */
//-----------------------------------------------------------------------------
ReportDraw::ReportDraw( CallMode mode, app::util::Heap * heap, app::util::AppRenderingManager * render_man, void * cursor )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_cursorBuff( cursor )
  , m_mode( mode )
  , m_gameData( NULL )
  , m_myStatus( NULL )
  , m_configSave( NULL )
  , m_zukanData( NULL )
  , m_playTime( NULL )
  , m_eventWork( NULL )
  , m_myItem( NULL )
  , m_pokeParty( NULL )
  , m_pokePartyCount( 0 )
  , m_fieldMap( NULL )
  , m_zoneDataLoader( NULL )
  , m_pPokeIcon( NULL )
  , m_pokeIconLoadCount( 0 )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pReportPlayer( NULL )
  , m_isMovePlayer( false )
  , m_playerUpdate( false )
  , m_pMenuWindow( NULL )
  , m_menuResult( 0xff )
  , m_mainSeq( MAINSEQ_START )
  , m_subSeq( 0 )
  , m_setupSeq( 0 )
  , m_endCount( 0 )
  , m_isEnd( false )
  , m_isDrawEnable( true )
{
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ReportDraw::~ReportDraw()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ取得
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::GetGameData(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  m_gameData = gm->GetGameData();

  m_myStatus   = m_gameData->GetPlayerStatus();
  m_configSave = m_gameData->GetConfig();
  m_zukanData  = m_gameData->GetZukanData();
  m_playTime   = m_gameData->GetPlayTime();
  m_eventWork  = m_gameData->GetEventWork();
  m_myItem     = m_gameData->GetMyItem();

  m_pokeParty = m_gameData->GetPlayerParty();
  m_pokePartyCount = m_pokeParty->GetMemberCount();

  m_fieldMap       = gm->GetFieldmap();
  m_zoneDataLoader = m_gameData->GetFieldZoneDataLoader();
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期化
 * @date    2015.08.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::Initialize(void)
{
  switch( m_setupSeq )
  {
  case 0:
    Sound::ChangeBGMPlayerVolume( Sound::BGM_VOLUME_APP, Sound::BGM_FADE_APP );
    m_setupSeq++;
    /* FALL THROUGH */
  case 1:
    if( LoadMessage() == false )
    {
      break;
    }
    m_setupSeq++;
    /* FALL THROUGH */
  case 2:
    if( CreatePokeIcon() == false )
    {
      break;
    }
    CreateMenuWindow();
    m_setupSeq++;
    /* FALL THROUGH */
  case 3:
    if( LoadLayout() == false )
    {
      break;
    }
    m_setupSeq++;
    /* FALL THROUGH */
  case 4:
    CreateLayout();
    CreateFieldReportPlayer();
    m_setupSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了処理
 * @date    2015.08.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::Terminate(void)
{
  switch( m_setupSeq )
  {
  case 0:
    m_isDrawEnable = false;
    if( IsDrawing() != false )
    {
      return false;
    }
    DeleteFieldReportPlayer();
    DeleteLayout();
    UnloadLayout();
    m_setupSeq++;
    /* FALL THROUGH */
  case 1:
    if( DeleteMenuWindow() == false )
    {
      break;
    }
    DeletePokeIcon();
    UnloadMessage();
    Sound::ChangeBGMPlayerVolume( Sound::BGM_VOLUME_NORMAL, Sound::BGM_FADE_APP );
    m_setupSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::Update(void)
{
  app::ui::UIView::Update();

  UpdateNowDate();

  if( m_isMovePlayer != false )
  {
    if( m_pReportPlayer->Update( m_playerUpdate ) != false )
    {
      m_pReportPlayer->Terminate();
      m_isMovePlayer = false;
    }
  }

  app::util::G2DUtil * g2d = GetG2DUtil();

  switch( m_mainSeq )
  {
  case MAINSEQ_START:   // セーブしますか？
    AddSubView( m_pMenuWindow );
    SetInputListener( this );
    if( m_mode == CALL_MODE_NORMAL )
    {
      SetMessage( report_02_01 );
    }
    else
    {
      SetMessage( report_02_10 );
    }
    m_mainSeq = MAINSEQ_START_WAIT;
    /* FALL THROUGH */
  case  MAINSEQ_START_WAIT:   // 開始メッセージ待ち
    if( g2d->PrintMessage() != gfl2::str::StrWin::RES_DONE )
    {
      break;
    }
    m_mainSeq = MAINSEQ_YESNO_INIT;
    /* FALL THROUGH */
  case MAINSEQ_YESNO_INIT:    // "はい/いいえ"セット
    if( m_pMenuWindow->IsCreate() != false )
    {
      m_pMenuWindow->AddItem( m_pMsgData, report_02_06, false );
      m_pMenuWindow->AddItem( m_pMsgData, report_02_07, true );
#if PM_DEBUG
      m_pMenuWindow->Debug_YesNoPosAssertInvalid();
#endif  // PM_DEBUG
      m_pMenuWindow->StartMenu( 0, app::tool::MenuWindow::PUT_RT, MENU_PX, MENU_PY );
      m_mainSeq = MAINSEQ_YESNO_MAIN;
    }
    break;

  case MAINSEQ_YESNO_MAIN:    // "はい/いいえ"選択
    if( m_menuResult == 0 )
    {
      m_pMenuWindow->EndMenu();
      m_isMovePlayer = true;
      m_playerUpdate = false;
      m_pReportPlayer->Initialize( m_heap->GetDeviceHeap(), m_fieldMap->GetMoveModelManager(), *m_gameData, m_fieldMap->GetEffectManager() );
      SetMessage( report_02_02 );
      m_mainSeq = MAINSEQ_SAVE_START;
    }
    else if( m_menuResult == 1 )
    {
      m_pMenuWindow->EndMenu();
      SetEnd();
    }
    break;

  case MAINSEQ_SAVE_START:    // 書き込んでいます
    {

      if( g2d->PrintMessage() != gfl2::str::StrWin::RES_DONE )
      {
        break;
      }
      SetTimerIcon( true );

      // レコード
      Savedata::IncRecord( Savedata::Record::RECID_REPORT_COUNT );

      // 前回の日付を退避
      m_playTime->PushSaveTime();
      // 現在の日付をセット
      /* 以下、@note sangoのBUGFIXコメント
          BTS[7070] レポート画面の現在時刻が更新されない不具合を修正 ( BUGFIX_BTS7070_20130730 )
            テンポラリのGameTimeに時間をセットしているため、
            セーブデータのGameTimeは更新されない。
            レポート終了後にEvTimeで更新される。
      */
      GameSys::DeviceDate dvice_date;
      gfl2::system::Date * system_date = dvice_date.GetDate();
      Savedata::GameTime game_time;
      game_time.SetDate( system_date );
      m_playTime->SetSaveTime( &game_time );
      //セーブ開始
      GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      GameSys::GameData * gd    = gm->GetGameData();
      gd->SaveStart( m_heap->GetDeviceHeap() );

      m_mainSeq = MAINSEQ_SAVE_MAIN;
    }
    break;

  case MAINSEQ_SAVE_MAIN:     // セーブ処理
    {
      GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      GameSys::GameData * gd    = gm->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result result = gd->SaveMain();
      if( result == gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE )
      {
        break;
      }

      SetTimerIcon( false );
      m_playerUpdate = true;

      if( result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        // 成功
        SetMessage( report_02_03 );
        VanishSaveTimeWindow();
        Sound::PlaySE( SEQ_SE_SYS_NJ_005 );
        g2d->StartAnime( 0, LA_REPORTRES_REPORT_MIN_LOW_000_OUT );
        m_mainSeq = MAINSEQ_SAVE_COMOLETE;
      }
      else
      {
        // 失敗
        m_playTime->PopSaveTime();
        SetMessage( report_02_05 );
        m_mainSeq = MAINSEQ_SAVE_FAILED;
      }
    }
    break;

  case MAINSEQ_SAVE_COMOLETE:     // セーブしました
    if( g2d->PrintMessage() != gfl2::str::StrWin::RES_DONE )
    {
      break;
    }
    m_endCount = 0;
    m_mainSeq = MAINSEQ_SAVE_COMOLETE_WAIT;
    break;

  case MAINSEQ_SAVE_COMOLETE_WAIT:  // 完了SE待ち
    if( m_endCount >= COMPLETE_WAIT )
    {
      SetEnd();
//      m_mainSeq = MAINSEQ_END_WAIT;
    }
    else
    {
      m_endCount++;
    }
    break;

  case MAINSEQ_SAVE_FAILED:   // 失敗しました
    if( g2d->PrintMessage() == gfl2::str::StrWin::RES_DONE )
    {
      m_mainSeq = MAINSEQ_END_WAIT;
    }
    break;
  
  case MAINSEQ_END_WAIT:    // 終了待ち
    if( m_endCount >= END_COUNT_MAX )
    {
      SetEnd();
    }
    else
    {
      m_endCount++;
    }
    break;

  case MAINSEQ_END:   // 終了
    m_isEnd = true;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.08.17
 *
 * @param   displayNo   描画ディスプレイ番号
 */
//-----------------------------------------------------------------------------
void ReportDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0, App::FieldMenu::DRAW_PRI_APP );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayout
 * @brief   レイアウトリソース読み込み
 * @date    2015.08.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::LoadLayout(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_REPORT, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( ARCID_REPORT ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_REPORT,
      GARC_ReportRes_ReportRes_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_REPORT, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( ARCID_REPORT ) == false )
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
 * @func    UnloadLayout
 * @brief   レイアウトリソース削除
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::UnloadLayout(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::CreateLayout(void)
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { m_pLytResBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_REPORTRES_REPORT_MIN_LOW_000_BFLYT,
      LA_REPORTRES_REPORT_MIN_LOW_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( m_pMsgBuff, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    GFL_NELEMS(data_tbl),
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_REPORT_MIN_LOW_000_PANE_MESSAGE_T );

    g2d->CreateMessageWindow(
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
      0,
      lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, &res_id ),
      lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, &res_id ),
      m_heap );

    g2d->CreateMsgCursor( m_heap, 0, PANENAME_REPORT_MIN_LOW_000_PANE_MESSAGE_T, PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00 );
    g2d->StartAnime( 0, LA_REPORTRES_REPORT_MIN_LOW_000__MESSAGE_T__CURSOR_00_KEEP );
  }

  SetDefaultString();
  InitPokeIcon();
  InitBeadsIcon();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteLayout
 * @brief   レイアウト削除
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::DeleteLayout(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->DeleteMsgCursor();
  g2d->DeleteMessageWindow();
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.08.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::LoadMessage(void)
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
      GARC_message_report_DAT,
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
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
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
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreatePokeIcon
 * @brief   ポケモンアイコン生成
 * @date    2015.08.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::CreatePokeIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pPokeIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::PokeIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), pml::PokeParty::MAX_MEMBERS );
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      m_pPokeIcon->AllocTextureBuffer( i );
    }
    m_pPokeIcon->FileOpen( m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsFileOpen() == false )
    {
      break;
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pokeIconLoadCount >= m_pokePartyCount )
    {
      m_subSeq = 4;
      break;
    }
    else
    {
      m_pPokeIcon->ReadRequest( m_pokeIconLoadCount, m_pokeParty->GetMemberPointerConst(m_pokeIconLoadCount) );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( m_pPokeIcon->IsLoadFinished(m_pokeIconLoadCount) != false )
    {
      m_pokeIconLoadCount++;
      m_subSeq = 2;
    }
    break;

  case 4:
    m_pPokeIcon->FileClose(m_heap->GetSystemHeap());
    m_subSeq++;
    /* FALL THROUGH */
  case 5:
    if( m_pPokeIcon->IsFileClose() == false )
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
 * @func    DeletePokeIcon
 * @brief   ポケモンアイコン削除
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::DeletePokeIcon(void)
{
  GFL_SAFE_DELETE( m_pPokeIcon );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuWindow
 * @brief   メニューウィンドウ生成
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::CreateMenuWindow(void)
{
  m_pMenuWindow = GFL_NEW(m_heap->GetSystemHeap()) app::tool::MenuWindow( m_heap, m_renderingManager, m_cursorBuff );
  m_pMenuWindow->SetListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuWindow
 * @brief   メニューウィンドウ削除
 * @date    2015.08.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::DeleteMenuWindow(void)
{
  if( m_pMenuWindow == NULL )
  {
    return true;
  }
  if( m_pMenuWindow->IsModuleFree() == false )
  {
    return false;
  }
  m_pMenuWindow->RemoveFromSuperView();
  GFL_SAFE_DELETE( m_pMenuWindow );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー
 * @date    2015.08.18
 *
 * @param   id  メニュー項目のID
 *
 * @note  override MenuWindowListener
 */
//-----------------------------------------------------------------------------
void ReportDraw::SelectMenu( u32 id )
{
  m_menuResult = id;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetDefaultString
 * @brief   デフォルト文字列書き込み
 * @date    2015.08.17
 */
//-----------------------------------------------------------------------------
void ReportDraw::SetDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  // 日付
  UpdateNowDate();

  // 地名   @note NULLチェックしてるのはランチャーから起動したときのため。
  if( m_fieldMap != NULL )
  {
    Field::ZoneDataAccessor accessor( m_zoneDataLoader->GetZoneData(m_fieldMap->GetZoneID()) );
    m_pWordSet->RegisterPlaceName( 0, accessor.GetPlaceLabelID() );
    g2d->SetTextBoxPaneStringExpand( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_PLACE, report_01_08 );
  }

  // 図鑑
  if( m_eventWork->CheckEventFlag(SYS_FLAG_ZUKAN_GET) == false )
  {
    g2d->SetPaneVisible( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_ZUKAN, false );
    g2d->SetPaneVisible( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_ZUKAN_COUNT_00, false );
  }
  else
  {
    g2d->SetTextBoxPaneString( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_ZUKAN, static_cast<u32>(report_01_01) );
    g2d->SetTextBoxPaneString( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_ZUKAN_COUNT_01, static_cast<u32>(report_01_03) );

    // 地方図鑑の数を取得
    g2d->SetRegisterNumber( 0, m_zukanData->GetLocalPokeGetCountStatic( PokeTool::ExtendData::LOCAL_AREA_A ), 3, print::NUM_DISP_LEFT );
    g2d->SetTextBoxPaneStringExpand( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_ZUKAN_COUNT_00, report_01_02 );
  }

  // プレイ時間
  g2d->SetTextBoxPaneString( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_PLAYTIME, report_01_04 );

  { // 前回のレポート
    s32 year  = m_playTime->GetSaveYear();
    s32 month = m_playTime->GetSaveMonth();
    s32 day   = m_playTime->GetSaveDay();
    if( year == 0 && month == 0 && day == 0 )
    {
      VanishSaveTimeWindow();
    }
    else
    {
      g2d->SetTextBoxPaneString( 0, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_LASTDATE, report_01_06 );
      PutDay( year, month, day, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_DAY_00 );
      PutTime( m_playTime->GetSaveHour(), m_playTime->GetSaveMinute(), PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_HOUR_02, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_COLON_02, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_MIN_02 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutTime
 * @brief   時間表示
 * @date    2015.08.18
 *
 * @param   h       時
 * @param   m       分
 * @param   pane_h  時表示ペイン
 * @param   pane_c  ":"表示ペイン
 * @param   pane_m  分表示ペイン
 */
//-----------------------------------------------------------------------------
void ReportDraw::PutTime( u32 h, u32 m, gfl2::lyt::LytPaneIndex pane_h, gfl2::lyt::LytPaneIndex pane_c, gfl2::lyt::LytPaneIndex pane_m )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  // 時
  g2d->SetRegisterNumber( 0, h, 3, print::NUM_DISP_LEFT );
  g2d->SetTextBoxPaneStringExpand( 0, pane_h, report_01_05_01 );
  // :
  g2d->SetTextBoxPaneString( 0, pane_c, report_01_05_02 );
  // 分
  g2d->SetRegisterNumber( 0, m, 2, print::NUM_DISP_ZERO );
  g2d->SetTextBoxPaneStringExpand( 0, pane_m, report_01_05_03 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutDay
 * @brief   日付表示
 * @date    2015.08.18
 *
 * @param   y     年
 * @param   m     月
 * @param   d     日
 * @param   pane  表示ペイン
 */
//-----------------------------------------------------------------------------
void ReportDraw::PutDay( u32 y, u32 m, u32 d, gfl2::lyt::LytPaneIndex pane )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  g2d->SetRegisterNumber( 0, y, 4, print::NUM_DISP_LEFT );
  g2d->SetRegisterNumber( 1, m, 2, print::NUM_DISP_ZERO );
  g2d->SetRegisterNumber( 2, d, 2, print::NUM_DISP_ZERO );
  g2d->SetTextBoxPaneStringExpand( 0, pane, report_01_07 );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateNowDate
 * @brief   現在時刻更新
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void ReportDraw::UpdateNowDate(void)
{
  // 現在時間
  GameSys::DeviceDate dvice_date;
  gfl2::system::Date * system_date = dvice_date.GetDate();
  gfl2::system::Date::GetNow( system_date );
  PutDay( system_date->GetYear(), system_date->GetMonth(), system_date->GetDay(), PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_DAY );
  PutTime( system_date->GetHour(), system_date->GetMinute(), PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_HOUR_00, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_COLON_00, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_MIN_00 );
  // プレイ時間
  PutTime( m_playTime->GetPlayHour(), m_playTime->GetPlayMinute(), PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_HOUR_01, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_COLON_01, PANENAME_REPORT_MIN_LOW_000_PANE_TEXT_MIN_01 );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPokeIcon
 * @brief   ポケモンアイコン初期化
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void ReportDraw::InitPokeIcon(void)
{
  static const gfl2::lyt::LytPaneIndex poke_icon[] =
  {
    PANENAME_REPORT_MIN_LOW_000_PANE_PICON_00,
    PANENAME_REPORT_MIN_LOW_000_PANE_PICON_01,
    PANENAME_REPORT_MIN_LOW_000_PANE_PICON_02,
    PANENAME_REPORT_MIN_LOW_000_PANE_PICON_03,
    PANENAME_REPORT_MIN_LOW_000_PANE_PICON_04,
    PANENAME_REPORT_MIN_LOW_000_PANE_PICON_05,
  };
  static const gfl2::lyt::LytPaneIndex ball_icon[] =
  {
    PANENAME_REPORT_MIN_LOW_000_PANE_BALLICON_00,
    PANENAME_REPORT_MIN_LOW_000_PANE_BALLICON_01,
    PANENAME_REPORT_MIN_LOW_000_PANE_BALLICON_02,
    PANENAME_REPORT_MIN_LOW_000_PANE_BALLICON_03,
    PANENAME_REPORT_MIN_LOW_000_PANE_BALLICON_04,
    PANENAME_REPORT_MIN_LOW_000_PANE_BALLICON_05,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( i < m_pokePartyCount )
    {
      m_pPokeIcon->ReplaceReadTexture( i, lytwk->GetPicturePane(poke_icon[i]) );
      g2d->SetPaneVisible( 0, poke_icon[i], true );
      g2d->SetPaneVisible( 0, ball_icon[i], false );
    }
    else
    {
      g2d->SetPaneVisible( 0, poke_icon[i], false );
      g2d->SetPaneVisible( 0, ball_icon[i], true );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitBeadsIcon
 * @brief   ビーズアイコン初期化
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void ReportDraw::InitBeadsIcon(void)
{
  struct BEADS_ICON
  {
    u32 item;
    gfl2::lyt::LytPaneIndex pane;
  };

  static const BEADS_ICON tbl[] =
  {
    { ITEM_NOOMARUBIIZU, PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_00 },
    { ITEM_HONOOBIIZU,   PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_01 },
    { ITEM_MIZUBIIZU,    PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_02 },
    { ITEM_DENKIBIIZU,   PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_03 },
    { ITEM_KUSABIIZU,    PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_04 },
    { ITEM_KOORIBIIZU,   PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_05 },
    { ITEM_KAKUTOUBIIZU, PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_06 },
    { ITEM_DOKUBIIZU,    PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_07 },
    { ITEM_ZIMENBIIZU,   PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_08 },
    { ITEM_HIKOUBIIZU,   PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_09 },
    { ITEM_ESUPAABIIZU,  PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_10 },
    { ITEM_MUSIBIIZU,    PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_11 },
    { ITEM_IWABIIZU,     PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_12 },
    { ITEM_GOOSUTOBIIZU, PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_13 },
    { ITEM_DORAGONBIIZU, PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_14 },
    { ITEM_AKUBIIZU,     PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_15 },
    { ITEM_HAGANEBIIZU,  PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_16 },
    { ITEM_FEARIIBIIZU,  PANENAME_REPORT_MIN_LOW_000_PANE_BEADSICON_17 },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  f32 px = BEADS_ICON_PX;
  u32 cnt = 0;

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( m_myItem->Check(tbl[i].item,1) == false )
    {
      continue;
    }
    f32 py = BEADS_ICON_PY1;
    if( ( cnt & 1 ) != 0 )
    {
      py = BEADS_ICON_PY2;
    }
    gfl2::math::VEC3 pos( px, py, 0.0f );
    g2d->SetPanePos( 0, tbl[i].pane, &pos );
    g2d->SetPaneVisible( 0, tbl[i].pane, true );
    px += BEADS_ICON_SX;
    cnt++;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMessage
 * @brief   メッセージをセット
 * @date    2015.08.18
 *
 * @param   id  メッセージID
 */
//-----------------------------------------------------------------------------
void ReportDraw::SetMessage( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  if( id == report_02_03 )
  {
    g2d->SetRegisterPlayerName( 0, m_myStatus );
    g2d->GetStringExpand( str, id );
  }
  else
  {
    g2d->GetString( str, id );
  }

  {
    print::MessageWindow * win = g2d->GetMsgWin();
    if( id == report_02_02 )
    {
      win->SetMessageSpeed( Savedata::ConfigSave::MSG_NORMAL );
      win->SetMessageAutoFlg( true );
    }
    else
    {
      win->SetMessageSpeed( m_configSave->GetTalkingSpeed() );
      win->SetMessageAutoFlg( false );
    }
  }

  g2d->SetMessage( str );

  // 表示
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_REPORT_MIN_LOW_000_PANE_MESSAGE_T );
  gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1, &res_id );
  pane->SetVisible( true );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTimerIcon
 * @brief   タイマーアイコンセット
 * @date    2015.08.18
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void ReportDraw::SetTimerIcon( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( flg != false )
  {
    g2d->StartAnime( 0, LA_REPORTRES_REPORT_MIN_LOW_000__MESSAGE_T__TIMERICON_IN );
    App::Tool::TimeIcon::StartAnime( g2d->GetLayoutWork( 0 ), LA_REPORTRES_REPORT_MIN_LOW_000__MESSAGE_T__TIMERICON_KEEP );
    // 表示
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_REPORT_MIN_LOW_000_PANE_MESSAGE_T );
    gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON, &res_id );
    pane->SetVisible( true );
  }
  else
  {
    g2d->StartAnime( 0, LA_REPORTRES_REPORT_MIN_LOW_000__MESSAGE_T__TIMERICON_OUT );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishSaveTimeWindow
 * @brief   前回のセーブ時間のウィンドウを非表示
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void ReportDraw::VanishSaveTimeWindow(void)
{
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_REPORT_MIN_LOW_000_PANE_WINDOW_00, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEnd
 * @brief   終了へ
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void ReportDraw::SetEnd(void)
{
  if( m_isMovePlayer != false )
  {
    return;
  }
//  GetG2DUtil()->SetLayoutDrawEnable( 0, false );
  m_mainSeq = MAINSEQ_END;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsModuleEnd
 * @brief   終了チェック
 * @date    2015.08.17
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::IsModuleEnd(void)
{
  return m_isEnd;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCancel
 * @brief   キャンセルされたか
 * @date    2015.08.18
 *
 * @retval  true  = キャンセル
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ReportDraw::IsCancel(void)
{
  if( m_menuResult == 1 )
  {
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateFieldReportPlayer
 * @brief   主人公動作クラス生成
 * @date    2015.12.09
 */
//-----------------------------------------------------------------------------
void ReportDraw::CreateFieldReportPlayer(void)
{
  m_pReportPlayer = GFL_NEW(m_heap->GetSystemHeap()) Field::FieldReportPlayer();
  m_isMovePlayer = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFieldReportPlayer
 * @brief   主人公動作クラス削除
 * @date    2015.12.09
 */
//-----------------------------------------------------------------------------
void ReportDraw::DeleteFieldReportPlayer(void)
{
  GFL_SAFE_DELETE( m_pReportPlayer );
}


//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2015.08.18
 *
 * @param   touch_panel タッチパネル
 * @param   is_touch    true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult ReportDraw::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  if( m_mainSeq == MAINSEQ_END_WAIT )
  {
    if( is_touch != false )
    {
      SetEnd();
      return DISABLE_ACCESS;
    }
  }
  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.08.17
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult ReportDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_mainSeq == MAINSEQ_END_WAIT )
  {
    if( button->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) )
    {
      SetEnd();
      return DISABLE_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
