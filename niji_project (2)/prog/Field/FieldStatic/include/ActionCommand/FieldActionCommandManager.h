#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandManager.h
*  @brief  フィールドアクションコマンドマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "../../include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandBase.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------------------
//! @brief フィールドアクションコマンドマネージャー
//----------------------------------------------------------------------------------------
class FieldActionCommandManager
{
public:

  //=====================================================================================================================
  //! @brief マネージャー自身に関する操作
  //=====================================================================================================================

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldActionCommandManager();

  //----------------------------------------------------------------------------
  /**
  *  @brief    初期化
  *
  *  @param    pHeap    ヒープ
  */
  //-----------------------------------------------------------------------------
  void Initialize(gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------------
  /**
  *  @brief    破棄
  */
  //-----------------------------------------------------------------------------
  void Terminate(void);

  //=====================================================================================================================
  //! @brief アクションコマンドのワークに関する操作
  //=====================================================================================================================

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークの初期化
  *
  *  @param  pActionCommandWork   アクションコマンドワーク
  *  @param  pModel               動作モデル
  */
  //-----------------------------------------------------------------------------
  static void InitializeActionCommandWork(ActionCommandWork* pActionCommandWork, Field::MoveModel::FieldMoveModel* pModel);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークのリセット
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  */
  //-----------------------------------------------------------------------------
  static void ClearActionCommandWork(ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークから共有ワークを取得
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  size                必要なサイズ
  *
  *  @return 共有ワーク
  */
  //-----------------------------------------------------------------------------
  static void* GetActionCommandWork(ActionCommandWork* pActionCommandWork, u32 size);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークにアクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommand(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークに回転アクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  *  @param  dirY                回転Y
  *  @param  dirX                回転X
  *  @param  dirZ                回転Z
  *  @param  timeToUse           回転にかかるフレーム
  *  @param  isEnableIdleInit    待機モーションに切り替えるか
  *  @param  isCompressMotion    旋回モーションの再生をtimeToUseで指定したフレーム内で収めるかどうか
  *  @param  isEnableTurnMotionInit 旋回開始時にモーションを切り替えるか
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommandRotation(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, f32 dirY, f32 dirX, f32 dirZ, u32 timeToUse, b32 isEnableIdleInit, b32 isCompressMotion, b32 isEnableTurnMotionInit, b32 isEnableIdleWait );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークに指定地点移動アクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  *  @param  isWalk              歩きかどうか
  *  @param  pos                 目標座標
  *  @param  qua                 目標姿勢
  *  @param  isAdjustQua         目標姿勢を利用するかどうか
  *  @param  isEnableActionStartMove 歩き始めを再生するか
  *  @param  isEnableActionEndMove   歩き終わりを再生するか
  *  @param  thresholdAngleForMotion 旋回モーションを再生する角度差
  *  @param  isSlow              低速かどうか(momiji追加)
  *  @param  isCustomGoalCheck   ゴール判定を最適化するか(ワープがなくなる、momiji追加)
  */
  //-----------------------------------------------------------------------------
	static void SetActionCommandTargetMove(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, b32 isWalk, gfl2::math::Vector3 pos, gfl2::math::Quaternion qua, b32 isAdjustQua, b32 isEnableActionStartMove, b32 isEnableActionEndMove, f32 thresholdAngleForMotion, b32 isSlow = false, b32 isCustomGoalCheck = false, b32 isMoveEndIdleWait = true, b32 isRunToWalk = false, f32 runToWalkDistance = 0.0f, s32 mositonChangeFrame = -1 );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークに指定地点移動(任意モーション)アクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  *  @param  motionId            モーションID
  *  @param  pos                 目標座標
  *  @param  qua                 目標姿勢
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommandTargetMoveWithOptionalMotion(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, u32 motionId, gfl2::math::Vector3 pos, gfl2::math::Quaternion qua);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークにステップ移動アクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  *  @param  isWalk              歩きかどうか
  *  @param  stepCnt             ステップ数
  *  @param  enableCollision     コリジョンON or OFF
  *  @param  moveRate            モーションによる移動ベクトルを適用する割合
  *  @param  isEnableWalkEnd     歩き終わりをおこなうか 
  *  @param  isEnableIdleInit    Idle状態に戻すか
  *  @param  isSlow              低速かどうか(momiji追加)
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommandStepMove(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, b32 isWalk, u32 stepCnt, b32 enableCollision, f32 moveRate, b32 isEnableWalkEnd = true, b32 isEnableIdleInit = true, b32 isSlow = false );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークにステップ移動(開始・終了無し)アクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  *  @param  stepCnt             ステップ数
  *  @param  isOneFrame          1フレームで実行するかどうか。
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommandStepMoveWithoutStartEnd(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, u32 stepCnt, b32 isOneFrame = false);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークにモーション連続再生アクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  *  @param  pAnimationIdArray   アニメーションID配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  *  @param  pFrameStartArray    フレーム開始    配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  *  @param  pLoopMaxArray       ループ回数      配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  *  @param  pBlendArray         ブレンドフレーム配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommandContinuousPlayBack(ActionCommandWork*   pActionCommandWork,
                                                 FieldActionCommandId actionCommandId,
                                                 u32*                 pAnimationIdArray,
                                                 s16*                 pFrameStartArray,
                                                 s16*                 pLoopMaxArray,
                                                 s16*                 pBlendArray);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークからアクションコマンドの実行
  *
  *  *コマンドの終了状態を保持している。
  *  *コマンドを実行した人が、ActionCommandWorkをクリアする必要がある。
  */
  //-----------------------------------------------------------------------------
  static void UpdateActionCommand(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワーク内にあるアクションコマンドのシーケンス(実行状況)を指定分だけ進める
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  add                 進めたいシーケンス値
  */
  //-----------------------------------------------------------------------------
  static void AddActionCommandWorkSequence(ActionCommandWork* pActionCommandWork, s32 add);

private:

  //=====================================================================================================================
  //! @brief 内部
  //=====================================================================================================================

  // アクションコマンドテーブル
  static ActionCommandBase* m_pTbl[ FIELD_ACTION_COMMAND_MAX ];
};

}; //end of namespace MoveModel
}; //end of namespace Field
