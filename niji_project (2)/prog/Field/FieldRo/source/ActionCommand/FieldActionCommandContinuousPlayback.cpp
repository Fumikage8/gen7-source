//=================================================================================================
/**
*  @file   ActionCommandContinuousPlayback.cpp
*  @brief  フィールド用 アクションコマンド モーション連続再生
*  @author yy-ikeuchi
*  @date   2015.02.27
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandContinuousPlayback.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
#include "Field/FieldRo/include/Fieldmap.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief コンストラクタ
*/
//-----------------------------------------------------------------------------
ActionCommandContinuousPlayback::ActionCommandContinuousPlayback()
{
  m_pFuncTbl[0] = &ActionCommandContinuousPlayback::ActionInit;
  m_pFuncTbl[1] = &ActionCommandContinuousPlayback::ActionMain;
  m_pFuncTbl[2] = &ActionCommandContinuousPlayback::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandContinuousPlayback::ActionInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork    = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));    
  pWork->loopCnt = -1;

  // プレイヤーのライド専用処理
  b32 isRide = false;
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // 自身と自身が乗っている動作モデルにもモーションを流し込む
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      // プレイヤーのモーションIDをライド状況にあったモーションIDに変換する
      u32 convMotionId       = 0;
      s32 convMotionEndFrame = 0;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetMoveModelManager()->ConvertPlayerMotionIdToRideMotionId( pWork->animationId[pWork->state],
                                                                                                                                 pPlayer->GetOnMoveModel()->GetCharacterId(),
                                                                                                                                 &convMotionId,
                                                                                                                                 &convMotionEndFrame );

      // ブレンド値をマニュアル設定
      if( pWork->blend[pWork->state] >= 0  )
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimationSmooth                  ( convMotionId,(u32)pWork->blend[pWork->state] );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimationSmooth( convMotionId,(u32)pWork->blend[pWork->state] );
      }
      // ブレンド値のデフォルト設定
      else
      {
        pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( convMotionId );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( convMotionId );
      }

      // アニメーションのループ明示設定（未設定orループしない場合はリソース準拠にする）
      if( pWork->loopMax[pWork->state] >= 1 ){
        pPlayer->GetCharaDrawInstance()->SetAnimationLoop                  ( true );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationLoop( true );
      }


      // アニメーションの開始位置設定
      pPlayer->GetCharaDrawInstance()->SetAnimationStartFrame                  ( (f32)pWork->frameStart [pWork->state] );
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationStartFrame( (f32)pWork->frameStart [pWork->state] );

      // 終了フレームがリソースから設定されている場合
      if( convMotionEndFrame >= 0 )
      {
        pPlayer->GetCharaDrawInstance()->SetAnimationEndFrame                  ( (f32)convMotionEndFrame );
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationEndFrame( (f32)convMotionEndFrame );
      }

      isRide = true;
    }
  }

  // ライド以外の場合
  if( !isRide )
  {
    // ブレンド値をマニュアル設定
    if( pWork->blend[pWork->state] >= 0  )
    {
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimationSmooth( pWork->animationId[pWork->state],(u32)pWork->blend[pWork->state] );
    }
    // ブレンド値のデフォルト設定
    else
    {
      pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( pWork->animationId[pWork->state] );
    }

    // アニメーションのループ明示設定（未設定orループしない場合はリソース準拠にする）
    if( pWork->loopMax[pWork->state] >= 1 ){
      pActionCommandWork->pModel->GetCharaDrawInstance()->SetAnimationLoop( true );
    }

    // アニメーションの開始位置設定
    pActionCommandWork->pModel->GetCharaDrawInstance()->SetAnimationStartFrame( (f32)pWork->frameStart [pWork->state] );
  }

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandContinuousPlayback::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // ループ時でなければ継続してアクションコマンドを実行する
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));  

  if( pWork->loopMax[pWork->state] >= 0 ){
    // @note 処理負荷対策:もしもアニメーション停止中に実行されていた場合は、アニメーション終了扱いとする
    if (!pActionCommandWork->pModel->GetCharaDrawInstance()->IsAnimationLastFrame() && !pActionCommandWork->pModel->IsAnimationStopByUnRegist() )
    {
      return ACTION_COMMAND_RETURN_ONE_TIME_END;
    }
  }
  // 指定回数分ループしていなければ更に継続する
  pWork->loopCnt++;
  if( pWork->loopCnt < pWork->loopMax[pWork->state] )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  pWork->state++;

  if( pWork->state < REGIST_MAX_CNT ){
    // 次のモーションがあるなら最初から
    if( pWork->animationId[pWork->state] != INVALID_ANIMATION_ID )
    {
      FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, -1 );
      return ACTION_COMMAND_RETURN_CONTINUE;
    }
  }

  // 次のモーションがないなら終了
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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandContinuousPlayback::ActionEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}

} // mmodel
} // field
