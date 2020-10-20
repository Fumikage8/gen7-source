#pragma once
//=================================================================================================
/**
*  @file   ActionCommandRefuse.h
*  @brief  フィールド用 アクションコマンド 拒否する
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldActionCommandBase.h"
#include "FieldActionCommand_define.h"

namespace Field{ namespace MoveModel {

class ActionCommandRefuse : public ActionCommandBase
{
private:

  //  登録できるシーケンスの最大数
  static const u32 ACMDFUNC_SEQ_MAX = 3;

  // このアクションでのワークのマッピング方法
  struct Work
  {
    s16 frameMax;
    s16 frame;
  };

  // アクション処理型( 静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています )
  typedef Field::MoveModel::ACTION_COMMAND_RETURN (ActionCommandRefuse::*ActionCommandFunc)( ActionCommandWork* );

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandRefuse(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandRefuse(){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_REFUSE; }

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの実行
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  * @brief  アクションコマンドの終了
  *
  * @param  pActionCommandWork  アクションコマンドワーク
  *
  * @retval ACTION_COMMAND_RETURN_END
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionEnd(ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  UpdateActionCommand関数内で実行される処理
  *
  *  @retval ACTION_COMMAND_RETURN_ONE_TIME_END(続く)
  *  @retval ACTION_COMMAND_RETURN_END         (終わり)
  *  @note   静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています
  */
  //-----------------------------------------------------------------------------
  virtual Field::MoveModel::ACTION_COMMAND_RETURN ExecuteFunc(ActionCommandWork* pActionCommandWork) { return (this->*m_pFuncTbl[ pActionCommandWork->sequence ])( pActionCommandWork ); }

private:

  ActionCommandFunc  m_pFuncTbl[ACMDFUNC_SEQ_MAX];    ///< シーケンス分の関数テーブル
};

} // MoveModel
} // Field

