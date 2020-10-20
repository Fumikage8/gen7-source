//=================================================================================================
/**
*  @file   FieldActionCommandStepJump.cpp
*  @brief  フィールド用 アクションコマンド 段差ジャンプ
*  @author yy-ikeuchi
*  @date   2015.02.27
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepJump.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
// record
#include "Savedata/include/Record.h"
// コリジョン
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/RaycastCustomCallback.h"

// カメラ
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Fieldmap.h"

const gfl2::math::Vector3 Field::MoveModel::ActionCommandStepJump::PLAYER_JUMP_MOVE( 0.0f,-70.0f,150.0f ); // 未ライド時の段差ジャンプはZに150cm程前に飛ぶ
const gfl2::math::Vector3 Field::MoveModel::ActionCommandStepJump::RIDE_JUMP_MOVE  ( 0.0f,-70.0f,250.0f ); //   ライド時の段差ジャンプはZに250cm程前に飛ぶ

// ジャンプ中のカメラ遅延設定(非ライド)
Field::MoveModel::ActionCommandStepJump::CAMERA_DELAY Field::MoveModel::ActionCommandStepJump::m_CameraDelaySetting =
{
  5,
  24,
  gfl2::math::Easing::INOUT_QUAD
};

// ジャンプ中のカメラ遅延設定(ライド)
Field::MoveModel::ActionCommandStepJump::CAMERA_DELAY Field::MoveModel::ActionCommandStepJump::m_CameraDelaySettingRide =
{
  0,
  24,
  gfl2::math::Easing::INOUT_QUAD
};

// 着地時のカメラ揺れの設定、ライドだけ
Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_SETTING Field::MoveModel::ActionCommandStepJump::m_CameraShakeSetting[ Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_TYPE_MAX ] =
{
  // 軽量
  { 
    1.0f, 100.0f, 15.0f, 0.45f, 0,
    1.0f, 100.0f, 10.0f, 0.45f, 0,
  },
  
  // 中量
  { 
    1.0f, 100.0f, 25.0f, 0.6f, 0,
    1.0f, 100.0f, 10.0f, 0.5f, 0,
  },
  
  // 重量
  { 
    1.0f, 100.0f, 50.0f, 0.53f, 0,
    1.0f, 100.0f, 0.8f, 0.6f, 0,
  },
};



namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief コンストラクタ
*/
//-----------------------------------------------------------------------------
ActionCommandStepJump::ActionCommandStepJump()
{
  m_pFuncTbl[0] = &ActionCommandStepJump::ActionInitJump;
  m_pFuncTbl[1] = &ActionCommandStepJump::ActionMainJump;
  m_pFuncTbl[2] = &ActionCommandStepJump::ActionInitLanding;
  m_pFuncTbl[3] = &ActionCommandStepJump::ActionMainLanding;
  m_pFuncTbl[4] = &ActionCommandStepJump::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの初期化(ジャンプ)
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandStepJump::ActionInitJump(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
#if USE_RTTI_DEBUG
  RegistRtti();
#endif

  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  
  // 地面に吸着はしない
  pActionCommandWork->pModel->SetForceLanding(false);

  // コリジョンによる押し返しを受けない
  pActionCommandWork->pModel->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );

  // プレイヤーの専用処理
  b32 isRide = false;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      // 一連のジャンプモーションを再生
      const u32 MOTION_SLOT_JUMP   = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+17;
      const u32 MOTION_SLOT_OFFSET = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
      pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_JUMP + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_JUMP + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      isRide = true;
    }

    // 自機操作中のみ、ジャンプにあわせてカメラに演出をいれる
    GameSys::GameManager*         pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap*                     pFieldMap      = pGameManager->GetFieldmap();
    Field::Camera::CameraManager* pCameraManager = pFieldMap->GetCameraManager();

    // ジャンプ中のカメラ遅延設定
    if( pCameraManager && pCameraManager->GetTarget() )
    {
      if( isRide )
      {
        pCameraManager->GetTarget()->Delay( m_CameraDelaySettingRide.waitFrame, m_CameraDelaySettingRide.moveFrame, m_CameraDelaySettingRide.moveEaseFunc );
      }
      else
      {
        pCameraManager->GetTarget()->Delay( m_CameraDelaySetting.waitFrame, m_CameraDelaySetting.moveFrame, m_CameraDelaySetting.moveEaseFunc );
      }
      
      // 遅延追従を停止
      pCameraManager->GetTarget()->DelayFollowPause();
    }
  }

  // ジャンプ開始からジャンプループまでのモーションを再生
  if( !isRide )
  {
    pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_FI051_STEPJUMP01_START);
  }

  // 段差ジャンプによる移動後の座標を取得
  gfl2::math::Matrix  rotMat;
  gfl2::math::Vector4 moveVec(ActionCommandStepJump::PLAYER_JUMP_MOVE.x,
                              ActionCommandStepJump::PLAYER_JUMP_MOVE.y,
                              ActionCommandStepJump::PLAYER_JUMP_MOVE.z);
  if( isRide )
  {
    moveVec.x = ActionCommandStepJump::RIDE_JUMP_MOVE.x;
    moveVec.y = ActionCommandStepJump::RIDE_JUMP_MOVE.y;
    moveVec.z = ActionCommandStepJump::RIDE_JUMP_MOVE.z;
  }
  pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(rotMat);
  moveVec = rotMat.TransformNormal(moveVec);
  pWork->dstPos = pActionCommandWork->pModel->GetLocalSRT().translate + gfl2::math::Vector3(moveVec.x,moveVec.y,moveVec.z);
  pWork->dstPosAfterAdjustGround = pWork->dstPos;

  // 接地を考慮した移動後の座標に変換する
  if( pActionCommandWork->pModel->GetCollisionSceneContainerForGround() )
  {
    // 遥か上空から地下深くまでレイキャストをするように
    f32 startEndOffset = 1000000.0f;

    // 地面の高さを取得
    RaycastCustomCallback::HIT_DATA hitData;
    gfl2::math::Vector4 start(pWork->dstPos.x,pWork->dstPos.y+startEndOffset,pWork->dstPos.z,0.0f);
    gfl2::math::Vector4   end(pWork->dstPos.x,pWork->dstPos.y-startEndOffset,pWork->dstPos.z,0.0f);
    if( pActionCommandWork->pModel->GetCollisionSceneContainerForGround()->RaycastFromStaticActorsMesh(start,end,&hitData) )
    {
      pWork->dstPosAfterAdjustGround = gfl2::math::Vector3(hitData.intersection.x,hitData.intersection.y,hitData.intersection.z);
      pWork->dstPosAfterAdjustGround.y += Field::MoveModel::FieldMoveModel::GROUND_OFFSET;
    }
  }

  // モーション単発再生分だけアクションコマンドを実行する
  pWork->frame     = 0;
  pWork->frameMax  = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
  pWork->startPos  = pActionCommandWork->pModel->GetLocalSRT().translate;
  pWork->startQua  = pActionCommandWork->pModel->GetLocalSRT().rotation;

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行(ジャンプ)
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandStepJump::ActionMainJump(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 補完分を考慮する
  gfl2::math::Vector3 interp( (pWork->dstPosAfterAdjustGround - pWork->dstPos) / (f32)pWork->frameMax );
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetPosition( pActionCommandWork->pModel->GetLocalSRT().translate + interp );

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
  if( pWork->frame <= pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 地面に吸着する
  pActionCommandWork->pModel->SetForceLanding(true);

  // コリジョンによる押し返しを受ける
  pActionCommandWork->pModel->OffStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの初期化(着地)
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandStepJump::ActionInitLanding(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  b32 isRide = false;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );

	  // @fix cov_ctr[12530]:NULLチェック追加
	  if ( pPlayer && pPlayer->GetOnMoveModel())
    {
      isRide = true;
    }

    // 自機操作中のみ、ジャンプにあわせてカメラに演出をいれる
    GameSys::GameManager*         pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap*                     pFieldMap      = pGameManager->GetFieldmap();
    Field::Camera::CameraManager* pCameraManager = pFieldMap->GetCameraManager();

    if( pCameraManager )
    {
      // 遅延追従を再開
      if( pCameraManager->GetTarget() )
      {
        pCameraManager->GetTarget()->DelayFollowResume();
      }

      // 着地時のカメラ揺れの設定、ライドだけ
      if( pCameraManager->GetMainViewCamera() && isRide )
      {
        u32 shakeType = CAMERA_SHAKE_TYPE_LIGHT;
        
        // 地面馬は重量級
        if( pPlayer->GetOnMoveModel()->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
        {
          shakeType = CAMERA_SHAKE_TYPE_HEAVY;
        }

        // ケンタとカイリキーは中量級
        if( pPlayer->GetOnMoveModel()->GetCharacterId() == CHARA_MODEL_ID_PM0128_00 || pPlayer->GetOnMoveModel()->GetCharacterId() == CHARA_MODEL_ID_PM0068_00 )
        {
          shakeType = CAMERA_SHAKE_TYPE_MIDDLE;
        }

        // 縦ゆれをリクエストする
        if( m_CameraShakeSetting[ shakeType ].vertical.firstVelocity != 0.0f )
        {
          pCameraManager->GetMainViewCamera()->GetControllerBase()->ShakeVerticalRequest(
            m_CameraShakeSetting[ shakeType ].vertical.mass,
            m_CameraShakeSetting[ shakeType ].vertical.k,
            m_CameraShakeSetting[ shakeType ].vertical.firstVelocity,
            m_CameraShakeSetting[ shakeType ].vertical.damping
            );
        }

        if( m_CameraShakeSetting[ shakeType ].horizon.firstVelocity != 0.0f )
        {
          // 横ゆれをリクエストする
          pCameraManager->GetMainViewCamera()->GetControllerBase()->ShakeHorizonRequest(
            m_CameraShakeSetting[ shakeType ].horizon.mass,
            m_CameraShakeSetting[ shakeType ].horizon.k,
            m_CameraShakeSetting[ shakeType ].horizon.firstVelocity,
            m_CameraShakeSetting[ shakeType ].horizon.damping
            );
        }
      }
    }

    // 入力があったら着地モーションは無視
    if ((pPlayer && pPlayer->IsInputStick())
      || (pPlayer && isRide))
    {
      FieldActionCommandManager::AddActionCommandWorkSequence(pActionCommandWork, 2);
      return ACTION_COMMAND_RETURN_CONTINUE;
    }
  }

  // ジャンプ開始からジャンプループまでのモーションを再生
  pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_FI052_STEPJUMP01_END);

  // モーション単発再生分だけアクションコマンドを実行する
  Work* pWork     = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));    
  pWork->frame    =  0;
  pWork->frameMax = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行(着地)
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandStepJump::ActionMainLanding(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 主人公専用処理
  if (pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER)
  {
    // 入力があったら着地モーションは無視
    FieldMoveModelPlayer *pPlayer = static_cast<FieldMoveModelPlayer*>(pActionCommandWork->pModel);
    if (pPlayer && pPlayer->IsInputStick())
    {
      FieldActionCommandManager::AddActionCommandWorkSequence(pActionCommandWork, 1);
      return ACTION_COMMAND_RETURN_CONTINUE;
    }
  }

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  pWork->frame++;
  if( pWork->frame < pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの終了
*
*  @param  pActionCommandWork  アクションコマンドワーク
*
*  @retval ACTION_COMMAND_RETURN_END
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepJump::ActionEnd(ActionCommandWork* pActionCommandWork)
{
  // ズレがないように正しい値で座標を上書き
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork(pActionCommandWork, sizeof(Work)));
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetPosition(pWork->dstPosAfterAdjustGround);
  
  // 主人公専用処理
  if (pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER)
  {
    FieldMoveModelPlayer *pPlayer = static_cast<FieldMoveModelPlayer*>(pActionCommandWork->pModel);
    if (pPlayer)
    {
      // レコード(段差ジャンプ)
      Savedata::IncRecord(Savedata::Record::RECID_DANSA);
    }
  }

  return ACTION_COMMAND_RETURN_END;
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void ActionCommandStepJump::RegistRtti(void) const
{
#if USE_RTTI_DEBUG
#endif
}

} // mmodel
} // field