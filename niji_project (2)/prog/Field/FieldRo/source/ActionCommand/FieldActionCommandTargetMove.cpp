//=================================================================================================
/**
*  @file   FieldActionCommandTargetMove.cpp
*  @brief  フィールド用 アクションコマンド ステップ移動
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandTargetMove.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include <niji_conv_header/field/chara/chara_model_id.h>
#include "niji_reference_files/script/FieldPawnTypes.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief コンストラクタ
*/
//-----------------------------------------------------------------------------
ActionCommandTargetMove::ActionCommandTargetMove()
{
  m_pFuncTbl[0]  = &ActionCommandTargetMove::ActionPrevTurnInit;
  m_pFuncTbl[1]  = &ActionCommandTargetMove::ActionTurnMain;
  m_pFuncTbl[2]  = &ActionCommandTargetMove::ActionStartInit;
  m_pFuncTbl[3]  = &ActionCommandTargetMove::ActionMain;
  m_pFuncTbl[4]  = &ActionCommandTargetMove::ActionStepInit;
  m_pFuncTbl[5]  = &ActionCommandTargetMove::ActionStepMain;
  m_pFuncTbl[6]  = &ActionCommandTargetMove::ActionEndInit;
  m_pFuncTbl[7]  = &ActionCommandTargetMove::ActionMain;
  m_pFuncTbl[8]  = &ActionCommandTargetMove::ActionIdleInit;
  m_pFuncTbl[9]  = &ActionCommandTargetMove::ActionMain;
  m_pFuncTbl[10] = &ActionCommandTargetMove::ActionAfterTurnInit;
  m_pFuncTbl[11] = &ActionCommandTargetMove::ActionTurnMain;
  m_pFuncTbl[12] = &ActionCommandTargetMove::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(最初の旋回)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionPrevTurnInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 目標ベクトルを取得
  gfl2::math::Vector3 targetVec(0.0f,0.0f,0.0f);
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  targetVec   = pWork->movePosition - pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition();
  targetVec.y = 0.0f;
  targetVec   = targetVec.Normalize();

  // 目標地点についていれば終了
  gfl2::math::Vector3 def( pWork->movePosition - pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition() );

  // PlayerならばPlayerのインスタンスを取得
  FieldMoveModelPlayer* pPlayer = NULL;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
  }

  // ゴールレンジを求める。
  f32 goal_range = GOAL_WALK_RANGE;
  if ( pWork->isCustomGoalCheck )
  {
    goal_range = 1.0f; // 厳密にみるので限りなく近くてもよい
  }
  else if( pPlayer && pPlayer->GetOnMoveModel() )
  {
    goal_range = GOAL_RIDE_RUN_RANGE;
  }
  else
  {
    if( pWork->isWalk )
    {
      goal_range = GOAL_WALK_RANGE;
    }
    else
    {
      goal_range = GOAL_RUN_RANGE;
    }
  }

  if( def.Length() < goal_range )
  {
    // 目標地点に吸着
    pActionCommandWork->pModel->GetCharaDrawInstance()->SetPosition( pWork->movePosition );

    const u32 JUMP_IDLE_FUNC = 8;
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, JUMP_IDLE_FUNC );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  // ライド&バンバドロにのっているかどうか調べる
  b32 isRide      = false;
  if( pPlayer && pPlayer->GetOnMoveModel() )
  {
    isRide = true;
  }

  // 必ずゴールにつけるようにコリジョンをオフにする
  pWork->isPrevCollisionActiveTerrain            = false;
  pWork->isPrevCollisionActiveStatic             = false;
  pWork->isPrevCollisionActiveTraffic            = false;
  pWork->isPrevCollisionActiveOshikaeshiNaminori = false;
  pWork->isPrevCollisionActiveOshikaeshiRoba2    = false;
  pWork->isPrevCollisionActiveOshikaeshiRide     = false;
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TERRAIN ) )
  {
    pWork->isPrevCollisionActiveTerrain = pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TERRAIN )->IsActive();
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TERRAIN )->SetActive(false);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_STATIC_MODEL ) )
  {
    pWork->isPrevCollisionActiveStatic = pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_STATIC_MODEL )->IsActive();
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_STATIC_MODEL )->SetActive(false);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL ) )
  {
    pWork->isPrevCollisionActiveTraffic = pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL )->IsActive();
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL )->SetActive(false);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI ) )
  {
    pWork->isPrevCollisionActiveOshikaeshiNaminori = pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->IsActive();
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->SetActive(false);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 ) )
  {
    pWork->isPrevCollisionActiveOshikaeshiRoba2 = pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->IsActive();
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive(false);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE ) )
  {
    pWork->isPrevCollisionActiveOshikaeshiRide = pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE )->IsActive();
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(false);
  }

  // ライドは旋回モーションを待機で代用
  if( pPlayer && pPlayer->GetOnMoveModel() )
  {
    const u32 MOTION_SLOT_WAIT   = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+0; // 待機、歩きループ、歩き開始、歩き終わり
    const u32 MOTION_SLOT_OFFSET = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
    pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_WAIT + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
    pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_WAIT + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
  }

  gfl2::math::Vector3 UnitXVec(1.0f,0.0,0.0f);
  gfl2::math::Vector3 UnitZVec(0.0f,0.0,1.0f);

  // 指定角度以内なら旋回モーションの再生を取りやめる
  gfl2::math::Vector3 chrDir( pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat().Transform(UnitZVec) );
  b32 isChangeMotion = true;
	f32 dot = gfl2::math::Clamp(-1.0f, 1.0f, chrDir.Dot(targetVec));
	f32 deg = gfl2::math::ConvRadToDeg(acosf(dot));
  if( pWork->thresholdAngleForMotion > deg )
  {
    isChangeMotion = false;
  }

  // 目標姿勢を取得
  gfl2::math::Quaternion targetQua;
  if( UnitXVec.Dot(targetVec) > 0.0f )
  {
    targetQua.RadianYToQuaternion( acosf( UnitZVec.Dot(targetVec) ) );
  }
  else
  {
    targetQua.RadianYToQuaternion(-acosf( UnitZVec.Dot(targetVec) ) );
  }

  if( isChangeMotion && !isRide )
  {
    // キャラクターのローカルに変換する逆行列を取得
    gfl2::math::Matrix34 chrRot,chrRotInv;
    pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(chrRot);
    chrRotInv = chrRot.Inverse();

    // 目標の向きベクトルを取得
    gfl2::math::Matrix34 targetRot;
    targetQua.QuaternionToMatrix(targetRot);
    gfl2::math::Vector4 targetDir(0.0f,0.0f,1.0f,0.0f);
    targetDir = targetRot.TransformNormal(targetDir);

    // 目標の向きベクトルをキャラクターのローカルにする
    gfl2::math::Vector4 targetLocalDir = chrRotInv.TransformNormal(targetDir);

    // 目標がキャラクターの左側なら
    if( targetLocalDir.x >= 0.0f )
    {
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI017_TURN01_LEFT  );
    }
    // 目標がキャラクターの右側なら
    else
    {
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI016_TURN01_RIGHT );
    }
  }

  // 次へ
  pWork->frame            = 0;
  pWork->startQuaternion  = pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  pWork->targetQuaternion = targetQua;
  if(isChangeMotion)
  {
    pWork->frameMax = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
  }
  else
  {
    const f32 MAX_FRAME = 8.0f;
    pWork->frameMax = (s32)(deg/180.0f * MAX_FRAME) + 1;
  }

  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(歩き始め)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionStartInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{ 
  Work* pWork     = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  
  // 歩きはじめスキップ処理
  if( !pWork->isEnableActionStartMove )
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  pWork->frame    = 0;
  pWork->frameMax = 0;

  // プレイヤーの専用処理
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      // 自身と自身が乗っている動作モデルにもモーションを流し込む
      const u32 MOTION_SLOT_WALK_START = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+2; // 待機、歩きループ、歩き開始、歩き終わり
      const u32 MOTION_SLOT_RUN_START  = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+2; // 待機、走りループ、走り開始、走り終わり
      const u32 MOTION_SLOT_OFFSET     = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
      if( pWork->isWalk )
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_WALK_START + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_WALK_START + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      }
      else
      { 
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_RUN_START  + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_RUN_START  + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      }
      pWork->frameMax = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
    }
  }

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(歩き中)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionStepInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // ワークを取得
  Work* pWork           = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  u32   animationStepId = pWork->isWalk ? ( pWork->isSlow ? P2_BASE_EV9999_EVWALK01 : P2_BASE_FI010_WALK01 ) : P2_BASE_FI012_RUN01;

  // プレイヤーの専用処理
  b32 isRide = false;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      // 自身と自身が乗っている動作モデルにもモーションを流し込む
      const u32 MOTION_SLOT_WALK_LOOP = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+1; // 待機、歩きループ、歩き開始、歩き終わり
      const u32 MOTION_SLOT_RUN_LOOP  = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+1; // 待機、走りループ、走り開始、走り終わり
      const u32 MOTION_SLOT_OFFSET    = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
      if( pWork->isWalk )
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_WALK_LOOP + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_WALK_LOOP + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
      }
      else
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_RUN_LOOP  + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_RUN_LOOP  + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
      }
      isRide = true;
    }
  }

  // 歩きモーションの再生
  if( !isRide )
  {
		if (pWork->mositonChangeFrame != -1)
		{
			pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimationSmooth(animationStepId, pWork->mositonChangeFrame);
		}
		else
		{
			pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation(animationStepId);
		}
  }

  pWork->frame    = 0;
  pWork->frameMax = MAX_LOOP * (u32)( pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame() );

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(歩き終了)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionEndInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork     = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 歩き終わりスキップ処理
  if( !pWork->isEnableActionEndMove )
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }


  pWork->frame    = 0;
  pWork->frameMax = 0;

  // プレイヤー専用処理
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      // 乗られている動作モデルにもモーションを流し込む
      const u32 MOTION_SLOT_WALK_END = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+3; // 待機、走りループ、走り開始、走り終わり
      const u32 MOTION_SLOT_RUN_END  = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+3; // 待機、走りループ、走り開始、走り終わり
      const u32 MOTION_SLOT_OFFSET   = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
      if( pWork->isWalk )
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_WALK_END + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_WALK_END + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      }
      else
      { 
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_RUN_END  + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_RUN_END  + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      }
      pWork->frameMax = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
    }
  }

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(待機)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionIdleInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
	Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork(pActionCommandWork, sizeof(Work)));

  // 待機モーションを単発再生する
	if (pWork->mositonChangeFrame != -1)
	{
		pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimationSmooth((u32)(pActionCommandWork->pModel->GetDefaultIdleAnimationId()), pWork->mositonChangeFrame );
	}
	else
	{
	  pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation((u32)(pActionCommandWork->pModel->GetDefaultIdleAnimationId()) );
	}

  // プレイヤー専用処理
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( (u32)( pActionCommandWork->pModel->GetDefaultIdleAnimationId() ) ); 
    }
  }

  // 待機モーションに戻った瞬間に終了する

  pWork->frame    = 0;
  pWork->frameMax = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationController()->GetInterpTotalFrame()+1;

  // 次へ
  if ( pWork->isMoveEndIdleWait )
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  }
  else
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 );
  }
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(最後の旋回)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionAfterTurnInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 最後の旋回を行わずに終了させる
  if( !pWork->isAdjustQuaternion )
  {
    const u32 JUMP_END_FUNC = 2;
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, JUMP_END_FUNC );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  // キャラクターローカルに変換する逆行列を取得
  gfl2::math::Matrix34 startRot,startRotInv;
  pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(startRot);
  startRotInv = startRot.Inverse();

  // 目標の向きベクトルを取得
  gfl2::math::Matrix34 targetRot;
  pWork->moveQuaternion.QuaternionToMatrix(targetRot);
  gfl2::math::Vector4 targetVec(0.0f,0.0f,1.0f,0.0f);
  targetVec = targetRot.TransformNormal(targetVec);
  gfl2::math::Vector4 targetLocalVec = startRotInv.TransformNormal(targetVec);
  if( targetLocalVec.x == 0.0f )
  {
    const u32 JUMP_END_FUNC = 2;
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, JUMP_END_FUNC );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  // 対象がプレイヤーで且つライド状態か調べる
  b32 isRide    = false;
  b32 isKairiky = false;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      if( pPlayer->GetOnMoveModel()->GetCharacterId() != CHARA_MODEL_ID_PM0068_00 )
      {
        const u32 MOTION_SLOT_WAIT   = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+0; // 待機、歩きループ、歩き開始、歩き終わり
        const u32 MOTION_SLOT_OFFSET = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_WAIT + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_WAIT + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
      }
      else
      {
        isKairiky = true;
      }
      isRide = true;
    }
  }

  // 右か左にターンする
  if( targetLocalVec.x >= 0.0f )
  {
    if( isKairiky )
    {
      FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_RI086_TURN01_LEFT );
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation ( P2_BASE_RI086_TURN01_LEFT );
    }
    else if( !isRide )
    {
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI017_TURN01_LEFT );
    }
  }
  else
  {
    if( isKairiky )
    {
      FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_RI085_TURN01_RIGHT );
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation ( P2_BASE_RI085_TURN01_RIGHT );
    }
    else if( !isRide )
    {
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI016_TURN01_RIGHT );
    }
  }
  
  // 次へ
  pWork->frame            = 0;
  pWork->frameMax         = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
  pWork->startQuaternion  = pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  pWork->targetQuaternion = pWork->moveQuaternion;
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
  if( pWork->frame <= pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(旋回始め)の実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionTurnMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 現在の姿勢を目標の姿勢へと補完する
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  gfl2::math::Quaternion qua( pWork->startQuaternion );
  qua.Slerp( pWork->targetQuaternion , ( (f32)pWork->frame+1.0f ) / (f32)pWork->frameMax );
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetRotationQuat(qua);

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
  if( pWork->frame < pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(歩き中)の実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionStepMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 目標地点についていれば終了
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  gfl2::math::Vector3 def( pWork->movePosition - pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition() );

  // 接地をするならY座標は不要
  if( pActionCommandWork->pModel->IsForceLanding() )
  {
    def.y = 0.0f;
  }

  // PlayerならばPlayerのインスタンスを取得
  FieldMoveModelPlayer* pPlayer = NULL;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
  }

  // 走り→歩き判定
	if (pWork->isRunToWalk && pActionCommandWork->pModel->GetEventId() != Field::FIELD_EVENTID_PLAYER )
	{
		if (pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationId() == P2_BASE_FI012_RUN01)
		{
			gfl2::math::Vector3 work(def);
			work.y = 0.0f;

      // 指定距離以下になっていたら、歩きに変更
			if ( work.Length() <= pWork->runToWalkDistance)
			{
				pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimationSmooth( P2_BASE_FI010_WALK01, 4 );
			}
		}
	}
  
  //
  // ゴール判定 @note momijiで判定式を変更
  //

  b32 isGoal = false;

  if (pWork->isCustomGoalCheck)
  {
    // momijiゴール判定(一部仕様のみ適用)、進行方向を見ることで厳密にしている

    // 非常に近い 
    def.y = 0.0f;
    if (def.Length() < 1.0f)
    {
      isGoal = true;
    }
    else
    {
      // 開始位置→目標位置ベクトルと現在位置→目標位置ベクトルの向きが逆なら付いている
      gfl2::math::Vector3 work((pWork->movePosition - pWork->startPosition));
      work.y = 0.0f;

      // @fix GFCHECK[2976]:スクリプト記述とタイミングにより、0除算が発生する可能性があるため念のためケア
      if (work.Length() < 0.0001)
      {
        isGoal = true;
      }
      else
      {
        work.NormalizeRet();
        if (work.Dot(def.Normalize()) <= 0.0f)
        {
          isGoal = true;
        }
      }
    }
  }
  else
  {
    // nijiゴール判定

    // ゴールレンジを求める。
    f32 goal_range = GOAL_WALK_RANGE;
    if (pPlayer && pPlayer->GetOnMoveModel())
    {
      goal_range = GOAL_RIDE_RUN_RANGE;
    }
    else
    {
      if (pWork->isWalk)
      {
        goal_range = GOAL_WALK_RANGE;
      }
      else
      {
        goal_range = GOAL_RUN_RANGE;
      }
    }

    if (def.Length() < goal_range)
    {
      isGoal = true;
    }
  }

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
  if( pWork->frame < pWork->frameMax && isGoal == false )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

#if PM_DEBUG
  if( isGoal == false )
  {
    // ついていないのにここにくる = 制限時間オーバー
    GFL_ASSERT_MSG( 0 , "Error ActionCommand: Not arrive at the destination" );
  }
#endif

  pActionCommandWork->pModel->GetCharaDrawInstance()->SetPosition(pWork->movePosition);

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの終了
*
*  @param  pActionCommandWork  アクションコマンドワーク
*
*  @retval ACTION_COMMAND_RETURN_END
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMove::ActionEnd(ActionCommandWork* pActionCommandWork)
{
  // コリジョンを元に戻す
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TERRAIN ) )
  {
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TERRAIN )->SetActive(pWork->isPrevCollisionActiveTerrain);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_STATIC_MODEL ) )
  {
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_STATIC_MODEL )->SetActive(pWork->isPrevCollisionActiveStatic);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL ) )
  {
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL )->SetActive(pWork->isPrevCollisionActiveTraffic);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI ) )
  {
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->SetActive(pWork->isPrevCollisionActiveOshikaeshiNaminori);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 ) )
  {
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive(pWork->isPrevCollisionActiveOshikaeshiRoba2);
  }
  if( pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE ) )
  {
    pActionCommandWork->pModel->GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(pWork->isPrevCollisionActiveOshikaeshiRide);
  }

  return ACTION_COMMAND_RETURN_END;
}

} // mmodel
} // field
