// ============================================================================
/*
 * @file JoinFestaRecruiterEvent.cpp
 * @brief ジョインフェスタのリクルーターで人物操作するイベント
 * @date 2016.02.19
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFesta/include/Event/JoinFestaRecruiterEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/FieldGimmickJoinFestaRoom.h"
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
void JoinFestaRecruiterEvent::StartEvent( GameSys::GameManager* pGameManager, const BootMode bootMode )
{
  JoinFestaRecruiterEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaRecruiterEvent>( pGameManager->GetGameEventManager() );
  p_event->SetBootMode( bootMode );
}

JoinFestaRecruiterEvent::JoinFestaRecruiterEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap ),
 m_bootMode()
{
}


JoinFestaRecruiterEvent::~JoinFestaRecruiterEvent()
{
}


bool JoinFestaRecruiterEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaRecruiterEvent::InitFunc( GameSys::GameManager* pGameManager )
{
}


void JoinFestaRecruiterEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaRecruiterEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  const bool isRoom = ( pGameManager->GetFieldmap()->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;

  // 城内でないと機能しない
  if( !isRoom )
  {
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }

  bool isEnd = true;
  switch( m_bootMode ){
  case BOOT_MODE_LOAD:
    {
      isEnd = UpdateBootModeLoad( pGameManager );
    }
    break;
  case BOOT_MODE_UNLOAD:
    {
      isEnd = UpdateBootModeUnload( pGameManager );
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
 *  @brief  リクルーターで呼んだ人物を表示させる
 *  @return trueで終了
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaRecruiterEvent::UpdateBootModeLoad( GameSys::GameManager* pGameManager )
{
  enum{
    SEQ_JF_DYNAMIC_LOAD,      //!< JF動的モデル生成
    SEQ_JF_DYNAMIC_LOAD_WAIT, //!< JF動的モデル生成待ち
    SEQ_END,
  };

  Field::FieldGimmickJoinFestaRoom*         pJfGimmick = static_cast<Field::FieldGimmickJoinFestaRoom*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  switch( GetSeqNo() ){
  case SEQ_JF_DYNAMIC_LOAD:      //!< JF動的モデル生成
    {
      // 出現リクエスト
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = &pJoinFestaPersonalDataManager->GetRecruiterPersonal();
      pJfDynamicSystem->DynamickLoadOneRequest( pPersonal );

      SetSeqNo(SEQ_JF_DYNAMIC_LOAD_WAIT);
    }
    break;
  case SEQ_JF_DYNAMIC_LOAD_WAIT: //!< JF動的モデル生成待ち
    {
      // 出現待ち
      if( pJfDynamicSystem->IsDynamickLoadOneRequestEnd() )
      {
        // リクルーターの表示状態を表示に変更
        pJoinFestaPersonalDataManager->SetRecruiterVisibleOn();
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
 *  @brief  リクルーターで呼んだ人物を削除する
 *  @return trueで終了
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaRecruiterEvent::UpdateBootModeUnload( GameSys::GameManager* pGameManager )
{
  enum{
    SEQ_JF_DYNAMIC_UNLOAD,      //!< JF動的モデル破棄
    SEQ_JF_DYNAMIC_UNLOAD_WAIT, //!< JF動的モデル破棄待ち
    SEQ_END,
  };

  Field::FieldGimmickJoinFestaRoom*         pJfGimmick = static_cast<Field::FieldGimmickJoinFestaRoom*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  switch( GetSeqNo() ){
  case SEQ_JF_DYNAMIC_UNLOAD:      //!< JF動的モデル破棄
    {
      // 削除リクエスト
      NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = pJoinFestaPersonalDataManager->GetRecruiterPersonal(); 
      const JoinFestaFriendKey jfFriendKey = jfPersonal.GetJoinFestaFriendKey();

      pJfDynamicSystem->DynamickUnloadOneRequest( jfFriendKey );

      SetSeqNo(SEQ_JF_DYNAMIC_UNLOAD_WAIT);
    }
    break;
  case SEQ_JF_DYNAMIC_UNLOAD_WAIT: //!< JF動的モデル破棄待ち
    {
      // 削除待ち
      if( pJfDynamicSystem->IsDynamickUnloadOneRequestEnd() )
      {
        // リクルーターを削除
        GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetRecruiterPersonal();
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
