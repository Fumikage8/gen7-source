// ============================================================================
/*
 * @file JoinFestaFacilitiesModelUpdateEvent.cpp
 * @brief ジョインフェスタの施設を最新情報で更新するイベント
 * @date 2016.02.06
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFesta/include/Event/JoinFestaFacilitiesModelUpdateEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタ施設システム
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)


JoinFestaFacilitiesModelUpdateEvent::JoinFestaFacilitiesModelUpdateEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap )
{
}


JoinFestaFacilitiesModelUpdateEvent::~JoinFestaFacilitiesModelUpdateEvent()
{
}


bool JoinFestaFacilitiesModelUpdateEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaFacilitiesModelUpdateEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  // @fix GFNMCat[3161]：動的に会話ターゲットなっている配置を削除する事があるのでクリアする
  pGameManager->GetFieldmap()->GetEventCheck()->ClearTalkTarget();
}


void JoinFestaFacilitiesModelUpdateEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaFacilitiesModelUpdateEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GFL_ASSERT( pGameManager->GetFieldmap() );
  GFL_ASSERT( pGameManager->GetFieldmap()->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA );
  Field::FieldGimmickJoinFesta*                 pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = pJfGimmick->GetJoinFestaFacilitiesSystem();

  enum{
    SEQ_FACILITIES_TERMINATE_WAIT,    //!< 施設破棄待ち
    SEQ_FACILITIES_INITIALIZE,        //!< 施設初期化
    SEQ_FACILITIES_INITIALIZE_WAIT,   //!< 施設初期化待ち

    SEQ_END,
  };

  switch( GetSeqNo() ){
  case SEQ_FACILITIES_TERMINATE_WAIT:         //!< 施設破棄
    {
      // 店員動作モデルの破棄終了チェック
      // 全ての施設モデル削除
      if( pJfFacilitiesSystem->TerminateMoveModel() == true &&
          pJfFacilitiesSystem->DeleteAllFacilitiesModel() == true )
      {
        // 全ての施設データ削除
        pJfFacilitiesSystem->DeleteAllFacilitiesData();

        SetSeqNo(SEQ_FACILITIES_INITIALIZE);
      }
    }
    break;
  case SEQ_FACILITIES_INITIALIZE:        //!< 施設初期化
    {
      // 施設データ作成
      pJfFacilitiesSystem->CreateAllFacilitiesData();
      // 店員の動作モデルセットアップ
      pJfFacilitiesSystem->SetupMoveModel();
      SetSeqNo(SEQ_FACILITIES_INITIALIZE_WAIT);
    }
    break;
  case SEQ_FACILITIES_INITIALIZE_WAIT:   //!< 施設初期化待ち
    {
      // 動作モデルセットアップ待ち
      if( pJfFacilitiesSystem->IsSetupMoveModel() )
      {
        // 全ての施設モデル作成
        pJfFacilitiesSystem->CreateAllFacilitiesModel();
        SetSeqNo(SEQ_END);
      }
    }
    break;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)