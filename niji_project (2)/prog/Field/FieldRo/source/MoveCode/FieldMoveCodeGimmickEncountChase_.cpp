//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountChase.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント接近型
*  @author saita_kazuki
*  @date   2015.02.17
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
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountChase.h"
// PlacementData
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountChaseActor.h"
// GimmickEncount
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
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
  case STATE_WAIT:
    this->MainProcess_StateWait( pMoveCodeWork->pModel, pWork);
    break;
  case STATE_CHASE_PLAYER:
    this->MainProcess_StateChasePlayer( pMoveCodeWork->pModel, pWork);
    break;
  case STATE_MISS:
    this->MainProcess_StateMiss( pMoveCodeWork->pModel, pWork);
    break;
  case STATE_GO_BACK:
    this->MainProcess_StateGoBack( pMoveCodeWork->pModel, pWork);
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
void FieldMoveCodeGimmickEncountChase::Resume( FieldMoveCodeWork *pMoveCodeWork ) const
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作コード処理　中断
 */
//----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::Suspend( FieldMoveCodeWork *pMoveCodeWork ) const
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
void FieldMoveCodeGimmickEncountChase::Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  this->UpdateStateParameter( pWork->state, pCharaDrawInstance, pWork);

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
void FieldMoveCodeGimmickEncountChase::MainProcess_StateNone( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  this->InitializeStateParameter( STATE_WAIT, pCharaDrawInstance, pWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (待機)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::MainProcess_StateWait( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // プレイヤーが範囲に入ったら接近状態に遷移
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  if( pLocalWork->GetNotifyState() == GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT)
  {
    this->InitializeStateParameter( STATE_CHASE_PLAYER, pCharaDrawInstance, pWork);
    return;
  }

  // 状態遷移更新
  this->UpdateStateParameter( STATE_WAIT, pCharaDrawInstance, pWork);

  // SE制御
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->standbySEID, 0);
  // 草シンボルのみ、15フレームで散り草SEを鳴らす
  if( FieldMoveCodeGimmickEncountCommon::IsGrassSymbol( pMoveModel))
  {
    FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, SEQ_SE_FLD_GE_SEKKIN_KUSA_CHIRIKUSA, 15);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (プレイヤーに接近)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::MainProcess_StateChasePlayer( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // プレイヤーが範囲から出たら逃げられた状態に遷移
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  if( pLocalWork->GetNotifyState() == GimmickEncount::NOTIFY_STATE_NONE)
  {
    this->InitializeStateParameter( STATE_MISS, pCharaDrawInstance, pWork);
    return;
  }

  // 接近処理
  gfl2::math::Vector3 charaPos = pCharaDrawInstance->GetPosition();
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  gfl2::math::Vector3 vec = (playerPos - charaPos);

  f32 speed = pWork->moveSpeed;
  f32 length = vec.Length();
  if( length < speed)
  {
    speed = length;
  }

  // @fix NMCat[4040] 類似案件。ゼロベクトルをNormalizeしないようにする。移動量がないということなのでvecには念のためゼロベクトルを入れておく
  if( gfl2::math::IsAlmostZero( vec.Length() ) )
  {
    vec = gfl2::math::Vector3::GetZero();
  }
  else
  {
    vec = vec.Normalize() * speed;
  }
  charaPos += vec;

  pCharaDrawInstance->SetPosition( charaPos);

  // 状態遷移更新
  this->UpdateStateParameter( STATE_CHASE_PLAYER, pCharaDrawInstance, pWork);

  // SE制御
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->moveSEID, 0);
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (プレイヤーに逃げられた)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::MainProcess_StateMiss( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // プレイヤーが範囲に入ったら接近状態に遷移
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  if( pLocalWork->GetNotifyState() == GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT)
  {
    this->InitializeStateParameter( STATE_CHASE_PLAYER, pCharaDrawInstance, pWork);
    return;
  }

  // 待ち時間が経過したら初期位置に戻るに遷移
  if( pWork->waitCnt++ >= 30)
  {
    this->InitializeStateParameter( STATE_GO_BACK, pCharaDrawInstance, pWork);
    return;
  }

  // 状態遷移更新
  this->UpdateStateParameter( STATE_MISS, pCharaDrawInstance, pWork);

  // SE制御
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->moveSEID, 0);
}

//----------------------------------------------------------------------------
/**
*  @brief  状態ごとの処理 (初期位置に戻る)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::MainProcess_StateGoBack( FieldMoveModel* pMoveModel, Work* pWork) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // プレイヤーが範囲に入ったら接近状態に遷移
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  if( pLocalWork->GetNotifyState() == GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT)
  {
    this->InitializeStateParameter( STATE_CHASE_PLAYER, pCharaDrawInstance, pWork);
    return;
  }

  // 移動処理
  gfl2::math::Vector3 firstPos = pLocalWork->GetFirstPosition();

  gfl2::math::Vector3 pos = pCharaDrawInstance->GetPosition();
  gfl2::math::Vector3 vec = (firstPos - pos);

  // @fix NMCat[4040] ゼロベクトルをNormalizeしていたため、ベクトル長判定を計算の前にも行う
  if( gfl2::math::IsAlmostZero( vec.Length() ) )
  {
    this->InitializeStateParameter( STATE_WAIT, pCharaDrawInstance, pWork);
    return;
  }

  f32 speed = pWork->moveSpeed;
  f32 length = vec.Length();
  if( length < speed)
  {
    speed = length;
  }
  gfl2::math::Vector3 moveVec = vec.Normalize() * speed;
  pos += moveVec;
  pCharaDrawInstance->SetPosition( pos);

  // 初期位置に戻ったら待機状態に遷移
  if( gfl2::math::IsAlmostZero( (firstPos - pos).Length() ))
  {
    this->InitializeStateParameter( STATE_WAIT, pCharaDrawInstance, pWork);
    return;
  }

  // 状態遷移更新
  this->UpdateStateParameter( STATE_GO_BACK, pCharaDrawInstance, pWork);

  // SE制御
  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();
  FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pCharaDrawInstance, pLocalWork, playerPos, pWork->moveSEID, 0);
}

/**
 * @brief 指定状態のパラメーターに初期化する
 */
void FieldMoveCodeGimmickEncountChase::InitializeStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const
{
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  GimmickEncount::ChaseData* pData = reinterpret_cast<GimmickEncount::ChaseData*>( pLocalWork->GetTypesData());
  f32 speed = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerRunSpeed();

  gfl2::math::Vector3 playerPos = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerPosition();

  pWork->state = state;

  switch( state)
  {
  case STATE_NONE:
    {
      pWork->moveSpeed    = 0.0f;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = 0;
      pWork->moveSEID     = 0;
    }
    break;

  case STATE_WAIT:
    {
      FieldMoveCodeGimmickEncountCommon::StopSE( pWork->moveSEID);

      pWork->moveSpeed    = 0.0f;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_WAIT);

      // モーションが変更されるのでSE更新
      FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pModel, pLocalWork, playerPos, pWork->standbySEID, 0);
    }
    break;

  case STATE_CHASE_PLAYER:
    {
      FieldMoveCodeGimmickEncountCommon::StopSE( pWork->standbySEID);

      pWork->moveSpeed    = speed * pData->moveSpeedRate;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);

      // モーションが変更されるのでSE更新
      FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( pModel, pLocalWork, playerPos, pWork->moveSEID, 0);
    }
    break;

  case STATE_MISS:
    {
      pWork->moveSpeed    = 0.0f;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);
    }
    break;

  case STATE_GO_BACK:
    {
      pWork->moveSpeed    = (speed * pData->moveSpeedRate) * 0.8f;  // 戻るときは2割減速
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);
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
void FieldMoveCodeGimmickEncountChase::UpdateStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const
{
  GimmickEncount::LocalWork* pLocalWork = pWork->pActor->GetLocalWork();
  GimmickEncount::ChaseData* pData = reinterpret_cast<GimmickEncount::ChaseData*>( pLocalWork->GetTypesData());
  f32 speed = pWork->pActor->GetGimmickEncountManager()->GetWork().GetPlayerRunSpeed();

  pWork->state = state;

  switch( state)
  {
  case STATE_NONE:
    {
      pWork->moveSpeed    = 0.0f;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = 0;
      pWork->moveSEID     = 0;
    }
    break;

  case STATE_WAIT:
    {
      pWork->moveSpeed    = 0.0f;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_WAIT);
    }
    break;

  case STATE_CHASE_PLAYER:
    {
      pWork->moveSpeed    = speed * pData->moveSpeedRate;
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);
    }
    break;

  case STATE_MISS:
    {
      pWork->moveSpeed    = 0.0f;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

      FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( pModel, MOTION_INDEX_MOVE);
    }
    break;

  case STATE_GO_BACK:
    {
      pWork->moveSpeed    = (speed * pData->moveSpeedRate) * 0.8f;  // 戻るときは2割減速
      pWork->waitCnt      = 0;
      pWork->standbySEID  = pData->standbySEID;
      pWork->moveSEID     = pData->moveSEID;

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
