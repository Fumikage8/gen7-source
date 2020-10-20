//======================================================================
/**
* @file StaffrollEvent.cpp
* @date 2016/11/22 19:21:11
* @author saito_nozomu
* @brief スタッフロール呼び出し
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "App/AppEvent/include/Staffroll/StaffrollEvent.h"

#include  <Sound/include/Sound.h>
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"

#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
* @brief   コンストラクタ
*
* @param   heap  ヒープ
*/
//-----------------------------------------------------------------------------
StaffrollEvent::StaffrollEvent(gfl2::heap::HeapBase * heap)
: GameSys::GameEvent(heap)
{
}

//-----------------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//-----------------------------------------------------------------------------
StaffrollEvent::~StaffrollEvent()
{
}


//-----------------------------------------------------------------------------
/**
* @func    StartEvent
* @brief   イベント起動
* @date    2015.06.11
*
* @param   man     ゲームマネージャクラス
*
* @return  StaffrollEventクラス
*/
//-----------------------------------------------------------------------------
StaffrollEvent * StaffrollEvent::StartEvent(GameSys::GameManager * gmgr)
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, StaffrollEvent >(ev_man);
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*
* @retval  true  = 起動してよい
* @retval  false = 起動しない
*/
//-----------------------------------------------------------------------------
bool StaffrollEvent::BootChk(GameSys::GameManager * /*gmgr*/)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*/
//-----------------------------------------------------------------------------
void StaffrollEvent::InitFunc(GameSys::GameManager * gmgr)
{
  //復帰座標の設定
  m_playerLocation = *gmgr->GetGameData()->GetStartLocation();
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   呼び出される関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*
* @return  イベント制御関数の戻り値
*/
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT StaffrollEvent::MainFunc(GameSys::GameManager * gmgr)
{
  enum
  {
    EVSEQ_PROC_FIELCLOSE = 0,
    EVSEQ_PROC_CREATE,
    EVSEQ_PROC_WAIT,
    EVSEQ_PROC_FIELDOPEN,
    EVSEQ_PROC_END,
  };

  switch (GetSeqNo()){
  case EVSEQ_PROC_FIELCLOSE:
  {
    Field::EventFieldmapClose* p_event;

    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventFieldmapClose >(gmgr->GetGameEventManager());
    Field::EventFieldmapClose::Desc desc;
    desc.closeType = Field::FM_CLOSE_MODE_APP; // メモリを全破棄
    p_event->SetDesc(desc);
    p_event->SetFadeCallBack(NULL, NULL, NULL);
  }
  SetSeqNo(EVSEQ_PROC_CREATE);
  break;

  case EVSEQ_PROC_CREATE:
  {
    App::Staffroll::StaffrollProc* proc = GameSys::CallStaffrollProc(true);//niji同様、momijiでもスタッフロールは初回時のみ表示するため、引数はtrue固定でＯＫ
    SetSeqNo(EVSEQ_PROC_WAIT);
    break;
  }

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if (gmgr->IsProcessExists())
    {
      break;
    }
    SetSeqNo(EVSEQ_PROC_FIELDOPEN);
    break;
  case EVSEQ_PROC_FIELDOPEN:
  {
    Field::EventFieldmapOpen* p_event;
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventFieldmapOpen >(gmgr->GetGameEventManager());
    p_event->SetDesc(Field::FM_OPEN_MODE_APP_RECOVER, m_playerLocation);
    p_event->SetFadeCallBack(NULL, NULL, NULL);
  }
  SetSeqNo(EVSEQ_PROC_END);
  break;
  case EVSEQ_PROC_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   呼び出される関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*/
//-----------------------------------------------------------------------------
void StaffrollEvent::EndFunc(GameSys::GameManager * gmgr)
{
}

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)