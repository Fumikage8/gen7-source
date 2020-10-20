//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.cpp
*  @brief  フィールド用 動作コード プレイヤー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
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
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodePlayer.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
// Sound
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
// Camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/FieldCameraUiDeviceTranslator.h"
// debug
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
// zoneData
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
// record
#include "Savedata/include/Record.h"
// Attr
#include "Field/FieldStatic/include/FieldAttribute.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // コントローラーの入力を取得
  GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  f32                       stickX         = game_manager->GetFieldmap()->GetCameraUiDeviceTranslator()->GetX();
  f32                       stickY         = game_manager->GetFieldmap()->GetCameraUiDeviceTranslator()->GetY();
  f32                       stickLength    = sqrt( stickX*stickX + stickY*stickY );

  // キャラコンバートリストから自身をセットアップ
  // note: InitProcess関数に記述できる処理ではありますが、そうするとSetupWork関数にバイナリリソースのロードが事前に必要なってしまうので書いていません
  if( !pWork->isSetupFromAppData )
  {
    // 状態にあったキャラクターIDを取得
    Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    u32 charaId = pPlayer->GetCharacterId();
    if( pPlayer->GetOnMoveModel() )
    {
      charaId = pPlayer->GetOnMoveModel()->GetCharacterId();
    }

    // リソースから自身をセットアップするために特別にconst_castを使っています
    ApplicationDataForPlayer* pPlayerData     = (Field::MoveModel::ApplicationDataForPlayer*)GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( charaId );
    FieldMoveCodePlayer*      pMoveCodePlayer = const_cast<FieldMoveCodePlayer*>(this);
    pMoveCodePlayer->SetInterpStickDefault( pPlayerData->interpStickDefault );
    pMoveCodePlayer->SetInterpStickRun( pPlayerData->interpStickRun );
    pMoveCodePlayer->SetTurnSpeed( pPlayerData->runTurnSpeed );
    pMoveCodePlayer->SetTurnDeg( pPlayerData->runTurnDegree );
    pMoveCodePlayer->SetNeutralRange( pPlayerData->neutralRange );
    pMoveCodePlayer->SetPiyoriMaxValue( pPlayerData->piyoriMaxValue );
    pMoveCodePlayer->SetPiyoriAngle( pPlayerData->piyoriAngle );
    // 終了
    pWork->isSetupFromAppData = true;
  }

#if PM_DEBUG
  // デバッグメニューから
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu )
  {
    FieldMoveCodePlayer* pMoveCodePlayer = const_cast<FieldMoveCodePlayer*>(this);
    if( pFieldDebugMenu->isUsePiyoriMaxValue() )
    {
      pMoveCodePlayer->SetPiyoriMaxValue( pFieldDebugMenu->GetPiyoriMaxValue() );
      pMoveCodePlayer->SetPiyoriAngle   ( pFieldDebugMenu->GetPiyoriAngle() );
    }
    if( pFieldDebugMenu->isUseRunTurnSpeed() )
    {
      pMoveCodePlayer->SetTurnSpeed( (f32)pFieldDebugMenu->GetRunTurnSpeed() );
    }
  }
#endif

  // スティックの倒した方向から目標姿勢を更新
  f32 fNeutralRange = (f32)m_neutralRange / 100.0f;
  if( stickLength > fNeutralRange )
  {
    // 前回の目標姿勢の更新
    pWork->inputDirPrev = pWork->inputDir;

    // スティックの倒した方向を取得
    gfl2::math::Vector raw_move       ( stickX, 0.0f, stickY, 0.0f );
    gfl2::math::Vector normal_raw_move( raw_move.Normalize3() );
    pWork->inputDir = gfl2::math::NormalizeRadianAngle( atan2( normal_raw_move.z, normal_raw_move.x ) + 0.5f * PI );

    // 目標姿勢の更新
    gfl2::math::Quaternion qua; 
    qua.RadianYToQuaternion(pWork->inputDir);
    pWork->targetQua = qua;
  }
  // 入力がなければロック解除
  else
  {
    stickX      = 0.0f;
    stickY      = 0.0f;
    stickLength = 0.0f;
    UnLockDir(pMoveCodeWork);
  }

  // 現姿勢の向きと目標姿勢の向きの内積を取る 
  {
    // 現姿勢の向きベクトルを取得
    gfl2::math::Matrix  nowRotMat;
    gfl2::math::Vector4 nowRotVec(0.0f,0.0,1.0f);
    pCharaModelInstance->GetRotationQuat().QuaternionToMatrix( nowRotMat );  
    nowRotVec   = nowRotMat.TransformNormal(nowRotVec);
    nowRotVec.y = 0.0f;
    nowRotVec   = nowRotVec.Normalize();

    // 目標姿勢の向きベクトルを取得
    gfl2::math::Matrix  targetRotMat;
    gfl2::math::Vector4 targetRotVec(0.0f,0.0f,1.0f);
    pWork->targetQua.QuaternionToMatrix( targetRotMat );  
    targetRotVec   = targetRotMat.TransformNormal(targetRotVec);
    targetRotVec.y = 0.0f;
    targetRotVec   = targetRotVec.Normalize();

    // 現姿勢の向きと目標姿勢の向きの内積を取る
    f32 dot = gfl2::math::Clamp( nowRotVec.Dot(targetRotVec), -1.0f , 1.0f );
    pWork->angleToTarget = gfl2::math::ConvRadToDeg( acosf( dot ) );

    // ターゲットが右側かどうか調べる
    gfl2::math::Vector4 nowRotVecRight(1.0f,0.0,0.0f);
    nowRotVecRight   = nowRotMat.TransformNormal(nowRotVecRight);
    nowRotVecRight.y = 0.0f;
    nowRotVecRight   = nowRotVecRight.Normalize();
    f32 dotRight = gfl2::math::Clamp( nowRotVecRight.Dot(targetRotVec), -1.0f , 1.0f );
    if( dotRight <= 0.0f )
    {
      pWork->isTargetRightSide = true;
    }
    else
    {
      pWork->isTargetRightSide = false;
    }
  }

  // 道が傾いているかを調べる
  pWork->isMoveSlope = false;
  gfl2::math::Vector3 groundNormalXZ(pMoveCodeWork->pModel->GetGroundNormal().x,0.0f,pMoveCodeWork->pModel->GetGroundNormal().z);
  if( ( IsWalkLoopState(pMoveCodeWork) || IsRunLoopState(pMoveCodeWork) ) && groundNormalXZ.Length() > FLT_EPSILON )
  {
    // 坂を上っているか調べる
    gfl2::math::Vector3 nowRotVec(0.0f,0.0,1.0f);
    nowRotVec   = pCharaModelInstance->GetRotationQuat().Transform( nowRotVec );
    nowRotVec.y = 0.0f;
    nowRotVec   = nowRotVec.Normalize();
    if( groundNormalXZ.Dot( nowRotVec ) < 0.0f && gfl2::math::Vector3( 0.0f,1.0f,0.0f ).Dot(pMoveCodeWork->pModel->GetGroundNormal() ) < 0.99f )
    {
      pWork->isMoveSlope = true;
    }
  }

  // 坂道を一定期間移動していなければ坂道移動状態にはしない
  if( pWork->isMoveSlope ){ pWork->moveSlopeCnt++; }
  else                    { pWork->moveSlopeCnt=0; }
  u32 SLOPE_START_FRAME = 4;
#if PM_DEBUG
  if( pFieldDebugMenu )
  {
    SLOPE_START_FRAME = pFieldDebugMenu->GetWaitSlopeFrame();
  }
