//=============================================================================
/**
 * @file    RideDemoEvent.cpp
 * @brief   ライド選択画面の説明デモイベント
 * @author  Hiroyuki Nakamura
 * @date    2016.03.30
 */
//=============================================================================

// module
#include "App/AppEvent/include/FieldMenu/RideDemoEvent.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "App/FieldDemo/include/FieldDemoProc.h"
// resource
#include "arc_index/demo_seq_script.gaix"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
RideDemoEvent::RideDemoEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
  , m_rideID( Savedata::FieldMenu::RIDE_ID_RUSH )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
RideDemoEvent::~RideDemoEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2016.03.31
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  RideDemoEventクラス
 */
//-----------------------------------------------------------------------------
RideDemoEvent * RideDemoEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, RideDemoEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupParam
 * @brief   パラメータ設定
 * @date    2016.03.31
 *
 * @param   id  ライドID
 */
//-----------------------------------------------------------------------------
void RideDemoEvent::SetupParam( Savedata::FieldMenu::RideID id )
{
  m_rideID = id;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2016.03.31
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool RideDemoEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2016.03.31
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void RideDemoEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    MainFunc
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2016.03.31
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT RideDemoEvent::MainFunc( GameSys::GameManager * gmgr )
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
      static const gfl2::fs::ArcFile::ARCDATID tbl[] =
      {
        GARC_demo_seq_script_ev9941_sm_kenta_BSEQ,
        GARC_demo_seq_script_ev9942_sm_mooland_BSEQ,
        GARC_demo_seq_script_ev9943_sm_banba_BSEQ,
        GARC_demo_seq_script_ev9944_sm_kairiki_BSEQ,
        GARC_demo_seq_script_ev9946_sm_lapras_BSEQ,
        GARC_demo_seq_script_ev9945_sm_samehada_BSEQ,
        GARC_demo_seq_script_ev9947_sm_lizardon_BSEQ,
      };
      GameSys::CallFieldDemoProc( tbl[m_rideID], 0 );
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
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2016.03.31
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void RideDemoEvent::EndFunc( GameSys::GameManager * gmgr )
{
}


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
