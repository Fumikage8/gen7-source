//======================================================================
/**
 * @file FieldMoveCodeTrainerPath.cpp
 * @date 2015/07/30 19:34:49
 * @author miyachi_soichi
 * @brief パス有トレーナー動作コード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// Field
#include "Field/FieldRo/include/Traffic/FieldTrafficActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerPath.h"
// Effect
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaRipple.h"
// Collision
#include "Field/FieldRo/include/Collision/BaseShape.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)

// 旋回が必要になる角度
static const f32 NEED_TURN_RAD = gfl2::math::ConvDegToRad( 30.0f );

/**
 * @class FieldMoveCodeTrainerPath
 * @brief パス有トレーナー動作コード
 */

/**
 *  @brief  動作コード処理  メイン
 */
void FieldMoveCodeTrainerPath::MainProcess( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );

  Field::MoveModel::FieldMoveModel *pModel = pMoveCodeWork->pModel;

  switch( pWork->state )
  {
  case STATE_NONE:
    MainProcess_StateNone( pModel, pWork );
    break;
  case STATE_WALK:
    MainProcess_StateWalk( pModel, pWork );
    break;
  case STATE_CORNER:
    MainProcess_StateCorner( pModel, pWork );
    break;
  case STATE_TURN_BEFORE_EX:
    MainProcess_StateTurnBeforeEx( pModel, pWork );
    break;
  case STATE_IDLE:
    MainProcess_StateIdle( pModel, pWork );
    break;
  case STATE_TURN_AFTER_EX:
    MainProcess_StateTurnAfterEx( pModel, pWork );
    break;
  case STATE_WALKSTAY:
    MainProcess_StateWalkStay( pModel, pWork );
    break;
  case STATE_WAIT:
    MainProcess_StateWait( pModel, pWork );
    break;
  }

  // パス進捗を保存
  SaveProgress( pWork );
}

/**
 *  @brief  動作コード処理  再開
 */
void FieldMoveCodeTrainerPath::Resume( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );

  if( pWork->pTrainerActor->IsBattleFinished())
  {
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( 1.0f );
  }
  else
  {
    Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( pWork->motionSpeedRatio );
  }

  // @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
  if( pWork->isEncount)
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

  // 移動量は無視する
  pMoveCodeWork->pModel->OnStateBit( MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
}

/**
 *  @brief  動作コード処理  中断
 */
void FieldMoveCodeTrainerPath::Suspend( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  Field::MoveModel::FieldMoveModel *pModel = pMoveCodeWork->pModel;
  if( pWork->state == STATE_NONE )
  {
    MainProcess_StateNone( pModel, pWork );
  }

  pWork->motionSpeedRatio = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();

  // @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
  if( pWork->isEncount == false)
  {
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( 0.0f );
    // @fix NMCat[668] 戦闘するトレーナーはエフェクトを止めない
    // @fix GFNMCat[773]
    Field::Effect::IEffectBase *pEffect = pMoveCodeWork->pModel->GetCharaRipple();
    if( pEffect )
    {
      Field::Effect::EffectCharaRipple *pRipple = reinterpret_cast<Field::Effect::EffectCharaRipple*>( pEffect );
      pRipple->Suspend();
    }
  }
}

/**
 *  @brief  動作コード処理  復帰
 */