#endif
  if( pWork->moveSlopeCnt < SLOPE_START_FRAME )
  {
    pWork->isMoveSlope = false;
  }

  // 自身がたっているところが草むらかどうか
  u32 groundAttr = pMoveCodeWork->pModel->GetGroundAttribute();
  b32 isKusamura = Field::Attribute::IsGrass( groundAttr ) ? true:false;

  // 歩きループ・走りループ時以外なら草むらにいないと見做す
  if( !IsWalkLoopState(pMoveCodeWork) && !IsRunLoopState(pMoveCodeWork) )
  {
    isKusamura = false;
  }

  // 草むらにいる期間を調べる
  if( isKusamura ){ pWork->moveKusamuraCnt++; }
  else            { pWork->moveKusamuraCnt=0; }

  // 草むらに一定期間いなければ草むら移動状態にはしない
  u32 KUSAMURA_START_FRAME = 2;
  if( pWork->moveKusamuraCnt < KUSAMURA_START_FRAME ){ pWork->isMoveKusamura = false; }
  else                                               { pWork->isMoveKusamura = true ; pWork->moveKusamuraCnt = KUSAMURA_START_FRAME; }

  // スティックの倒した量をみてステートを選定
  u32 newState = GetStateFromStickLevel( stickLength );

  // ステートを状況をみて上書き
  newState = MainProcessSubOverrideState( pMoveCodeWork , newState , pWork->nowState , pWork->oldState );

  // ステートの変更
  if ( pWork->nowState != newState )
  {
    // ステートの変更処理
    MainProcessSubChangeState(pMoveCodeWork,newState,pWork->nowState,pWork->oldState);
  }

  // 待機しているフレームをカウントする
  if( pWork->nowState == PLAYER_MOVE_WAIT && !pMoveCodeWork->pModel->isEnableInterest() && pWork->idleFrame >= pWork->waitFrame )
  {
    pWork->waitFrame++;
  }

  // 旋回時・ぴより時は動作モデルの向きを固定する
  if( pWork->nowState == PLAYER_MOVE_RUN_TURN
   || pWork->nowState == PLAYER_MOVE_RUN_END
   || pWork->nowState == PLAYER_MOVE_PIYORI_RIGHT_START
   || pWork->nowState == PLAYER_MOVE_PIYORI_RIGHT_LOOP
   || pWork->nowState == PLAYER_MOVE_PIYORI_RIGHT_END
   || pWork->nowState == PLAYER_MOVE_PIYORI_LEFT_START
   || pWork->nowState == PLAYER_MOVE_PIYORI_LEFT_LOOP
   || pWork->nowState == PLAYER_MOVE_PIYORI_LEFT_END
   || pWork->nowState == PLAYER_MOVE_TSUMADUKI )
  {
    // 走り終了時に移動開始の姿勢補完が終わっていなければ中断しない
    if( pWork->nowState == PLAYER_MOVE_RUN_END && pWork->moveStartTurnFrame > 0 )
    {
    }
    else
    {
      pWork->targetQua = pCharaModelInstance->GetRotationQuat();
    }
  }

  // 旋回による移動禁止時間の更新
  if( pWork->moveStartTurnFrame > 0 )
  {
    pWork->moveStartTurnFrame--;
  }
  else
  {
    pWork->moveStartTurnFrame = 0;
  }

  // 目標角度に向いていたなら移動開始をキャンセルする
  const f32 MOVE_START_THRESHOLD = 1.0f;
  if( pWork->angleToTarget <= MOVE_START_THRESHOLD )
  {
    pWork->moveStartTurnFrame = 0;
  }

  // 坂道を上がっている場合は遅くなる
  f32 slopeDampingRate(1.0f);
  if( pWork->isMoveSlope )
  {
    // 坂道での移動減衰率を設定
    f32 dotNormalUnitY( gfl2::math::Clamp( gfl2::math::Vector3( 0.0f,1.0f,0.0f ).Dot(pMoveCodeWork->pModel->GetGroundNormal()) , 0.95f , 1.0f ) );
    f32 slopeRate     ( gfl2::math::Clamp( (1.0f - dotNormalUnitY) , 0.0f , 1.0f ) );
    slopeDampingRate = 1.0f - (slopeRate * m_slopeDampingRate);
  #if PM_DEBUG
    // デバッグメニューから
    if( pFieldDebugMenu && pFieldDebugMenu->isUseSlopeDampingRate() )
    {
      slopeDampingRate = 1.0f - (slopeRate * pFieldDebugMenu->GetSlopeDampingRate()); 
    }
  #endif
  }

  // 移動開始時のOriginベクトルの無効フレーム
  f32 speedRate = slopeDampingRate;
  if( pWork->moveStartTurnFrame > 0 )
  {
    speedRate = 0.0f;
  }

  f32 debugRate = 1.0f;
#if PM_DEBUG
  {
    // 移動速度調整
    Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
    if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_PLAYER         )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetPlayerSpeedRateForRun();     }
      else                      { debugRate = pFieldDebugMenu->GetPlayerSpeedRateForWalk();    }
    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetKentarosSpeedRateForRun();   }
      else                      { debugRate = pFieldDebugMenu->GetKentarosSpeedRateForWalk();  }
    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetSamehaderSpeedRateForRun();  }
      else                      { debugRate = pFieldDebugMenu->GetSamehaderSpeedRateForWalk(); }
    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetMuurandSpeedRateForRun();    }
      else                      { debugRate = pFieldDebugMenu->GetMuurandSpeedRateForWalk();   }
    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetRoba2SpeedRateForRun();      }
      else                      { debugRate = pFieldDebugMenu->GetRoba2SpeedRateForWalk();     }
    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetKairikySpeedRateForRun();    }
      else                      { debugRate = pFieldDebugMenu->GetKairikySpeedRateForWalk();   }
    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   )
    {
      if( IsInputDashButton() ) { debugRate = pFieldDebugMenu->GetLaplaceSpeedRateForRun();    }
      else                      { debugRate = pFieldDebugMenu->GetLaplaceSpeedRateForWalk();   }
    }
  }
#endif

  // 急停止時の移動速度調整
  if( pWork->oldState == PLAYER_MOVE_RUN_START && pWork->nowState == PLAYER_MOVE_RUN_END )
  {
#if PM_DEBUG
    if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  ) { speedRate *= (f32)pFieldDebugMenu->GetKentarosSuddenStopRate();  }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER ) { speedRate *= (f32)pFieldDebugMenu->GetSamehaderSuddenStopRate(); }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   ) { speedRate *= (f32)pFieldDebugMenu->GetMuurandSuddenStopRate();   }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     ) { speedRate *= (f32)pFieldDebugMenu->GetRoba2SuddenStopRate();     }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   ) { speedRate *= (f32)pFieldDebugMenu->GetKairikySuddenStopRate();   }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   ) { speedRate *= (f32)pFieldDebugMenu->GetLaplaceSuddenStopRate();   }
#else
    if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  ) { speedRate *= 0.5f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER ) { speedRate *= 1.0f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   ) { speedRate *= 0.5f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     ) { speedRate *= 0.5f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   ) { speedRate *= 0.8f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   ) { speedRate *= 1.0f; }
#endif
  }

  // Originジョイントによる移動ベクトルを変換するスケール/クォータニオンを設定する
  pMoveCodeWork->pModel->SetCustomValueForOrigin( m_PlayerMoveSettingTable[newState].moveSpeed * speedRate * debugRate , pCharaModelInstance->GetRotationQuat() );

  // 現在の姿勢から目標姿勢への補完レート
  f32 turnRate = (f32)m_interpStickDefault/100.0f;

  // 走り時の現在の姿勢から目標姿勢への補完レート
#if PM_DEBUG
  b32 isUseTurnRunRate = false;
#endif
  if( pWork->nowState == PLAYER_MOVE_RUN_START 
   || pWork->nowState == PLAYER_MOVE_RUN_END
   || IsRunLoopState( pMoveCodeWork ) )
  {
    turnRate =(f32)m_interpStickRun/100.0f;
#if PM_DEBUG
    isUseTurnRunRate = true;
#endif
  }

#if PM_DEBUG
  {
    Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
    if( pFieldDebugMenu->IsUsePlayerTurnRate() )
    {
      if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_PLAYER         )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetPlayerTurnRateForRun()        / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetPlayerTurnRateForDefault()    / 100.0f; }
      }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetKentarosTurnRateForRun()      / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetKentarosTurnRateForDefault()  / 100.0f; }
      }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetSamehaderTurnRateForRun()     / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetSamehaderTurnRateForDefault() / 100.0f; }
      }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetMuurandTurnRateForRun()       / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetMuurandTurnRateForDefault()   / 100.0f; }
      }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetRoba2TurnRateForRun()         / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetRoba2TurnRateForDefault()     / 100.0f; }
      }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetKairikyTurnRateForRun()       / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetKairikyTurnRateForDefault()   / 100.0f; }
      }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   )
      {
        if( isUseTurnRunRate ) { turnRate = (f32)pFieldDebugMenu->GetLaplaceTurnRateForRun()       / 100.0f; }
        else                   { turnRate = (f32)pFieldDebugMenu->GetLaplaceTurnRateForDefault()   / 100.0f; }
      }
    }
  }
