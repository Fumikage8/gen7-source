//=================================================================================================
/**
*  @file   FieldActionCommandRotation.cpp
*  @brief  フィールド用 アクションコマンド 回転
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandRotation.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief コンストラクタ
*/
//-----------------------------------------------------------------------------
ActionCommandRotation::ActionCommandRotation()
{
  m_pFuncTbl[0] = &ActionCommandRotation::ActionRotationInit;
  m_pFuncTbl[1] = &ActionCommandRotation::ActionRotationMain;
  m_pFuncTbl[2] = &ActionCommandRotation::ActionIdleInit;
  m_pFuncTbl[3] = &ActionCommandRotation::ActionIdleMain;
  m_pFuncTbl[4] = &ActionCommandRotation::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(回転)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandRotation::ActionRotationInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 指定フレームで指定方向へ補完するための下準備
  poke_3d::model::CharaModel* pCharaModel = pActionCommandWork->pModel->GetCharaDrawInstance();
  Work* pWork     = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  pWork->frame    = 0;
  pWork->frameMax = pWork->timeToUse;
  pWork->startQua = pCharaModel->GetRotationQuat();
  pWork->targetQua.RadianYXZToQuaternion( gfl2::math::ConvDegToRad( pWork->dirX ),
                                          gfl2::math::ConvDegToRad( pWork->dirY ),
                                          gfl2::math::ConvDegToRad( pWork->dirZ ) );

  // 対象がプレイヤーで且つライド状態か調べる
  b32 isRide = false;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      isRide = true;
    }
  }

  // 旋回開始時のモーション変更をカットする
  if( !pWork->isEnableTurnMotionInit )
  {
    // 次へ
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  // ライド時は待機モーションを再生する
  if( isRide )
  {
    u32 animationId = pActionCommandWork->pModel->GetDefaultIdleAnimationId();
    if( pWork->isCompressMotion )
    {
      u32 blendRate = gfl2::math::min( (u32)pCharaModel->GetAnimationDefaultBlendFrame(animationId) , pWork->timeToUse );
      pCharaModel->ChangeAnimationSmooth( animationId,blendRate );
      static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimationSmooth( animationId,blendRate );
    }
    else
    {
      pCharaModel->ChangeAnimation( animationId );
      static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( animationId );
    }
  }
  // 未ライド時は旋回モーションを再生する
  else
  {
    // キャラクターローカルに変換する逆行列を取得
    gfl2::math::Matrix34 startRot,startRotInv;
    pWork->startQua.QuaternionToMatrix(startRot);
    startRotInv = startRot.Inverse();

    // 目標の向きベクトルを取得
    gfl2::math::Matrix34 targetRot;
    pWork->targetQua.QuaternionToMatrix(targetRot);
    gfl2::math::Vector4 targetVec(0.0f,0.0f,1.0f,0.0f);
    targetVec = targetRot.TransformNormal(targetVec);
    gfl2::math::Vector4 targetLocalVec = startRotInv.TransformNormal(targetVec);

    // 左にターンする
    if( targetLocalVec.x >= 0.0f )
    {
      if( pWork->isCompressMotion )
      {
        u32 blendRate = gfl2::math::min( (u32)pCharaModel->GetAnimationDefaultBlendFrame(P2_BASE_FI017_TURN01_LEFT) , pWork->timeToUse );
        pCharaModel->ChangeAnimationSmooth( P2_BASE_FI017_TURN01_LEFT,blendRate );
      }
      else
      {
        pCharaModel->ChangeAnimation( P2_BASE_FI017_TURN01_LEFT );
      }
    }
    // 右にターンする
    else
    {
      if( pWork->isCompressMotion )
      {
        u32 blendRate = gfl2::math::min( (u32)pCharaModel->GetAnimationDefaultBlendFrame(P2_BASE_FI016_TURN01_RIGHT) , pWork->timeToUse );
        pCharaModel->ChangeAnimationSmooth( P2_BASE_FI016_TURN01_RIGHT,blendRate );
      }
      else
      {
        pCharaModel->ChangeAnimation( P2_BASE_FI016_TURN01_RIGHT );
      }
    }

    // 旋回モーションがないものがある
    if( pCharaModel->GetAnimationEndFrame() <= 0 )
    {
      pCharaModel->ChangeAnimation( pActionCommandWork->pModel->GetDefaultIdleAnimationId() );
    }
  }

  // 旋回フレームに併せて早送りをする
  if( pWork->isCompressMotion )
  {
    f32 stepFrame = (pCharaModel->GetAnimationEndFrame()) / ( (f32)pWork->frameMax );
    pCharaModel->SetAnimationStepFrame(stepFrame);
    if( isRide )
    {
      static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel )->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationStepFrame(stepFrame);
    }
  }

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(回転)の実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandRotation::ActionRotationMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // ターゲット姿勢にむけて補完する
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  gfl2::math::Quaternion qua = pWork->startQua;
  qua.Slerp( pWork->targetQua , ((f32)pWork->frame+1.0f) / ((f32)pWork->frameMax+1.0f) );
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetRotationQuat(qua);

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
  if( pWork->frame <= pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 念の為にターゲット姿勢にあわせる
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetRotationQuat(pWork->targetQua);

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(待機)の初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandRotation::ActionIdleInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork     = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  if(!pWork->isEnableIdleInit)
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
  }

  // 待機モーションを単発再生する
  pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( (u32)( pActionCommandWork->pModel->GetDefaultIdleAnimationId() ) );

  // ライドポケモンの待機モーションも単発再生する
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( pActionCommandWork->pModel->GetDefaultIdleAnimationId() );
    }
  }
   
  if (!pWork->isEnableIdleWait)
  {
    FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 2 );
    return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
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
*  @brief  アクションコマンド(待機)の実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandRotation::ActionIdleMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

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
*  @brief  アクションコマンドの終了
*
*  @param  pActionCommandWork  アクションコマンドワーク
*
*  @retval ACTION_COMMAND_RETURN_END
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandRotation::ActionEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}

} // mmodel
} // field
