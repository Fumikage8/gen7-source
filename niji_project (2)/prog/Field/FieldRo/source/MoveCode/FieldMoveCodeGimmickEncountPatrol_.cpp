//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountPatrol.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント巡回型
*  @author saita_kazuki
*  @date   2015.09.05
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
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountPatrol.h"
// PlacementData
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountPatrolActor.h"
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
void FieldMoveCodeGimmickEncountPatrol::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
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
  case STATE_MOVE_PATH:
    this->MainProcess_StateMovePath( pMoveCodeWork->pModel, pWork);
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
void FieldMoveCodeGimmickEncountPatrol::Resume( FieldMoveCodeWork *pMoveCodeWork ) const
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作コード処理　中断
 */
//----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountPatrol::Suspend( FieldMoveCodeWork *pMoveCodeWork ) const
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
void FieldMoveCodeGimmickEncountPatrol::Comeback( FieldMoveCodeWork *pMoveCodeWork,b32 isLoad ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  this->UpdateStateParameter( pWork->state, pCharaDrawInstance, pWork);

  // パス進捗を復帰
  TrafficPath *pPath = pWork->pPath;
  pPath->SetProgress( pWork->progress);

  // @fix GFNMCat[4394 ] ローカルワークの内容を飛び出し演出に使用しているので復帰
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
void FieldMoveCodeGimmickEncountPatrol::MainProcess_StateNone( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 状態遷移更新
  this->UpdateStateParameter( STATE_MOVE_PATH, pCharaDrawInstance, pWork);

  // パスに合わせる
  TrafficPath *pPath = pWork->pPath;
  UpdateCharaDrawInstanceByTrafficPath( pCharaDrawInstance, pPath);

  pCharaDrawInstance->ChangeAnimation( MOTION_INDEX_MOVE);
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (パス移動)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountPatrol::MainProcess_StateMovePath( FieldMoveModel* pMoveModel, Work* pWork) const
{
  TrafficPath *pPath = pWork->pPath;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // パスを進捗する
  pPath->Progress( pWork->progressSpeed );

  // パスから位置を更新
  UpdateCharaDrawInstanceByTrafficPath( pCharaDrawInstance, pPath);

  // 状態遷移更新
  this->UpdateStateParameter( STATE_MOVE_PATH, pCharaDrawInstance, pWork);

  // 進捗を保存
  pWork->progress = pPath->GetProgress();

  // SE制御
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->moveSEID, 0);
}

//----------------------------------------------------------------------------
/**
*  @brief  パスからキャラの更新
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountPatrol::UpdateCharaDrawInstanceByTrafficPath( poke_3d::model::BaseModel* pCharaDrawInstance, TrafficPath* pPath) const
{
  // パスの位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pCharaDrawInstance->SetPosition( vPos );

#if 0
  // パスの初期方向に設定する
  gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
  pCharaDrawInstance->SetRotation( vRot );
#endif
}

/**
 * @brief 指定状態のパラメーターに更新する
 */
void FieldMoveCodeGimmickEncountPatrol::UpdateStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const
{
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  GimmickEncount::PatrolData* pData = reinterpret_cast<GimmickEncount::PatrolData*>( pLocalWork->GetTypesData());
  f32 speed = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerWalkSpeed();

  pWork->state = state;

  switch( state)
  {
  case STATE_NONE:
    {
      pWork->progressSpeed = 0.0f;
      pWork->standbySEID   = 0;
      pWork->moveSEID      = 0;
    }
    break;

  case STATE_MOVE_PATH:
    {
      pWork->progressSpeed  = speed * pData->moveSpeedRate;
      pWork->standbySEID    = pData->standbySEID;
      pWork->moveSEID       = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
