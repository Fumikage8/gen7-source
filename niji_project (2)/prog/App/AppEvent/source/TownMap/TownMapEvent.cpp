//=============================================================================
/**
 * @file    TownMapEvent.cpp
 * @brief   タウンマップ画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.14
 */
//=============================================================================

// module
#include "App/AppEvent/include/TownMap/TownMapEvent.h"
#include "App/AppEvent/include/TownMap/TownMapEventListener.h"

// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/TownMap/include/TownMapProc.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

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
TownMapEvent::TownMapEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
  , m_eventListener( NULL )
{
  gfl2::std::MemClear( &m_appParam, sizeof(m_appParam) );
#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
TownMapEvent::~TownMapEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.10.14
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  TownMapEventクラス
 */
//-----------------------------------------------------------------------------
TownMapEvent * TownMapEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, TownMapEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetParam
 * @brief   パラメータ設定
 * @date    2015.10.14
 *
 * @param   mode  呼び出しモード
 */
//-----------------------------------------------------------------------------
void TownMapEvent::SetParam( App::TownMap::CallMode mode )
{
  m_appParam.call_mode = mode;
}

//--------------------------------------------------------------
/**
 * @brief   終了を得るリスナーをセット
 * @param   pTownMapEventListener       TownMapEventListener
 */
//--------------------------------------------------------------
void TownMapEvent::SetTownMapEventListener(TownMapEventListener* pTownMapEventListener)
{
  m_eventListener = pTownMapEventListener;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetParam
 * @brief   パラメータ取得
 * @date    2015.10.14
 *
 * @return  パラメータ
 */
//-----------------------------------------------------------------------------
const App::TownMap::APP_PARAM * TownMapEvent::GetParam(void) const
{
  return &m_appParam;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool TownMapEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void TownMapEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT TownMapEvent::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,
    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    {
      GameSys::GameData * gd = gmgr->GetGameData();
      Field::ZoneDataAccessor accessor( gd->GetFieldZoneDataLoader()->GetZoneData(gd->GetNowZoneID()) );
      m_appParam.land_no = accessor.GetIslandIndex() - 1;   // 0オリジンに変更
      App::TownMap::TownMapProc * proc = GameSys::SetNextTownMapProc();
      proc->Setup( &m_appParam );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      SetSeqNo( EVSEQ_PROC_END );
    }
    break;

  case EVSEQ_PROC_END:
    if(m_eventListener){
      m_eventListener->OnTermination(m_appParam.warp_id);
    }
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
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void TownMapEvent::EndFunc( GameSys::GameManager * gmgr )
{
}


//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugTownMapLauncherCall = false;

void TownMapEvent::SetupLauncherCall( bool flg )
{
  DebugTownMapLauncherCall = flg;
}

bool TownMapEvent::IsLauncherCall(void)
{
  return DebugTownMapLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
