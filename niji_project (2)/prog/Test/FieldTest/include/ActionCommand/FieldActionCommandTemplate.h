#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeTemplate.h
*  @brief  フィールド用 アクションコマンド テンプレート(こちらをコピーして量産してください)
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldActionCommandBase.h"
#include "FieldActionCommand_define.h"

namespace Field{ namespace MoveModel {

class ActionCommandTemplate : public ActionCommandBase
{
public:

  struct Work
  {
    s16 frame_max;
    s16 frame;
  };

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandTemplate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandTemplate(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_TEMPLATE; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの初期化
  *
  *  @param  アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  static ACTION_COMMAND_RETURN ActionInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの実行
  *
  *  @param  アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  static ACTION_COMMAND_RETURN ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork);
};

} // MoveModel
} // Field

