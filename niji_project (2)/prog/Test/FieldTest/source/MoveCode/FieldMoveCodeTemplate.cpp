//=================================================================================================
/**
*  @file   FieldMoveCodeTemplate.cpp
*  @brief  フィールド用 動作コード テンプレート(こちらをコピーして量産してください)
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveModel/FieldMoveModel_define.h"
#include "../../include/MoveCode/FieldMoveCode_define.h"
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeTemplate.h"
#include "../../include/ActionCommand/FieldActionCommandManager.h"
#include "../../include/ActionCommand/FieldActionCommand_define.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTemplate::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_TEMPLATE; 

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTemplate::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  pMoveCodeWork->pModel->SetActionCommand( FIELD_ACTION_COMMAND_TEMPLATE );
}

}; //end of namespace MoveModel
}; //end of namespace Field
