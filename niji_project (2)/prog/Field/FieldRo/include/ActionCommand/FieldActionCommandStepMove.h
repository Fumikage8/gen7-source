#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandStepMove.cpp
*  @brief  フィールド用 アクションコマンド ステップ移動
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldActionCommandBase.h"
#include <math/include/gfl2_Quaternion.h>
#include <math/include/gfl2_Vector.h>

namespace Field{ namespace MoveModel {

// ステップ移動共通処理
class ActionCommandStepMove : public ActionCommandBase
{
public:

  // このアクションでのワークのマッピング方法
  struct Work
  {
    s32 frameMax;
    s32 frame;
    b32 isWalk;
    u32 stepCnt;
    b32 enbaleCollision;
    f32 moveRate;
    b32 isEnableWalkEnd;
    b32 isEnableIdleInit;
    b8  isSlow;
  };

private:

  //  登録できるシーケンスの最大数
  static const u32 ACMDFUNC_SEQ_MAX = 9;

  // アクション処理型( 静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています )
  typedef Field::MoveModel::ACTION_COMMAND_RETURN (ActionCommandStepMove::*ActionCommandFunc)( ActionCommandWork* );

public:

  //-----------------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandStepMove(const ActionCommandStepMove &other){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandStepMove();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandStepMove(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_STEP_MOVE; }

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(歩き始め)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionStartInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(歩き中)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionStepInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(歩き終了)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionEndInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(待機)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionIdleInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
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

  //-----------------------------------------------------------------------------
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

} // mmodel
} // field

