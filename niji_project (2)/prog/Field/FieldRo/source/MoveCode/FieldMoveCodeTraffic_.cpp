/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMoveCodeTraffic.cpp
 *  @brief  往来NPC専用動作コード
 *  @author miyachi_soichi
 *  @date   2015/06/26
 */
// Field
#include "Field/FieldRo/include/Traffic/FieldTrafficActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTraffic.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)

//! 折り返し用角度
static const f32 FIELD_TRAFFIC_LIM_ROT_FOR_TURN = 120.0f;
//! 高速旋回用角度
static const f32 FIELD_TRAFFIC_LIM_ROT_FOR_QUICK = 15.0f;

// チェックサイズ
static const f32 FIELD_TRAFFIC_CHECK_SIZE = 37.5f + 100.0f;
static const f32 FIELD_TRAFFIC_CHECK_LENGTH_close = 160.0f;
static const f32 FIELD_TRAFFIC_CHECK_LENGTH_open  = 220.0f;

/**
 *  @brief  動作コード処理　メイン
 */
void FieldMoveCodeTraffic::MainProcess( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );

  switch( pWork->state )
  {
  case STATE_NONE:
    MainProcess_StateNone( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_MOVE_FORWORD:
    MainProcess_StateMoveForword( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_STAY_FORWORD:
    MainProcess_StateStayForword( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_MOVE_BACKWORD:
    MainProcess_StateMoveBackword( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_STAY_BACKWORD:
    MainProcess_StateStayBackword( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_GOAL:
    MainProcess_StateGoal( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_SUICIDE:
    MainProcess_StateSuicide( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_EMOTE_ROT:
    MainProcess_StateEmoteRot( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_EMOTE:
    MainProcess_StateEmote( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_ROTATE:
    MainProcess_StateRotate( pMoveCodeWork->pModel, pWork );
    break;
  case STATE_ROTATE_QUICK:
    MainProcess_StateRotateQuick( pMoveCodeWork->pModel, pWork );
    break;
  }
}

/**
 *  @brief  動作コード処理　再開
 */
void FieldMoveCodeTraffic::Resume( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  Field::TrafficActor *pActor = pWork->actor;

  ResumeTalker( pMoveCodeWork );
}

/**
 *  @brief  動作コード処理　中断
 */
void FieldMoveCodeTraffic::Suspend( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  Field::TrafficActor *pActor = pWork->actor;
  FieldMoveModel *pMoveModel = pMoveCodeWork->pModel;
  pMoveModel->GetCharaDrawInstance()->ChangeAnimation( pActor->GetStayID() );
}

/**
 *  @brief  動作コード処理  復帰
 */
void FieldMoveCodeTraffic::Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  Field::TrafficActor *pActor = pWork->actor;
  FieldMoveModel *pMoveModel = pMoveCodeWork->pModel;
  pMoveModel->GetCharaDrawInstance()->ChangeAnimation( pActor->GetStayID() );

  // @fix NMCat[3699]逆流設定
  if( pWork->state == STATE_MOVE_BACKWORD || pWork->state == STATE_STAY_BACKWORD )
  {
    Field::TrafficActor *pActor = pWork->actor;
    TrafficPath *pPath = pActor->GetPath();
    pPath->SetFlags( TrafficPath::FLAG_REVERSE, true );
  }
}

/**
 *  @brief  状態処理：未初期化
 */
void FieldMoveCodeTraffic::MainProcess_StateNone( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  TrafficPath *pPath = pWork->path;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // パスの初期位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pCharaDrawInstance->SetPosition( vPos );
  // パスの初期方向に設定する
  gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
  pCharaDrawInstance->SetRotation( vRot );
  
  // 移動状態へ移行
  pWork->state = STATE_MOVE_FORWORD;
  // 移動モーションに設定
  pCharaDrawInstance->ChangeAnimation( pActor->GetWalkID() );
  // 移動量は無視する
  pMoveModel->OnStateBit( MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
}

/**
 *  @brief  状態処理：すすむ
 */
void FieldMoveCodeTraffic::MainProcess_StateMoveForword( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  TrafficPath *pPath = pWork->path;
  poke_3d::model::BaseModel *pBaseModel = pMoveModel->GetCharaDrawInstance();

  // パスを進捗する
  gfl2::math::Vector3 vSpd = pBaseModel->GetWalkSpeed( -1.0f );
  pPath->Progress( vSpd.z );
  f32 fProgress = pPath->GetProgress();
  pActor->SetProgress( fProgress );

  // パスの方向に設定する
  if( !pPath->CheckFlags( TrafficPath::FLAG_CORNER ) )
  {
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    pBaseModel->SetRotation( vRot );
  }

  // 到着した
  if( fProgress >= 1.0f )
  {
    // 到着後処理に移行
    pWork->state = STATE_GOAL;
    pWork->life = 30.0f;
    // 待機モーションに設定
    pBaseModel->ChangeAnimation( pActor->GetStayID() );
  }
  // 遮った
  else if( !pActor->CheckEnableWalk( FIELD_TRAFFIC_CHECK_SIZE, FIELD_TRAFFIC_CHECK_LENGTH_close ) )
  {
    // 到着後処理に移行
    pWork->state = STATE_STAY_FORWORD;
    pWork->life = 30.0f * 2.0f;
    // 待機モーションに設定
    pBaseModel->ChangeAnimation( pActor->GetStayID() );
  }
#if 0
  // 話しかけた
  else if( pActor->IsEmote() )
  {
    CalcRotationPlayer( pMoveModel, pWork );
  }
#endif
  // 旋回が必要になった
  else if( pPath->CheckFlags( TrafficPath::FLAG_CORNER ) )
  {
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    gfl2::math::Vector3 vSelf = pBaseModel->GetRotation();
    f32 diff_rot = gfl2::math::NormalizeRadianAngle( vRot.y - vSelf.y );

    u32 motion_id = 0;
    if( diff_rot < 0.0f ) { motion_id = pActor->GetTurnRID(); }
    else                  { motion_id = pActor->GetTurnLID(); }
    // 回転モーションに設定
    pBaseModel->ChangeAnimation( motion_id );
    // 回転設定
    f32 frame = pBaseModel->GetAnimationEndFrame();
    pWork->delta_rot = diff_rot / frame;
    // 元の遷移に戻れるように
    pWork->next = pWork->state;
    pWork->motion_id = pActor->GetWalkID();
    // 回転状態へ
    pWork->state = STATE_ROTATE;
  }

  // パスの位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pBaseModel->SetPosition( vPos );
}

/**
 *  @brief  状態処理：すすむ待機
 */
void FieldMoveCodeTraffic::MainProcess_StateStayForword( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
  // すすむ
  if( pActor->CheckEnableWalk( FIELD_TRAFFIC_CHECK_SIZE, FIELD_TRAFFIC_CHECK_LENGTH_open ) )
  {
    // 移動モーションに設定
    pCharaDrawInstance->ChangeAnimation( pActor->GetWalkID() );
    // 移動状態へ移行
    pWork->state = STATE_MOVE_FORWORD;
    return;
  }
#if 0
  // 話しかけた
  else if( pActor->IsEmote() )
  {
    CalcRotationPlayer( pMoveModel, pWork );
  }
#endif
  if( pWork->life > 0.0f )
  {
    pWork->life -= 1.0f;
    return;
  }

  // もどる
  SetReverse( pMoveModel, pWork );
}

/**
 *  @brief  状態処理：もどる
 */
void FieldMoveCodeTraffic::MainProcess_StateMoveBackword( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  TrafficPath *pPath = pWork->path;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // パスを進捗する
  gfl2::math::Vector3 vSpd = pCharaDrawInstance->GetWalkSpeed( -1.0f );
  pPath->Progress( vSpd.z );
  f32 fProgress = pPath->GetProgress();
  pActor->SetProgress( fProgress );

  // パスの方向に設定する
  if( !pPath->CheckFlags( TrafficPath::FLAG_CORNER ) )
  {
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    pCharaDrawInstance->SetRotation( vRot );
  }

  // 到着した
  if( fProgress <= 0.0f )
  {
    // 到着後処理に移行
    pWork->state = STATE_GOAL;
    pWork->life = 30.0f;
    // 待機モーションに設定
    pCharaDrawInstance->ChangeAnimation( pActor->GetStayID() );
  }
  // 遮った
  else if( !pActor->CheckEnableWalk( FIELD_TRAFFIC_CHECK_SIZE, FIELD_TRAFFIC_CHECK_LENGTH_close ) )
  {
    // 到着後処理に移行
    pWork->state = STATE_STAY_BACKWORD;
    pWork->life = 30.0f * 2.0f;
    // 待機モーションに設定
    pCharaDrawInstance->ChangeAnimation( pActor->GetStayID() );
  }
#if 0
  // 話しかけた
  else if( pActor->IsEmote() )
  {
    CalcRotationPlayer( pMoveModel, pWork );
  }
#endif
  // 旋回が必要になった
  else if( pPath->CheckFlags( TrafficPath::FLAG_CORNER ) )
  {
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    gfl2::math::Vector3 vSelf = pCharaDrawInstance->GetRotation();
    f32 diff_rot = gfl2::math::NormalizeRadianAngle( vRot.y - vSelf.y );

    u32 motion_id = 0;
    if( diff_rot < 0.0f ) { motion_id = pActor->GetTurnRID(); }
    else                  { motion_id = pActor->GetTurnLID(); }
    // 回転モーションに設定
    pCharaDrawInstance->ChangeAnimation( motion_id );
    // 回転設定
    f32 frame = pCharaDrawInstance->GetAnimationEndFrame();
    pWork->delta_rot = diff_rot / frame;
    // 元の遷移に戻れるように
    pWork->next = pWork->state;
    pWork->motion_id = pActor->GetWalkID();
    // 回転状態へ
    pWork->state = STATE_ROTATE;
  }

  // パスの位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pCharaDrawInstance->SetPosition( vPos );
}

/**
 *  @brief  状態処理：もどる待機
 */
void FieldMoveCodeTraffic::MainProcess_StateStayBackword( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
  // もどる
  if( pActor->CheckEnableWalk( FIELD_TRAFFIC_CHECK_SIZE, FIELD_TRAFFIC_CHECK_LENGTH_open ) )
  {
    // 移動モーションに設定
    pCharaDrawInstance->ChangeAnimation( pActor->GetWalkID() );
    // 移動状態へ移行
    pWork->state = STATE_MOVE_BACKWORD;
    return;
  }
#if 0
  // 話しかけた
  else if( pActor->IsEmote() )
  {
    CalcRotationPlayer( pMoveModel, pWork );
  }
#endif
  if( pWork->life > 0.0f )
  {
    pWork->life -= 1.0f;
    return;
  }

  // すすむ
  SetReverse( pMoveModel, pWork );
}

/**
 *  @brief  状態処理：到着後
 */
void FieldMoveCodeTraffic::MainProcess_StateGoal( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  if( pWork->life > 0.0f )
  {
    pWork->life -= 1.0f;
    return;
  }

  pWork->state = STATE_SUICIDE;
  pMoveModel->SetVisible( false );
  // @fix NMCat[570]:消えたキャラクタに話しかけられないようにする
  StaticActor* pStaticActor = pMoveModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_TALK );
  if( pStaticActor )
  {
    pStaticActor->SetActive( false );
  }
}

/**
 *  @brief  状態処理：自殺待ち
 */
void FieldMoveCodeTraffic::MainProcess_StateSuicide( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  // 演者の終了の通知
  pWork->actor->NotifyEndTraffic();
}

/**
 *  @brief  状態処理：エモート回転
 */
void FieldMoveCodeTraffic::MainProcess_StateEmoteRot( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 回転
  {
    gfl2::math::Vector3 self_rot = pCharaDrawInstance->GetRotation();
    self_rot.y += pWork->delta_rot;
    pCharaDrawInstance->SetRotation( self_rot );
  }

  if( pCharaDrawInstance->IsAnimationLastFrame() )
  {
    pWork->delta_rot = 0.0f;
    pWork->state = STATE_EMOTE;
    pCharaDrawInstance->ChangeAnimation( pActor->GetEmoteID() );
  }
}

/**
 *  @brief  状態処理：エモート
 */
void FieldMoveCodeTraffic::MainProcess_StateEmote( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
  
  if( pCharaDrawInstance->IsAnimationLastFrame() )
  {
    TrafficActor *pActor = pWork->actor;
    pActor->NotifyEndEmote();
    SetReverse( pMoveModel, pWork );
  }
}

/**
 *  @brief  状態処理：回転
 */
void FieldMoveCodeTraffic::MainProcess_StateRotate( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 回転
  {
    gfl2::math::Vector3 self_rot = pCharaDrawInstance->GetRotation();
    self_rot.y += pWork->delta_rot;
    pCharaDrawInstance->SetRotation( self_rot );
  }

  if( pCharaDrawInstance->IsAnimationLastFrame() )
  {
    pWork->state = pWork->next;
    pCharaDrawInstance->ChangeAnimation( pWork->motion_id );
  }
}

/**
 *  @brief  状態処理：回転
 */
void FieldMoveCodeTraffic::MainProcess_StateRotateQuick( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();

  // 回転
  {
    gfl2::math::Vector3 self_rot = pCharaDrawInstance->GetRotation();
    self_rot.y += pWork->delta_rot;
    pCharaDrawInstance->SetRotation( self_rot );
  }

  {
    pWork->state = pWork->next;
    pCharaDrawInstance->ChangeAnimation( pWork->motion_id );
  }
}

/**
 *  @brief  反転設定
 */
void FieldMoveCodeTraffic::SetReverse( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  TrafficActor *pActor = pWork->actor;
  TrafficPath *pPath = pWork->path;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
  
  // 遷移設定
  if( pWork->state == STATE_EMOTE )
  {
    pWork->life = 30.0f * 2.0f;
    pWork->motion_id = 0;
  }
  else if( pWork->state == STATE_STAY_FORWORD )
  {
    pWork->next = STATE_STAY_BACKWORD;
    pWork->motion_id = 0;
    pWork->life = 30.0f * 2.0f;
    pPath->SetFlags( TrafficPath::FLAG_REVERSE, true );
  }
  else if( pWork->state == STATE_STAY_BACKWORD )
  {
    pWork->next = STATE_STAY_FORWORD;
    pWork->motion_id = 0;
    pWork->life = 30.0f * 2.0f;
    pPath->SetFlags( TrafficPath::FLAG_REVERSE, false );
  }

  u32 motion_id = 0;
  f32 diff_rot = 0.0f;
  // 角度計算
  {
    gfl2::math::Vector3 path_rot = pPath->GetCurrentRotation();
    gfl2::math::Vector3 self_rot = pCharaDrawInstance->GetRotation();
    diff_rot = gfl2::math::NormalizeRadianAngle( path_rot.y - self_rot.y );
    if( diff_rot < 0.0f ) { motion_id = pActor->GetTurnRID(); }
    else                  { motion_id = pActor->GetTurnLID(); }
  }
  
  f32 abs_lim_rot = gfl2::math::ConvDegToRad( FIELD_TRAFFIC_LIM_ROT_FOR_QUICK );
  f32 abs_dif_rot = gfl2::math::Abs<f32>( diff_rot );

  if( abs_dif_rot < abs_lim_rot )
  {
    // 高速回転
    pWork->delta_rot = diff_rot;
    pWork->state = STATE_ROTATE_QUICK;
  }
  else
  {
    // 通常回転
    // モーションの遷移
    pCharaDrawInstance->ChangeAnimation( motion_id );
    // 回転設定
    f32 frame = pCharaDrawInstance->GetAnimationEndFrame();
    pWork->delta_rot = diff_rot / frame;
    pWork->state = STATE_ROTATE;
  }
}

/**
 *  @brief  プレイヤー方向を向く設定
 */
void FieldMoveCodeTraffic::CalcRotationPlayer( FieldMoveModel *pMoveModel, Work *pWork ) const
{
  // 戻り先の指定
  u32 dstState = pWork->state;
  if( dstState == STATE_MOVE_FORWORD || dstState == STATE_STAY_FORWORD )
  {
    pWork->next = STATE_STAY_FORWORD;
  }
  else if( dstState == STATE_MOVE_BACKWORD || dstState == STATE_STAY_BACKWORD )
  {
    pWork->next = STATE_STAY_BACKWORD;
  }

  TrafficActor *pActor = pWork->actor;
  poke_3d::model::BaseModel *pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
  u32 motion_id = 0;
  f32 diff_rot = 0.0f;
  gfl2::math::Vector3 self_rot;
  // 角度計算
  {
    FieldMoveModel *pPlayer = pActor->GetTalker();
    gfl2::math::Vector3 player_pos = pPlayer->GetCharaDrawInstance()->GetPosition();
    gfl2::math::Vector3 self_pos = pCharaDrawInstance->GetPosition();
    gfl2::math::Vector3 dir = player_pos - self_pos;
    self_rot = pCharaDrawInstance->GetRotation();
    f32 fRotY = atan2( dir.x, dir.z );
    diff_rot = gfl2::math::NormalizeRadianAngle( fRotY - self_rot.y );
    if( diff_rot < 0.0f ) { motion_id = pActor->GetTurnRID(); }
    else                  { motion_id = pActor->GetTurnLID(); }
  }
  
  f32 abs_lim_rot = gfl2::math::ConvDegToRad( FIELD_TRAFFIC_LIM_ROT_FOR_QUICK );
  f32 abs_dif_rot = gfl2::math::Abs<f32>( diff_rot );

  if( abs_dif_rot < abs_lim_rot )
  {
    // 高速回転
    self_rot.y += diff_rot;
    pCharaDrawInstance->SetRotation( self_rot );
    // 遷移先の指定
    pWork->state = STATE_EMOTE;
    // モーションの遷移
    pCharaDrawInstance->ChangeAnimation( pActor->GetEmoteID() );
  }
  else
  {
    // モーションの遷移
    pCharaDrawInstance->ChangeAnimation( motion_id );
    // 回転角の設定
    f32 frame = pCharaDrawInstance->GetAnimationEndFrame();
    pWork->delta_rot = diff_rot / frame;
    // 回転状態へ
    pWork->state = STATE_EMOTE_ROT;
  }
}

void FieldMoveCodeTraffic::ResumeTalker( FieldMoveCodeWork *pMoveCodeWork ) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  Field::TrafficActor *pActor = pWork->actor;
  poke_3d::model::CharaModel *pCharaDrawInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // パスの復元
  TrafficPath *pPath = pActor->GetPath();
  pWork->path = pPath;
  // 進捗の再設定
  pPath->SetProgress( pActor->GetProgress() );
  // エモートはOffにします
  pActor->NotifyEndEmote();
  // 逆流設定
  if( pWork->state == STATE_MOVE_BACKWORD || pWork->state == STATE_STAY_BACKWORD )
  {
    pPath->SetFlags( TrafficPath::FLAG_REVERSE, true );
  }
  // パスの初期位置に設定する
  gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
  pCharaDrawInstance->SetPosition( vPos );
  // パスの初期方向に設定する
  gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
  pCharaDrawInstance->SetRotation( vRot );

  // 各状態ごとに復元しなければならないモーション
  switch( pWork->state )
  {
  case STATE_MOVE_FORWORD:// breakthrough
  case STATE_MOVE_BACKWORD:
    pCharaDrawInstance->ChangeAnimation( pActor->GetWalkID() );
    break;
  case STATE_NONE:// breakthrough
  case STATE_GOAL:// breakthrough
  case STATE_SUICIDE:
    // 何もしない
    break;
  default:
    // 待機に戻す
    if( pPath->CheckFlags( TrafficPath::FLAG_REVERSE ) )
    {
      pWork->life = 30.0f * 2.0f;
      pWork->state = STATE_STAY_BACKWORD;
    }
    else
    {
      pWork->life = 30.0f * 2.0f;
      pWork->state = STATE_STAY_FORWORD;
    }
    break;
  }
}

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)
