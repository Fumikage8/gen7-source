//=================================================================================================
/**
*  @file   FieldActionCommandManager.cpp
*  @brief  フィールド用 アクションコマンドマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandBase.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepMove.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepMoveWithoutStartEnd.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepJump.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandTargetMove.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandTargetMoveWithOptionalMotion.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandRotation.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandRefuse.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandContinuousPlayback.h"

namespace Field{ namespace MoveModel{

//=====================================================================================================================
//! @brief マネージャー自身に関する操作
//=====================================================================================================================

// アクションコマンドテーブル
ActionCommandBase* FieldActionCommandManager::m_pTbl[ FIELD_ACTION_COMMAND_MAX ];

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldActionCommandManager::FieldActionCommandManager()
{
}

//----------------------------------------------------------------------------
/**
 *  @brief    初期化
 *
 *  @param    pHeap    ヒープ
 */
//-----------------------------------------------------------------------------
void FieldActionCommandManager::Initialize(gfl2::heap::HeapBase* pHeap)
{
  // m_pTblを生成
  m_pTbl[FIELD_ACTION_COMMAND_NONE                             ] = GFL_NEW( pHeap ) ActionCommandBase();
  m_pTbl[FIELD_ACTION_COMMAND_TEMPLATE                         ] = NULL;

  // 振り向き系
  m_pTbl[FIELD_ACTION_COMMAND_ROTATION                         ] = GFL_NEW( pHeap ) ActionCommandRotation();

  // 移動系
  m_pTbl[FIELD_ACTION_COMMAND_STEP_MOVE                        ] = GFL_NEW( pHeap ) ActionCommandStepMove();
  m_pTbl[FIELD_ACTION_COMMAND_STEP_MOVE_WITHOUT_START_END      ] = GFL_NEW( pHeap ) ActionCommandStepMoveWithoutStartEnd();
  m_pTbl[FIELD_ACTION_COMMAND_TARGET_MOVE                      ] = GFL_NEW( pHeap ) ActionCommandTargetMove();
  m_pTbl[FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION ] = GFL_NEW( pHeap ) ActionCommandTargetMoveWithOptionalMotion();

  // その場移動系

  // ジャンプ系
  m_pTbl[FIELD_ACTION_COMMAND_STEP_JUMP                        ] = GFL_NEW( pHeap ) ActionCommandStepJump();

  // ウェイト系

  // 描画フラグ系

  // ワープ系

  // アイコン表示
  
  // ローラースケート系

  // 忍び足系

  // 自転車系

  // 波乗り系

  // やぎ系

  // アニメーションアクション
  m_pTbl[FIELD_ACTION_COMMAND_REFUSE]              = GFL_NEW( pHeap ) ActionCommandRefuse();
  m_pTbl[FIELD_ACTION_COMMAND_CONTINUOUS_PLAYBACK] = GFL_NEW( pHeap ) ActionCommandContinuousPlayback();
}


//----------------------------------------------------------------------------
/**
 *  @brief    破棄
 */
//-----------------------------------------------------------------------------
void FieldActionCommandManager::Terminate(void)
{
  for( u32 i=0; i<FIELD_ACTION_COMMAND_MAX; ++i )
  {
    if( m_pTbl[i] )
    {
      GFL_DELETE m_pTbl[i];
      m_pTbl[i] = NULL;
    }
  }
}

