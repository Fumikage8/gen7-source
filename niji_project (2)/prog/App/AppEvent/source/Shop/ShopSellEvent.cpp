//=============================================================================
/**
 * @file    ShopSellEvent.cpp
 * @brief   ショップ売却画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================

// module
#include "App/AppEvent/include/Shop/ShopSellEvent.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/FieldBag/include/BagProc.h"
#include "App/AppEvent/include/Bag/BagGamedata.h"

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
ShopSellEvent::ShopSellEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
{
  gfl2::std::MemClear( &m_appParam, sizeof(app::bag::APP_PARAM) );

#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ShopSellEvent::~ShopSellEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.10.02
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  ShopSellEventクラス
 */
//-----------------------------------------------------------------------------
ShopSellEvent * ShopSellEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, ShopSellEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool ShopSellEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void ShopSellEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT ShopSellEvent::MainFunc( GameSys::GameManager * gmgr )
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
    gfl2::std::MemClear( &m_appParam, sizeof(m_appParam) );
    m_appParam.call_mode = app::bag::CALL_SHOP;
    m_appParam.pocket_no = gmgr->GetGameData()->GetBagPlayData()->GetPocketPos();
    {
      app::bag::BagProc * proc = GameSys::SetNextBagProc();
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
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void ShopSellEvent::EndFunc( GameSys::GameManager * gmgr )
{
}


//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugShopSellLauncherCall = false;

void ShopSellEvent::SetupLauncherCall( bool flg )
{
  DebugShopSellLauncherCall = flg;
}

bool ShopSellEvent::IsLauncherCall(void)
{
  return DebugShopSellLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
