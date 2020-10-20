//=================================================================================================
/**
*  @file   FieldActionCommandTemplate.cpp
*  @brief  フィールド用 アクションコマンド テンプレート(こちらをコピーして量産してください)
*  @author yy-ikeuchi
*  @date   2015.02.27
*/
//=================================================================================================
#include "../../include/ActionCommand/FieldActionCommandTemplate.h"
#include "../../include/ActionCommand/FieldActionCommandManager.h"
#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "../../include/MoveModel/FieldMoveModel.h"

namespace Field{ namespace MoveModel {

ActionCommandTemplate::ActionCommandTemplate()
{
  m_pFuncTbl[0] = ActionCommandTemplate::ActionInit;
  m_pFuncTbl[1] = ActionCommandTemplate::ActionMain;
  m_pFuncTbl[2] = FieldActionCommandManager::ActionCommandEnd;
  m_SeqNum = 3;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの初期化
*
*  @param  アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandTemplate::ActionInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));    
  pWork->frame_max = 30;
  pWork->frame     =  0;
  pActionCommandWork->pModel->onStateBit( FIELD_MOVE_MODEL_STATEBIT_ACMD );

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行
*
*  @param  アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandTemplate::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->frame++;
  if( pWork->frame < pWork->frame_max )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

} // mmodel
} // field
