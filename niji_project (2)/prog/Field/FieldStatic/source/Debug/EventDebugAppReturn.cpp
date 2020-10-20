//======================================================================
/**
 * @file EventDebugAppReturn.cpp
 * @date 2016/06/17 16:15:59
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#include "Field/FieldStatic/include/Debug/EventDebugAppReturn.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "System/include/HeapDefine.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldRo/include/Debug/Menu/FieldDebugMenuTerrain.h"
#include <niji_conv_header/field/world/world_id.h>
#include <niji_conv_header/field/zone/zone_id.h>
#include "Field/FieldRo/include/Event/FieldEventDebugTerrainCheck.h"
#include "FieldScript/include/FieldScriptSystem.h"

#include "NetStatic/NetEvent/include/JoinFestaOpenEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaCloseEvent.h"

#include "Field/FieldRo/include/Fieldmap.h"

GFL_NAMESPACE_BEGIN(Field)

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool DebugAppReturn::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void DebugAppReturn::InitFunc(GameSys::GameManager* gmgr)
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT DebugAppReturn::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_START,

    SEQ_APP_KEEP_FIELD_CLOSE,
    SEQ_APP_KEEP_FIELD_OPEN,

    SEQ_APP_FIELD_CLOSE,
    SEQ_APP_FIELD_OPEN,

    SEQ_JF_JOINFESTA_OPEN,
    SEQ_JF_JOINFESTA_CLOSE,

    SEQ_END,
  };

  switch( this->GetSeqNo() )
  {
  case SEQ_START:

    // 復帰時に設定するため、現在のロケーションを保持しておく
    m_playerLocation.worldId  = gmgr->GetFieldmap()->GetWorldID();
    m_playerLocation.zoneId   = gmgr->GetFieldmap()->GetZoneID();
    m_playerLocation.position = gmgr->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
    m_playerLocation.quaternion = gmgr->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;

    if( m_Mode == MODE_APP_KEEP )
    {
      GFL_PRINT( "Areaリソースキープ型　アプリジャンプ\n" );
      this->SetSeqNo(SEQ_APP_KEEP_FIELD_CLOSE);
    }
    else if( m_Mode == MODE_APP )
    {
      GFL_PRINT( "Areaリソース全破棄型　アプリジャンプ\n" );
      this->SetSeqNo(SEQ_APP_FIELD_CLOSE);
    }
    else
    {
      GFL_PRINT( "JF型　アプリジャンプ\n" );
      this->SetSeqNo(SEQ_JF_JOINFESTA_OPEN);
    }
    break;

   // AREAリソースKEEP型のアプリジャンプ
  case SEQ_APP_KEEP_FIELD_CLOSE:
    {
      // クローズイベント
      EventFieldmapClose* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
            gmgr->GetGameEventManager( ) );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP_KEEPRES; // メモリ
      p_event->SetDesc( desc );

      this->SetSeqNo(SEQ_APP_KEEP_FIELD_OPEN);
    }
    break;
  case SEQ_APP_KEEP_FIELD_OPEN:
    {
      FieldmapDesc desc;
      desc.openMode = FM_OPEN_MODE_APP_RECOVER;
      desc.playerLocation = m_playerLocation;

      // オープンイベント
      EventFieldmapOpen* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
              gmgr->GetGameEventManager( ) );
      p_event->SetDesc( desc);

      this->SetSeqNo(SEQ_END);
    }
    break;

   // AREAリソースKEEPなしのアプリジャンプ
  case SEQ_APP_FIELD_CLOSE:
    {
      // クローズイベント
      EventFieldmapClose* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
            gmgr->GetGameEventManager( ) );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );

      this->SetSeqNo(SEQ_APP_FIELD_OPEN);
    }
    break;
  case SEQ_APP_FIELD_OPEN:
    {
      FieldmapDesc desc;
      desc.openMode = FM_OPEN_MODE_APP_RECOVER;
      desc.playerLocation = m_playerLocation;

      // オープンイベント
      EventFieldmapOpen* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
              gmgr->GetGameEventManager( ) );
      p_event->SetDesc( desc);

      this->SetSeqNo(SEQ_END);
    }
    break;

  // JFジャンプ
  case SEQ_JF_JOINFESTA_OPEN:
    {
      NetEvent::JoinFestaOpenEvent::StartEvent( gmgr, true );
      this->SetSeqNo(SEQ_JF_JOINFESTA_CLOSE);
    }
    break;
  case SEQ_JF_JOINFESTA_CLOSE:
    {
      NetEvent::JoinFestaCloseEvent::StartEvent( gmgr );

      this->SetSeqNo(SEQ_END);
    }
    break;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void DebugAppReturn::EndFunc(GameSys::GameManager* gmgr)
{
}


GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
