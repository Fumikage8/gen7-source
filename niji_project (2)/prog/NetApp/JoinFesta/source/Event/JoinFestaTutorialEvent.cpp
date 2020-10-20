// ============================================================================
/*
 * @file JoinFestaTutorialEvent.cpp
 * @brief ジョインフェスタのチュートリアルで人物操作するイベント
 * @date 2016.02.19
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFesta/include/Event/JoinFestaTutorialEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// ジョインフェスタ動的動作モデルロードシステム
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h" // PersonalEvent

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)


//--------------------------------------------------------------
/**
 * @brief   ジョインフェスタからフィールドに遷移するイベント呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 */
//--------------------------------------------------------------
void JoinFestaTutorialEvent::StartEvent( GameSys::GameManager* pGameManager, const BootMode bootMode )
{
  JoinFestaTutorialEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaTutorialEvent>( pGameManager->GetGameEventManager() );
  p_event->SetBootMode( bootMode );
}

JoinFestaTutorialEvent::JoinFestaTutorialEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap ),
 m_bootMode()
{
}


JoinFestaTutorialEvent::~JoinFestaTutorialEvent()
{
}


bool JoinFestaTutorialEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaTutorialEvent::InitFunc( GameSys::GameManager* pGameManager )
{
}


void JoinFestaTutorialEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaTutorialEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  const bool isRoom = ( pGameManager->GetFieldmap()->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;

  // 城外でないと機能しない
  if( isRoom )
  {
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }

  bool isEnd = true;
  switch( m_bootMode ){
  case BOOT_MODE_FIRST:
    {
      isEnd = UpdateBootModeFirst( pGameManager );
    }
    break;
  case BOOT_MODE_SECOND:
    {
      isEnd = UpdateBootModeSecond( pGameManager );
    }
    break;
  }

  if( isEnd )
  {
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}


/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  チュートリアルNPC始めの1人をだす
 *  @return trueで終了
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaTutorialEvent::UpdateBootModeFirst( GameSys::GameManager* pGameManager )
{
  enum{
    SEQ_JF_DYNAMIC_LOAD,      //!< JF動的モデル生成
    SEQ_JF_DYNAMIC_LOAD_WAIT, //!< JF動的モデル生成待ち
    SEQ_END,
  };

  Field::FieldGimmickJoinFesta*             pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  switch( GetSeqNo() ){
  case SEQ_JF_DYNAMIC_LOAD:      //!< JF動的モデル生成
    {
      // 出現リクエスト
      pJfDynamicSystem->DynamickLoadOneRequest( (u32)0 );
      SetSeqNo(SEQ_JF_DYNAMIC_LOAD_WAIT);
    }
    break;
  case SEQ_JF_DYNAMIC_LOAD_WAIT: //!< JF動的モデル生成待ち
    {
      // 出現待ち
      if( pJfDynamicSystem->IsDynamickLoadOneRequestEnd() )
      {
        SetSeqNo(SEQ_END);
      }
    }
    break;
  case SEQ_END:
    return true;
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  チュートリアルNPC次の2人をだす
 *  @return trueで終了
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaTutorialEvent::UpdateBootModeSecond( GameSys::GameManager* pGameManager )
{
  enum{
    SEQ_JF_DYNAMIC_LOAD,      //!< JF動的モデル生成
    SEQ_JF_DYNAMIC_LOAD_WAIT, //!< JF動的モデル生成待ち
    SEQ_JF_DYNAMIC_LOAD2,      //!< JF動的モデル生成
    SEQ_JF_DYNAMIC_LOAD_WAIT2, //!< JF動的モデル生成待ち
    SEQ_END,
  };

  Field::FieldGimmickJoinFesta*             pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  switch( GetSeqNo() ){
  case SEQ_JF_DYNAMIC_LOAD:      //!< JF動的モデル生成
    {
      // 出現リクエスト
      pJfDynamicSystem->DynamickLoadOneRequest( 1 );
      SetSeqNo(SEQ_JF_DYNAMIC_LOAD_WAIT);
    }
    break;
  case SEQ_JF_DYNAMIC_LOAD_WAIT: //!< JF動的モデル生成待ち
    {
      // 出現待ち
      if( pJfDynamicSystem->IsDynamickLoadOneRequestEnd() )
      {
        SetSeqNo(SEQ_JF_DYNAMIC_LOAD2);
      }
    }
    break;
  case SEQ_JF_DYNAMIC_LOAD2:      //!< JF動的モデル生成
    {
      // 出現リクエスト
      pJfDynamicSystem->DynamickLoadOneRequest( 2 );
      SetSeqNo(SEQ_JF_DYNAMIC_LOAD_WAIT2);
    }
    break;
  case SEQ_JF_DYNAMIC_LOAD_WAIT2: //!< JF動的モデル生成待ち
    {
      // 出現待ち
      if( pJfDynamicSystem->IsDynamickLoadOneRequestEnd() )
      {
        SetSeqNo(SEQ_END);
      }
    }
    break;
  case SEQ_END:
    return true;
  }

  return false;
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)
