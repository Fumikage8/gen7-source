//=============================================================================
/**
 * @file    TrainerPassEvent.cpp
 * @brief   トレーナーパス画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================

// module
#include "App/AppEvent/include/TrainerPass/TrainerPassEvent.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/TrainerPass/include/TrainerPassProc.h"

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
TrainerPassEvent::TrainerPassEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
{
  gfl2::std::MemClear( &m_appParam, sizeof(App::TrainerPass::APP_PARAM) );
  gfl2::std::MemClear( &m_photoAppParam, sizeof(app::photo::APP_PARAM) );

#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
TrainerPassEvent::~TrainerPassEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.11.06
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  TrainerPassEventクラス
 */
//-----------------------------------------------------------------------------
TrainerPassEvent * TrainerPassEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, TrainerPassEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetStampMode
 * @brief   スタンプ用の設定
 * @date    2015.11.06
 *
 * @param   stamp   スタンプ番号
 */
//-----------------------------------------------------------------------------
void TrainerPassEvent::SetStampMode( u32 stamp )
{
  m_appParam.call_mode = App::TrainerPass::CALL_MODE_STAMP;
  m_appParam.stamp_no  = stamp;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.11.06
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool TrainerPassEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.11.06
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void TrainerPassEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.11.06
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT TrainerPassEvent::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,

    EVSEQ_PHOTO_PROC_CREATE,
    EVSEQ_PHOTO_PROC_WAIT,

    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    {
      App::TrainerPass::TrainerPassProc * proc = GameSys::SetNextTrainerPassProc();
      proc->Setup( &m_appParam );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      // 証明写真画面を呼び出す
      if( m_appParam.end_mode == App::TrainerPass::END_MODE_PHOTO )
      {
        SetSeqNo( EVSEQ_PHOTO_PROC_CREATE );
      }
      else
      {
        SetSeqNo( EVSEQ_PROC_END );
      }
    }
    break;

  case EVSEQ_PHOTO_PROC_CREATE:
    {
      app::photo::TrainerPhotoProc * proc = GameSys::CallTrainerPhotoProc();
      proc->Initialize( &m_photoAppParam );
    }
    SetSeqNo( EVSEQ_PHOTO_PROC_WAIT );
    break;

  case EVSEQ_PHOTO_PROC_WAIT:
    if( !gmgr->IsProcessExists() )
    {
      SetSeqNo( EVSEQ_PROC_CREATE );
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
 * @date    2015.11.06
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void TrainerPassEvent::EndFunc( GameSys::GameManager * gmgr )
{
}



//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugTrainerPassLauncherCall = false;

void TrainerPassEvent::SetupLauncherCall( bool flg )
{
  DebugTrainerPassLauncherCall = flg;
}

bool TrainerPassEvent::IsLauncherCall(void)
{
  return DebugTrainerPassLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
