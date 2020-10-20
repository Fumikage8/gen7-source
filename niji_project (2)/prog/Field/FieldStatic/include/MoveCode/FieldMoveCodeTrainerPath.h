//======================================================================
/**
 * @file FieldMoveCodeTrainerPath.h
 * @date 2015/07/30 19:27:54
 * @author miyachi_soichi
 * @brief パス有トレーナー動作コード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_MOVECODE_TRAINERPATH_H_INCLUDED__
#define __FIELD_MOVECODE_TRAINERPATH_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// Field
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeBase.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"

// 前方参照
namespace Field {
  class FieldTrainerActor;
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)

/**
 * @class FieldMoveCodeTrainerPath
 * @brief パス有トレーナー動作コード
 */
class FieldMoveCodeTrainerPath : public FieldMoveCodeBase
{
  // コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN(FieldMoveCodeTrainerPath);

public:
  /**
   *  ワーク構造体
   */
  struct Work
  {
    u32   state;            //!< 状態
    f32   eyeLengthRate;    //!< 視線距離倍率
    f32   eyeHeightRate;    //!< 視線距離倍率
    f32   motionSpeedRatio; //!< モーション速度倍率
    s32   jointHeadIndex;   //!< 頭ジョイントの番号
    s32   locatorEyeIndex;  //!< 視線用ロケータ番号
    f32   delta_rot;        //!< 旋回角度
    u32   motionID;         //!< 演出用固有モーションIDとりおき
    u32   repeatCount;      //!< 固有モーション再生回数
    f32   startFrame;       //!< 固有モーション再生開始フレーム
    s32   comebackAnimID;   //!< 復帰時のアニメーションID
    f32   comebackAnimFrame;//!< 復帰時アニメーションフレーム
    f32   comebackAnimStepFrame;  //!< 復帰時アニメーションステップフレーム
    f32   progress;         //!< パス進捗
    FieldTrainerActor*  pTrainerActor; //!< アクター
    b32   isUseOldEyePos;              //!< 前回の視線の注視点を利用するかどうか
    gfl2::math::Vector3 oldEyePos;     //!< 前回の視線の注視点
    u32   isEncount : 1;        //!< @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
    u32   isReverse : 1;
    u32   padding   : 30;
  }; // struct Work

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldMoveCodeTrainerPath( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveCodeTrainerPath( void );

  /**
   *  @brief  対応している動作コードの取得
   *  @return 動作コード
   */
  virtual FIELD_MOVE_CODE_ID GetMVCode( void ) const { return FIELD_MOVE_CODE_TRAINER_PATH; }

  /**
   *  @brief  動作コード処理  初期化
   */
  virtual void InitProcess( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理  メイン
   */
  virtual void MainProcess( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理  再開
   */
  virtual void Resume( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理  中断
   */
  virtual void Suspend( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
  *  @brief  動作コード処理  終了
  */
  virtual void Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const;
  /**
  *  @brief  動作コード処理  復帰
  */
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork,b32 isLoad ) const;

private:
  /**
   *  @brief  トレーナーの状態
   */
  enum State
  {
    STATE_NONE,             //!< なし
    STATE_WALK,             //!< パス移動
    STATE_CORNER,           //!< 曲がり角
    STATE_TURN_BEFORE_EX,   //!< 固有前旋回
    STATE_IDLE,             //!< 固有モーション再生
    STATE_TURN_AFTER_EX,    //!< 固有後旋回
    STATE_WALKSTAY,         //!< 移動できない
    STATE_WAIT,             //!< エンカウント後

    STATE_MAX,
  }; // enum State

  // 各状態でのプロセス
  void MainProcess_StateNone( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateWalk( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateCorner( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateTurnBeforeEx( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateIdle( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateTurnAfterEx( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateWalkStay( FieldMoveModel *pModel, Work *pWork ) const;
  void MainProcess_StateWait( FieldMoveModel *pModel, Work *pWork ) const;

  /**
   *  @brief  演出点の算出
   */
  const Field::Traffic::PathPointData *CalcPathPointData( Field::FieldTrainerActor *pTrainerActor, f32 fNowProgress, f32 fOldProgress ) const;

  /**
   *  @brief  アニメーション変更
   */
  void ChangeAnimationSmooth( FieldMoveModel *pModel, Work *pWork, u32 animationID, bool bLoop = true, u32 changeFrame = 0 ) const;
  void ChangeAnimation( FieldMoveModel *pModel, Work *pWork, u32 animationID, bool bLoop = true ) const;

  /**
   *  @brief  視線コリジョンの更新
   */
  void UpdateCollisionShapeByJoint( FieldMoveModel *pModel, Work *pWork ) const;

  /**
   *  @brief  パス進捗の保存
   */
  void SaveProgress( Work *pWork ) const ;

#ifdef ENABLE_TEST_EYE_DRAW
  void UpdateEyeModel( FieldMoveModel *pModel, Work *pWork ) const;
#endif // ENABLE_TEST_EYE_DRAW
};

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_MOVECODE_TRAINERPATH_H_INCLUDED__
