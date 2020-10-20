//=================================================================================================
/**
*  @file   FieldActionCommandStepMoveWithoutStartEnd.cpp
*  @brief  フィールド用 アクションコマンド ステップ移動(開始・終了無し)
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepMoveWithoutStartEnd.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// Collision
#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/RaycastCustomCallback.h"
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
ActionCommandStepMoveWithoutStartEnd::ActionCommandStepMoveWithoutStartEnd()
{
  m_pFuncTbl[0] = &ActionCommandStepMoveWithoutStartEnd::ActionStepInit;
  m_pFuncTbl[1] = &ActionCommandStepMoveWithoutStartEnd::ActionMain;
  m_pFuncTbl[2] = &ActionCommandStepMoveWithoutStartEnd::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(歩き中)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMoveWithoutStartEnd::ActionStepInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // ワークを取得
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

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
      pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_WALK_LOOP + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_WALK_LOOP + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET );
      pWork->frameMax = (u32)( pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame() );
      pWork->frame    = 0;
      isRide          = true;
    }
  }

  // 一歩分のフレーム数
  f32 stepFrame = 0.0f;

  // 歩きモーションの再生
  if( isRide )
  {
    stepFrame = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
  }
  else
  {
    pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_FI010_WALK01);
    stepFrame = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame() / 2.0f;
    // 歩き・走りモーションのN歩分から合計再生フレームを求める
    pWork->frameMax = pWork->stepCnt * (u32)( stepFrame );
    pWork->frame    = 0;
  }

  // 1フレームでアニメーションの再生を終わらせる
  if( pWork->isOneFrame )
  {
    f32 tempStepFrame0 = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();
    f32 tempStepFrame1 = isRide ? static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetCharaDrawInstance()->GetAnimationStepFrame() : 0.0f;
    gfl2::math::Vector4 nowPosition( pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition().x,
                                     pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition().y,
                                     pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition().z,
                                     0.0f );

    // アニメーションを一歩分毎に再生する
    pActionCommandWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( stepFrame );
    if( isRide )
    {
      static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationStepFrame( stepFrame );
    }

    // 歩数分一気に進める
    gfl2::math::Vector3 addPos(0.0f,0.0f,0.0f);
    for( u32 i=0 ; i<pWork->stepCnt ; i++ )
    {
      // 一歩分更新
      pActionCommandWork->pModel->GetCharaDrawInstance()->UpdateAnimation();
      if( isRide )
      {
        static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->UpdateAnimation();
      }

      // ジョイント"Origin"の移動要素を取得
      gfl2::math::Vector3  originMove( pActionCommandWork->pModel->GetCharaDrawInstance()->GetWalkSpeed( -1.0f ) );
      gfl2::math::Matrix34 mat;
      pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(mat);
      originMove.x = 0.0f;
      originMove.y = 0.0f;
      addPos      += mat.TransformNormal( originMove );
    }

    // レイキャスト
    RaycastCustomCallback::HIT_DATA hitData;
    gfl2::math::Vector4 newPosition( pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition().x + addPos.x,
                                     pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition().y + addPos.y,
                                     pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition().z + addPos.z,
                                     0.0f );

    if( pActionCommandWork->pModel->GetCollisionSceneContainerForWall()->RaycastFromStaticActorsMesh( nowPosition,newPosition,&hitData )
     && pActionCommandWork->pModel->GetDynamicActor(COLLISION_TYPE_TERRAIN)
     && pActionCommandWork->pModel->GetDynamicActor(COLLISION_TYPE_TERRAIN)->GetShapeType() == BaseActor::SHAPE_SPHERE )
    {
      // 交点から自機へのベクトルを取得
      gfl2::math::Vector3 intersection(hitData.intersection.x,hitData.intersection.y,hitData.intersection.z);
      gfl2::math::Vector3 playerToIntersection( intersection - pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition() );

      // 交点よりコリジョンの半径分手前に自機を移動させるようベクトルを更新
      SphereShape* pSphere = static_cast<SphereShape*>(pActionCommandWork->pModel->GetDynamicActor(COLLISION_TYPE_TERRAIN)->GetShape());
      f32 adjustLength = playerToIntersection.Length() - pSphere->m_r;
      if( adjustLength > 0.0f )
      {
        gfl2::math::Vector3 adjsutVec( playerToIntersection.Normalize() );
        addPos = adjsutVec * adjustLength;
      }
    }

    // 1アニメーション分の移動量を動作モデルに適用する
    pActionCommandWork->pModel->GetCharaDrawInstance()->SetPosition( pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition() + addPos );
    pWork->frameMax  = 0;


    // アニメーションの設定を基に戻す
    pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( (u32)( pActionCommandWork->pModel->GetDefaultIdleAnimationId() ) );
    pActionCommandWork->pModel->GetCharaDrawInstance()->SetAnimationStepFrame( tempStepFrame0 );
    if( isRide )
    {
      static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( (u32)( pActionCommandWork->pModel->GetDefaultIdleAnimationId() ) ); 
      static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationStepFrame( tempStepFrame1 );
    }
  }

  // 次へ
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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMoveWithoutStartEnd::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame += pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();
  if( pWork->frame <= pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMoveWithoutStartEnd::ActionEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}

} // mmodel
} // field
