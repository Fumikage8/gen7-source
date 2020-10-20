#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandRotation.cpp
*  @brief  フィールド用 アクションコマンド 回転
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldActionCommandBase.h"
#include <math/include/gfl2_Quaternion.h>
#include <math/include/gfl2_Vector.h>

namespace Field{ namespace MoveModel {

// 回転処理
class ActionCommandRotation : public ActionCommandBase
{
public:

  // このアクションでのワークのマッピング方法
  struct Work
  {
    f32 dirY;
    f32 dirX;
    f32 dirZ;
    s32 frameMax;
    s32 frame;
    u32 timeToUse;
    gfl2::math::Quaternion startQua;
    gfl2::math::Quaternion targetQua;
    b32 isEnableIdleInit;
    b32 isCompressMotion;
    b32 isEnableTurnMotionInit; // 旋回開始時にモーションを切り替えるか
    b32 isEnableIdleWait;
  };

private:

  //  登録できるシーケンスの最大数
  static const u32 ACMDFUNC_SEQ_MAX = 5;

  //  アクション処理型( 静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています )
  typedef Field::MoveModel::ACTION_COMMAND_RETURN (ActionCommandRotation::*ActionCommandFunc)( ActionCommandWork* );

public:

  //-----------------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandRotation();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandRotation(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_ROTATION; }

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(回転)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionRotationInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

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
  *  @brief  アクションコマンド(回転)の実行
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionRotationMain(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(待機)の実行
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionIdleMain(Field::MoveModel::ActionCommandWork* pActionCommandWork);

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

