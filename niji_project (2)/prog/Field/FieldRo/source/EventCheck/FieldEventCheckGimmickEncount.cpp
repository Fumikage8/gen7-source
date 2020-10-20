//======================================================================
/**
 * @file FieldEventCheckGimmickEncount.cpp
 * @date 2015/09/02 12:01:49
 * @author saita_kazuki
 * @brief ギミックエンカウントのコリジョンチェック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./FieldEventCheckGimmickEncount.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"

// collision
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"

// placement
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"

// event
#include "Field/FieldStatic/include/Event/EventGimmickEncount.h"

// movemodel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @brief コリジョンチェック
 *  @param pGameManager ゲームマネージャー
 *  @param pPlayer プレイヤーの動作モデル
 *  @param pEncount エンカウント
 *  @retval true エンカウントバトルイベント発生
 *  @retval false エンカウントなし
 */
bool EventCheckGimmickEncount::Check( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer)
{
  IFieldGimmickEncountActorBase* pEncountActor = CheckEncount( pGameManager, pPlayer);
#if PM_DEBUG
  if( pEncountActor && FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_GIMMICK_ENCOUNT ) )
#else
  if( pEncountActor)
#endif
  {
    EventGimmickEncount::Description desc;
    desc.pActor     = pEncountActor;
    desc.pEncount   = pGameManager->GetFieldmap()->GetEncount();
    desc.pEventWork = pGameManager->GetGameData()->GetEventWork();

    EventGimmickEncount* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGimmickEncount>( pGameManager->GetGameEventManager());
    pEvent->Setup( desc );
    return true;
  }

  CheckAction( pGameManager, pPlayer);

  return false;
}

/**
 *  @brief エンカウントコリジョンチェック
 *  @param pGameManager ゲームマネージャー
 *  @param pPlayer プレイヤーの動作モデル
 *  @retval true エンカウント発生
 *  @retval false エンカウントなし
 */
IFieldGimmickEncountActorBase* EventCheckGimmickEncount::CheckEncount( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer)
{
  DynamicActor* pDynamicActor = pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT);
  if( pDynamicActor == NULL)
  {
    return NULL; 
  }

  BaseActor* pHitActor = pDynamicActor->GetCollisionResult( 0)->pTargetActor;

  if( pHitActor == NULL)
  {
    return NULL;
  }

  void* pUserData = pHitActor->GetUserData();
  IFieldGimmickEncountActorBase* pActor = reinterpret_cast<IFieldGimmickEncountActorBase*>( pUserData);

  pActor->Notify( GimmickEncount::NOTIFY_STATE_ENCOUNT_COLLISION_HIT);

  return pActor;
}

/**
 *  @brief アクションコリジョンチェック
 *  @param pGameManager ゲームマネージャー
 *  @param pPlayer プレイヤーの動作モデル
 */
void EventCheckGimmickEncount::CheckAction( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer)
{
  DynamicActor* pDynamicActor = pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION);
  if( pDynamicActor == NULL)
  {
    return; 
  }

  for( u32 i = 0; i < BaseActor::MAX_COLLISION_RESULT_CNT; ++i)
  {
    BaseActor* pHitActor = pDynamicActor->GetCollisionResult( i)->pTargetActor;

    if( pHitActor != NULL)
    {
      void* pUserData = pHitActor->GetUserData();
      IFieldGimmickEncountActorBase* pActor = reinterpret_cast<IFieldGimmickEncountActorBase*>( pUserData);
      pActor->Notify( GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT);
    }
  }

  return;
}



GFL_NAMESPACE_END( Field )
