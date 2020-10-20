//=================================================================================================
/**
*  @file   FieldActionCommandBase.cpp
*  @brief  フィールド用 アクションコマンド　ベース
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/ActionCommand/FieldActionCommandBase.h"
#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "../../include/MoveModel/FieldMoveModel_define.h"
#include "../../include/MoveModel/FieldMoveModel.h"

namespace Field{ namespace MoveModel{

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの更新
*
*  @retval ACMD_RET_ONE_TIME_END(続く)
*  @retval ACMD_RET_END         (終わり)
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandBase::UpdateActionCommand(ActionCommandWork* pActionCommandWork) const
{
  ACTION_COMMAND_RETURN ret = ACTION_COMMAND_RETURN_END;
  s32 roopCount = 0;

  GFL_ASSERT( ACMDFUNC_SEQ_MAX >= m_SeqNum );
  
  // シーケンスチェック
  if( m_SeqNum > pActionCommandWork->sequence )
  {
    do
    {
      ret = (m_pFuncTbl[ pActionCommandWork->sequence ])( pActionCommandWork );

      // シーケンスオーバーチェック
      GFL_ASSERT( m_SeqNum > pActionCommandWork->sequence );

      ++roopCount;

      // コンティニューが1000回続いたら、明らかにおかしいためアサートストップ
      GFL_ASSERT_STOP( roopCount < 1000 );

    }while( ret == ACTION_COMMAND_RETURN_CONTINUE ); // CONTINUEの間動作  @whilecheck
  }

  if( ret == ACTION_COMMAND_RETURN_END )
  {
    // アクションコマンドの実行が完了したことを、
    // モデルに設定
    pActionCommandWork->pModel->offStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD );
    pActionCommandWork->pModel->onStateBit ( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD_END );
  }

  return ret;
}

} // MoveModel
} // Field
