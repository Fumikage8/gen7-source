//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEventCheckTrainerEye.cpp
 *  @brief  トレーナー視線エンカウントチェック
 *  @author saita_kazuki
 *  @date   2015.06.02
 */
//=============================================================================

#include "./FieldEventCheckTrainerEye.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"


// collision
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"

// placement
#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"

// script
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/EventScriptCall.h"

// movemodel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// savedata
#include "Savedata/include/EventWork.h"

// trainer
#include "Trainer/Trainer/include/tr_tool.h"
#include "Trainer/Trainer/include/TrainerPairData.h"
#include "Battle/include/battle_def.h"

// conv_header
#include <niji_conv_header/trainer/trid_def.h>
#include <niji_conv_header/field/flagwork/FlagDefine.h>
#include <niji_conv_header/field/script/inc/trainer.inc>

// reference
#include "niji_reference_files/script/FieldPawnTypes.h"

//
#include "Field/FieldRo/include/Fieldmap.h"

#define ENABLE_SCRIPT 1
#if ENABLE_SCRIPT == 0
#include "Field/FieldStatic/include/event/EventBattleCall.h"
#include "System/include/HeapDefine.h"
#endif

#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerEye.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerPath.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"

// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

#define DEBUG_HIT_PRINT_ONLY 0

GFL_NAMESPACE_BEGIN( Field)

/**
 *  @brief 視線エンカウントチェック
 *  @param pGameManager ゲームマネージャー。プレイヤーのパーティーなどの取得に使用
 *  @param pPlayer プレイヤーの動作モデル
 *  @retval true 視線エンカウントバトルイベント発生
 *  @retval false  視線エンカウントなし
 */
bool EventCheckTrainerEye::CheckTrainerEye( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer)
{
  // プレイヤーから視線のDynamicアクターを取得
  DynamicActor* pDynamicActor = pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_TRAINER_EYE);
  if( pDynamicActor == NULL)
  {
    return false; 
  }

  // 衝突したアクターを取得する。NULLなら衝突していない
  BaseActor* pCollisionActor = pDynamicActor->GetCollisionResult( 0)->pTargetActor;
  if( pCollisionActor == NULL)
  {
#if DEBUG_HIT_PRINT_ONLY
    GFL_PRINT( "TrainerEye\n");
#endif
    return false; 
  }

#if DEBUG_HIT_PRINT_ONLY
  GFL_PRINT( "TrainerEye Hit!\n");
  return false;
#endif

  // モデルを取得
  void* pUserData = pCollisionActor->GetUserData();
  FieldTrainerActor* pTrainerActor = reinterpret_cast<FieldTrainerActor*>( pUserData);
  MoveModel::FieldMoveModel* pMoveModel = pTrainerActor->GetMoveModel();

  // トレーナー対戦フラグをチェック。
  if( pTrainerActor->IsBattleFinished())
  {
    return false;
  }

  //視線エンカウントスクリプトコール
  if( CallTrainerScript( pGameManager, pTrainerActor, true ) == false ){
    return false;
  }

  return true;
}


/**
 * @brief 視線トレーナーエンカウントスクリプトコール
 */
bool EventCheckTrainerEye::CallTrainerScript( GameSys::GameManager* pGameManager, FieldTrainerActor* pTrainerActor, bool is_eye  )
{
  FieldScript::FieldScriptSystem* pScriptSystem = pGameManager->GetFieldScriptSystem();
  const FieldTrainerAccessor::TrainerData* pTrainerData = pTrainerActor->GetData();
  MoveModel::FieldMoveModel* pMoveModel = pTrainerActor->GetMoveModel();
  u32 scr_id = SCRID_EYE_ENCOUNT;

  if( !is_eye ){
    scr_id = pTrainerData->scriptID;
  }

  FieldScript::Work::TRAINER_EYE_HIT eyeIndex = FieldScript::Work::TRAINER_EYE_HIT0;
  FieldTrainerActor* pPairTrainerActor = SearchPairTrainerActor( pGameManager, pTrainerActor, &eyeIndex );

  if( pPairTrainerActor != NULL )
  {
    // ☆ダブルバトル

    // プレイヤーのパーティーとバトル参加可能ポケモン数を取得
    pml::PokeParty* pParty = pGameManager->GetGameData()->GetPlayerParty();
    u32 enableMemberNum = pParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE);

    if( is_eye && enableMemberNum < 2)
    {
      // バトル参加可能ポケモンが2匹いないならエンカウント不可
      return false;
    }
    //動作モデルのステータス変更
    SetTrainerModelStatus( pMoveModel );
    SetTrainerModelStatus( pPairTrainerActor->GetMoveModel() );

    EventScriptCall::CallScript( pGameManager, scr_id, pTrainerActor, pMoveModel );

    // 視線トレーナー情報の格納
    FieldScript::Work* pScriptWork = pScriptSystem->GetScriptObject()->GetWork();

    if( eyeIndex == FieldScript::Work::TRAINER_EYE_HIT0 ){
      SetEncountTrainerData( pScriptWork, pTrainerData, FieldScript::Work::TRAINER_EYE_HIT0, FieldScript::SCR_TR_BTL_RULE_DOUBLE );
      SetEncountTrainerData( pScriptWork, pPairTrainerActor->GetData(), FieldScript::Work::TRAINER_EYE_HIT1, FieldScript::SCR_TR_BTL_RULE_DOUBLE );
    }else{
      SetEncountTrainerData( pScriptWork, pTrainerData, FieldScript::Work::TRAINER_EYE_HIT1, FieldScript::SCR_TR_BTL_RULE_DOUBLE );
      SetEncountTrainerData( pScriptWork, pPairTrainerActor->GetData(), FieldScript::Work::TRAINER_EYE_HIT0, FieldScript::SCR_TR_BTL_RULE_DOUBLE );    
    }
  }
  else
  {
    // ☆シングルバトル
    SetTrainerModelStatus( pMoveModel );

    EventScriptCall::CallScript( pGameManager, scr_id, pTrainerActor, pMoveModel );
    // 視線トレーナー情報の格納
    FieldScript::Work* pScriptWork = pScriptSystem->GetScriptObject()->GetWork();
    SetEncountTrainerData( pScriptWork, pTrainerData, FieldScript::Work::TRAINER_EYE_HIT0 , FieldScript::SCR_TR_BTL_RULE_SINGLE );
  }

  // 集中演出を解除
  {
    // カメラフォーカスエフェクト影響のSTOP
    Fieldmap* pFieldMap = pGameManager->GetFieldmap();
    if( pFieldMap->GetCameraManager()->GetFocus() )
    {
      pFieldMap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::EFFECT | Camera::Area::Focus::EventStopBit::BGM | Camera::Area::Focus::EventStopBit::SE | Camera::Area::Focus::EventStopBit::INTERES );
    }
  }

  return true;
}

