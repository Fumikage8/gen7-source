//=================================================================================================
/**
*  @file   FieldMoveCodeTrainerEye.cpp
*  @brief  フィールド視線トレーナー用 動作コード
*  @author saita_kazuki
*  @date   2015.06.09
*/
//=================================================================================================
// gfl2
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
// Singletonアクセス用
#include "Field/FieldRo/include/Fieldmap.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// Field
#include "Field/FieldRo/include/Traffic/FieldTrafficActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
// Effect
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaRipple.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerEye.h"
// Collision
#include "Field/FieldRo/include/Collision/BaseShape.h"
// Convヘッダー
#include <niji_conv_header/field/flagwork/FlagDefine.h>
#include "niji_conv_header/field/chara/p2_base_fi.h"

namespace Field{ namespace MoveModel {

// 状態ごとのメインプロセス関数群
const FieldMoveCodeTrainerEye::StateFunc FieldMoveCodeTrainerEye::m_scStateFunc[ FieldMoveCodeTrainerEye::STATE_MAX ] =
{
  &FieldMoveCodeTrainerEye::MainProcess_StateNone,
  &FieldMoveCodeTrainerEye::MainProcess_StateNotMoveSearch,
  &FieldMoveCodeTrainerEye::MainProcess_StateBreak,
  &FieldMoveCodeTrainerEye::MainProcess_StateWait,
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  (this->*m_scStateFunc[pWork->state])( pMoveCodeWork->pModel, pWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  再開
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // バトル前後でモーション再生スピードを変える
  if( pWork->pTrainerActor->IsBattleFinished())
  {
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame(1.0f);
  }
  else
  {
    Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame(pWork->motionSpeedRate);
  }

  // @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
  // @fix GFNMCat[887] イベント状態のV字回復時にはエンカウント情報を維持する
  if( pWork->isEncount && !pMoveCodeWork->pModel->IsNeedForceResume() )
  {
    pWork->isEncount = false;
  }

  // @fix GFNMCat[773]
  Field::Effect::IEffectBase *pEffect = pMoveCodeWork->pModel->GetCharaRipple();
  if( pEffect )
  {
    Field::Effect::EffectCharaRipple *pRipple = reinterpret_cast<Field::Effect::EffectCharaRipple*>( pEffect );
    pRipple->Resume();
  }
};

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  中断
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  Field::MoveModel::FieldMoveModel *pModel = pMoveCodeWork->pModel;
  if( pWork->state == STATE_NONE )
  {
    MainProcess_StateNone( pModel, pWork );
  }

  // 速度際限のため退避
  pWork->motionSpeedRate = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();

  // @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
  if( pWork->isEncount == false)
  {
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame(0.0f);
    // @fix NMCat[668] 戦闘するトレーナーはエフェクトを止めない
    // @fix GFNMCat[773]
    Field::Effect::IEffectBase *pEffect = pMoveCodeWork->pModel->GetCharaRipple();
    if( pEffect )
    {
      Field::Effect::EffectCharaRipple *pRipple = reinterpret_cast<Field::Effect::EffectCharaRipple*>( pEffect );
      pRipple->Suspend();
    }
  }

  pWork->comebackAnimFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationFrame();

};

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  復帰
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::Comeback( FieldMoveCodeWork *pMoveCodeWork,b32 isLoad ) const
{
  // ワーク取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // モデル取得
  poke_3d::model::BaseModel*  pCharaDrawInstance  = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 対戦フラグをチェックして分岐
  if( pWork->pTrainerActor->IsBattleFinished())
  {
    // 待機状態にする
    pWork->state = STATE_WAIT;
    u32 IdleMotionID = pMoveCodeWork->pModel->GetDefaultIdleAnimationId();
    this->ChangeAnimation( pMoveCodeWork->pModel, pWork, IdleMotionID);
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame(1.0f);
  }
  else
  {
    // @fix GFNMCat[921]:初期化前ComeBackに対応
    if( pWork->state != STATE_NONE )
    {
      // サーチモーションにする
      this->ChangeAnimationSmooth( pMoveCodeWork->pModel, pWork, pWork->motionID );
      pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationFrame(pWork->comebackAnimFrame);
      pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame(pWork->comebackAnimStepFrame);
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  アニメーション変更
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::ChangeAnimationSmooth( FieldMoveModel* pModel, Work* pWork, u32 motionID, f32 stepFrame, bool isLoop, u32 changeFrame) const
{
  poke_3d::model::CharaModel* pCharaModelInstance = pModel->GetCharaDrawInstance();
  pCharaModelInstance->ChangeAnimationSmooth( motionID, changeFrame);
  pCharaModelInstance->SetAnimationLoop( isLoop);
  pCharaModelInstance->SetAnimationStepFrame( stepFrame);
}
void FieldMoveCodeTrainerEye::ChangeAnimation( FieldMoveModel* pModel, Work* pWork, u32 motionID, f32 stepFrame, bool isLoop) const
{
  poke_3d::model::CharaModel* pCharaModelInstance = pModel->GetCharaDrawInstance();
  pCharaModelInstance->ChangeAnimation( motionID);
  pCharaModelInstance->SetAnimationLoop( isLoop);
  pCharaModelInstance->SetAnimationStepFrame( stepFrame);
}

//----------------------------------------------------------------------------
/**
*  @brief  ジョイントからコリジョン更新
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::UpdateCollisionShapeByJoint( FieldMoveModel* pModel, Work* pWork) const
{
  // ジョイントのワールド行列を取得
  poke_3d::model::BaseModel*  pCharaDrawInstance  = pModel->GetCharaDrawInstance();
  gfl2::math::Matrix34        jointHeadMtx        = pCharaDrawInstance->GetJointWorldMatrix( pWork->jointHeadIndex);
  gfl2::math::Matrix34        locatorEyeMtx       = pCharaDrawInstance->GetJointWorldMatrix( pWork->locatorEyeIndex);

  // ジョイントの位置、視線パラメータを取得
  gfl2::math::Vector3 eyePos    = jointHeadMtx.GetElemPosition();
  gfl2::math::Vector3 eyeParam  = locatorEyeMtx.GetElemScaling();

  f32 eyeX = (eyeParam.x * 0.5f);
  f32 eyeY = (eyeParam.y) * pWork->eyeHeightRate;
  f32 eyeZ = ((eyeParam.z * pWork->eyeLengthRate) * 0.5f);

  // シェイプのローカルなx軸、z軸を表すベクトルを求める
  gfl2::math::Vector3 axisX, axisZ;

  // 位置のオフセットを求める
  gfl2::math::Vector3 offset( 0.0f, 0.0f, eyeZ);
  offset = jointHeadMtx.TransformNormal( offset);

  gfl2::math::Vector3 cx( 0.0f, -1.0f, 0.0f);
  gfl2::math::Vector3 cz( 0.0f, 0.0f, 1.0f);
  axisX = jointHeadMtx.TransformNormal( cx).Normalize();
  axisZ = jointHeadMtx.TransformNormal( cz).Normalize();

  StaticActor* pStaticActor = pModel->GetStaticActor( COLLISION_STATIC_TYPE_TRAINER_EYE );
  gfl2::math::Vector3 parentPos;
  parentPos = pStaticActor->GetPosition();

  // シェイプを更新
  
  {
    // 最新の視線を視線内包するBOX形状、フォーカス演出で使用 @note 判定としては線でよいが、実装時期を鑑みて細い箱にする事になった
    f32 width = 5.0f;
    gfl2::math::Vector3 boxOffset( offset );
    boxOffset.y -= width;

    BoxShape* pBoxShape = reinterpret_cast<BoxShape*>( pStaticActor->GetShape());
    pBoxShape->m_pos    = (eyePos - parentPos) + boxOffset;
    pBoxShape->m_e0     = width;
    pBoxShape->m_e1     = eyeZ;
    pBoxShape->m_u0     = axisX;
    pBoxShape->m_u1     = axisZ;
    pBoxShape->m_height = width * 2.0f;

    // 判定は無効
    pStaticActor->SetActive(false);
  }

  {
    offset.y = 0.0f;
    if( !pWork->isUseOldEyePos )
    {
      pWork->oldEyePos      = (eyePos - parentPos) + (axisZ*eyeZ*2.0f);
      pWork->isUseOldEyePos = true;
    }
    StaticActor* pStaticActor = pModel->GetStaticActor( COLLISION_STATIC_TYPE_TRAINER_EYE_NEW);
    TriangleShape* pTriangleShape = reinterpret_cast<TriangleShape*>( pStaticActor->GetShape());
    pTriangleShape->m_pos0 = (eyePos - parentPos);
    pTriangleShape->m_pos1 = (eyePos - parentPos) + (axisZ*eyeZ*2.0f);
    pTriangleShape->m_pos2 = pWork->oldEyePos;
    pWork->oldEyePos = pTriangleShape->m_pos1;

    // カリング中は判定を無効
    if( pModel->IsEnableCulilng() )
    {
      pStaticActor->SetActive(false);
      pWork->isUseOldEyePos = false;
    }
    else
    {
      pStaticActor->SetActive(true);
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  無効なパラメーターでコリジョン更新
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::UpdateCollisionShapeByDisableParameter( FieldMoveModel* pModel) const
{
  StaticActor* pStaticActor = pModel->GetStaticActor( COLLISION_STATIC_TYPE_TRAINER_EYE);
  BoxShape* pBoxShape = reinterpret_cast<BoxShape*>( pStaticActor->GetShape());
  pBoxShape->m_pos    = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f);
  pBoxShape->m_e0     = 0.0f;
  pBoxShape->m_e1     = 0.0f;
  pBoxShape->m_height = 0.0f;
  pBoxShape->m_u0     = gfl2::math::Vector3( 1.0f, 0.0f, 0.0f);
  pBoxShape->m_u1     = gfl2::math::Vector3( 0.0f, 0.0f, 1.0f);

  {
    StaticActor* pStaticActor = pModel->GetStaticActor( COLLISION_STATIC_TYPE_TRAINER_EYE_NEW);
    TriangleShape* pTriangleShape = reinterpret_cast<TriangleShape*>( pStaticActor->GetShape());
    pTriangleShape->m_pos0   = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f);
    pTriangleShape->m_pos1   = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f);
    pTriangleShape->m_pos2   = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  状態：なし(未初期化)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::MainProcess_StateNone( FieldMoveModel* pModel, Work* pWork) const
{
  // トレーナー対戦フラグをチェックして視線トレーナーの状態を分岐
  if( pWork->pTrainerActor->IsBattleFinished())
  {
    // エンカウントしているので待機にする

    // ワーク設定
    pWork->state = STATE_WAIT;

    // モーション設定
    poke_3d::model::BaseModel*  pCharaDrawInstance  = pModel->GetCharaDrawInstance();
    u32 IdleMotionID = pModel->GetDefaultIdleAnimationId();
    if( pCharaDrawInstance->GetAnimationId() != IdleMotionID )
    {
      this->ChangeAnimation( pModel, pWork, IdleMotionID);
    }
  }
  else
  {
    // エンカウントしていないので探索にする

    const FieldTrainerAccessor::MoveData* pMoveData = pWork->pTrainerActor->GetMoveData();

    poke_3d::model::BaseModel*  pCharaDrawInstance  = pModel->GetCharaDrawInstance();

    // ワーク設定
    pWork->state            = STATE_NOT_MOVE_SEARCH;
    pWork->motionID         = pMoveData->motionID;
    pWork->jointHeadIndex   = pCharaDrawInstance->GetJointIndex( "Head");
    pWork->locatorEyeIndex  = pCharaDrawInstance->GetJointIndex( "Loc_Eye");

    // モーション設定
    if( pCharaDrawInstance->GetAnimationId() != pWork->motionID )
    {
      this->ChangeAnimation( pModel, pWork, pWork->motionID );
      pCharaDrawInstance->SetAnimationFrame( pMoveData->startFrame );
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  状態：探索中(エンカウント可、移動しない)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::MainProcess_StateNotMoveSearch( FieldMoveModel* pModel, Work* pWork) const
{
  this->UpdateCollisionShapeByJoint( pModel, pWork);

#ifdef ENABLE_TEST_EYE_DRAW
  this->UpdateEyeModel( pModel, pWork);
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  状態：探索していない(エンカウント不可)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::MainProcess_StateBreak( FieldMoveModel* pModel, Work* pWork) const
{
  // 探索していない状態で、視線トレーナー用モーションではないときに使用する
  // 視線トレーナー用モーション以外は視線ジョイントのスケール値(視線パラメーター)を操作しない

  // ※ 現状未使用

  this->UpdateCollisionShapeByDisableParameter( pModel);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  状態：待機(エンカウント後)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::MainProcess_StateWait( FieldMoveModel* pModel, Work* pWork) const
{
  // 特に何もしない
}

#ifdef ENABLE_TEST_EYE_DRAW
//----------------------------------------------------------------------------
/**
*  @brief  視線描画用 更新処理
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::UpdateEyeModel( FieldMoveModel* pModel, Work* pWork) const
{
  // トレーナーアクター取得
  StaticActor*        pStaticActor  =  pModel->GetStaticActor( COLLISION_STATIC_TYPE_TRAINER_EYE);
  void*               pUserData     =  pStaticActor->GetUserData();
  FieldTrainerActor*  pTrainerActor =  reinterpret_cast<FieldTrainerActor*>( pUserData);

  poke_3d::model::BaseModel* pEyeModel = pTrainerActor->GetEyeModel();

  b32 isEnableTrainerEye = false; /*FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_TRAINER_EYE_DRAW );*/
  if( isEnableTrainerEye != pEyeModel->IsVisible() )
  {
    pEyeModel->SetVisible( isEnableTrainerEye);
  }
  if( isEnableTrainerEye == false)
  {
    return;
  }

  // ジョイント行列取得
  poke_3d::model::BaseModel*  pCharaDrawInstance  = pModel->GetCharaDrawInstance();
  gfl2::math::Matrix34        jointHeadMtx        = pCharaDrawInstance->GetJointWorldMatrix( pWork->jointHeadIndex);
  gfl2::math::Matrix34        locatorEyeMtx       = pCharaDrawInstance->GetJointWorldMatrix( pWork->locatorEyeIndex);

  // 視線パラメータを取得
  gfl2::math::Vector3 eyeParam  = locatorEyeMtx.GetElemScaling();
  f32 eyeZ = ((eyeParam.z * pWork->eyeLengthRate) * 0.5f);

  // 位置
  gfl2::math::Vector3 offset( 0.0f, 0.0f, eyeZ);
  offset = jointHeadMtx.TransformNormal( offset);

  gfl2::math::Vector3 pos = jointHeadMtx.GetElemPosition();
  pos += offset;

  // 回転
  gfl2::math::Quaternion rot;
  rot.MatrixToQuaternion( jointHeadMtx);

  // スケール
  gfl2::math::Vector3 scl;
  scl.x = (eyeParam.x * pWork->eyeHeightRate ) / 100.0f;
  scl.y = (eyeParam.y) / 100.0f;
  scl.z = (eyeParam.z * pWork->eyeLengthRate) / 100.0f;

  // 視線モデル更新
  pEyeModel->SetPosition( pos);
  pEyeModel->SetRotationQuat( rot);
  pEyeModel->SetScale( scl);
}
#endif

}; //end of namespace MoveModel
}; //end of namespace Field
