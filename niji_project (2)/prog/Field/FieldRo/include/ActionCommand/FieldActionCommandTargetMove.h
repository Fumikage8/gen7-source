#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandTargetMove.cpp
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
class ActionCommandTargetMove : public ActionCommandBase
{
public:

  // このアクションでのワークのマッピング方法
  struct Work
  {
    gfl2::math::Vector3    movePosition;
    gfl2::math::Vector3    startPosition;
    gfl2::math::Quaternion moveQuaternion;
    gfl2::math::Quaternion startQuaternion;
    gfl2::math::Quaternion targetQuaternion;
    s32                    frameMax;
    s32                    frame;
    b32                    isWalk;
    b32                    isAdjustQuaternion;
    b32                    isEnableActionStartMove; //< 歩き始めを行なうか指定
    b32                    isEnableActionEndMove;   //< 歩き終わりを行なうか指定
    f32                    thresholdAngleForMotion;
    b8                     isPrevCollisionActiveTerrain;
    b8                     isPrevCollisionActiveStatic;
    b8                     isPrevCollisionActiveTraffic;
    b8                     isPrevCollisionActiveOshikaeshiNaminori;
    b8                     isPrevCollisionActiveOshikaeshiRoba2;
    b8                     isPrevCollisionActiveOshikaeshiRide;
    b8                     isSlow:1;
    b8                     isCustomGoalCheck:1;
    b8                     isMoveEndIdleWait:1;
		b8                     isRunToWalk:1;
		f32                    runToWalkDistance;
		s32                    mositonChangeFrame;
  };

private:

  //  登録できるシーケンスの最大数
  static const u32 ACMDFUNC_SEQ_MAX = 13;

  //  アクション処理型( 静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています )
  typedef Field::MoveModel::ACTION_COMMAND_RETURN (ActionCommandTargetMove::*ActionCommandFunc)( ActionCommandWork* );

  // 最大再生数
  static const u32 MAX_LOOP   = 50;
  // ゴールとみなす閾値
  static const u32 GOAL_WALK_RANGE = 21;
  static const u32 GOAL_RUN_RANGE  = 21;
  static const u32 GOAL_RIDE_RUN_RANGE = 41;

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandTargetMove();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandTargetMove(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_TARGET_MOVE; }

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(最初の旋回始め)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionPrevTurnInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(最後の旋回始め)の初期化
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionAfterTurnInit(Field::MoveModel::ActionCommandWork* pActionCommandWork);

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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(旋回始め)の実行
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionTurnMain(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(歩き中)の実行
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionStepMain(Field::MoveModel::ActionCommandWork* pActionCommandWork);

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

