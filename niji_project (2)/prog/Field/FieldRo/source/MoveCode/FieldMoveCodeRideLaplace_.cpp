//=================================================================================================
/**
*  @file   FieldMoveCodeRideLaplace.cpp
*  @brief  フィールド用 動作コード サメハダーライド
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideLaplace.h"
// Sound
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideLaplace::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::MainProcess(pMoveCodeWork);

  // 共有・拡張ワークを取得
  LaplaceWork* pLaplaceWork = (LaplaceWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(LaplaceWork) );
  ExtendWork*   pExtendWork = (ExtendWork* )FieldMoveCodeManager::GetMoveCodeExtendWork( pMoveCodeWork, sizeof(ExtendWork) );

  // ラッシュ時のみ擬似慣性を用意する
  if( pLaplaceWork->playerWork.nowState == PLAYER_MOVE_WALK_LOOP
   || pLaplaceWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP )
  {
    // 一定フレーム間の姿勢を確保
    for( u32 i=ExtendWork::QUA_ARRAY_SIZE-1 ; i>0 ; i-- )
    {
      pExtendWork->moveModelQuaArray[i] = pExtendWork->moveModelQuaArray[i-1];
    }
    pExtendWork->moveModelQuaArray[0] = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();

    // 迂回角度(0～180度)を0.0f～1.0fのレンジに変換
    f32 degRate = pLaplaceWork->playerWork.angleToTarget/180.0f;

    // 曲がりにくさを求める
    u32 index = 0;
    if      ( pLaplaceWork->playerWork.nowState == PLAYER_MOVE_WALK_LOOP ) { index = (u32)( (f32)(ExtendWork::QUA_ARRAY_SIZE-1) * degRate * m_slipRateWalk ); }
    else if ( pLaplaceWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP  ) { index = (u32)( (f32)(ExtendWork::QUA_ARRAY_SIZE-1) * degRate * m_slipRateDash  ); }
    index = gfl2::math::Clamp( index, (u32)0 , (u32)(ExtendWork::QUA_ARRAY_SIZE-1) );
    gfl2::math::Quaternion moveQua( pExtendWork->moveModelQuaArray[index] );

    // 擬似慣性を設定
    pMoveCodeWork->pModel->SetCustomValueForOrigin( m_PlayerMoveSettingTable[ pLaplaceWork->playerWork.nowState ].moveSpeed , moveQua );
  }
  else
  {
    // 一定フレーム間の姿勢を初期化
    for( u32 i=0 ; i<ExtendWork::QUA_ARRAY_SIZE ; i++ )
    {
      pExtendWork->moveModelQuaArray[i] = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  ポスト
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideLaplace::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::PostProcess( pMoveCodeWork );

  // 泣き声
  if( IsTrgInputDashButton() )
  {
    // @fix GFNMCat[965] 泣き声SEを後勝ちにする
    if( Sound::IsSEPlaying( SEQ_SE_FLD_RIDE_RAPLACE_PV_SPEEDUP ) ){ Sound::StopSE( SEQ_SE_FLD_RIDE_RAPLACE_PV_SPEEDUP ); }
    Sound::PlaySE(SEQ_SE_FLD_RIDE_RAPLACE_PV_SPEEDUP);
  }

  // 走り/ラッシュエフェクトの表示位置を変更
  Field::MoveModel::FieldMoveModelPlayer* pPlayer            = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pLaplace           = pPlayer->GetOnMoveModel();
  s32                                     jointWaistIndex    = pLaplace->GetCharaDrawInstance()->GetJointIndex("Waist");
  gfl2::math::Vector3                     jointWaistWorldPos = pLaplace->GetCharaDrawInstance()->GetJointWorldPosition(jointWaistIndex);
  gfl2::math::Vector3                     offset             = jointWaistWorldPos - pPlayer->GetCharaDrawInstance()->GetPosition();
  offset.y -= WATER_DEPTH;
  if(pPlayer->GetLaplaceFlow())
  {
    pPlayer->GetLaplaceFlow()->setOffsetPosition( offset );
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  再開
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideLaplace::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
{
  FieldMoveCodePlayer::Resume(pMoveCodeWork);
};

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  中断
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideLaplace::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::Suspend( pMoveCodeWork );

  // 拡張ワークを初期化
  ExtendWork* pExtendWork = (ExtendWork*)FieldMoveCodeManager::GetMoveCodeExtendWork( pMoveCodeWork, sizeof(ExtendWork) );
  GFL_ASSERT( pExtendWork );
  for( u32 i=0 ; i<ExtendWork::QUA_ARRAY_SIZE ; i++ )
  {
    pExtendWork->moveModelQuaArray[i] = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  }

  // 子キャラ(ライドポケモン)も待機状態に変更
  if( !pMoveCodeWork->pModel->CheckStateBit( FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND ) )
  {
    Field::MoveModel::FieldMoveModelPlayer* pPlayer    = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pSamehader = pPlayer->GetOnMoveModel();
    if( pSamehader )
    {
      if( pSamehader->GetCharaDrawInstance()->GetAnimationId() != (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) )
      {
        pSamehader->GetCharaDrawInstance()->ChangeAnimation( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
      }

      // 演出効果を停止
      if(pPlayer->GetLaplaceWait())
      {
        pPlayer->GetLaplaceWait()->SetVisible(false);
      }
      if(pPlayer->GetLaplaceFlow())
      {
        pPlayer->GetLaplaceFlow()->DeleteEmitter();
      }
      Sound::StopSE(SEQ_SE_FLD_RIDE_RAPLACE_CRUISE);
    }
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodeRideLaplace::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  return FieldMoveCodePlayer::MainProcessSubOverrideState(pMoveCodeWork,state,nowState,oldState);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideLaplace::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  FieldMoveCodePlayer::MainProcessSubChangeState(pMoveCodeWork,state,nowState,oldState);

  // 子キャラ(ライドポケモン)もモーションを変更
  Field::MoveModel::FieldMoveModelPlayer* pPlayer      = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pLaplace     = pPlayer->GetOnMoveModel();
  LaplaceWork*                            pLaplaceWork = (LaplaceWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  if( pLaplace )
  {
    pLaplace->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
    if( !pLaplaceWork->playerWork.isContinueMotion )
    {
      pLaplace->GetCharaDrawInstance()->ChangeAnimation(m_PlayerMoveSettingTable[state].motionId);
    }
  }

  // 待機エフェクト再生
  if( pLaplaceWork->playerWork.nowState == PLAYER_MOVE_WAIT )
  {
    if(pPlayer->GetLaplaceWait())
    {
      pPlayer->GetLaplaceWait()->SetVisible( true );
    }
  }
  // 通常移動時のエフェクト/SE再生
  if( pLaplaceWork->playerWork.nowState == PLAYER_MOVE_WALK_LOOP )
  {
    Sound::PlaySE(SEQ_SE_FLD_RIDE_RAPLACE_CRUISE);
    if(pPlayer->GetLaplaceFlow())
    {
      pPlayer->GetLaplaceFlow()->CreateEmitter();
    }
  }
  // 高速移動時のエフェクト再生
  if( pLaplaceWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP )
  {
    if(pPlayer->GetLaplaceFlow())
    {
      pPlayer->GetLaplaceFlow()->CreateEmitter();
    }
  }

  // 待機のエフェクト停止
  if( pLaplaceWork->playerWork.oldState == PLAYER_MOVE_WAIT )
  {
    if(pPlayer->GetLaplaceWait())
    {
      pPlayer->GetLaplaceWait()->SetVisible( false );
    }
  }
  // 通常移動時のエフェクト/SE停止
  if( pLaplaceWork->playerWork.oldState == PLAYER_MOVE_WALK_LOOP )
  {
    Sound::StopSE(SEQ_SE_FLD_RIDE_RAPLACE_CRUISE);
    if(pPlayer->GetLaplaceFlow())
    {
      pPlayer->GetLaplaceFlow()->DeleteEmitter();
    }
  }
  // 高速移動時のエフェクト停止
  if( pLaplaceWork->playerWork.oldState == PLAYER_MOVE_RUN_LOOP )
  {
    if(pPlayer->GetLaplaceFlow())
    {
      pPlayer->GetLaplaceFlow()->DeleteEmitter();
    }
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
