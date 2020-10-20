//=============================================================================
/**
 * @file    WazaOmoidasiEvent.cpp
 * @brief   技思い出し画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.12.22
 */
//=============================================================================

// module
#include "App/AppEvent/include/WazaOmoidasi/WazaOmoidasiEvent.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/WazaOmoidasi/include/WazaOmoidasiProc.h"
#include "App/WazaOshie/include/WazaOshieProc.h"

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
WazaOmoidasiEvent::WazaOmoidasiEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
  , m_resultParam( NULL )
  , m_partyIndex( 0 )
{
  gfl2::std::MemClear( &m_appParam, sizeof(App::WazaOmoidasi::APP_PARAM) );
  m_appParam.delete_pos = pml::MAX_WAZA_NUM;
#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
WazaOmoidasiEvent::~WazaOmoidasiEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.12.22
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  WazaOmoidasiEventクラス
 */
//-----------------------------------------------------------------------------
WazaOmoidasiEvent * WazaOmoidasiEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, WazaOmoidasiEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetParam
 * @brief   パラメータ設定
 * @date    2015.12.22
 *
 * @param   party_index 手持ちポケモンの並び位置
 * @param   result      イベント終了パラメータ
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiEvent::SetParam( u32 party_index, RESULT_PARAM * result )
{
  m_appParam.pp = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty()->GetMemberPointer( party_index );
  m_resultParam = result;
  m_partyIndex  = party_index;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.12.22
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.12.22
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.12.22
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT WazaOmoidasiEvent::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,

    EVSEQ_WAZAWASURE_PROC_CREATE,
    EVSEQ_WAZAWASURE_PROC_WAIT,

    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    {
      App::WazaOmoidasi::WazaOmoidasiProc * proc = GameSys::SetNextWazaOmoidasiProc();
      proc->Setup( &m_appParam );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      if( m_appParam.end_mode == App::WazaOmoidasi::END_DECIDE )
      {
        m_resultParam->set_waza = m_appParam.select_no;
        SetSeqNo( EVSEQ_PROC_END );
      }
      else if( m_appParam.end_mode == App::WazaOmoidasi::END_CANCEL )
      {
        m_resultParam->set_waza = WAZANO_NULL;
        SetSeqNo( EVSEQ_PROC_END );
      }
      else
      {
        SetSeqNo( EVSEQ_WAZAWASURE_PROC_CREATE );
      }
    }
    break;

  case EVSEQ_WAZAWASURE_PROC_CREATE:
    SetWazaOhsieParam();
    {
      App::WazaOshie::Proc * proc = GameSys::CallWazaOshieProc();
      proc->SetAppParam( &m_wazaOshieParam );
    }
    SetSeqNo( EVSEQ_WAZAWASURE_PROC_WAIT );
    break;

  case EVSEQ_WAZAWASURE_PROC_WAIT:
    if( !gmgr->IsProcessExists() )
    {
      if( m_wazaOshieParam.end_mode == 0 )
      {
        m_appParam.delete_pos = pml::MAX_WAZA_NUM;
      }
      else
      {
        m_appParam.delete_pos = m_wazaOshieParam.select_index;  // 選択した技の位置
      }
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
 * @date    2015.12.22
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiEvent::EndFunc( GameSys::GameManager * gmgr )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetWazaOshieParam
 * @brief   技教え画面パラメータ設定
 * @date    2015.12.22
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiEvent::SetWazaOhsieParam(void)
{
  gfl2::std::MemClear( &m_wazaOshieParam, sizeof(App::WazaOshie::APP_PARAM) );
  m_wazaOshieParam.mode = App::WazaOshie::MODE_WAZA_OBOE;
  m_wazaOshieParam.select_poke_index = m_partyIndex;                      // ポケモン位置
  m_wazaOshieParam.upper_draw = 1;                                        // 上画面の描画フラグ
  m_wazaOshieParam.waza_no = static_cast<WazaNo>( m_appParam.select_no ); // 覚えさせる技
}



//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugWazaOmoidasiLauncherCall = false;

void WazaOmoidasiEvent::SetupLauncherCall( bool flg )
{
  DebugWazaOmoidasiLauncherCall = flg;
}

bool WazaOmoidasiEvent::IsLauncherCall(void)
{
  return DebugWazaOmoidasiLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