#endif

  // 移動開始時の現在の姿勢から目標姿勢への補完レート
  if( pWork->moveStartTurnFrame > 0 )
  {
#if PM_DEBUG
    Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
    if( pFieldDebugMenu )
    {
      if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_PLAYER         ) { turnRate = (f32)pFieldDebugMenu->GetPlayerStartTurnRate()    / 100.0f; }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  ) { turnRate = (f32)pFieldDebugMenu->GetKentarosStartTurnRate()  / 100.0f; }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER ) { turnRate = (f32)pFieldDebugMenu->GetSamehaderStartTurnRate() / 100.0f; }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   ) { turnRate = (f32)pFieldDebugMenu->GetMuurandStartTurnRate()   / 100.0f; }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     ) { turnRate = (f32)pFieldDebugMenu->GetRoba2StartTurnRate()     / 100.0f; }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   ) { turnRate = (f32)pFieldDebugMenu->GetKairikyStartTurnRate()   / 100.0f; }
      else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   ) { turnRate = (f32)pFieldDebugMenu->GetLaplaceStartTurnRate()   / 100.0f; }
    }
#else
    if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_PLAYER         ) { turnRate = 0.6f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  ) { turnRate = 0.6f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER ) { turnRate = 0.3f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   ) { turnRate = 0.6f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     ) { turnRate = 0.6f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   ) { turnRate = 0.6f; }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   ) { turnRate = 0.3f; }
