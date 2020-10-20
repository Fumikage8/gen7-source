//=================================================================================================
/**
*  @file   FieldActionCommandStepMove.cpp
*  @brief  フィールド用 アクションコマンド ステップ移動
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepMove.h"
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
ActionCommandStepMove::ActionCommandStepMove()
{
  m_pFuncTbl[0] = &ActionCommandStepMove::ActionStartInit;
  m_pFuncTbl[1] = &ActionCommandStepMove::ActionMain;
  m_pFuncTbl[2] = &ActionCommandStepMove::ActionStepInit;
  m_pFuncTbl[3] = &ActionCommandStepMove::ActionMain;
  m_pFuncTbl[4] = &ActionCommandStepMove::ActionEndInit;
  m_pFuncTbl[5] = &ActionCommandStepMove::ActionMain;
  m_pFuncTbl[6] = &ActionCommandStepMove::ActionIdleInit;
  m_pFuncTbl[7] = &ActionCommandStepMove::ActionMain;
  m_pFuncTbl[8] = &ActionCommandStepMove::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(歩き始め)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMove::ActionStartInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{ 
  Work* pWork  = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  pWork->frame = 0;

  // コリジョンによる押し返しを受けない
  if( !pWork->enbaleCollision )
  {
    pActionCommandWork->pModel->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );
  }

  // プレイヤーの専用処理
  b32 isRide = false;
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
      isRide          = true;
    }
  }

  // 歩き始めモーションを単発再生する
  if( !isRide )
  {
    pWork->frameMax = 0;
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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMove::ActionStepInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
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
      pWork->frameMax = pWork->stepCnt * (u32)( pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame() );
      pWork->frame    = 0;
      isRide          = true;
    }
  }

  // 歩きモーションの再生
  if( !isRide )
  {
    pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation(animationStepId);
    // 歩き・走りモーションのN歩分から合計再生フレームを求める( 歩き・走りモーションは二歩分で作られている )
    pWork->frameMax = pWork->stepCnt * (u32)( pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame() / 2.0f );
    pWork->frame    = 0;
  }

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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMove::ActionEndInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork  = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // WalkEndをスキップする場合
  if( !pWork->isEnableWalkEnd )
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 ); //WalkEndをスキップしIdleInitへ 
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }


  pWork->frame = 0;

  // コリジョンによる押し返しを受けるように戻す
  if( !pWork->enbaleCollision )
  {
    pActionCommandWork->pModel->OffStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );
  }

  // プレイヤー専用処理
  b32 isRide = false;
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
      isRide          = true;
    }
  }

  // 歩き終わりモーションを単発再生する
  if( !isRide )
  {
    pWork->frameMax = 0;
  }
  
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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMove::ActionIdleInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork  = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // IdleInitをスキップする場合
  if( !pWork->isEnableIdleInit )
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 ); //Endへ 
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  // 待機モーションを単発再生する
  pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( (u32)( pActionCommandWork->pModel->GetDefaultIdleAnimationId() ) );

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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMove::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 移動量をカスタム
  const u32 MOVE_SEQUENCE = 3;
  if( pActionCommandWork->sequence == MOVE_SEQUENCE )
  {
    pActionCommandWork->pModel->SetCustomValueForOrigin( pWork->moveRate , pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat() );
  }

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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandStepMove::ActionEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}

} // mmodel
} // field
