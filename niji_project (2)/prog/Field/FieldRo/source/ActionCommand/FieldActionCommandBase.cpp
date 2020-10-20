//=================================================================================================
/**
*  @file   FieldActionCommandBase.cpp
*  @brief  フィールド用 アクションコマンド　ベース
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandBase.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

namespace Field{ namespace MoveModel{

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの更新
*
*  @retval ACTION_COMMAND_RETURN_ONE_TIME_END(続く)
*  @retval ACTION_COMMAND_RETURN_END         (終わり)
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandBase::UpdateActionCommand(ActionCommandWork* pActionCommandWork)
{
  ACTION_COMMAND_RETURN ret = ACTION_COMMAND_RETURN_END;
  s32 roopCount = 0;

  // シーケンスチェック
  if( m_seqNum > pActionCommandWork->sequence )
  {
    do
    {
      ret = ExecuteFunc(pActionCommandWork);

      // シーケンスオーバーチェック
      GFL_ASSERT( m_seqNum > pActionCommandWork->sequence );

      ++roopCount;

      // コンティニューが1000回続いたら、明らかにおかしいためアサートストップ
      GFL_ASSERT_STOP( roopCount < 1000 );

    }while( ret == ACTION_COMMAND_RETURN_CONTINUE ); // CONTINUEの間動作  @whilecheck
  }

  if( ret == ACTION_COMMAND_RETURN_END )
  {
    // アクションコマンドの実行が完了したことを、
    // モデルに設定
    pActionCommandWork->pModel->DisableActionCommand();
  }

  return ret;
}

} // MoveModel
} // Field