#endif
  }

  // 現在の姿勢から目標姿勢への補完
  gfl2::math::Quaternion nowQua( pCharaModelInstance->GetRotationQuat() );
  nowQua.Slerp( pWork->targetQua , gfl2::math::Clamp(turnRate, 0.0f, 1.0f) );
  pCharaModelInstance->SetRotationQuat( nowQua );
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  ポスト
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // 壁に垂直にあたると止まる処理
  if( pWork->lockDirDisableFrame <= 0 && !IsLockDir( pMoveCodeWork ) && ( IsWalkLoopState( pMoveCodeWork ) || IsRunLoopState( pMoveCodeWork ) ) )
  {
    // コリジョンによるXZ平面の補正ベクトルを求める
    Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    gfl2::math::Vector3 adjustVec( pPlayer->GetCharaDrawInstance()->GetPosition() - pPlayer->GetPositionPrevUpdateEnd() );
    adjustVec.y = 0.0f;

    // 補正ベクトルが小さい場合はコリジョンによる押し返しを受けていないはずなのでスルー
    if( adjustVec.Length() >= 0.01f )
    {
      adjustVec = adjustVec.Normalize();

      // 進行方向ベクトルを取得
      gfl2::math::Vector3 unitZ(0.0f,0.0f,-1.0f);
      unitZ = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( unitZ );

      // 押し返し方向と進行方向が逆向きであれば壁に垂直にあたっている
      f32 THRESHOLD = 23.0f;
      f32 dot       = gfl2::math::Clamp( adjustVec.Dot( unitZ ), -1.0f , 1.0f );
      f32 angle     = gfl2::math::ConvRadToDeg( acosf( dot ) );

    #if PM_DEBUG
      // デバッグメニューから
      Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
      if( pFieldDebugMenu )
      {
        THRESHOLD = pFieldDebugMenu->GetWallStopAngle();
      }
    #endif

      if( angle < THRESHOLD )
      {
        LockDir( pMoveCodeWork,false );
      }
    }
  }

  if( pWork->lockDirDisableFrame > 0 )
  {
    pWork->lockDirDisableFrame--;
  }

  // @fix GFNMcat[4152]イベント起動後の10フレーム内にBボタンを押すとムーランドがモーションせずに鳴く、対処。Postで参照するため、減算をMainからPostに移動
  if( pWork->dashDisableFrame > 0 )
  {
    pWork->dashDisableFrame--;
  }

};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  再開
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
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
void FieldMoveCodePlayer::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 初期化してはいけないものの値を覚えておく
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  f32 lockDir            = pWork->lockDir;
  b32 isLockDir          = pWork->isLockDir;
  b32 isLockFromOutside  = pWork->isLockFromOutside;
  b32 isSetupFromAppData = pWork->isSetupFromAppData;

  // 共有ワークの初期化
  InitProcess( pMoveCodeWork );

  // 初期化してはいけないものを基に戻す
  pWork->lockDir            = lockDir;
  pWork->isLockDir          = isLockDir;
  pWork->isLockFromOutside  = isLockFromOutside;
  pWork->isSetupFromAppData = isSetupFromAppData;

  // 外部からロックされていなければ初期化
  if( !IsLockDirFromOutside(pMoveCodeWork) )
  {
    UnLockDir( pMoveCodeWork );
  }

  // デフォルト姿勢にする
  if( !pMoveCodeWork->pModel->CheckStateBit( FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND ) )
  {
    if( pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationId() != (s32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) )
    {
      pMoveCodeWork->pModel->GetCharaDrawInstance()->ChangeAnimation( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
    }
  }
};

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  復帰
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::Comeback(FieldMoveCodeWork* pMoveCodeWork,b32 isLoad) const
{
  if( isLoad )
  {
    Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
    pWork->isSetupFromAppData = false;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  アナログスティックの倒し具合からステートを取得
*
*  @param  stick  アナログスティックの倒し具合
*  @return ステート
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::GetStateFromStickLevel( f32 stick ) const
{
  for( s32 i=PLAYER_MOVE_MAX-1 ; i>=0 ; i-- )
  {
    if( m_PlayerMoveSettingTable[i].stickLevel >= 0.0f
     && m_PlayerMoveSettingTable[i].stickLevel <= stick )
    {
      return i;
    }
  }
  return PLAYER_MOVE_WAIT;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // ダッシュのボタンの入力状況を取得
  b32 isDashButton    = IsInputDashButton();
  b32 isTrgDashButton = IsTrgInputDashButton();

  // 共有ワークを取得
  Work* pWork    = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  s32   newState = state;

  // レバガチャ中かどうか
  b32 isRebaGatya = false;

  // ダッシュボタンの入力を無効
  if( pWork->dashDisableFrame > 0 )
  {
    isDashButton    = false;
    isTrgDashButton = false;
  }

  //=================================================================================
  // 切り替え(割り込み)
  //=================================================================================
  {
    //-------------------------------------------------------------------------------
    // ムーランドの発見状態解除 
    //-------------------------------------------------------------------------------
    if( !isDashButton )
    {
      pWork->isDiscover = false;
    }

    //-------------------------------------------------------------------------------
    // 壁に垂直にぶつかったとき
    //-------------------------------------------------------------------------------
    if( IsLockDir(pMoveCodeWork) )
    {
      // 入力方向と無視方向のベクトルを取得
      gfl2::math::Quaternion qua0;
      gfl2::math::Quaternion qua1;
      gfl2::math::Vector3    vec0(0.0f,0.0f,1.0f);
      gfl2::math::Vector3    vec1(0.0f,0.0f,1.0f);
      qua0.RadianYToQuaternion(pWork->inputDir);
      qua1.RadianYToQuaternion(pWork->lockDir );
      vec0 = qua0.Transform( vec0 );
      vec1 = qua1.Transform( vec1 );

      // 無視方向と違う入力がきたのでロック解除
      const f32 INVALID_RANGE = 0.95f;
      if     ( vec1.Dot(vec0) < INVALID_RANGE )
      {
        UnLockDir(pMoveCodeWork);
      }
      // 無視方向と同じ入力がされた場合は"待機"ステートに切り替え
      else if( vec1.Dot(vec0) >= INVALID_RANGE && newState != PLAYER_MOVE_WAIT )
      {
        pWork->angleToTarget = 0.0f;
        pWork->targetQua     = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
        newState             = PLAYER_MOVE_WAIT;
        //newState           = PLAYER_MOVE_WALLHIT;
      }
    }

    //------------------------------------------------------------------------------
    // ぴより
    //------------------------------------------------------------------------------

    // レバガチャをしているか
    if( newState != PLAYER_MOVE_WAIT )
    {
      // 現在の入力方向と1フレーム前の入力方向を取得
      gfl2::math::Quaternion qua0;
      gfl2::math::Quaternion qua1;
      gfl2::math::Vector3    vec0(0.0f,0.0f,1.0f);
      gfl2::math::Vector3    vec1(0.0f,0.0f,1.0f);
      qua0.RadianYToQuaternion(pWork->inputDir);
      qua1.RadianYToQuaternion(pWork->inputDirPrev);
      vec0 = qua0.Transform( vec0 );
      vec1 = qua1.Transform( vec1 );

      // 現姿勢の向きと目標姿勢の向きの内積を取る
      f32 dot   = gfl2::math::Clamp( vec0.Dot(vec1), -1.0f , 1.0f );
      f32 angle = gfl2::math::ConvRadToDeg( acosf( dot ) );
      isRebaGatya = (angle > m_piyoriAngle) ? true:false;
    }

    // ぴより値の更新
    if( IsWalkLoopState( pMoveCodeWork )
     && isRebaGatya 
     && m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_START].motionId > 0
     && m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_START ].motionId > 0 )
    {
      pWork->piyoriNowValue++;
    }
    else
    {
      pWork->piyoriNowValue = 0;
    }

    // ぴより値が一定量を超えると"ピヨリ"ステートに切り替え
    if( pWork->piyoriNowValue >= m_piyoriMaxValue )
    {
      // カメラの向きを取得
      gfl2::math::Vector3    cameraZ(0.0f,0.0f,1.0f);
      gfl2::math::Quaternion qua    (0.0f,0.0f,0.0f,1.0f);
      qua.RadianYToQuaternion( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetCameraManager()->GetMainViewCamera()->GetRotationY() );
      cameraZ   = qua.Transform(cameraZ);
      cameraZ.y = 0.0f;
      cameraZ   = cameraZ.Normalize();

      // 現姿勢の向きを取得
      gfl2::math::Vector3 nowRotVec(0.0f,0.0,1.0f);
      nowRotVec   = pCharaModelInstance->GetRotationQuat().Transform( nowRotVec );  
      nowRotVec.y = 0.0f;
      nowRotVec   = nowRotVec.Normalize();

      // カメラと向き合うなら
      const f32 THRESHOLD = -0.94f;
      if( nowRotVec.Dot( cameraZ ) < THRESHOLD )
      {
        if( pWork->isTargetRightSide )
        {
          newState = PLAYER_MOVE_PIYORI_RIGHT_START;
        }
        else
        {
          newState = PLAYER_MOVE_PIYORI_LEFT_START;
        }
        pWork->piyoriNowValue = 0;
      }
    }

    //------------------------------------------------------------------------------
    // 躓き
    //------------------------------------------------------------------------------

    // 連打を受け付けるタイミングでなければ連打数を初期化
    if( pWork->timeLeftForRepeat > 0 )
    {
      pWork->timeLeftForRepeat--;
    }
    else
    {
      pWork->timeLeftForRepeat   = 0;
      pWork->repeatDashButtonCnt = 0;
    }

    // 歩きループと走りループ以外のステートに一度でもいけば連打数を初期化
    if( !IsWalkLoopState( pMoveCodeWork ) && !IsRunLoopState( pMoveCodeWork ) )
    {
      pWork->timeLeftForRepeat   = 0;
      pWork->repeatDashButtonCnt = 0;
    }

    // 躓きと見做す連打上限数、連打と見做す間隔(フレーム)を取得
    u8 MAX_REPEAT_CNT          =  8;
    u8 MAX_REPEAT_ACCEPT_FRAME = 15;
#if PM_DEBUG
    Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
    if( pFieldDebugMenu )
    {
      MAX_REPEAT_CNT          = pFieldDebugMenu->GetTsumadukiCnt();
      MAX_REPEAT_ACCEPT_FRAME = pFieldDebugMenu->GetTsumadukiAcceptFrame();
    }
#endif

    // 歩きループと走りループ中のみは連打を受け付ける
    if( isTrgDashButton
     && m_PlayerMoveSettingTable[PLAYER_MOVE_TSUMADUKI].motionId > 0
     && ( IsWalkLoopState( pMoveCodeWork ) || IsRunLoopState( pMoveCodeWork ) ) )
    {
      pWork->timeLeftForRepeat = MAX_REPEAT_ACCEPT_FRAME;
      pWork->repeatDashButtonCnt++;
    }

    // 連打回数が上限を過ぎると躓きステートに
    if( pWork->repeatDashButtonCnt > MAX_REPEAT_CNT )
    {
      // @fix GFNMCat[385] 目の前に壁があるときは躓かないようにする
      gfl2::math::Quaternion nowQua( pCharaModelInstance->GetRotationQuat() );
      gfl2::math::Vector4    front4( nowQua.Transform( gfl2::math::Vector4(0.0f,0.0f,90.0f,0.0f) ) ); // 90cmはつまずきモーションを考慮した範囲
      gfl2::math::Vector3    front3( front4.x,front4.y,front4.z );
      if( pMoveCodeWork->pModel->GetHitCntFromOshikaeshiScene( pCharaModelInstance->GetPosition(),
                                                               pCharaModelInstance->GetPosition() + front3,
                                                               0,
                                                               false,
                                                               false,
                                                               false ) )
      {
        newState = PLAYER_MOVE_TSUMADUKI;
      }
      else
      {
        pWork->timeLeftForRepeat   = 0;
        pWork->repeatDashButtonCnt = 0;
      }
    }

    //---------------------------------------------------------------------------------
    // 歩き
    //---------------------------------------------------------------------------------

    // 前回は歩いており、現在は"待機状態"のときは"歩き終了"ステートに切り替え
    if( newState == PLAYER_MOVE_WAIT && ( nowState == PLAYER_MOVE_WALK_START || IsWalkLoopState( pMoveCodeWork ) ) )
    {
      if( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_END].motionId > 0 )
      {
        newState = PLAYER_MOVE_WALK_END;
      }
    }

    // 歩きループ(坂道)にきりかえ
    if( newState == PLAYER_MOVE_WALK_LOOP && pWork->isMoveSlope )
    {
      newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_LOOP_SLOPE].motionId > 0 ) ? PLAYER_MOVE_WALK_LOOP_SLOPE : PLAYER_MOVE_WALK_LOOP;
    }

    // 歩きループ(草むら)にきりかえ
    // note 歩きループ(坂道)が優先
    if( newState == PLAYER_MOVE_WALK_LOOP && pWork->isMoveKusamura )
    {
      newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_LOOP_KUSAMURA].motionId > 0 ) ? PLAYER_MOVE_WALK_LOOP_KUSAMURA : PLAYER_MOVE_WALK_LOOP;
    }

    //---------------------------------------------------------------------------------
    // 走り
    //---------------------------------------------------------------------------------

    // "歩き開始or歩きループ"中にダッシュボタンを押したときは"走り"ステートに切り替え
    if( isDashButton && (newState == PLAYER_MOVE_WALK_START || newState == PLAYER_MOVE_WALK_LOOP || newState == PLAYER_MOVE_WALK_LOOP_SLOPE || newState == PLAYER_MOVE_WALK_LOOP_KUSAMURA) )
    {
      newState = PLAYER_MOVE_RUN_START;
      if( m_PlayerMoveSettingTable[newState].motionId < 0 )
      {
        newState = PLAYER_MOVE_RUN_LOOP;
      }
    }

    // 前回は走っており、現在は"待機状態"のときは"走り終了"ステートに切り替え
    if( newState == PLAYER_MOVE_WAIT && ( nowState == PLAYER_MOVE_RUN_START || IsRunLoopState( pMoveCodeWork ) || nowState == PLAYER_MOVE_RUN_TURN ) )
    {
      if( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_END].motionId > 0 )
      {
        newState = PLAYER_MOVE_RUN_END;
      }
    }

    // 走りループ(坂道)に切り替え
    if( newState == PLAYER_MOVE_RUN_LOOP && pWork->isMoveSlope )
    {
      newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_SLOPE].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_SLOPE : PLAYER_MOVE_RUN_LOOP;
    }

    // 走りループ(草むら)にきりかえ
    // note 走りループ(坂道)が優先
    if( newState == PLAYER_MOVE_RUN_LOOP && pWork->isMoveKusamura )
    {
      newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_KUSAMURA].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_KUSAMURA : PLAYER_MOVE_RUN_LOOP;
    }

    // ムーランド専用処理:走りループ(発見状態)に切り替え
    if( ( newState == PLAYER_MOVE_RUN_LOOP || newState == PLAYER_MOVE_RUN_LOOP_SLOPE || newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA ) && pWork->isDiscover )
    {
      newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_DISCOVER].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_DISCOVER : newState;
    }


    //---------------------------------------------------------------------------------
    // その他
    //---------------------------------------------------------------------------------

    // 旋回
    if( pWork->angleToTarget > (f32)m_runTurnDeg )
    {
      if( (nowState != PLAYER_MOVE_RUN_TURN)
       && (newState == PLAYER_MOVE_RUN_LOOP || newState == PLAYER_MOVE_RUN_LOOP_SLOPE || newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA || newState == PLAYER_MOVE_RUN_LOOP_DISCOVER || newState == PLAYER_MOVE_RUN_END )
       && m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_TURN].motionId != -1 )
      {
        // 待機からの直接の旋回でなければ"走り旋回"ステートに切り替え
        if( nowState != PLAYER_MOVE_WAIT && oldState != PLAYER_MOVE_WAIT )
        {
          newState = PLAYER_MOVE_RUN_TURN;
        }
      }
    }
  }

  //=================================================================================
  // 遷移
  //=================================================================================
  {
    // モーションの再生が終わった
    if( pCharaModelInstance->IsAnimationLastFrame() )
    {
      // 壁あたりが終わると"待機"ステートに遷移
      if     ( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_WALLHIT].motionId    && nowState == PLAYER_MOVE_WALLHIT )
      {
        newState = PLAYER_MOVE_WAIT;
      }
      // 歩き始めが終わり坂道を登っていないと"歩きループ"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_START].motionId && nowState == PLAYER_MOVE_WALK_START && !pWork->isMoveSlope )
      {
        newState = PLAYER_MOVE_WALK_LOOP;
      }
      // 歩き始めが終わり坂道を上っていると"歩きループ(坂道)"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_START].motionId && nowState == PLAYER_MOVE_WALK_START &&  pWork->isMoveSlope )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_LOOP_SLOPE].motionId > 0 ) ? PLAYER_MOVE_WALK_LOOP_SLOPE : PLAYER_MOVE_WALK_LOOP;
      }
      // 歩き始めが終わり草むらを歩いていると"歩きループ(草むら)"ステートに遷移
      // note 歩きループ(坂道)が優先
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_START].motionId && nowState == PLAYER_MOVE_WALK_START && pWork->isMoveKusamura )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_LOOP_KUSAMURA].motionId > 0 ) ? PLAYER_MOVE_WALK_LOOP_KUSAMURA : PLAYER_MOVE_WALK_LOOP;
      }
      // 走り始めが終わり発見状態なら"走りループ(発見状態)"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_START].motionId  && nowState == PLAYER_MOVE_RUN_START && pWork->isDiscover && m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_DISCOVER].motionId > 0 )
      {
        newState = PLAYER_MOVE_RUN_LOOP_DISCOVER;
      }
      // 走り始めが終わり坂道を登っていないと"走りループ"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_START].motionId  && nowState == PLAYER_MOVE_RUN_START && !pWork->isMoveSlope )
      {
        newState = PLAYER_MOVE_RUN_LOOP;
      }
      // 走り始めが終わり坂道を登っていると"走りループ(坂道)"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_START].motionId  && nowState == PLAYER_MOVE_RUN_START &&  pWork->isMoveSlope )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_SLOPE].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_SLOPE : PLAYER_MOVE_RUN_LOOP;
      }
      // 走り始めが終わり草むらを走っていると"走りループ(草むら)"ステートに遷移
      // note 走りループ(坂道)が優先
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_START].motionId && nowState == PLAYER_MOVE_RUN_START && pWork->isMoveKusamura )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_KUSAMURA].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_KUSAMURA : PLAYER_MOVE_RUN_LOOP;
      }
      // ぴより始め(右)が終わると"ぴよりループ(右)"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_START].motionId  && nowState == PLAYER_MOVE_PIYORI_RIGHT_START )
      {
        newState = PLAYER_MOVE_PIYORI_RIGHT_LOOP;
        Savedata::IncRecord( Savedata::Record::RECID_GURUGURU );
      }
      // ぴより始め(左)が終わると"ぴよりループ(左)"ステートに遷移
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_START].motionId  && nowState == PLAYER_MOVE_PIYORI_LEFT_START )
      {
        newState = PLAYER_MOVE_PIYORI_LEFT_LOOP;
        Savedata::IncRecord( Savedata::Record::RECID_GURUGURU );
      }
      // ぴよりループ(左)ステートになるとレバガチャを続けているうちは同じステートに遷移する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_LOOP].motionId && nowState == PLAYER_MOVE_PIYORI_LEFT_LOOP   && isRebaGatya )
      {
        newState = PLAYER_MOVE_PIYORI_LEFT_LOOP;
        Savedata::IncRecord( Savedata::Record::RECID_GURUGURU );
      }
      // ぴよりループ(右)ステートになるとレバガチャを続けているうちは同じステートに遷移する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_LOOP].motionId && nowState == PLAYER_MOVE_PIYORI_RIGHT_LOOP && isRebaGatya )
      {
        newState = PLAYER_MOVE_PIYORI_RIGHT_LOOP;
        Savedata::IncRecord( Savedata::Record::RECID_GURUGURU );
      }
      // ぴよりループ(左)ステートになりレバガチャが終わると"ぴより終了(左)"ステートに遷移する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_LOOP].motionId && nowState == PLAYER_MOVE_PIYORI_LEFT_LOOP   && !isRebaGatya )
      {
        newState = PLAYER_MOVE_PIYORI_LEFT_END;
      }
      // ぴよりループ(右)ステートになりレバガチャが終わると"ぴより終了(右)"ステートに遷移する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_LOOP].motionId && nowState == PLAYER_MOVE_PIYORI_RIGHT_LOOP && !isRebaGatya )
      {
        newState = PLAYER_MOVE_PIYORI_RIGHT_END;
      }
      // アイドルステートが終わると"待機"ステートに遷移
      else if( (pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_IDLE_A   ].motionId && nowState == PLAYER_MOVE_IDLE_A)
            || (pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_IDLE_B   ].motionId && nowState == PLAYER_MOVE_IDLE_B)
            || (pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_IDLE_WARM].motionId && nowState == PLAYER_MOVE_IDLE_WARM)
            || (pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_IDLE_COOL].motionId && nowState == PLAYER_MOVE_IDLE_COOL) )
      {
        newState = PLAYER_MOVE_WAIT;
        pWork->waitFrame = 0;
      }
    }
  }

  //=================================================================================
  // 継続
  //=================================================================================
  {
    // モーションの再生が終わっていない
    if( !pCharaModelInstance->IsAnimationLastFrame() )
    {
      // "壁あたり"ステートになると専用モーションの最後まで継続する
      if     ( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_WALLHIT].motionId  && nowState == PLAYER_MOVE_WALLHIT  )
      {
        newState = PLAYER_MOVE_WALLHIT;
      }
      // "ぴより開始(右)"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_START].motionId && nowState == PLAYER_MOVE_PIYORI_RIGHT_START   )
      {
        newState = PLAYER_MOVE_PIYORI_RIGHT_START;
      }
      // "ぴより開始(左)"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_START].motionId && nowState == PLAYER_MOVE_PIYORI_LEFT_START   )
      {
        newState = PLAYER_MOVE_PIYORI_LEFT_START;
      }
      // "ぴよりループ(右)"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_LOOP].motionId && nowState == PLAYER_MOVE_PIYORI_RIGHT_LOOP   )
      {
        newState = PLAYER_MOVE_PIYORI_RIGHT_LOOP;
      }
      // "ぴよりループ(左)"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_LOOP].motionId && nowState == PLAYER_MOVE_PIYORI_LEFT_LOOP   )
      {
        newState = PLAYER_MOVE_PIYORI_LEFT_LOOP;
      }
      // "ぴより終了(右)"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_RIGHT_END].motionId && nowState == PLAYER_MOVE_PIYORI_RIGHT_END   )
      {
        newState = PLAYER_MOVE_PIYORI_RIGHT_END;
      }
      // "ぴより終了(左)"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_PIYORI_LEFT_END].motionId && nowState == PLAYER_MOVE_PIYORI_LEFT_END   )
      {
        newState = PLAYER_MOVE_PIYORI_LEFT_END;
      }
      // "走り旋回"ステートになると専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_TURN].motionId && nowState == PLAYER_MOVE_RUN_TURN )
      {
        newState = PLAYER_MOVE_RUN_TURN;
      }
      // "歩き終わり"ステートになると何も入力がなければ専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_END].motionId && nowState == PLAYER_MOVE_WALK_END && newState == PLAYER_MOVE_WAIT )
      {
        newState = PLAYER_MOVE_WALK_END;
      }
      // "走り終わり"ステートになると何も入力がなければ専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_END].motionId  && nowState == PLAYER_MOVE_RUN_END  && newState == PLAYER_MOVE_WAIT )
      {
        newState = PLAYER_MOVE_RUN_END;
      }
      // "躓き"ステートになると何も入力がなければ専用モーションの最後まで継続する
      else if( pCharaModelInstance->GetAnimationId() == m_PlayerMoveSettingTable[PLAYER_MOVE_TSUMADUKI].motionId  && nowState == PLAYER_MOVE_TSUMADUKI )
      {
        newState = PLAYER_MOVE_TSUMADUKI;
      }
    }

    // 歩き中or走り中
    if( newState == PLAYER_MOVE_WALK_START || newState == PLAYER_MOVE_RUN_START )
    {
      // "歩きループ"ステートになると入力があり続けるうちは継続する
      if     ( !isDashButton && IsWalkLoopState( pMoveCodeWork ) && !pWork->isMoveSlope )
      {
        newState = PLAYER_MOVE_WALK_LOOP;
      }
      // "歩きループ(坂道)"ステートになると入力があり続けるうちは継続する
      else if( !isDashButton && IsWalkLoopState( pMoveCodeWork ) && pWork->isMoveSlope )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_LOOP_SLOPE].motionId > 0 ) ? PLAYER_MOVE_WALK_LOOP_SLOPE : PLAYER_MOVE_WALK_LOOP;
      }
      // "歩きループ(草むら)"ステートになると入力があり続けるうちは継続する
      // note 歩きループ(坂道)が優先
      else if( !isDashButton && IsWalkLoopState( pMoveCodeWork ) && pWork->isMoveKusamura )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_WALK_LOOP_KUSAMURA].motionId > 0 ) ? PLAYER_MOVE_WALK_LOOP_KUSAMURA : PLAYER_MOVE_WALK_LOOP;
      }
      // "走りループ(発見状態)"ステートになると入力があり続けるうちは継続する
      else if(  isDashButton && IsRunLoopState( pMoveCodeWork ) && pWork->isDiscover && m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_DISCOVER].motionId > 0 )
      {
        newState = PLAYER_MOVE_RUN_LOOP_DISCOVER;
      }
      // "走りループ"ステートになると入力があり続けるうちは継続する
      else if(  isDashButton && IsRunLoopState( pMoveCodeWork ) && !pWork->isMoveSlope )
      {
        newState = PLAYER_MOVE_RUN_LOOP;
      }
      // "走りループ(坂道)"ステートになると入力があり続けるうちは継続する
      else if(  isDashButton && IsRunLoopState( pMoveCodeWork ) && pWork->isMoveSlope )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_SLOPE].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_SLOPE : PLAYER_MOVE_RUN_LOOP;
      }
      // "走りループ(草むら)"ステートになると入力があり続けるうちは継続する
      // note 走りループ(坂道)が優先
      else if(  isDashButton && IsRunLoopState( pMoveCodeWork ) && pWork->isMoveKusamura )
      {
        newState = ( m_PlayerMoveSettingTable[PLAYER_MOVE_RUN_LOOP_KUSAMURA].motionId > 0 ) ? PLAYER_MOVE_RUN_LOOP_KUSAMURA : PLAYER_MOVE_RUN_LOOP;
      }
    }
  }

  //=================================================================================
  // 例外(状況関係なく一番最後に行いたい)
  //=================================================================================
  EventWork * pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

  {
    // 待機状態でダッシュボタンを押していると特殊待機に切り替え
    if( (newState == PLAYER_MOVE_WAIT)
     && (isDashButton)
     && (m_PlayerMoveSettingTable[PLAYER_MOVE_WAIT_TOKUSHU].motionId > 0 ) )
    {
      newState = PLAYER_MOVE_WAIT_TOKUSHU;
    }

    // ムーランド処理:待機状態で発見状態なら発見待機に切り替え
    if( (newState == PLAYER_MOVE_WAIT || newState == PLAYER_MOVE_WAIT_TOKUSHU )
     && (pWork->isDiscover)
     && (m_PlayerMoveSettingTable[PLAYER_MOVE_WAIT_DISCOVER].motionId > 0 ) )
    {
      newState = PLAYER_MOVE_WAIT_DISCOVER;
    }

    // 一定期間待機していたらアイドリングに切り替え
    if( (newState == PLAYER_MOVE_WAIT)
     && (pWork->idleFrame < pWork->waitFrame)
     && (m_PlayerMoveSettingTable[pWork->idleState].motionId > 0 ) 
     && (!pEventWork->CheckEventFlag( SYS_FLAG_DRESSUP_BEFORE )) )  // @fix 大森さん要望：冒頭はバッグをもっていないので、アイドリングモーションに切り替えない。
    {
      newState = pWork->idleState;
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
void FieldMoveCodePlayer::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  poke_3d::model::CharaModel* pCharaModelInstance = pPlayer->GetCharaDrawInstance();

  // 歩きが中断したときの踏み込んでいる足を記憶する
  if( !pPlayer->IsRideMoveModel() && IsWalkLoopState( pMoveCodeWork ) )
  {
    pWork->nextFoot = ( pCharaModelInstance->GetAnimationEndFrame()/2.0f > pCharaModelInstance->GetAnimationFrame() ) ? 1 : 0;
  }

  // ステートにあったデフォルトモーションを取得
  u32 newState = state;
  s32 next_anime_id = m_PlayerMoveSettingTable[newState].motionId;

  // 待機アニメーションだけは外部から上書き可能
  if( newState == PLAYER_MOVE_WAIT )
  {
    next_anime_id = (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );
  }

  // 旋回終了時はモーション終了後の向きに合わせる
  if( nowState == PLAYER_MOVE_RUN_TURN )
  {
    gfl2::math::Quaternion addQua;
    addQua.RadianYXZToQuaternion( gfl2::math::Vector(0.0f,PI,0.0f,0.0f) );
    gfl2::math::Quaternion newQua = pCharaModelInstance->GetRotationQuat() * addQua;
    pCharaModelInstance->SetRotationQuat( newQua );

    pWork->targetQua     = newQua;
    pWork->nextFoot      = 0;
    pWork->angleToTarget = 0.0f;
  }

  u32 nowAnimationFrame = pCharaModelInstance->GetAnimationFrame();

  pWork->isContinueMotion = false;
  if( newState == PLAYER_MOVE_WAIT && pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationId() == next_anime_id )
  {
    pWork->isContinueMotion = true;
  }

  // モーションの変更
  if( !pWork->isContinueMotion )
  {
    // 急旋回モーションは逆向きで終わるためブレンドが狙い通りにできない、それを解決するために走りループモーションに瞬時に切り替えブレンドをする
    // note:急旋回モーションの終了ポーズは、走りループモーションの開始ポーズの逆向き
    if( nowState == PLAYER_MOVE_RUN_TURN )
    {
      pCharaModelInstance->ChangeAnimationSmooth(P2_BASE_FI012_RUN01,0);
    }

    pCharaModelInstance->ChangeAnimation(next_anime_id);
  }
  pCharaModelInstance->SetAnimationStepFrame( m_PlayerMoveSettingTable[newState].motionSpeed );

  // ステートの保存
  pWork->oldState = pWork->nowState;
  pWork->nowState = newState;

  b32 isContinueFrame = false;

  // 走りループ(通常/坂道/草むら/発見)⇔走りループ(通常/坂道/草むら/発見) or 歩きループ(通常/坂道/草むら)⇔歩きループ(通常/坂道/草むら) の場合は見た目が正しくなるように同じフレームから再生させる
  if( ( nowState == PLAYER_MOVE_RUN_LOOP           && newState == PLAYER_MOVE_RUN_LOOP_SLOPE     )
   || ( nowState == PLAYER_MOVE_RUN_LOOP           && newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  )
   || ( nowState == PLAYER_MOVE_RUN_LOOP           && newState == PLAYER_MOVE_RUN_LOOP_DISCOVER  )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_SLOPE     && newState == PLAYER_MOVE_RUN_LOOP           )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_SLOPE     && newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_SLOPE     && newState == PLAYER_MOVE_RUN_LOOP_DISCOVER  )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  && newState == PLAYER_MOVE_RUN_LOOP           )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  && newState == PLAYER_MOVE_RUN_LOOP_SLOPE     )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  && newState == PLAYER_MOVE_RUN_LOOP_DISCOVER  )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER  && newState == PLAYER_MOVE_RUN_LOOP           )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER  && newState == PLAYER_MOVE_RUN_LOOP_SLOPE     )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER  && newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  )
   || ( nowState == PLAYER_MOVE_WALK_LOOP          && newState == PLAYER_MOVE_WALK_LOOP_SLOPE    )
   || ( nowState == PLAYER_MOVE_WALK_LOOP          && newState == PLAYER_MOVE_WALK_LOOP_KUSAMURA )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_SLOPE    && newState == PLAYER_MOVE_WALK_LOOP          ) 
   || ( nowState == PLAYER_MOVE_WALK_LOOP_SLOPE    && newState == PLAYER_MOVE_WALK_LOOP_KUSAMURA )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA && newState == PLAYER_MOVE_WALK_LOOP          )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA && newState == PLAYER_MOVE_WALK_LOOP_SLOPE    ) )
  {
    isContinueFrame = true;
    pCharaModelInstance->SetAnimationFrame( nowAnimationFrame );
  }

  // 歩きループ(通常/坂道/草むら)⇔走りループ(通常/坂道/草むら) の場合は見た目が正しくなるように同じフレームから再生させる
  // この遷移があるのはプレイヤーのみで、走り開始があるライド状態では走らない
  if( ( nowState == PLAYER_MOVE_WALK_LOOP          && newState == PLAYER_MOVE_RUN_LOOP           )
   || ( nowState == PLAYER_MOVE_WALK_LOOP          && newState == PLAYER_MOVE_RUN_LOOP_SLOPE     )
   || ( nowState == PLAYER_MOVE_WALK_LOOP          && newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_SLOPE    && newState == PLAYER_MOVE_RUN_LOOP           )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_SLOPE    && newState == PLAYER_MOVE_RUN_LOOP_SLOPE     )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_SLOPE    && newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA && newState == PLAYER_MOVE_RUN_LOOP           )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA && newState == PLAYER_MOVE_RUN_LOOP_SLOPE     )
   || ( nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA && newState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  )
   || ( nowState == PLAYER_MOVE_RUN_LOOP           && newState == PLAYER_MOVE_WALK_LOOP          )
   || ( nowState == PLAYER_MOVE_RUN_LOOP           && newState == PLAYER_MOVE_WALK_LOOP_SLOPE    )
   || ( nowState == PLAYER_MOVE_RUN_LOOP           && newState == PLAYER_MOVE_WALK_LOOP_KUSAMURA )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_SLOPE     && newState == PLAYER_MOVE_WALK_LOOP          )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_SLOPE     && newState == PLAYER_MOVE_WALK_LOOP_SLOPE    )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_SLOPE     && newState == PLAYER_MOVE_WALK_LOOP_KUSAMURA )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  && newState == PLAYER_MOVE_WALK_LOOP          )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  && newState == PLAYER_MOVE_WALK_LOOP_SLOPE    )
   || ( nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA  && newState == PLAYER_MOVE_WALK_LOOP_KUSAMURA ) )
  {
    isContinueFrame = true;
    pCharaModelInstance->SetAnimationFrame( nowAnimationFrame );
  }

  // 歩き始め/走り始めが無いものは踏み出し足を都度かえる
  if( !isContinueFrame && !pPlayer->IsRideMoveModel() && IsWalkLoopState( pMoveCodeWork ) )
  {
    pCharaModelInstance->SetAnimationFrame( pCharaModelInstance->GetAnimationEndFrame()/2.0f * (f32)pWork->nextFoot );
  }

  // 旋回時の重みの更新
  u32 startTurnFrame = 0;

