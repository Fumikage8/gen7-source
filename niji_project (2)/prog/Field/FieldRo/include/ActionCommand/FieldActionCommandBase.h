#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandBase.h
*  @brief  フィールド用 アクションコマンド ベース
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Quaternion.h>
#include "../ActionCommand/FieldActionCommand_define.h"

namespace Field{ namespace MoveModel {

class FieldMoveModel;

//----------------------------------------------------------------------------------------
//! @brief アクションコマンド ベース(何もしない
//----------------------------------------------------------------------------------------
class ActionCommandBase
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandBase(void) : m_seqNum(0){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandBase(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_NONE; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの更新
  *
  *  @retval ACTION_COMMAND_RETURN_ONE_TIME_END(続く)
  *  @retval ACTION_COMMAND_RETURN_END         (終わり)
  */
  //-----------------------------------------------------------------------------
  Field::MoveModel::ACTION_COMMAND_RETURN UpdateActionCommand(ActionCommandWork* pActionCommandWork);

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  UpdateActionCommand関数内で実行される処理
  *
  *  @retval ACTION_COMMAND_RETURN_ONE_TIME_END(続く)
  *  @retval ACTION_COMMAND_RETURN_END         (終わり)
  */
  //-----------------------------------------------------------------------------
  virtual Field::MoveModel::ACTION_COMMAND_RETURN ExecuteFunc(ActionCommandWork* pActionCommandWork){ return Field::MoveModel::ACTION_COMMAND_RETURN_END; }

protected:

  //----------------------------------------------------------------------------
  /**
  *  @brief  シーケンスの最大数を設定
  *
  *  @param  seqNum シーケンスの最大数
  */
  //-----------------------------------------------------------------------------
  void SetSequenceMaxNum(u32 seqNum){ m_seqNum = seqNum; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  シーケンスの最大数を取得
  *
  *  @return シーケンスの最大数
  */
  //-----------------------------------------------------------------------------
  u32 GetSequenceMaxNum(void){ return m_seqNum; }

private:

  u32   m_seqNum;   ///< シーケンス数
};

}; //end of namespace MoveModel
}; //end of namespace Field
