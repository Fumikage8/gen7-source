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

  //  アクション処理型(静的メンバ関数じゃないと登録できない)
  typedef Field::MoveModel::ACTION_COMMAND_RETURN (*ActionCommandFunc)( ActionCommandWork* );

  //  登録できるシーケンスの最大数
  static const u32 ACMDFUNC_SEQ_MAX = 16;
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandBase(void) : m_SeqNum(0){}

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
  *  @retval ACMD_RET_ONE_TIME_END(続く)
  *  @retval ACMD_RET_END         (終わり)
  */
  //-----------------------------------------------------------------------------
  Field::MoveModel::ACTION_COMMAND_RETURN UpdateActionCommand(ActionCommandWork* pActionCommandWork) const;

protected:

  ActionCommandFunc  m_pFuncTbl[ACMDFUNC_SEQ_MAX];    ///< シーケンス分の関数テーブル
  u32                m_SeqNum;                        ///< シーケンス数

};

}; //end of namespace MoveModel
}; //end of namespace Field
