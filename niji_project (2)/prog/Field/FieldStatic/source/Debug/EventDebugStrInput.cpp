//======================================================================
/**
 * @file EventDebugStrInput.cpp
 * @date 2016/01/28 15:45:27
 * @author miyachi_soichi
 * @brief デバッグ：キャラクタ名入力
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
// event
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
// self
#include "Field/FieldStatic/include/Debug/EventDebugStrInput.h"
// StrInput
#include "App/StrInput/include/StrInputProc.h"
#include "App/StrInput/include/StrInputFrame.h"
#include "GameSys/include/DllProc.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @class  EventDebugStrInput
 *  @brief デバッグ：キャラクタ名入力
 */
/**
 *  @brief  コンストラクタ
 *  @param  pHeap   workようヒープ
 */
EventDebugStrInput::EventDebugStrInput( gfl2::heap::HeapBase *pHeap )
: GameSys::GameEvent( pHeap )
, name(System::STRLEN_PLAYER_NAME + System::EOM_LEN,pHeap)
{
  m_pAppParam = GFL_NEW( pHeap ) App::StrInput::StrInputParam;
  SetSeqNo( SEQ_NO_CloseField );
}

/**
 *  @brief  イベントを動かしてよいかどうかの判定関数
 *  @param  pGameManager  ゲームマネージャ
 *  @retval true  起動して良い
 *  @retval false 起動してはいけない
 */
bool EventDebugStrInput::BootChk( GameSys::GameManager *pGameManager )
{
  return true;
}

/**
 *  @brief  初期化関数
 *  @param  pGameManager  ゲームマネージャ
 */
void EventDebugStrInput::InitFunc( GameSys::GameManager *pGameManager )
{
}

/**
 *  @brief  メイン関数
 *  @param  pGameManager    ゲームマネージャ
 *  @return GMEVENT_RESUTLT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventDebugStrInput::MainFunc( GameSys::GameManager *pGameManager )
{
  switch( GetSeqNo() )
  {
  case SEQ_NO_CloseField:
    {
      // 復帰時に設定するため、現在のロケーションを保持しておく
      m_player_location.worldId    = pGameManager->GetFieldmap()->GetWorldID();
      m_player_location.zoneId     = pGameManager->GetFieldmap()->GetZoneID();
      m_player_location.position   = pGameManager->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
      m_player_location.quaternion = pGameManager->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;

      // クローズ
      EventFieldmapClose *p_close;
      p_close = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose>( pGameManager->GetGameEventManager() );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP;
      p_close->SetDesc( desc );
      p_close->SetFadeCallBack( NULL, NULL, NULL );
    }
    AddSeqNo();
    break;
  case SEQ_NO_CreateProc:
    {
      m_pAppParam->type = App::StrInput::TYPE_CHARACTER_NAME_DEBUG;
      m_pAppParam->strBuf = &name;
      App::StrInput::Proc *pProc = GameSys::CallStrInputProc();
      pProc->SetAppParam( m_pAppParam );
    }
    AddSeqNo();
    break;
  case SEQ_NO_WaitProc:
    if( pGameManager->IsProcessExists() ) { break; }
    AddSeqNo();
    break;
  case SEQ_NO_OpenField:
    {
      // オープンイベント
      EventFieldmapOpen *p_open;
      p_open = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( pGameManager->GetGameEventManager() );
      p_open->SetDesc( FM_OPEN_MODE_APP_RECOVER, m_player_location );
      p_open->SetFadeCallBackDefault();
    }
    AddSeqNo();
    break;
  case SEQ_NO_End:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 *  @brief  終了関数
 *  @param  pGameManager    ゲームマネージャ
 */
void EventDebugStrInput::EndFunc( GameSys::GameManager *pGameManager )
{
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
