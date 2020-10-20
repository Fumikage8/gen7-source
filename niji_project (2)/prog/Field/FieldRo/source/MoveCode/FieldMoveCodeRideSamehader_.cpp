//=================================================================================================
/**
*  @file   FieldMoveCodeRideSamehader.cpp
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
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideSamehader.h"
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
void FieldMoveCodeRideSamehader::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::MainProcess(pMoveCodeWork);

  // 共有・拡張ワークを取得
  SamehaderWork* pSamehaderWork = (SamehaderWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(SamehaderWork) );
  ExtendWork*    pExtendWork    = (ExtendWork*)FieldMoveCodeManager::GetMoveCodeExtendWork( pMoveCodeWork, sizeof(ExtendWork) );

  // ラッシュ時のみ擬似慣性を用意する
  if( pSamehaderWork->playerWork.nowState == PLAYER_MOVE_WALK_LOOP
   || pSamehaderWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP )
  {
    // 一定フレーム間の姿勢を確保
    for( u32 i=ExtendWork::QUA_ARRAY_SIZE-1 ; i>0 ; i-- )
    {
      pExtendWork->moveModelQuaArray[i] = pExtendWork->moveModelQuaArray[i-1];
    }
    pExtendWork->moveModelQuaArray[0] = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();

    // 迂回角度(0～180度)を0.0f～1.0fのレンジに変換
    f32 degRate = pSamehaderWork->playerWork.angleToTarget/180.0f;

    // 曲がりにくさを求める
    u32 index = 0;
    if      ( pSamehaderWork->playerWork.nowState == PLAYER_MOVE_WALK_LOOP ) { index = (u32)( (f32)(ExtendWork::QUA_ARRAY_SIZE-1) * degRate * m_slipRateWalk ); }
    else if ( pSamehaderWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP  ) { index = (u32)( (f32)(ExtendWork::QUA_ARRAY_SIZE-1) * degRate * m_slipRateDash  ); }
    index = gfl2::math::Clamp( index, (u32)0 , (u32)(ExtendWork::QUA_ARRAY_SIZE-1) );
    gfl2::math::Quaternion moveQua( pExtendWork->moveModelQuaArray[index] );

    // 擬似慣性を設定
    pMoveCodeWork->pModel->SetCustomValueForOrigin( m_PlayerMoveSettingTable[ pSamehaderWork->playerWork.nowState ].moveSpeed , moveQua );
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
void FieldMoveCodeRideSamehader::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::PostProcess( pMoveCodeWork );

  // 泣き声
  if( IsTrgInputDashButton() )
  {
    // @fix GFNMCat[965] 泣き声SEを後勝ちにする
    if( Sound::IsSEPlaying( SEQ_SE_FLD_RIDE_SAMEHADAR_PV_JETREADY ) ){ Sound::StopSE( SEQ_SE_FLD_RIDE_SAMEHADAR_PV_JETREADY ); }
    Sound::PlaySE(SEQ_SE_FLD_RIDE_SAMEHADAR_PV_JETREADY);
  }

  // ジェットエフェクトの表示位置を変更
  Field::MoveModel::FieldMoveModelPlayer* pPlayer            = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pSamehader         = pPlayer->GetOnMoveModel();
  s32                                     jointWaistIndex    = pSamehader->GetCharaDrawInstance()->GetJointIndex("Waist");
  gfl2::math::Vector3                     jointWaistWorldPos = pSamehader->GetCharaDrawInstance()->GetJointWorldPosition(jointWaistIndex);
  gfl2::math::Vector3                     offset             = jointWaistWorldPos - pPlayer->GetCharaDrawInstance()->GetPosition();
  offset.y -= WATER_DEPTH;
  offset.z -= 25.0f;

  // @fix GFNMCat[1668] サメハダーのジェット移動時のエフェクトが不自然に欠ける
  //                    水面より少し上に半透明エフェクトを出しソート順を制御する
  const f32 SORT_ADJUSTMENT = 7.5f;
  if( offset.y-SORT_ADJUSTMENT <= WATER_DEPTH )
  {
    offset.y = WATER_DEPTH + SORT_ADJUSTMENT;
  }

  if(pPlayer->GetSamehaderJet())
  {
    pPlayer->GetSamehaderJet()->setOffsetPosition( offset );
  }
  if(pPlayer->GetSamehaderRun())
  {
    pPlayer->GetSamehaderRun()->setOffsetPosition( offset );
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  再開
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideSamehader::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
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
void FieldMoveCodeRideSamehader::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
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
      if(pPlayer->GetSamehaderWait())
      {
        pPlayer->GetSamehaderWait()->SetVisible(false);
      }
      if(pPlayer->GetSamehaderJet())
      {
        pPlayer->GetSamehaderJet()->DeleteEmitter();
      }
      if(pPlayer->GetSamehaderRun())
      {
        pPlayer->GetSamehaderRun()->DeleteEmitter();
      }
      Sound::StopSE(SEQ_SE_FLD_RIDE_SAMEHADA_JET   );
      Sound::StopSE(SEQ_SE_FLD_RIDE_SAMEHADA_CRUISE);
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
u32 FieldMoveCodeRideSamehader::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
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
void FieldMoveCodeRideSamehader::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  FieldMoveCodePlayer::MainProcessSubChangeState(pMoveCodeWork,state,nowState,oldState);

  // 子キャラ(ライドポケモン)もモーションを変更
  Field::MoveModel::FieldMoveModelPlayer* pPlayer        = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pSamehader     = pPlayer->GetOnMoveModel();
  SamehaderWork*                          pSamehaderWork = (SamehaderWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(SamehaderWork) );
  if( pSamehader )
  {
    pSamehader->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
    if( !pSamehaderWork->playerWork.isContinueMotion )
    {
      pSamehader->GetCharaDrawInstance()->ChangeAnimation(m_PlayerMoveSettingTable[state].motionId);
    }
  }

  // 待機エフェクト再生
  if( pSamehaderWork->playerWork.nowState == PLAYER_MOVE_WAIT )
  {
    if(pPlayer->GetSamehaderWait())
    {
      pPlayer->GetSamehaderWait()->SetVisible( true ); 
    }
  }
  // 通常移動時のエフェクト/SE再生
  if( pSamehaderWork->playerWork.nowState == PLAYER_MOVE_WALK_LOOP )
  {
    Sound::PlaySE(SEQ_SE_FLD_RIDE_SAMEHADA_CRUISE);
    if(pPlayer->GetSamehaderRun())
    {
      pPlayer->GetSamehaderRun()->CreateEmitter();
    }
  }
  // ジェット時のエフェクト/SE再生
  if( pSamehaderWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP )
  {
    Sound::PlaySE(SEQ_SE_FLD_RIDE_SAMEHADA_JET);
    if(pPlayer->GetSamehaderJet())
    {
      pPlayer->GetSamehaderJet()->CreateEmitter();
    }
  }

  // 待機のエフェクト停止
  if( pSamehaderWork->playerWork.oldState == PLAYER_MOVE_WAIT )
  {
    if(pPlayer->GetSamehaderWait())
    {
      pPlayer->GetSamehaderWait()->SetVisible( false );
    }
  }
  // 通常移動時のエフェクト/SE停止
  if( pSamehaderWork->playerWork.oldState == PLAYER_MOVE_WALK_LOOP )
  {
    Sound::StopSE(SEQ_SE_FLD_RIDE_SAMEHADA_CRUISE);
    if(pPlayer->GetSamehaderRun())
    {
      pPlayer->GetSamehaderRun()->DeleteEmitter();
    }
  }
  // ジェット時のエフェクト/SE停止
  if( pSamehaderWork->playerWork.oldState == PLAYER_MOVE_RUN_LOOP )
  {
    Sound::StopSE(SEQ_SE_FLD_RIDE_SAMEHADA_JET);
    if(pPlayer->GetSamehaderJet())
    {
      pPlayer->GetSamehaderJet()->DeleteEmitter();
    }
  }
  // ジェットで岩を砕いた直後に壁ドンしないようにジェット直後は壁ドンを禁止
  if( state == PLAYER_MOVE_RUN_LOOP )
  {
    pSamehaderWork->playerWork.lockDirDisableFrame = 6;
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
