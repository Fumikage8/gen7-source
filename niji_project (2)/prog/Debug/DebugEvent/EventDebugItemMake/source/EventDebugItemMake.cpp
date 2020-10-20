#if PM_DEBUG
//=============================================================================
/**
 * @file    EventDebugItemMake.cpp
 * @brief   デバッグ道具生成画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.11.17
 */
//=============================================================================

// module
#include "Debug/DebugEvent/EventDebugItemMake/include/EventDebugItemMake.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"


#include "Field/FieldRo/include/Fieldmap.h"


#include "Field/FieldStatic/include/Event/EventProcessCall.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
EventDebugItemMake::EventDebugItemMake( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
EventDebugItemMake::~EventDebugItemMake()
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
 * @return  EventDebugItemMakeクラス
 */
//-----------------------------------------------------------------------------
EventDebugItemMake * EventDebugItemMake::StartEvent( GameSys::GameManager * gmgr, bool flag )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  EventDebugItemMake * ev = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventDebugItemMake >( ev_man );
  ev->SetFieldControlFlag( flag );
  return ev;
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
bool EventDebugItemMake::BootChk( GameSys::GameManager * /*gmgr*/ )
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
void EventDebugItemMake::InitFunc( GameSys::GameManager * /*gmgr*/ )
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
GameSys::GMEVENT_RESULT EventDebugItemMake::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    SEQ_FIELD_CLOSE,
    SEQ_APP_CALL,
    SEQ_APP_WAIT,
    SEQ_FIELD_OPEN,
    SEQ_END,
  };

  switch( GetSeqNo() )
  {
  case SEQ_FIELD_CLOSE:
    if( m_fieldControlFlag )
    {
      // クローズイベント
      EventFieldmapClose * p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldmapClose >( gmgr->GetGameEventManager() );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );
      p_event->SetFadeCallBackDefault();  //フェードはデフォルト
    }
    SetSeqNo( SEQ_APP_CALL );
    break;

  case SEQ_APP_CALL:
    m_pProc = GameSys::CallDebugItemMakeProc();
    SetSeqNo( SEQ_APP_WAIT );
    break;

  case SEQ_APP_WAIT:
    if( gmgr->GetProcManager()->IsRegisterProcess(m_pProc) == false )
    {
      SetSeqNo( SEQ_FIELD_OPEN );
    }
    break;

  case SEQ_FIELD_OPEN:
    if( m_fieldControlFlag )
    {
      EventFieldmapOpen * p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldmapOpen >( gmgr->GetGameEventManager() );
      p_event->SetDesc( FM_OPEN_MODE_APP_RECOVER, *gmgr->GetGameData()->GetStartLocation() );
      p_event->SetFadeCallBackDefault();
    }
    SetSeqNo( SEQ_END );
    break;

  case SEQ_END:
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
void EventDebugItemMake::EndFunc( GameSys::GameManager * gmgr )
{
}


void EventDebugItemMake::SetFieldControlFlag( bool flag )
{
  m_fieldControlFlag = flag;
}



GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif  // PM_DEBUG
