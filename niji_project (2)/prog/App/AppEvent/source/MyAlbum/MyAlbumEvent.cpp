//=============================================================================
/**
 * @file    MyAlbumEvent.cpp
 * @brief   マイアルバム画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2017.01.25
 */
//=============================================================================

// module
#include "App/AppEvent/include/MyAlbum/MyAlbumEvent.h"
// momiji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/MyAlbum/include/MyAlbumProc.h"
#include "App/FinderStudioDecoration//include/FinderStudioDecorationProc.h"
#include <NetStatic/NetEvent/include/FatalErrorEvent.h>

#if PM_DEBUG
#include "Debug/StartMenu/include/StartLauncher.h"
#endif


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
MyAlbumEvent::MyAlbumEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
{
  gfl2::std::MemClear( &m_appParam, sizeof(App::MyAlbum::APP_PARAM) );
  m_appParam.init_share = App::MyAlbum::MYALBUM_INIT_SHARE;   // 初期値

#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MyAlbumEvent::~MyAlbumEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2017.01.25
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  MyAlbumEventクラス
 */
//-----------------------------------------------------------------------------
MyAlbumEvent * MyAlbumEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, MyAlbumEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2017.01.25
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool MyAlbumEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2017.01.25
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void MyAlbumEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2017.01.25
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT MyAlbumEvent::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,

    EVSEQ_DECO_PROC_CREATE,
    EVSEQ_DECO_PROC_WAIT,

    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    {
      App::MyAlbum::MyAlbumProc * proc = GameSys::SetNextMyAlbumProc();
      proc->Setup( &m_appParam );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      if( m_appParam.end_mode == App::MyAlbum::END_MODE_DECO )
      {
        SetSeqNo( EVSEQ_DECO_PROC_CREATE );
      }
      else if( m_appParam.end_mode == App::MyAlbum::END_MODE_FATAL_ERR )
      {
        NetEvent::FatalError::EventParam eventParam = { NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED };
        NetEvent::FatalError::FatalErrorEvent::StartEvent( gmgr, eventParam );
      }
      else
      {
        SetSeqNo( EVSEQ_PROC_END );
      }
    }
    break;

  case EVSEQ_DECO_PROC_CREATE:
    {
      m_albumWork.photo_data       = m_appParam.photo_data;
      m_albumWork.photo_data_index = m_appParam.select_index;
      m_albumWork.photo_count      = m_appParam.photo_count;
      App::FinderStudio::Decoration::Proc * proc = GameSys::CallFinderStudioDecorationProc();
      proc->SetAlbumWork( &m_albumWork );
    }
    SetSeqNo( EVSEQ_DECO_PROC_WAIT );
    break;

  case EVSEQ_DECO_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      /// フェイタルエラー
      if( m_albumWork.event_result == App::FinderStudio::Static::EVE_RES_FATAL_ERROR )
      {
        NetEvent::FatalError::EventParam eventParam = { NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED };
        NetEvent::FatalError::FatalErrorEvent::StartEvent( gmgr, eventParam );
      }
      /// エラー終了
      else if( m_albumWork.event_result == App::FinderStudio::Static::EVE_RES_ERROR )
      {
        SetSeqNo( EVSEQ_PROC_END );
      }
      else
      {
        SetSeqNo( EVSEQ_PROC_CREATE );
      }
    }
    break;

  case EVSEQ_PROC_END:
#if PM_DEBUG
    if( IsLauncherCall() != false )
    {
      debug::launcher::LauncherProc::CallParam param;
      debug::launcher::LauncherProc::InitCallParam( &param );
      debug::launcher::LauncherProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( gmgr->GetProcManager(), &param );
      proc->CreateStartListData< debug::startmenu::StartLauncher >();
    }
#endif  // PM_DEBUG
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2017.01.25
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void MyAlbumEvent::EndFunc( GameSys::GameManager * gmgr )
{
}



//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugMyAlbumLauncherCall = false;

void MyAlbumEvent::SetupLauncherCall( bool flg )
{
  DebugMyAlbumLauncherCall = flg;
}

bool MyAlbumEvent::IsLauncherCall(void)
{
  return DebugMyAlbumLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
