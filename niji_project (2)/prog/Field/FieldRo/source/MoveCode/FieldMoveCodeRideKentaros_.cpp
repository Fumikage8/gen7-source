//=================================================================================================
/**
*  @file   FieldMoveCodeRideKentaros.cpp
*  @brief  フィールド用 動作コード ケンタロスライド
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
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideKentaros.h"
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
void FieldMoveCodeRideKentaros::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::MainProcess(pMoveCodeWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  ポスト
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKentaros::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::PostProcess( pMoveCodeWork );

  Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pKentaros     = pPlayer->GetOnMoveModel();
  KentarosWork*                           pKentarosWork = (KentarosWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(KentarosWork) );

  if( pKentarosWork->playerWork.nowState == PLAYER_MOVE_RUN_LOOP )
  {
    // ラッシュ時のエフェクトの作成
    if( pPlayer->GetCharaDrawInstance()->GetAnimationEndFrame() - 1 == pPlayer->GetCharaDrawInstance()->GetAnimationFrame() )
    {
      if(pPlayer->GetKentarosRushSmoke())
      {
        pPlayer->GetKentarosRushSmoke()->SetVisibleRush(true);
        pPlayer->GetKentarosRushSmoke()->CreateSmoke( pPlayer->GetCharaDrawInstance()->GetPosition() );
      }
    }
  }

  if( pKentarosWork->playerWork.nowState == PLAYER_MOVE_RUN_START )
  {
    // ラッシュ時のSE再生
    const u32 RUSH_SE_FRAME = 20;
    if( RUSH_SE_FRAME == pPlayer->GetCharaDrawInstance()->GetAnimationFrame() )
    {
      Sound::PlaySE(SEQ_SE_FLD_RIDE_RUSH);
    }
  }

  // 泣き声
  if( IsTrgInputDashButton() )
  {
    // @fix GFNMCat[965] 泣き声SEを後勝ちにする
    if( Sound::IsSEPlaying( SEQ_SE_FLD_RIDE_KENTAUROS_PV_RUSHSTART ) ){ Sound::StopSE( SEQ_SE_FLD_RIDE_KENTAUROS_PV_RUSHSTART ); }
    Sound::PlaySE(SEQ_SE_FLD_RIDE_KENTAUROS_PV_RUSHSTART);
  }

  // ラッシュエフェクトの表示位置を変更
  if( pPlayer->GetKentarosRushSmoke() )
  {
    s32                 jointWaistIndex    = pKentaros->GetCharaDrawInstance()->GetJointIndex("Waist");
    gfl2::math::Vector3 jointWaistWorldPos = pKentaros->GetCharaDrawInstance()->GetJointWorldPosition(jointWaistIndex);
    gfl2::math::Vector3 offset             = jointWaistWorldPos - pPlayer->GetCharaDrawInstance()->GetPosition();
    pPlayer->GetKentarosRushSmoke()->setOffsetPosition( offset );
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  再開
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKentaros::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
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
void FieldMoveCodeRideKentaros::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::Suspend( pMoveCodeWork );

  if( !pMoveCodeWork->pModel->CheckStateBit( FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND ) )
  {
    // 子キャラ(ライドポケモン)も待機状態に変更
    Field::MoveModel::FieldMoveModelPlayer* pPlayer   = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pKentaros = pPlayer->GetOnMoveModel();
    if( pKentaros )
    {
      if( pKentaros->GetCharaDrawInstance()->GetAnimationId() != (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) )
      {
        pKentaros->GetCharaDrawInstance()->ChangeAnimation( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
      }
      // 演出効果を停止
      if(pPlayer->GetKentarosRushSmoke())
      {
        pPlayer->GetKentarosRushSmoke()->SetVisibleRush(false);
        pPlayer->GetKentarosRushSmoke()->DeleteSmoke();
      }
      Sound::StopSE(SEQ_SE_FLD_RIDE_RUSH);
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
u32 FieldMoveCodeRideKentaros::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
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
void FieldMoveCodeRideKentaros::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  FieldMoveCodePlayer::MainProcessSubChangeState(pMoveCodeWork,state,nowState,oldState);

  // 子キャラ(ライドポケモン)もモーションを変更
  Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pKentaros     = pPlayer->GetOnMoveModel();
  KentarosWork*                           pKentarosWork = (KentarosWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  if( pKentaros )
  {
    pKentaros->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
    if( !pKentarosWork->playerWork.isContinueMotion )
    {
      pKentaros->GetCharaDrawInstance()->ChangeAnimation(m_PlayerMoveSettingTable[state].motionId);
    }
  }

  // ラッシュのエフェクト停止
  if( pKentarosWork->playerWork.oldState == PLAYER_MOVE_RUN_LOOP )
  {
    if(pPlayer->GetKentarosRushSmoke())
    {
      pPlayer->GetKentarosRushSmoke()->SetVisibleRush(false);
      pPlayer->GetKentarosRushSmoke()->DeleteSmoke();
    }
    Sound::StopSE(SEQ_SE_FLD_RIDE_RUSH);
  }

  // ラッシュ開始からラッシュループ以外に遷移した場合はSEをストップ
  if( pKentarosWork->playerWork.oldState == PLAYER_MOVE_RUN_START && pKentarosWork->playerWork.nowState != PLAYER_MOVE_RUN_LOOP )
  {
    Sound::StopSE(SEQ_SE_FLD_RIDE_RUSH);
  }

  // ラッシュで岩を砕いた直後に壁ドンしないようにラッシュ直後は壁ドンを禁止
  if( state == PLAYER_MOVE_RUN_LOOP )
  {
    pKentarosWork->playerWork.lockDirDisableFrame = 6;
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