#if PM_DEBUG
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu )
  {
    if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_PLAYER         ) { startTurnFrame = (f32)pFieldDebugMenu->GetPlayerStartTurnFrame();    }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  ) { startTurnFrame = (f32)pFieldDebugMenu->GetKentarosStartTurnFrame();  }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER ) { startTurnFrame = (f32)pFieldDebugMenu->GetSamehaderStartTurnFrame(); }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   ) { startTurnFrame = (f32)pFieldDebugMenu->GetMuurandStartTurnFrame();   }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     ) { startTurnFrame = (f32)pFieldDebugMenu->GetRoba2StartTurnFrame();     }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   ) { startTurnFrame = (f32)pFieldDebugMenu->GetKairikyStartTurnFrame();   }
    else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   ) { startTurnFrame = (f32)pFieldDebugMenu->GetLaplaceStartTurnFrame();   }
  }
#else
  if     ( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_PLAYER         ) { startTurnFrame = 4;  }
  else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KENTAROS  ) { startTurnFrame = 4;  }
  else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_SAMEHADER ) { startTurnFrame = 10; }
  else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_MUURAND   ) { startTurnFrame = 4;  }
  else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_ROBA2     ) { startTurnFrame = 4;  }
  else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_KAIRIKY   ) { startTurnFrame = 4;  }
  else if( pMoveCodeWork->moveCodeId == FIELD_MOVE_CODE_RIDE_LAPLACE   ) { startTurnFrame = 10; }