//=====================================================================================================================
//! @brief ActionCommandのワークに関する操作
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークの初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @param  pModel             動作モデル
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::InitializeActionCommandWork(ActionCommandWork* pActionCommandWork, Field::MoveModel::FieldMoveModel* pModel)
{
  pActionCommandWork->pModel          = pModel;
  pActionCommandWork->actionCommandId = static_cast<FieldActionCommandId>(0);
  pActionCommandWork->sequence        = 0;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークのリセット
*
*  @param  pActionCommandWork  アクションコマンドワーク
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::ClearActionCommandWork(ActionCommandWork* pActionCommandWork)
{
  pActionCommandWork->actionCommandId = static_cast<FieldActionCommandId>(0);
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->DisableActionCommand();
}

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
void* FieldActionCommandManager::GetActionCommandWork(ActionCommandWork* pActionCommandWork, u32 size)
{
  GFL_ASSERT( size < ACTION_COMMAND_WORK_SIZE );
  return reinterpret_cast<void*>(pActionCommandWork->work);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークにアクションコマンドの設定
*
*  @param  pActionCommandWork  アクションコマンドワーク
*  @param  actionCommandId     アクションコマンドID
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::SetActionCommand(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId)
{
  if( actionCommandId == FIELD_ACTION_COMMAND_ROTATION
   || actionCommandId == FIELD_ACTION_COMMAND_STEP_MOVE
   || actionCommandId == FIELD_ACTION_COMMAND_STEP_MOVE_WITHOUT_START_END
   || actionCommandId == FIELD_ACTION_COMMAND_TARGET_MOVE
   || actionCommandId == FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION
   || actionCommandId == FIELD_ACTION_COMMAND_CONTINUOUS_PLAYBACK )
  {
    GFL_ASSERT( false );
  }

  GFL_ASSERT( actionCommandId < FIELD_ACTION_COMMAND_MAX );
  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

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
*  @param  isEnableIdleWait アイドルモーション待ちを行うか？
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::SetActionCommandRotation(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, f32 dirY, f32 dirX, f32 dirZ, u32 timeToUse, b32 isEnableIdleInit, b32 isCompressMotion, b32 isEnableTurnMotionInit, b32 isEnableIdleWait )
{
  if( actionCommandId != FIELD_ACTION_COMMAND_ROTATION )
  {
    GFL_ASSERT( false );
  }

  ActionCommandRotation::Work* pActionCommandWorkRotation = reinterpret_cast<ActionCommandRotation::Work*>(GetActionCommandWork( pActionCommandWork, sizeof(ActionCommandRotation::Work) ));
  pActionCommandWorkRotation->dirY      = dirY;
  pActionCommandWorkRotation->dirX      = dirX;
  pActionCommandWorkRotation->dirZ      = dirZ;
  pActionCommandWorkRotation->timeToUse = timeToUse;
  pActionCommandWorkRotation->isEnableIdleInit = isEnableIdleInit;
  pActionCommandWorkRotation->isCompressMotion = isCompressMotion;
  pActionCommandWorkRotation->isEnableTurnMotionInit = isEnableTurnMotionInit;
  pActionCommandWorkRotation->isEnableIdleWait = isEnableIdleWait;

  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

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
void FieldActionCommandManager::SetActionCommandTargetMove(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, b32 isWalk, gfl2::math::Vector3 pos, gfl2::math::Quaternion qua, b32 isAdjustQua, b32 isEnableActionStartMove, b32 isEnableActionEndMove, f32 thresholdAngleForMotion, b32 isSlow, b32 isCustomGoalCheck, b32 isMoveEndIdleWait, b32 isRunToWalk, f32 runToWalkDistance, s32 mositonChangeFrame )
{
  if( actionCommandId != FIELD_ACTION_COMMAND_TARGET_MOVE )
  {
    GFL_ASSERT( false );
  }

  ActionCommandTargetMove::Work* pActionCommandWorkTargetMove = reinterpret_cast<ActionCommandTargetMove::Work*>(GetActionCommandWork( pActionCommandWork, sizeof(ActionCommandTargetMove::Work) ));
  pActionCommandWorkTargetMove->isWalk = isWalk;
  pActionCommandWorkTargetMove->startPosition = pActionCommandWork->pModel->GetCharaDrawInstance()->GetPosition();
  pActionCommandWorkTargetMove->movePosition       = pos;
  pActionCommandWorkTargetMove->moveQuaternion     = qua;
  pActionCommandWorkTargetMove->isAdjustQuaternion = isAdjustQua;
  pActionCommandWorkTargetMove->isEnableActionStartMove = isEnableActionStartMove;
  pActionCommandWorkTargetMove->isEnableActionEndMove   = isEnableActionEndMove;
  pActionCommandWorkTargetMove->thresholdAngleForMotion = thresholdAngleForMotion;
  pActionCommandWorkTargetMove->isSlow = isSlow;
  pActionCommandWorkTargetMove->isCustomGoalCheck = isCustomGoalCheck;
  pActionCommandWorkTargetMove->isMoveEndIdleWait = isMoveEndIdleWait;
	pActionCommandWorkTargetMove->isRunToWalk = isRunToWalk;
	pActionCommandWorkTargetMove->runToWalkDistance = runToWalkDistance;
	pActionCommandWorkTargetMove->mositonChangeFrame = mositonChangeFrame;

  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

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
void FieldActionCommandManager::SetActionCommandTargetMoveWithOptionalMotion(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, u32 motionId, gfl2::math::Vector3 pos, gfl2::math::Quaternion qua)
{
  if( actionCommandId != FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION )
  {
    GFL_ASSERT( false );
  }

  ActionCommandTargetMoveWithOptionalMotion::Work* pActionCommandWorkTargetMoveWithOptionalMotion = reinterpret_cast<ActionCommandTargetMoveWithOptionalMotion::Work*>(GetActionCommandWork( pActionCommandWork, sizeof(ActionCommandTargetMoveWithOptionalMotion::Work) ));
  pActionCommandWorkTargetMoveWithOptionalMotion->motionId       = motionId;
  pActionCommandWorkTargetMoveWithOptionalMotion->movePosition   = pos;
  pActionCommandWorkTargetMoveWithOptionalMotion->moveQuaternion = qua;

  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

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
void FieldActionCommandManager::SetActionCommandStepMove(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, b32 isWalk, u32 stepCnt, b32 enableCollision, f32 moveRate, b32 isEnableWalkEnd, b32 isEnableIdleInit, b32 isSlow )
{
  if( actionCommandId != FIELD_ACTION_COMMAND_STEP_MOVE )
  {
    GFL_ASSERT( false );
  }

  ActionCommandStepMove::Work* pActionCommandWorkStepMove = reinterpret_cast<ActionCommandStepMove::Work*>(GetActionCommandWork( pActionCommandWork, sizeof(ActionCommandStepMove::Work) ));
  pActionCommandWorkStepMove->isWalk          = isWalk;
  pActionCommandWorkStepMove->stepCnt         = stepCnt;
  pActionCommandWorkStepMove->enbaleCollision = enableCollision;
  pActionCommandWorkStepMove->moveRate        = moveRate;
  pActionCommandWorkStepMove->isEnableWalkEnd = isEnableWalkEnd;
  pActionCommandWorkStepMove->isEnableIdleInit= isEnableIdleInit;
  pActionCommandWorkStepMove->isSlow = isSlow;

  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

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
void FieldActionCommandManager::SetActionCommandStepMoveWithoutStartEnd(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId, u32 stepCnt, b32 isOneFrame)
{
  if( actionCommandId != FIELD_ACTION_COMMAND_STEP_MOVE_WITHOUT_START_END )
  {
    GFL_ASSERT( false );
  }

  ActionCommandStepMoveWithoutStartEnd::Work* pActionCommandWorkStepMoveWithoutStartEnd = reinterpret_cast<ActionCommandStepMoveWithoutStartEnd::Work*>(GetActionCommandWork( pActionCommandWork, sizeof(ActionCommandStepMoveWithoutStartEnd::Work) ));
  pActionCommandWorkStepMoveWithoutStartEnd->stepCnt = stepCnt;
  pActionCommandWorkStepMoveWithoutStartEnd->isOneFrame = isOneFrame;
  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

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
void FieldActionCommandManager::SetActionCommandContinuousPlayBack(ActionCommandWork*   pActionCommandWork,
                                                                   FieldActionCommandId actionCommandId,
                                                                   u32*                 pAnimationIdArray,
                                                                   s16*                 pFrameStartArray,
                                                                   s16*                 pLoopMaxArray,
                                                                   s16*                 pBlendArray)
{
  if( actionCommandId != FIELD_ACTION_COMMAND_CONTINUOUS_PLAYBACK )
  {
    GFL_ASSERT( false );
  }

  ActionCommandContinuousPlayback::Work* pActionCommandWorkContinuousPlayBack = reinterpret_cast<ActionCommandContinuousPlayback::Work*>(GetActionCommandWork( pActionCommandWork, sizeof(ActionCommandContinuousPlayback::Work) ));
  for( u32 i=0 ; i<ActionCommandContinuousPlayback::REGIST_MAX_CNT ; i++ )
  {
    pActionCommandWorkContinuousPlayBack->animationId[i] = pAnimationIdArray[i];
    pActionCommandWorkContinuousPlayBack->frameStart[i]  = pFrameStartArray[i];
    pActionCommandWorkContinuousPlayBack->loopMax[i]     = pLoopMaxArray[i];
    pActionCommandWorkContinuousPlayBack->blend[i]       = pBlendArray[i];
  }
  pActionCommandWorkContinuousPlayBack->loopCnt = 0;
  pActionCommandWorkContinuousPlayBack->state   = 0;

  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->EnableActionCommand();
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークからアクションコマンドの実行
*
*  *コマンドの終了状態を保持している。
*  *コマンドを実行した人が、ActionCommandWorkをクリアする必要がある。
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::UpdateActionCommand(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  ActionCommandBase* pAcmd;
  
  if( pActionCommandWork->actionCommandId < FIELD_ACTION_COMMAND_MAX )
  {
    pAcmd = m_pTbl[pActionCommandWork->actionCommandId];
  }
  else
  {
    GFL_ASSERT( pActionCommandWork->actionCommandId < FIELD_ACTION_COMMAND_MAX );
    pAcmd                               = m_pTbl[FIELD_ACTION_COMMAND_NONE];
    pActionCommandWork->actionCommandId = FIELD_ACTION_COMMAND_NONE;
    pActionCommandWork->sequence        = 0;
  }

  // 実行
  pAcmd->UpdateActionCommand( pActionCommandWork );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワーク内にあるアクションコマンドのシーケンス(実行状況)を指定分だけ進める
*
*  @param  pActionCommandWork  アクションコマンドワーク
*  @param  add                 進めたいシーケンス値
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::AddActionCommandWorkSequence(ActionCommandWork* pActionCommandWork, s32 add)
{
  if( (static_cast<s32>(pActionCommandWork->sequence) + add) >= 0 )
  {
    pActionCommandWork->sequence += add;
  }
}

} // MoveModel
} // Field
