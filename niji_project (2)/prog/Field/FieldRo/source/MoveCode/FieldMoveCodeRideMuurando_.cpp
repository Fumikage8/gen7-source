//=================================================================================================
/**
*  @file   FieldMoveCodeRideMuurando.cpp
*  @brief  フィールド用 動作コード ムーランドライド
*  @author ikuta_junya@gamefreak.co.jp
*  @date   2015.10.15
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// Singletonアクセス用
#include "Field/FieldRo/include/Fieldmap.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideMuurando.h"
// Sound
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "Field/FieldRo/include/Dowsing/FieldDowsingTool.h"

namespace Field{ namespace MoveModel {

  static const s32 SEARCH_LOST_SE( SEQ_SE_FLD_0004 );

  f32 g_EffectScaleTable[] =
  {
    1.0f * 100.0f,
    0.75f * 100.0f

  };

  // エフェクトが追従するジョイントを設定する
  void SetEffectParentJoint( Field::Effect::EffectBallonIcon* pIcon, Field::MoveModel::FieldMoveModelPlayer* pPlayer )
  {
    Field::MoveModel::FieldMoveModel* pMuurando = pPlayer->GetOnMoveModel();
    poke_3d::model::BaseModel *pBaseModel = pMuurando->GetCharaDrawInstance();
	  s32 joint_index = pBaseModel->GetJointIndex( "Spine2" );

    if( joint_index != -1 )
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
      pIcon->SetParentJoint( pNode->GetJointInstanceNode( joint_index ) );
    }
  }

  // サーチ結果からエフェクトを設定する
  void SetUpEffect( Field::MoveModel::FieldMoveModelPlayer* pPlayer, Dowsing::SearchRank::Value oldRank, Dowsing::SearchRank::Value newRank, const gfl2::math::Vector3& rOffset )
  {
    Dowsing::RANK_SETTING newSetting;
    Dowsing::Tool::GetSetting( newRank, &newSetting );

    switch( newRank )
    {
      // ギザギザ付き吹きだしを開始
    case Dowsing::SearchRank::RANK_A:
      {
        if( oldRank == Dowsing::SearchRank::RANK_B )
        {
          // 吹き出しの変更なら一瞬で変更
          if(pPlayer->GetMuurandoExclamationEx())
          {
            pPlayer->GetMuurandoExclamationEx()->RequestOpenForKeepMode( false );
          }
          if(pPlayer->GetMuurandoExclamationNormal())
          {
            pPlayer->GetMuurandoExclamationNormal()->RequestCloseForKeepMode( false ); 
          }
        }
        else
        {
          // 吹き出しが初めてなら最初から
          if(pPlayer->GetMuurandoExclamationEx())
          {
            pPlayer->GetMuurandoExclamationEx()->RequestOpenForKeepMode( true );
            pPlayer->GetMuurandoExclamationEx()->SetAnimationStepFrame( Dowsing::NOTICE_ICON_STEP_FRAME );
          }
        }

        // 開始時のみ行う共通設定
        if(pPlayer->GetMuurandoExclamationEx())
        {
          pPlayer->GetMuurandoExclamationEx()->SetScale( g_EffectScaleTable[ newRank ] * 0.01f );
          SetEffectParentJoint( pPlayer->GetMuurandoExclamationEx(), pPlayer );
        }

        break;
      } // case Dowsing::SearchRank::RANK_A:

      // 通常吹きだしを開始
    case Dowsing::SearchRank::RANK_B:
      {
        if( oldRank == Dowsing::SearchRank::RANK_A )
        {
          // 吹き出しの変更なら一瞬で変更
          if(pPlayer->GetMuurandoExclamationEx())
          {
            pPlayer->GetMuurandoExclamationEx()->RequestCloseForKeepMode( false );
          }
          if(pPlayer->GetMuurandoExclamationNormal())
          {
            pPlayer->GetMuurandoExclamationNormal()->RequestOpenForKeepMode( false );
          }
        }
        else
        {
          // 吹き出しが初めてなら最初から
          if(pPlayer->GetMuurandoExclamationNormal())
          {
            pPlayer->GetMuurandoExclamationNormal()->RequestOpenForKeepMode( true );
            pPlayer->GetMuurandoExclamationNormal()->SetAnimationStepFrame( Dowsing::NOTICE_ICON_STEP_FRAME );
          }
        }
        
        // 開始時のみ行う共通設定
        if(pPlayer->GetMuurandoExclamationNormal())
        {
          pPlayer->GetMuurandoExclamationNormal()->SetScale( g_EffectScaleTable[ newRank ] * 0.01f );
          SetEffectParentJoint( pPlayer->GetMuurandoExclamationNormal(), pPlayer );
        }
        break;
      } // case Dowsing::SearchRank::RANK_B:

      // OFF
      case Dowsing::SearchRank::RANK_LOST:
      case Dowsing::SearchRank::RANK_NO_SEARCH:
        {
          // 今のエフェクトをOFF
          switch( oldRank )
          {
          case Dowsing::SearchRank::RANK_A:
            {
              if(pPlayer->GetMuurandoExclamationEx())
              {
                pPlayer->GetMuurandoExclamationEx()->RequestCloseForKeepMode( true );
              }
              break;
            }
          case Dowsing::SearchRank::RANK_B:
            {
              if(pPlayer->GetMuurandoExclamationNormal())
              {
                pPlayer->GetMuurandoExclamationNormal()->RequestCloseForKeepMode( true );
              }
              break;
            }
          }
        } // Dowsing::SearchRank::RANK_LOST:
    } // switch( newRank )
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideMuurando::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
  {
    g_EffectScaleTable[0] = m_EffectScaleTable[0];
    g_EffectScaleTable[1] = m_EffectScaleTable[1];


    // ボタン入力状況を取得
    GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
    gfl2::ui::Button*         pButton        = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); 

    Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pMuurando     = pPlayer->GetOnMoveModel();
    MuurandoWork*                           pMuurandoWork = (MuurandoWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(MuurandoWork) );

    const Dowsing::Tool* pDowsingTool = game_manager->GetFieldmap()->GetDowsingTool();
    Dowsing::SearchRank::Value nowRank( static_cast<Dowsing::SearchRank::Value>( pMuurandoWork->distanceRank ) );
    Dowsing::SearchRank::Value newRank( Dowsing::SearchRank::RANK_NO_SEARCH );

    //
    // 基底更新
    //
    FieldMoveCodePlayer::MainProcess( pMoveCodeWork );

    gfl2::math::Vector3 effectOffset = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( m_EffectOffset );

    if(pPlayer->GetMuurandoExclamationEx())
    {
      pPlayer->GetMuurandoExclamationEx()->SetOffest( effectOffset );
    }
    if(pPlayer->GetMuurandoExclamationNormal())
    {
      pPlayer->GetMuurandoExclamationNormal()->SetOffest( effectOffset );
    }

    //
    // サーチ判定処理
    //
    if( this->IsMuurandSearchState( pMoveCodeWork ) )
    {
      // 生物感を出すため、サーチの見た目から数フレーム遅れて実際のサーチ判定を行う
      if( pMuurandoWork->searchStartWaitCount < m_SearchStartWait )
      {
        pMuurandoWork->searchStartWaitCount += 1;
      }
    }
    else
    {
      pMuurandoWork->searchStartWaitCount = 0;
    }

    // サーチ判定処理
    if( pMuurandoWork->searchStartWaitCount >= m_SearchStartWait )
    {
      // 現在位置
      gfl2::math::Vector3 pos( pPlayer->GetPositionPrevUpdateEnd() );

      // 進行方向ベクトルを取得
      gfl2::math::Vector3 dir(0.0f,0.0f,1.0f);
      dir = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( dir );

      // @fix GFNMcat[3485]:サーチ挙動が正しく機能しない場合があったケアを追加。主人公の向きを正規化して渡すように変更。
      dir.y = 0.0f;
      if( dir.Length() > EPSILON )
      {
        dir.NormalizeRet();
        newRank = pDowsingTool->Search( pos, dir, m_SearchAngle );
      }
      else
      {
        GFL_ASSERT( 0 );
      }
    }

    //
    // サーチ判定適用処理( なった瞬間 )
    //
    if( newRank != nowRank )
    {
      // 共通処理
      Dowsing::RANK_SETTING newSetting;
      Dowsing::RANK_SETTING nowSetting;
      Dowsing::Tool::GetSetting( newRank, &newSetting );
      Dowsing::Tool::GetSetting( nowRank, &nowSetting );

      // ループSEを変更
      if( nowSetting.loopSE != newSetting.loopSE )
      {
        // 今のSEを停止
        if( nowSetting.loopSE != SMID_NULL )
        {
          Sound::StopSE( nowSetting.loopSE );
        }

        // 新しいSEを再生
        if( newSetting.loopSE != SMID_NULL )
        {
          Sound::PlaySE( newSetting.loopSE );
        }
      }

      // 「!」SE再生
      if( newSetting.noticeSE != SMID_NULL )
      {
        Sound::PlaySE( newSetting.noticeSE );
      }

      // エフェクトの設定
      SetUpEffect( pPlayer, nowRank, newRank, m_EffectOffset );

      switch ( newRank )
      {
      case Dowsing::SearchRank::RANK_NO_SEARCH:
        {
          pMuurandoWork->playerWork.isDiscover = false;
          break;
        }

      case Dowsing::SearchRank::RANK_LOST:
        {
          // ロストの瞬間
          if( nowRank != Dowsing::SearchRank::RANK_NO_SEARCH )
          {
            // ロスト効果音
            Sound::PlaySE( SEARCH_LOST_SE );
          }

          pMuurandoWork->playerWork.isDiscover = false;
          break;
        }

      default:
        {
          // 発見状態になった瞬間
          if( newRank == Dowsing::SearchRank::RANK_A )
          {
            pMuurandoWork->playerWork.isDiscover = true;
          }
          else
          {
            pMuurandoWork->playerWork.isDiscover = false;
          }
          break;
        }
      }
    }
    //
    // サーチ判定適用処理( 継続 )
    //
    else
    {
    }

    // Work更新
    pMuurandoWork->distanceRank = newRank;

  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  ポスト
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideMuurando::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
  {
    Work* pWork      = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
    u8 dashDisableFrame =  pWork->dashDisableFrame; // postで減算されるので記憶

    FieldMoveCodePlayer::PostProcess( pMoveCodeWork );

    // 泣き声
    // @fix GFNMcat[4152]イベント起動後の10フレーム内にBボタンを押すとムーランドがモーションせずに鳴く、対処。禁止フレーム中は泣き声を再生しない
    if( IsTrgInputDashButton() && dashDisableFrame == 0 )
    {
      // @fix GFNMCat[965] 泣き声SEを後勝ちにする
      if( Sound::IsSEPlaying( SEQ_SE_FLD_RIDE_MOOLAND_PV_SEARCHIN ) ){ Sound::StopSE( SEQ_SE_FLD_RIDE_MOOLAND_PV_SEARCHIN ); }
      Sound::PlaySE(SEQ_SE_FLD_RIDE_MOOLAND_PV_SEARCHIN);
    }

    Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pMuurando     = pPlayer->GetOnMoveModel();
    MuurandoWork*                           pMuurandoWork = (MuurandoWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(MuurandoWork) );

  };

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  再開
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideMuurando::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
  {
    // 動作コードが中断している間に向きが変わることがあるので対処
    Work* pWork      = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
    pWork->targetQua = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideMuurando::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
  {
    // エフェクトをClose
    MuurandoWork* pMuurandoWork = (MuurandoWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(MuurandoWork) );
    Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    
    Dowsing::SearchRank::Value nowRank( static_cast<Dowsing::SearchRank::Value>( pMuurandoWork->distanceRank ) );
    Dowsing::RANK_SETTING nowSetting;
    Dowsing::Tool::GetSetting( nowRank, &nowSetting );
    
    // エフェクトをClose
    switch( nowRank )
    {
    case Dowsing::SearchRank::RANK_A:
      {
        if(pPlayer->GetMuurandoExclamationEx())
        {
          pPlayer->GetMuurandoExclamationEx()->RequestCloseForKeepMode( true );
        }
        break;
      } // case Dowsing::SearchRank::RANK_A:

    case Dowsing::SearchRank::RANK_B:
      {
        if(pPlayer->GetMuurandoExclamationNormal())
        {
          pPlayer->GetMuurandoExclamationNormal()->RequestCloseForKeepMode( true );
        }
        break;
      }
    }

    // 今のSEを停止
    if( nowSetting.loopSE != SMID_NULL )
    {
      Sound::StopSE( nowSetting.loopSE );
    }


    // 初期化してはいけないものの値を覚えておく
    // s8 distanceRank = pMuurandoWork->distanceRank;
    // u8 searchStartWaitCount = pMuurandoWork->searchStartWaitCount;

    // 内部で動作コードの初期化が呼ばれる
    FieldMoveCodePlayer::Suspend( pMoveCodeWork );
    
    // pMuurandoWork->distanceRank = distanceRank;
    // MuurandoWork->searchStartWaitCount = searchStartWaitCount;

    if( !pMoveCodeWork->pModel->CheckStateBit( FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND ) )
    {
      // 子キャラ(ライドポケモン)も待機状態に変更
      Field::MoveModel::FieldMoveModelPlayer* pPlayer   = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
      Field::MoveModel::FieldMoveModel* pMuurando = pPlayer->GetOnMoveModel();
      if( pMuurando )
      {
        if( pMuurando->GetCharaDrawInstance()->GetAnimationId() != (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) )
        {
          pMuurando->GetCharaDrawInstance()->ChangeAnimation( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
        }
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
  u32 FieldMoveCodeRideMuurando::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
  {
    //
    // 基底実行
    //
    u32 newState = FieldMoveCodePlayer::MainProcessSubOverrideState(pMoveCodeWork,state,nowState,oldState);
    
    //
    // ムーランド例外処理
    // @note RUN系=サーチ WALK系=通常移動です。RUNの方が移動速度が遅いので注意。
    //
    b32 isDashButton = IsInputDashButton();
    Work* pWork      = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

    // @fix GFNMcat[4152]イベント起動後の10フレーム内にBボタンを押すとムーランドがモーションせずに鳴く、関連対処。念のためダッシュ判定を厳密にする
    if( pWork->dashDisableFrame > 0 )
    {
      isDashButton = false;
    }

    MuurandoWork* pMuurandoWork = (MuurandoWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(MuurandoWork) );

    // 発見状態～サーチ/通常の間のレスポンスをよくするため、遷移をカットする
    if( pMuurandoWork->playerWork.isDiscover )
    {
      // 「歩き始め」「走り始め」状態をカットして「発見歩き」に移行
      if( newState == PLAYER_MOVE_RUN_START || newState == PLAYER_MOVE_WALK_START )
      {
        newState = PLAYER_MOVE_RUN_LOOP_DISCOVER;
      }

      // 「歩き終わり」「走り終わり」状態をカットして「発見待機」に移行
      if( newState == PLAYER_MOVE_RUN_END || newState == PLAYER_MOVE_WALK_END)
      {
        newState = PLAYER_MOVE_WAIT_DISCOVER;
      }
    }

    // サーチ～通常の間のレスポンスをよくするため、遷移をカットする
    if( isDashButton )
    {
      // 「歩き終わり」状態をカットして「走り待機」に移行
      if( newState == PLAYER_MOVE_WALK_END )
      {
        newState = PLAYER_MOVE_WAIT_TOKUSHU;
      }
    }
    else
    {
      // 「走り終わり」状態をカットして「待機」に移行
      if( newState == PLAYER_MOVE_RUN_END )
      {
        newState = PLAYER_MOVE_WAIT;
      }
    }

    return newState;
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void FieldMoveCodeRideMuurando::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
  {
    FieldMoveCodePlayer::MainProcessSubChangeState(pMoveCodeWork,state,nowState,oldState);

    // 子キャラ(ライドポケモン)もモーションを変更
    Field::MoveModel::FieldMoveModelPlayer* pPlayer       = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pMuurando     = pPlayer->GetOnMoveModel();
    MuurandoWork*                           pMuurandoWork = (MuurandoWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(MuurandoWork) );

    if( pMuurando )
    {
      pMuurando->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
      if( !pMuurandoWork->playerWork.isContinueMotion )
      {
        pMuurando->GetCharaDrawInstance()->ChangeAnimation(m_PlayerMoveSettingTable[state].motionId);
      }
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ムーランドのサーチ状態かどうか
  *
  *  @return trueならその状態、falseならその状態ではない
  */
  //-----------------------------------------------------------------------------
  b32 FieldMoveCodeRideMuurando::IsMuurandSearchState(const FieldMoveCodeWork* pMoveCodeWork) const
  {
    const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
    if(
      pWork->nowState == PLAYER_MOVE_RUN_START
      || pWork->nowState == PLAYER_MOVE_WAIT_TOKUSHU
      || pWork->nowState == PLAYER_MOVE_WAIT_DISCOVER
      || pWork->nowState == PLAYER_MOVE_RUN_LOOP
      || pWork->nowState == PLAYER_MOVE_RUN_LOOP_SLOPE
      || pWork->nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER
      || pWork->nowState == PLAYER_MOVE_RUN_END
      // ターン中は探索範囲の変動量が多いので、サーチしない
      // || pWork->nowState == PLAYER_MOVE_RUN_TURN
      )
    {
      return true;
    }
    return false;
  }
  b32 FieldMoveCodeRideMuurando::IsMuurandSearchStateEx(const FieldMoveCodeWork* pMoveCodeWork) const
  {
    const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
    MuurandoWork*                           pMuurandoWork = (MuurandoWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(MuurandoWork) );
    if(
      pWork->nowState == PLAYER_MOVE_RUN_START
      || pWork->nowState == PLAYER_MOVE_WAIT_TOKUSHU
      || pWork->nowState == PLAYER_MOVE_WAIT_DISCOVER
      || pWork->nowState == PLAYER_MOVE_RUN_LOOP
      || pWork->nowState == PLAYER_MOVE_RUN_LOOP_SLOPE
      || pWork->nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER
      || pWork->nowState == PLAYER_MOVE_RUN_END
      // ターン中は探索範囲の変動量が多いので、サーチしない
      // || pWork->nowState == PLAYER_MOVE_RUN_TURN
      )
    {
      if (pMuurandoWork->searchStartWaitCount >= m_SearchStartWait)
      {
        return true;
      }
    }
    return false;
  }
}; //end of namespace MoveModel
}; //end of namespace Field