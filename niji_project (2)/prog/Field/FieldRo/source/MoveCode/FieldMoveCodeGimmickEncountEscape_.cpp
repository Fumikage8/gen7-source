//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountEscape.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント逃走型
*  @author saita_kazuki
*  @date   2015.10.07
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeUtility.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncount_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountCommon.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountEscape.h"
// PlacementData
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountEscapeActor.h"
// GimmickEncount
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
// Path
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));

  // SE予約クリア
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  pLocalWork->SetSEParameter( false, SMID_NULL);

  switch( pWork->state)
  {
  case STATE_NONE:
    this->MainProcess_StateNone( pMoveCodeWork->pModel, pWork);
    break;
  case STATE_STANDBY:
    this->MainProcess_StateStandby( pMoveCodeWork->pModel, pWork);
    break;
  case STATE_MOVE_PATH:
    this->MainProcess_StateMovePath( pMoveCodeWork->pModel, pWork);
    break;
  case STATE_VANISH:
    this->MainProcess_StateVanish( pMoveCodeWork->pModel, pWork);
    break;
  }

  // ローカルワークに現在の位置を保存
  GimmickEncount::LocalWork::UpdateDescription desc;
  desc.nowPosition = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetPosition();
  pLocalWork->Update( desc);
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作コード処理　再開
 */
//----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::Resume( FieldMoveCodeWork *pMoveCodeWork ) const
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作コード処理　中断
 */
//----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::Suspend( FieldMoveCodeWork *pMoveCodeWork ) const
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作コード処理  復帰
 *
 *  @param  pMoveCodeWork  動作コードワーク
 *  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
 */
