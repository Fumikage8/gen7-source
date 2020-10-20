//=================================================================================================
/**
*  @file   FieldMoveCodeRideRoba2.cpp
*  @brief  フィールド用 動作コード 地面馬ライド
*  @author ikuta_junya@gamefreak.co.jp
*  @date   2015.10.15
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
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideRoba2.h"
// Sound
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"

// カメラ
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Fieldmap.h"

namespace Field{ namespace MoveModel {

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideRoba2::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
  {

    //
    // 基底更新
    //
    FieldMoveCodePlayer::MainProcess( pMoveCodeWork );
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  ポスト
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideRoba2::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
  {
    FieldMoveCodePlayer::PostProcess( pMoveCodeWork );

    // 泣き声
    if( IsTrgInputDashButton() )
    {
      // @fix GFNMCat[965] 泣き声SEを後勝ちにする
      if( Sound::IsSEPlaying( SEQ_SE_FLD_0007 ) ){ Sound::StopSE( SEQ_SE_FLD_0007 ); }
      Sound::PlaySE(SEQ_SE_FLD_0007);
    }

    // カメラ振動
    UpdateCameraShake( pMoveCodeWork );

  };

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  再開
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideRoba2::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
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
  void FieldMoveCodeRideRoba2::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
  { 
    Roba2Work* pRoba2Work = (Roba2Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Roba2Work) );
    
    // 初期化してはいけないものの値を覚えておく

    // 内部で動作コードの初期化が呼ばれる
    FieldMoveCodePlayer::Suspend( pMoveCodeWork );
    

    // 子キャラ(ライドポケモン)も待機状態に変更
    Field::MoveModel::FieldMoveModelPlayer* pPlayer   = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel* pRoba2 = pPlayer->GetOnMoveModel();
    if( pRoba2 )
    {
      if(pPlayer->GetCharaDrawInstance()->GetAnimationId() != (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ))
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimation  ( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
      }
      if( pRoba2->GetCharaDrawInstance()->GetAnimationId() != (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) )
      {
        pRoba2->GetCharaDrawInstance()->ChangeAnimation( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
      }
    }
  };

  //----------------------------------------------------------------------------
  /**
  *  @brief  指定ステートがアクセル関連ステートかチェック
  *
  *  @param  state   チェック対象ステート
  *
  *  @return trueならアクセル関連、falseならその他
  */
  //-----------------------------------------------------------------------------
  b32 FieldMoveCodeRideRoba2::IsAccelState( u32 state )
  {
    switch( state )
    {
    case PLAYER_MOVE_WAIT_TOKUSHU:
    case PLAYER_MOVE_RUN_START:
    case PLAYER_MOVE_RUN_LOOP:
    case PLAYER_MOVE_RUN_LOOP_SLOPE:
    case PLAYER_MOVE_RUN_LOOP_DISCOVER:
    case PLAYER_MOVE_RUN_END:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  u32 FieldMoveCodeRideRoba2::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
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
  void FieldMoveCodeRideRoba2::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
  {
    FieldMoveCodePlayer::MainProcessSubChangeState(pMoveCodeWork,state,nowState,oldState);

    // 子キャラ(ライドポケモン)もモーションを変更
    Field::MoveModel::FieldMoveModelPlayer* pPlayer    = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pRoba2     = pPlayer->GetOnMoveModel();
    Roba2Work*                              pRoba2Work = (Roba2Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Roba2Work) );

    if( pRoba2 )
    {
      pRoba2->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
      if( !pRoba2Work->playerWork.isContinueMotion )
      {
        pRoba2->GetCharaDrawInstance()->ChangeAnimation(m_PlayerMoveSettingTable[state].motionId);
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  カメラ振動関連更新処理
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideRoba2::UpdateCameraShake( FieldMoveCodeWork* pMoveCodeWork ) const
  {
    // 
    // 移動にあわせてカメラを振動させる
    // 
    Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pRoba2        = pPlayer->GetOnMoveModel();
    Roba2Work*                           pRoba2Work = (Roba2Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Roba2Work) );

    GameSys::GameManager*         pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap*                     pFieldMap      = pGameManager->GetFieldmap();
    Field::Camera::CameraManager* pCameraManager = pFieldMap->GetCameraManager();

    if( pCameraManager )
    {
      // @note 足跡エフェクトのルーチンを参考にしています、フレーム指定も16/4/3時点の足跡タイミングに合わせています
      const u32 ARRAY_SIZE = 4;
      u32 animationFrame     = pRoba2->GetCharaDrawInstance()->GetAnimationFrame();
      u32 animationLeftHand [ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
      u32 animationRightHand[ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
      u32 animationLeftFoot [ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
      u32 animationRightFoot[ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
      b32 isRun = false;

      switch( pRoba2->GetCharaDrawInstance()->GetAnimationId() )
      {
        // 通常歩き
      case P2_BASE_RI062_RUN01:
        {
          if( !m_CameraShakeSetting.isUseWalk ) return;

          animationLeftHand [0] = 5;
          animationRightHand[0] = 14;
          animationLeftFoot [0] = 15;
          animationRightFoot[0] = 6;
          break;
        }
        // 通常歩き終了
      case P2_BASE_RI064_RUNSTOP01:
        {
          if( !m_CameraShakeSetting.isUseWalk ) return;
          
          animationLeftHand [0] = 4;
          animationRightHand[0] = 8;
          animationLeftFoot [0] = 9;
          animationRightFoot[0] = 7;
          break;
        }
        // アクセル
      case P2_BASE_RI069_ACCELERUN01:
        {
          if( !m_CameraShakeSetting.isUseRun ) return;
          
          animationLeftHand [0] = 8;
          animationRightHand[0] = 5;
          animationLeftFoot [0] = 2;
          animationRightFoot[0] = 14;

          isRun = true;
          break;
        }
        // アクセル終了
      case P2_BASE_RI070_ACCELERUNSTOP01:
        {
          if( !m_CameraShakeSetting.isUseRun ) return;
          
          animationLeftHand [0] = 3;
          animationRightHand[0] = 5;
          animationLeftFoot [0] = 7;
          animationRightFoot[0] = 2;

          isRun = true;
          break;
        }

      default:
        {
          return;
        }
      }

      // 参照する振動設定を取得
      const CAMERA_SHAKE_SETTING_CORE* pSettingCore;
      if( isRun )
      {
        pSettingCore = &m_CameraShakeSetting.runShake;
      }
      else
      {
        pSettingCore = &m_CameraShakeSetting.walkShake;
      }

      // 振動するフレームかチェック
      b32 isHit     = false;
      for( u32 i=0 ; i<ARRAY_SIZE; i++ )
      {
        // 各足単位で振動発生をチェック
        if( pSettingCore->isLeftHand && animationFrame == animationLeftHand[i] )
        {
          isHit = true;
          break;
        }

        if( pSettingCore->isRightHand && animationFrame == animationRightHand[i] )
        {
          isHit = true;
          break;
        }

        if( pSettingCore->isLeftFoot && animationFrame == animationLeftFoot[i] )
        {
          isHit = true;
          break;
        }

        if( pSettingCore->isRightFoot && animationFrame == animationRightFoot[i] )
        {
          isHit = true;
          break;
        }
      }

      // 振動を実行
      if( isHit )
      {
        if( pCameraManager->GetMainViewCamera() )
        {
          // 縦ゆれをリクエストする
          pCameraManager->GetMainViewCamera()->GetControllerBase()->ShakeVerticalRequest(
            pSettingCore->mass,
            pSettingCore->k,
            pSettingCore->firstVelocity,
            pSettingCore->damping
            );
        }
      } 

    } // if( pCameraManager )
  }

}; //end of namespace MoveModel
}; //end of namespace Field