void FieldMoveCodeTrainerPath::Comeback( FieldMoveCodeWork *pMoveCodeWork,b32 isLoad ) const
{
  // ワーク取得
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );

  // 対戦フラグをチェックして分岐
  if( pWork->pTrainerActor->IsBattleFinished())
  {
    // エンカウント後状態にする
    pWork->state = STATE_WAIT;
    u32 IdleMotionID = pMoveCodeWork->pModel->GetDefaultIdleAnimationId();
    ChangeAnimation( pMoveCodeWork->pModel, pWork, IdleMotionID );
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( 1.0f );
  }
  else
  {
    // 終了時に取得していたアニメーションにする
    ChangeAnimationSmooth( pMoveCodeWork->pModel, pWork, pWork->comebackAnimID);
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationFrame( pWork->comebackAnimFrame );
    pMoveCodeWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( pWork->comebackAnimStepFrame );
  }

  // パス進捗の復帰
  Field::TrafficPath *pPath = pWork->pTrainerActor->GetTrafficPath();
  pPath->SetProgress( pWork->progress);
  pPath->SetFlags( TrafficPath::FLAG_REVERSE, pWork->isReverse );

  // @fix NMCat[1090]:Quat->Vec3
  {
    gfl2::math::Quaternion quat;
    gfl2::math::Matrix34 mtx;
    gfl2::math::Vector3 dir( 0.0f, 0.0f, 0.0f );
    f32 rot_x = 0.0f;
    f32 rot_y = 0.0f;
    f32 rot_z = 0.0f;
    poke_3d::model::BaseModel *pBaseModel = pMoveCodeWork->pModel->GetCharaDrawInstance();

    quat = pBaseModel->GetRotationQuat();
    quat.QuaternionToMatrix( mtx );
    dir = mtx.TransformNormal( gfl2::math::Vector3::GetZUnit() );
    f32 dst_y = dir.y;
    dir.y = 0.0f;
    rot_y = gfl2::math::Atan2Rad( dir.x, dir.z );
    rot_x = gfl2::math::Atan2Rad( dst_y, dir.Length() );

    gfl2::math::Vector3 rot( rot_x, rot_y, rot_z );
    pBaseModel->SetRotation( rot );
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  メイン処理  未初期化
 */
void FieldMoveCodeTrainerPath::MainProcess_StateNone( FieldMoveModel *pModel, Work *pWork ) const
{
  Field::TrafficPath *pPath = pWork->pTrainerActor->GetTrafficPath();

  // 対戦フラグをチェックして分岐
  if( pWork->pTrainerActor->IsBattleFinished() || !pPath )
  {
    // 待機へ
    pWork->state = STATE_WAIT;
    // モーション設定
    u32 IdleMotionID = pModel->GetDefaultIdleAnimationId();
    ChangeAnimation( pModel, pWork, IdleMotionID );
  }
  else
  {
    poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();
    // 開始位置
    gfl2::math::Vector3 pos = pPath->GetStartPosition();
    gfl2::math::Vector3 rot = pPath->GetStartRotation();
    pBaseModel->SetPosition( pos );
    pBaseModel->SetRotation( rot );
    // ワーク設定
    pWork->state            = STATE_WALK;
    pWork->jointHeadIndex   = pBaseModel->GetJointIndex( "Head" );
    pWork->locatorEyeIndex  = pBaseModel->GetJointIndex( "Loc_Eye" );
    // モーション設定
    ChangeAnimation( pModel, pWork, P2_BASE_FI010_WALK01 );
    // 移動量は無視する
    pModel->OnStateBit( MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
  }
}

/**
 *  @brief  メイン処理  パス移動
 */
void FieldMoveCodeTrainerPath::MainProcess_StateWalk( FieldMoveModel *pModel, Work *pWork ) const
{
  // トレーナーの動作データを取得
  poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();
  FieldTrainerActor *pActor = pWork->pTrainerActor;
  Field::TrafficPath *pPath = pWork->pTrainerActor->GetTrafficPath();

  if( !pActor->CheckEnableWalk() )
  {
    // 歩けない
    pWork->state = STATE_WALKSTAY;
    u32 IdleMotionID = pModel->GetDefaultIdleAnimationId();
    ChangeAnimation( pModel, pWork, IdleMotionID );
    return;
  }

  // パスを進捗する
  f32 fOldProgress = pPath->GetProgress();
  gfl2::math::Vector3 vSpd = pBaseModel->GetWalkSpeed( -1.0f );
  pPath->Progress( vSpd.z );
  f32 fNowProgress = pPath->GetProgress();

  // パスの方向に設定する
  if( !pPath->CheckFlags( TrafficPath::FLAG_CORNER ) )
  {
    // 曲がり角は後で設定する
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    pBaseModel->SetRotation( vRot );
  }

  // 演出点を算出する
  const Field::Traffic::PathPointData *pPointData = CalcPathPointData( pWork->pTrainerActor, fNowProgress, fOldProgress );

  // 演出点に到達した
  if( pPointData )
  {
    pWork->repeatCount = pPointData->count;
    pWork->startFrame = pPointData->startFrame;
    pWork->motionID = pPointData->motionID;
    f32 rotY = pPointData->rotateY;

    gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
    f32 diff_rot = gfl2::math::NormalizeRadianAngle( rotY - self_rot.y );
    
    if( NEED_TURN_RAD <= gfl2::math::Abs<f32>( diff_rot ) )
    {
      // 旋回が必要
      u32 motion = 0;
      if( diff_rot < 0.0f )   { motion = P2_BASE_FI016_TURN01_RIGHT; }
      else                    { motion = P2_BASE_FI017_TURN01_LEFT; }
      ChangeAnimation( pModel, pWork, motion, false );
      f32 frame = pBaseModel->GetAnimationEndFrame();

      pWork->state = STATE_TURN_BEFORE_EX;
      pWork->delta_rot = diff_rot / frame;
    }
    else
    {
      gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
      self_rot.y = rotY;
      pBaseModel->SetRotation( self_rot );
      // そのまま固有へ
      pWork->state = STATE_IDLE;
      ChangeAnimation( pModel, pWork, pWork->motionID );
      pBaseModel->SetAnimationFrame( pWork->startFrame );
    }
  }
  else if( pPath->CheckFlags( TrafficPath::FLAG_CORNER ) )
  {
    // 曲がり角対応
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
    f32 diff_rot = gfl2::math::NormalizeRadianAngle( vRot.y - self_rot.y );

    u32 motion = 0;
    if( diff_rot < 0.0f )   { motion = P2_BASE_FI016_TURN01_RIGHT; }
    else                    { motion = P2_BASE_FI017_TURN01_LEFT; }
    ChangeAnimation( pModel, pWork, motion, false );
    f32 frame = pBaseModel->GetAnimationEndFrame();

    pWork->state = STATE_CORNER;
    pWork->delta_rot = diff_rot / frame;
  }

  // パスの位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pBaseModel->SetPosition( vPos );

  // 視線の更新
  UpdateCollisionShapeByJoint( pModel, pWork );
#ifdef ENABLE_TEST_EYE_DRAW
  UpdateEyeModel( pModel, pWork );
#endif // ENABLE_TEST_EYE_DRAW
}

/**
 *  @brief  メイン処理  曲がり角対応
 */
void FieldMoveCodeTrainerPath::MainProcess_StateCorner( FieldMoveModel *pModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();

  if( pBaseModel->IsAnimationLastFrame() )
  {
    pWork->state = STATE_WALK;
    ChangeAnimation( pModel, pWork, P2_BASE_FI010_WALK01 );
  }
  else
  {
    gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
    self_rot.y += pWork->delta_rot;
    pBaseModel->SetRotation( self_rot );
  }

  // 視線の更新
  UpdateCollisionShapeByJoint( pModel, pWork );
#ifdef ENABLE_TEST_EYE_DRAW
  UpdateEyeModel( pModel, pWork );
#endif // ENABLE_TEST_EYE_DRAW
}

/**
 *  @brief  メイン処理  固有前旋回
 */
void FieldMoveCodeTrainerPath::MainProcess_StateTurnBeforeEx( FieldMoveModel *pModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();

  if( pBaseModel->IsAnimationLastFrame() )
  {
    pWork->state = STATE_IDLE;
    ChangeAnimation( pModel, pWork, pWork->motionID );
    pBaseModel->SetAnimationFrame( pWork->startFrame );
  }
  else
  {
    gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
    self_rot.y += pWork->delta_rot;
    pBaseModel->SetRotation( self_rot );
  }

  // 視線の更新
  UpdateCollisionShapeByJoint( pModel, pWork );
#ifdef ENABLE_TEST_EYE_DRAW
  UpdateEyeModel( pModel, pWork );
#endif // ENABLE_TEST_EYE_DRAW
}

/**
 *  @brief  メイン処理  固有モーション再生
 */
void FieldMoveCodeTrainerPath::MainProcess_StateIdle( FieldMoveModel *pModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();

  f32 frame = pBaseModel->GetAnimationFrame();
  f32 step = pBaseModel->GetAnimationStepFrame();

  if( frame <= pWork->startFrame && pWork->startFrame < frame + step )
  {
    -- pWork->repeatCount;
    if( pWork->repeatCount <= 0 )
    {
      // トレーナーの動作データを取得
      Field::TrafficPath *pPath = pWork->pTrainerActor->GetTrafficPath();
      gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
      gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
      f32 diff_rot = gfl2::math::NormalizeRadianAngle( vRot.y - self_rot.y );

      if( NEED_TURN_RAD <= gfl2::math::Abs<f32>( diff_rot ) )
      {
        // 旋回が必要
        u32 motion = 0;
        if( diff_rot < 0.0f )   { motion = P2_BASE_FI016_TURN01_RIGHT; }
        else                    { motion = P2_BASE_FI017_TURN01_LEFT; }
        ChangeAnimation( pModel, pWork, motion, false );
        f32 frame = pBaseModel->GetAnimationEndFrame();

        pWork->state = STATE_TURN_AFTER_EX;
        pWork->delta_rot = diff_rot / frame;
      }
      else
      {
        // パスの方向に設定する
        gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
        pBaseModel->SetRotation( vRot );
        // そのまま移動へ
        pWork->state = STATE_WALK;
        ChangeAnimation( pModel, pWork, P2_BASE_FI010_WALK01 );
      }
    } 
  }

  // 視線の更新
  UpdateCollisionShapeByJoint( pModel, pWork );
#ifdef ENABLE_TEST_EYE_DRAW
  UpdateEyeModel( pModel, pWork );
#endif // ENABLE_TEST_EYE_DRAW
}

/**
 *  @brief  メイン処理  固有後旋回
 */
void FieldMoveCodeTrainerPath::MainProcess_StateTurnAfterEx( FieldMoveModel *pModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();

  if( pBaseModel->IsAnimationLastFrame() )
  {
    pWork->state = STATE_WALK;
    ChangeAnimation( pModel, pWork, P2_BASE_FI010_WALK01 );
  }
  else
  {
    gfl2::math::Vector3 self_rot = pBaseModel->GetRotation();
    self_rot.y += pWork->delta_rot;
    pBaseModel->SetRotation( self_rot );
  }

  // 視線の更新
  UpdateCollisionShapeByJoint( pModel, pWork );
#ifdef ENABLE_TEST_EYE_DRAW
  UpdateEyeModel( pModel, pWork );
#endif // ENABLE_TEST_EYE_DRAW
}

/**
 *  @brief  メイン処理  歩けるまで待つ
 */
void FieldMoveCodeTrainerPath::MainProcess_StateWalkStay( FieldMoveModel *pModel, Work *pWork ) const
{
  FieldTrainerActor *pActor = pWork->pTrainerActor;
  poke_3d::model::BaseModel *pBaseModel = pModel->GetCharaDrawInstance();

  if( pActor->CheckEnableWalk() )
  {
    // 移動できる
    pWork->state = STATE_WALK;
    ChangeAnimation( pModel, pWork, P2_BASE_FI010_WALK01 );
  }

  // 視線の更新
  UpdateCollisionShapeByJoint( pModel, pWork );
#ifdef ENABLE_TEST_EYE_DRAW
  UpdateEyeModel( pModel, pWork );
#endif // ENABLE_TEST_EYE_DRAW
}

/**
 *  @brief  メイン処理  待機（エンカウント後）
 */
void FieldMoveCodeTrainerPath::MainProcess_StateWait( FieldMoveModel *pModel, Work *pWork ) const
{
  // 何もしない
}

/**
 *  @brief  演出点の算出
 */
const Field::Traffic::PathPointData *FieldMoveCodeTrainerPath::CalcPathPointData( Field::FieldTrainerActor *pTrainerActor, f32 fNowProgress, f32 fOldProgress ) const
{
  const Field::Traffic::PathPointData *pPointData = NULL;
  // 演出点のデータを取得する
  s32 num = pTrainerActor->GetPathPointDataNum();
  Field::TrafficPath *pPath = pTrainerActor->GetTrafficPath();

  // 逆流対応
  bool bRev = false;
  if( fNowProgress < fOldProgress &&
    pPath->CheckFlags( Field::TrafficPath::FLAG_REVERSE ) )
  {
    f32 dstProgress = fOldProgress;
    fOldProgress = fNowProgress;
    fNowProgress = dstProgress;
    bRev = true;
  }

  for( s32 i = 0; i < num; ++ i )
  {
    const Field::Traffic::PathPointData *dstData = pTrainerActor->GetPathPointData( i );
    if( !dstData ){ continue; }

    if( dstData->relayProgress <= 0.0f )
    {
      // 始点処理
      if( fNowProgress > 0.0f ) { continue; }
      pPointData = dstData;
      break;
    }
    else if( dstData->relayProgress >= 1.0f )
    {
      // 終点処理
      if( bRev ){ continue; }
      if( fNowProgress < 1.0f ){ continue; }
      pPointData = dstData;
      break;
    }
    else
    {
      // 中継点処理
      if( dstData->relayProgress <= fOldProgress ){ continue; }
      if( fNowProgress < dstData->relayProgress ){ continue; }
      // 条件に合致する
      pPointData = dstData;
      break;
    }
  }

  return pPointData;
}

/**
 *  @brief  アニメーション変更
 */
void FieldMoveCodeTrainerPath::ChangeAnimationSmooth( FieldMoveModel *pModel, Work *pWork, u32 animationID, bool bLoop, u32 changeFrame ) const
{
  poke_3d::model::BaseModel*  pCharaDrawInstance  = pModel->GetCharaDrawInstance();
  pCharaDrawInstance->ChangeAnimationSmooth( animationID, changeFrame );
  pCharaDrawInstance->SetAnimationStepFrame( 1.0f );
  pCharaDrawInstance->SetAnimationLoop( bLoop );
}
void FieldMoveCodeTrainerPath::ChangeAnimation( FieldMoveModel *pModel, Work *pWork, u32 animationID, bool bLoop ) const
{
  poke_3d::model::BaseModel*  pCharaDrawInstance  = pModel->GetCharaDrawInstance();
  pCharaDrawInstance->ChangeAnimation( animationID );
  pCharaDrawInstance->SetAnimationStepFrame( 1.0f );
  pCharaDrawInstance->SetAnimationLoop( bLoop );
}

/**
 *  @brief  視線コリジョンの更新
 */
void FieldMoveCodeTrainerPath::UpdateCollisionShapeByJoint( FieldMoveModel *pModel, Work *pWork ) const
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

  StaticActor* pStaticActor = pModel->GetStaticActor( COLLISION_STATIC_TYPE_TRAINER_EYE);
  gfl2::math::Vector3 parentPos;
  parentPos = pStaticActor->GetPosition();

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

/**
 *  @brief  パス進捗の保存
 */
void FieldMoveCodeTrainerPath::SaveProgress( Work *pWork ) const
{
  if( pWork->pTrainerActor)
  {
    Field::TrafficPath *pPath = pWork->pTrainerActor->GetTrafficPath();
    if( pPath)
    {
      pWork->progress = pPath->GetProgress();
      pWork->isReverse = pPath->CheckFlags( TrafficPath::FLAG_REVERSE );
    }
  }
}

#ifdef ENABLE_TEST_EYE_DRAW
/**
 *  @brief  デバッグ用：視線の描画
 */
void FieldMoveCodeTrainerPath::UpdateEyeModel( FieldMoveModel *pModel, Work *pWork ) const
{
  // トレーナーアクター取得
  poke_3d::model::BaseModel* pEyeModel = pWork->pTrainerActor->GetEyeModel();

  b32 isEnableTrainerEye = false; /*FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_TRAINER_EYE_DRAW );*/
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
  scl.y = eyeParam.y / 100.0f;
  scl.z = (eyeParam.z * pWork->eyeLengthRate) / 100.0f;

  // 視線モデル更新
  pEyeModel->SetPosition( pos);
  pEyeModel->SetRotationQuat( rot);
  pEyeModel->SetScale( scl);
}

#endif // ENABLE_TEST_EYE_DRAW

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)