/**
 * @brief 相方のアクターを見つける
 * @param pGameManager
 * @param pTrainerActor Hitしたトレーナーアクター
 * @param pIndex 相方が見つかった際にデータIndex(Enemy0かEnemy1か?)を0,1で返す
 * @return 相方のアクターがない場合はNULL
 */
FieldTrainerActor* EventCheckTrainerEye::SearchPairTrainerActor( GameSys::GameManager* pGameManager, FieldTrainerActor* pTrainerActor, FieldScript::Work::TRAINER_EYE_HIT* pIndex )
{
  PlacementDataManager* pPlacementData = pGameManager->GetFieldResident()->GetPlacementDataManager();
  const FieldTrainerAccessor::TrainerData* pTrainerData = pTrainerActor->GetData();
  FieldTrainerActor* pPair = NULL;
  trainer::TrainerPairData::TrainerPairIndex pair_index = trainer::TrainerPairData::TRAINER_PAIR_NONE;
  u32 pair_scrid = 0;


  pair_index = pGameManager->GetGameData()->GetTrainerPairData()->SearchPairData( pTrainerData->scriptID, &pair_scrid );

  switch( pair_index ){
  case trainer::TrainerPairData::TRAINER_PAIR_1ST:
    *pIndex = FieldScript::Work::TRAINER_EYE_HIT0;
    break;
  case trainer::TrainerPairData::TRAINER_PAIR_2ND:
    *pIndex = FieldScript::Work::TRAINER_EYE_HIT1;
    break;
  case trainer::TrainerPairData::TRAINER_PAIR_NONE:
  default:
    return NULL;
  }
  pPair = pPlacementData->SearchZoneScriptIDTrainer(pair_scrid);

  return pPair;
}

/**
 * @brief スクリプトワークにエンカウントトレーナー情報を格納
 * @param pScriptWork スクリプトワーク
 * @param pModel モデルワーク
 * @param pTrainerData トレーナーデータ
 * @param data_idx データインデックス
 * @param move_type 移動タイプ
 */
void EventCheckTrainerEye::SetEncountTrainerData(
  FieldScript::Work* pScriptWork,
  const FieldTrainerAccessor::TrainerData* pTrainerData,
  FieldScript::Work::TRAINER_EYE_HIT data_idx,
  u16 move_type
  )
{
  FieldScript::Work::TRAINER_HITDATA* pHitData;
  pHitData = pScriptWork->GetTrainerHitData( data_idx);

  // 格納先取得
  pHitData->event_id  = pTrainerData->eventID;
  pHitData->scr_id    = pTrainerData->scriptID;
  pHitData->move_type = move_type;
}


/**
 * @brief エンカウント時の動作モデルに対するステータス変更処理
 */
void EventCheckTrainerEye::SetTrainerModelStatus( MoveModel::FieldMoveModel* pMoveModel )
{
  // @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
  switch( pMoveModel->GetHeaderWork().moveCodeId)
  {
  case MoveModel::FIELD_MOVE_CODE_TRAINER_EYE:
    {
      u32 size = sizeof( Field::MoveModel::FieldMoveCodeTrainerEye::Work);
      void* work = MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveModel->GetMoveCodeWork(), size);
      Field::MoveModel::FieldMoveCodeTrainerEye::Work *pWork;
      pWork = static_cast<Field::MoveModel::FieldMoveCodeTrainerEye::Work*>( work);
      pWork->isEncount = true;
    }
    break;

  case MoveModel::FIELD_MOVE_CODE_TRAINER_PATH:
    {
      u32 size = sizeof( Field::MoveModel::FieldMoveCodeTrainerPath::Work);
      void* work = MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveModel->GetMoveCodeWork(), size);
      Field::MoveModel::FieldMoveCodeTrainerPath::Work *pWork;
      pWork = static_cast<Field::MoveModel::FieldMoveCodeTrainerPath::Work*>( work);
      pWork->isEncount = true;
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
}


GFL_NAMESPACE_END( Field)

