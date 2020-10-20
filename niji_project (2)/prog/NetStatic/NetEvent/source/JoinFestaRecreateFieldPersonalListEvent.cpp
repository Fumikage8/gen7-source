// ============================================================================
/*
 * @file JoinFestaRecreateFieldPersonalListEvent.cpp
 * @brief ジョインフェスタのフィールド出現用パーソナルデータリストを再作成するイベント
 * @date 2015.12.14
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaRecreateFieldPersonalListEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// ジョインフェスタ動的動作モデルロードシステム
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h" // PersonalEvent
//  ジョインフェスタリソースマネージャ
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"

GFL_NAMESPACE_BEGIN(NetEvent)


JoinFestaRecreateFieldPersonalListEvent::JoinFestaRecreateFieldPersonalListEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap )
{
}


JoinFestaRecreateFieldPersonalListEvent::~JoinFestaRecreateFieldPersonalListEvent()
{
}


bool JoinFestaRecreateFieldPersonalListEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaRecreateFieldPersonalListEvent::InitFunc( GameSys::GameManager* pGameManager )
{
}


void JoinFestaRecreateFieldPersonalListEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaRecreateFieldPersonalListEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  enum{
    SEQ_JF_DYNAMIC_SYSTEM_TERMINATE,    //!< JF動的モデル生成システム破棄 
    SEQ_RECREATE_FIELD_PERSONAL_LIST,   //!< フィールド出現用リストを再作成
    SEQ_JF_DYNAMIC_SYSTEM_INITIALIZE,   //!< JF動的モデル生成システム初期化
    SEQ_JF_DYNAMIC_SYSTEM_INITIALIZE_WAIT,//!< JF動的モデル生成システム初期化待ち
    SEQ_END,
  };

  switch( GetSeqNo() ){
  case SEQ_JF_DYNAMIC_SYSTEM_TERMINATE:    //!< JF動的モデル生成システム破棄 
    {
      // フィールドがあるときのみ、動的モデル生成システムを破棄する
      if( pGameManager->GetFieldmap() != NULL )
      {
        Field::FieldGimmickJoinFesta*             pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
        NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();

        // 破棄待ち
        if( pJfDynamicSystem->Terminate() == NetApp::JoinFesta::RESULT_FINISH )
        {
          // 破棄完了
          SetSeqNo(SEQ_RECREATE_FIELD_PERSONAL_LIST);
        }
      }
      else
      {
        SetSeqNo(SEQ_RECREATE_FIELD_PERSONAL_LIST);
      }
    }
    break;
  case SEQ_RECREATE_FIELD_PERSONAL_LIST:   //!< フィールド出現用リストを再作成
    {
      // フィールド出現用リスト再構築
      //gfl2::heap::HeapBase* pEventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->CreateFieldPersonalList();
      // イベント抽選もリスト構築時に行う
      NetAppLib::JoinFesta::JoinFestaPersonalEvent::PersonalEventActuate();

      SetSeqNo(SEQ_JF_DYNAMIC_SYSTEM_INITIALIZE);
    }
    break;
  case SEQ_JF_DYNAMIC_SYSTEM_INITIALIZE:   //!< JF動的モデル生成システム初期化
    {
      // フィールドがあるときのみ、動的モデル生成システムを初期化する
      if( pGameManager->GetFieldmap() != NULL )
      {
        Field::FieldGimmickJoinFesta*             pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
        NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();
        pJfDynamicSystem->Initialize( pJfGimmick->GetJoinFestaEffectSystem(), pJfGimmick->GetJoinFestaResourceManager()->GetDataTablePtr() );
        SetSeqNo(SEQ_JF_DYNAMIC_SYSTEM_INITIALIZE_WAIT);
      }
      else
      {
        SetSeqNo(SEQ_END);
      }
    }
    break;

  case SEQ_JF_DYNAMIC_SYSTEM_INITIALIZE_WAIT://!< JF動的モデル生成システム初期化待ち
    {
      // フィールドがあるときのみ、動的モデル生成システムを初期化する
      if( pGameManager->GetFieldmap() != NULL )
      {
        Field::FieldGimmickJoinFesta*             pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
        NetApp::JoinFesta::JFDynamicLoadSystem*   pJfDynamicSystem = pJfGimmick->GetJFDynamicLoadSystem();
        if( pJfDynamicSystem->IsInitializeComplete() )
        {
          SetSeqNo(SEQ_END);
        }
      }
    }
    break;
  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}


GFL_NAMESPACE_END(NetEvent)