//----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::Comeback( FieldMoveCodeWork *pMoveCodeWork,b32 isLoad ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  this->UpdateStateParameter( pWork->state, pCharaDrawInstance, pWork);

  // パス進捗を復帰
  if( pWork->pathType != PATH_TYPE_NONE)
  {
    TrafficPath *pPath = pWork->pPath[pWork->pathType];
    if( pPath)
    {
      pPath->SetProgress( pWork->progress);
    }
  }

  // @fix GFNMCat[4394 ] 類似案件：ローカルワークの内容を復帰
  if( pCharaDrawInstance )
  {
    GimmickEncount::LocalWork::UpdateDescription desc;
    desc.nowPosition = pCharaDrawInstance->GetPosition();

    GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
    pLocalWork->Update( desc);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (未初期化)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::MainProcess_StateNone( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 状態遷移
  this->InitializeStateParameter( STATE_STANDBY, pCharaDrawInstance, pWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (待機)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::MainProcess_StateStandby( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 状態遷移更新
  this->UpdateStateParameter( STATE_STANDBY, pCharaDrawInstance, pWork);

  // SE制御
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->standbySEID, 0);

  switch( pLocalWork->GetNotifyState())
  {
  case GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT:
  case GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_2_HIT:
    this->InitializeStateParameter( STATE_MOVE_PATH, pCharaDrawInstance, pWork);
    break;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (パス移動)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::MainProcess_StateMovePath( FieldMoveModel* pMoveModel, Work* pWork) const
{
  TrafficPath *pPath = pWork->pPath[pWork->pathType];
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // パスを進捗する
  pPath->Progress( pWork->progressSpeed );

  // パスから位置を更新
  this->UpdateCharaDrawInstanceByTrafficPath( pCharaDrawInstance, pWork->pPath[pWork->pathType]);

  // 状態遷移更新
  this->UpdateStateParameter( STATE_MOVE_PATH, pCharaDrawInstance, pWork);

  // SE制御
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->moveSEID, 0);

  // 進捗を保存
  pWork->progress = pPath->GetProgress();

  if( pWork->progress >= 1.0f)
  {
    this->InitializeStateParameter( STATE_VANISH, pCharaDrawInstance, pWork);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (消滅)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::MainProcess_StateVanish( FieldMoveModel* pMoveModel, Work* pWork) const
{
  // この状態まで来たらステート遷移は行わない

  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 状態遷移更新
  this->UpdateStateParameter( STATE_VANISH, pCharaDrawInstance, pWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  パスからキャラの初期化
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::InitializeCharaDrawInstanceByTrafficPath( poke_3d::model::BaseModel* pCharaDrawInstance, TrafficPath* pPath) const
{
  // パスの位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetStartPosition();
  pCharaDrawInstance->SetPosition( vPos );

  // パスの方向に設定する
  gfl2::math::Vector3 vRot = pPath->GetStartRotation();
  pCharaDrawInstance->SetRotation( vRot );
}

//----------------------------------------------------------------------------
/**
*  @brief  パスからキャラの更新
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::UpdateCharaDrawInstanceByTrafficPath( poke_3d::model::BaseModel* pCharaDrawInstance, TrafficPath* pPath) const
{
  // パスの位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pCharaDrawInstance->SetPosition( vPos );

  // パスの方向に設定する
  gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
  pCharaDrawInstance->SetRotation( vRot );
}

/**
 * @brief 指定状態のパラメーターに初期化する
 */
void FieldMoveCodeGimmickEncountEscape::InitializeStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const
{
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  GimmickEncount::EscapeData* pData = reinterpret_cast<GimmickEncount::EscapeData*>( pLocalWork->GetTypesData());

  f32 speed = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerRunSpeed();
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();

  pWork->state = state;

  switch( state)
  {
  case STATE_NONE:
    {
      pWork->progressSpeed= 0.0f;
      pWork->standbySEID  = 0;
      pWork->moveSEID     = 0;
      pWork->progress     = 0.0f;
      pWork->pathType     = PATH_TYPE_NONE;
    }
    break;

  case STATE_STANDBY:
    {
      pWork->progressSpeed= 0.0f;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;
      pWork->progress     = 0.0f;
      pWork->pathType     = PATH_TYPE_NONE;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_WAIT);

      // モーションが変更されるのでSE更新
      FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pModel, pLocalWork, playerPos, pWork->standbySEID, 0);
    }
    break;

  case STATE_MOVE_PATH:
    {
      pWork->progressSpeed= speed * pData->moveSpeedRate;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;
      pWork->progress     = 0.0f;

      switch( pWork->pActor->GetLocalWork()->GetNotifyState())
      {
      case GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT:
        pWork->pathType = PATH_TYPE_1;
        break;
      case GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_2_HIT:
        pWork->pathType = PATH_TYPE_2;
        break;
      }
      // パス使用開始
      TrafficPath *pPath = pWork->pPath[pWork->pathType];
      pPath->StartUse();
      // パスを初期位置に合わせる
      this->InitializeCharaDrawInstanceByTrafficPath( pModel, pPath);

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);

      // モーションが変更されるのでSE更新
      FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pModel, pLocalWork, playerPos, pWork->moveSEID, 0);
    }
    break;

  case STATE_VANISH:
    {
      // パス使用終了
      TrafficPath *pPath = pWork->pPath[pWork->pathType];
      pPath->EndUse();

      pWork->progressSpeed  = 0.0f;
      pWork->standbySEID    = 0;
      pWork->moveSEID       = 0;
      pWork->progress       = 0.0f;
      pWork->pathType       = PATH_TYPE_NONE;

      pModel->ChangeAnimationBindpose();

      pWork->pActor->UnregistActor();
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
}

/**
 * @brief 指定状態のパラメーターに更新する
 */
void FieldMoveCodeGimmickEncountEscape::UpdateStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const
{
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  GimmickEncount::EscapeData* pData = reinterpret_cast<GimmickEncount::EscapeData*>( pLocalWork->GetTypesData());

  GFL_ASSERT( pWork->state == state );
  pWork->state = state;

  switch( state)
  {
  case STATE_NONE:
    {
      pWork->progressSpeed  = 0.0f;
      pWork->standbySEID    = 0;
      pWork->moveSEID       = 0;
      pWork->progress       = 0.0f;
      pWork->pathType       = PATH_TYPE_NONE;
    }
    break;

  case STATE_STANDBY:
    {
      FieldMoveCodeGimmickEncountCommon::StopSE( pWork->moveSEID);

      pWork->progressSpeed  = 0.0f;
      pWork->standbySEID    = pData->standbySEID;
      pWork->moveSEID       = pData->moveSEID;
      pWork->progress       = 0.0f;
      pWork->pathType       = PATH_TYPE_NONE;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_WAIT);
    }
    break;

  case STATE_MOVE_PATH:
    {
      FieldMoveCodeGimmickEncountCommon::StopSE( pWork->standbySEID);

      f32 speed = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerRunSpeed();

      pWork->progressSpeed  = speed * pData->moveSpeedRate;
      pWork->standbySEID    = pData->standbySEID;
      pWork->moveSEID       = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);
    }
    break;

  case STATE_VANISH:
    {
      pWork->progressSpeed  = 0.0f;
      pWork->standbySEID    = 0;
      pWork->moveSEID       = 0;
      pWork->progress       = 0.0f;
      pWork->pathType       = PATH_TYPE_NONE;

      if( pModel->GetAnimationId() != -1)
      {
        pModel->ChangeAnimationBindpose();
      }
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