#endif

  // 待機から移動でいきなり反転しないよう重みをつける
  if( nowState == PLAYER_MOVE_WAIT 
   || nowState == PLAYER_MOVE_WAIT_TOKUSHU
   || nowState == PLAYER_MOVE_IDLE_A
   || nowState == PLAYER_MOVE_IDLE_B
   || nowState == PLAYER_MOVE_IDLE_WARM
   || nowState == PLAYER_MOVE_IDLE_COOL
   || nowState == PLAYER_MOVE_WALK_END
   || nowState == PLAYER_MOVE_RUN_END )
  {
    if( IsWalkLoopState( pMoveCodeWork ) 
     || IsRunLoopState ( pMoveCodeWork ) 
     || newState == PLAYER_MOVE_WALK_START
     || newState == PLAYER_MOVE_RUN_START )
    {
      pWork->moveStartTurnFrame = startTurnFrame;
    }
  }

  // 待機orアイドル状態でない場合はカウントをオフに
  if( pWork->nowState != PLAYER_MOVE_IDLE_A
   && pWork->nowState != PLAYER_MOVE_IDLE_B
   && pWork->nowState != PLAYER_MOVE_IDLE_WARM
   && pWork->nowState != PLAYER_MOVE_IDLE_COOL )
  {
    pWork->waitFrame = 0;
    pWork->idleFrame = 300 + System::GflUse::GetPublicRand(90); // 10秒から13秒

    if     ( Field::ZoneDataAccessor::IsWarmZone( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetZoneID() ) )
    {
       pWork->idleState = PLAYER_MOVE_IDLE_WARM;
    }
    else if( Field::ZoneDataAccessor::IsCoolZone( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetZoneID() ) )
    {
      pWork->idleState = PLAYER_MOVE_IDLE_COOL;
    }
    else
    {
      switch(System::GflUse::GetPublicRand(2))
      {
      case 0: pWork->idleState = PLAYER_MOVE_IDLE_A; break;
      case 1: pWork->idleState = PLAYER_MOVE_IDLE_B; break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  最後に有効だった入力方向をロックする
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isCallFromOutside  外部(イベント)からのロックかどうか
*  @return trueなら成功、falseなら失敗
*  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::LockDir(FieldMoveCodeWork* pMoveCodeWork,b32 isCallFromOutside) const
{
  const f32 RANGE = 3.0f;

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->angleToTarget < RANGE )
  {
    pWork->lockDir           = pWork->inputDir;
    pWork->isLockDir         = true;
    pWork->isLockFromOutside = isCallFromOutside;

    // @fix GFNMCat[519] Pushイベントを踏んでいるときはSEがならないようにする
    u32 hitCnt = 0;
    if( pMoveCodeWork->pModel->GetDynamicActor( COLLISION_TYPE_EVENT_PUSH ) 
     && pMoveCodeWork->pModel->GetHeaderResource().pCollisionSceneForEventPush )
    {
      pMoveCodeWork->pModel->GetDynamicActor( COLLISION_TYPE_EVENT_PUSH )->InitCollisionResult();
      pMoveCodeWork->pModel->GetDynamicActor( COLLISION_TYPE_EVENT_PUSH )->SetPosition( pMoveCodeWork->pModel->GetLocalSRT().translate );
      pMoveCodeWork->pModel->GetHeaderResource().pCollisionSceneForEventPush->Update();
      hitCnt += pMoveCodeWork->pModel->GetDynamicActor( COLLISION_TYPE_EVENT_PUSH )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }

    if( !hitCnt )
    {
      Sound::PlaySE(SEQ_SE_FLD_WALL_HIT);
    }

    // ロック時は"走り終わり" or "歩き終わり"から待機になるのですが、モーションの移動によって意図せぬ壁ずりを防ぐ
    pMoveCodeWork->pModel->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  最後に有効だった入力方向のロックを解除
*
*  @param  pMoveCodeWork      動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::UnLockDir(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork              = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->lockDir           = 0.0f;
  pWork->isLockDir         = false;
  pWork->isLockFromOutside = false;

  // ロック時による移動禁止を終了
  pMoveCodeWork->pModel->OffStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
}

//-----------------------------------------------------------------------------
/**
*  @brief  最後に有効だった入力方向をロックしているかどうか
*
*  @param  pMoveCodeWork  動作コードワーク
*  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsLockDir(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return pWork->isLockDir;
}

//-----------------------------------------------------------------------------
/**
*  @brief  最後に有効だった入力方向を外部(イベント)からロックしているかどうか
*
*  @param  pMoveCodeWork  動作コードワーク
*  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsLockDirFromOutside(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return (pWork->isLockDir && pWork->isLockFromOutside) ? true : false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動いている状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsMoveState( const FieldMoveCodeWork* pMoveCodeWork) const
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->nowState == PLAYER_MOVE_WALK_LOOP 
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP
   || pWork->nowState == PLAYER_MOVE_WALK_LOOP_SLOPE
   || pWork->nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP_SLOPE
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER
   || pWork->nowState == PLAYER_MOVE_WALK_START
   || pWork->nowState == PLAYER_MOVE_RUN_START
   || pWork->nowState == PLAYER_MOVE_WALK_END
   || pWork->nowState == PLAYER_MOVE_RUN_END
   || pWork->nowState == PLAYER_MOVE_RUN_TURN )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  歩きループor歩きループ(坂道)or歩きループ(草むら)状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsWalkLoopState(const FieldMoveCodeWork* pMoveCodeWork) const
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->nowState == PLAYER_MOVE_WALK_LOOP 
   || pWork->nowState == PLAYER_MOVE_WALK_LOOP_SLOPE
   || pWork->nowState == PLAYER_MOVE_WALK_LOOP_KUSAMURA )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  走りループor走りループ(坂道)or走りループ(草むら)状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsRunLoopState(const FieldMoveCodeWork* pMoveCodeWork) const
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->nowState == PLAYER_MOVE_RUN_LOOP
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP_SLOPE
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP_KUSAMURA
   || pWork->nowState == PLAYER_MOVE_RUN_LOOP_DISCOVER )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  歩き終了状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsWalkEndState(const FieldMoveCodeWork* pMoveCodeWork) const
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->nowState == PLAYER_MOVE_WALK_END )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  走り終了状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsRunEndState(const FieldMoveCodeWork* pMoveCodeWork) const
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->nowState == PLAYER_MOVE_RUN_END )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効なスティック入力がされているかどうか
*
*  @return trueなら有効な入力あり、falseならなし
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsInputStick(void) const
{
  // コントローラーの入力を取得
  GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*   pVecDevice     = device_manager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  f32                       stickX         = pVecDevice->GetX();
  f32                       stickY         = pVecDevice->GetY();
  f32                       stickLength    = sqrt( stickX*stickX + stickY*stickY );

  // スティックの倒した方向から目標姿勢を更新
  f32 fNeutralRange = (f32)m_neutralRange / 100.0f;
  if( stickLength > fNeutralRange )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ダッシュボタン入力がされているかどうか
*
*  @return trueなら有効な入力あり、falseならなし
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsInputDashButton(void) const
{
  // ボタン入力状況を取得
  GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::Button*         pButton        = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); 

  // ダッシュのボタンの入力状況を取得
#if PM_DEBUG
  b32                       isDashButton   = pButton->IsHold( gfl2::ui::BUTTON_B ) || pButton->IsHold( gfl2::ui::BUTTON_L );
#else
  b32                       isDashButton   = pButton->IsHold( gfl2::ui::BUTTON_B );
#endif

  return isDashButton;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ダッシュボタン入力がされているかどうか(Trigger)
*
*  @return trueなら有効な入力あり、falseならなし
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsTrgInputDashButton(void) const
{
  // ボタン入力状況を取得
  GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::Button*         pButton        = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); 

  // ダッシュのボタンの入力状況を取得
#if PM_DEBUG
  b32                       isDashButton   = pButton->IsTrigger( gfl2::ui::BUTTON_B );
#else
  b32                       isDashButton   = pButton->IsTrigger( gfl2::ui::BUTTON_B );
#endif

  return isDashButton;
}

//-----------------------------------------------------------------------------
/**
*  @brief  他からの入力を受け付ける状態かどうか
*
*  @return trueなら受け付ける、falseなら受け付けない
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodePlayer::IsEnableInput(const FieldMoveCodeWork* pMoveCodeWork) const
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if( pWork->nowState == PLAYER_MOVE_RUN_TURN
   || pWork->nowState == PLAYER_MOVE_TSUMADUKI  // @fix 高橋要望:つまずき中は下画面起動をフックする。
   || pWork->nowState == PLAYER_MOVE_PIYORI_RIGHT_START
   || pWork->nowState == PLAYER_MOVE_PIYORI_RIGHT_LOOP
   || pWork->nowState == PLAYER_MOVE_PIYORI_RIGHT_END
   || pWork->nowState == PLAYER_MOVE_PIYORI_LEFT_START
   || pWork->nowState == PLAYER_MOVE_PIYORI_LEFT_LOOP
   || pWork->nowState == PLAYER_MOVE_PIYORI_LEFT_END )
  {
    return false;
  }
  return true;
}

FieldMoveCodePlayer::PlayerMove FieldMoveCodePlayer::GetPlayerMove(const FieldMoveCodeWork* pMoveCodeWork)
{
  const Work* pWork = (const Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return static_cast<PlayerMove>( pWork->nowState );
}

}; //end of namespace MoveModel
}; //end of namespace Field
