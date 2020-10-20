//=================================================================================================
/**
*  @file   FieldActionCommandTargetMoveWithOptionalMotion.cpp
*  @brief  フィールド用 アクションコマンド 任意モーション移動
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandTargetMoveWithOptionalMotion.h"
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
ActionCommandTargetMoveWithOptionalMotion::ActionCommandTargetMoveWithOptionalMotion()
{
  m_pFuncTbl[0] = &ActionCommandTargetMoveWithOptionalMotion::ActionInit;
  m_pFuncTbl[1] = &ActionCommandTargetMoveWithOptionalMotion::ActionMain;
  m_pFuncTbl[2] = &ActionCommandTargetMoveWithOptionalMotion::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMoveWithOptionalMotion::ActionInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 次へ
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  pActionCommandWork->pModel->GetCharaDrawInstance()->ChangeAnimation( pWork->motionId );
  pWork->frame            = 0;
  pWork->frameMax         = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
  pWork->startPosition    = pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition();
  pWork->startQuaternion  = pActionCommandWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );

  // 最低動作に必要なフレームを下回っていたときは再設定
  if( pWork->frameMax < MOVE_FRAME )
  {
    pWork->frameMax = MOVE_FRAME;
  }
  return Field::MoveModel::ACTION_COMMAND_RETURN_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMoveWithOptionalMotion::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 現在の姿勢を目標の姿勢へと補完する
  gfl2::math::Quaternion qua( pWork->startQuaternion );
  qua.Slerp( pWork->moveQuaternion , gfl2::math::Clamp( ( (f32)pWork->frame+1.0f ) / (f32)MOVE_FRAME , 0.0f, 1.0f ) );
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetRotationQuat(qua);

  // 現在の座標を目標の座標へと補完する
  gfl2::math::Vector3 pos( pWork->startPosition );
  pos.Lerp( pWork->movePosition    , gfl2::math::Clamp( ( (f32)pWork->frame+1.0f ) / (f32)MOVE_FRAME , 0.0f, 1.0f ) );
  pActionCommandWork->pModel->GetCharaDrawInstance()->SetPosition(pos);

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
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
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandTargetMoveWithOptionalMotion::ActionEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}

} // mmodel
} // field
