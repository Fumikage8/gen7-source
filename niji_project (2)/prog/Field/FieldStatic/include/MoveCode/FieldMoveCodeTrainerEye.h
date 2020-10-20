#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeTrainerEye.h
*  @brief  フィールド視線トレーナー用 動作コード
*  @author saita_kazuki
*  @date   2015.06.09
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"


// 前方参照
namespace Field {
  class FieldTrainerActor;
}

namespace Field{ namespace MoveModel {

class FieldMoveCodeTrainerEye : public FieldMoveCodeBase
{
public:

  struct Work
  {
    u32   state;              ///< 状態
    s32   motionID;           ///< 視線モーションのID
    f32   eyeLengthRate;      ///< 視線距離倍率
    f32   eyeHeightRate;      ///< 視線距離倍率
    f32   motionSpeedRate;    ///< モーション速度倍率
    s32   jointHeadIndex;     ///< 頭ジョイントの番号
    s32   locatorEyeIndex;    ///< 視線用ロケーターの番号
    f32   comebackAnimFrame;  ///< 復帰時アニメーションフレーム
    f32   comebackAnimStepFrame;  ///< 復帰時のアニメーション再生速度
    FieldTrainerActor*  pTrainerActor; //!< アクター
    b32   isUseOldEyePos;              //!< 前回の視線の注視点を利用するかどうか
    gfl2::math::Vector3 oldEyePos;     //!< 前回の視線の注視点
    b32   isEncount;          //!< @note 社内BTS #5626の類似 [これからエンカウントする場合はSusspend処理でStepFrameを操作しない]
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeTrainerEye(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeTrainerEye();

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_TRAINER_EYE; }
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  再開
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Resume(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  終了
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  isSave         セーブ時に呼ばれたかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void Comeback(FieldMoveCodeWork* pMoveCodeWork,b32 isLoad) const;


private:

  // アニメーション
  void ChangeAnimationSmooth( FieldMoveModel* pModel, Work* pWork, u32 motionID, f32 stepFrame = 1.0f, bool isLoop = true, u32 changeFrame = 0) const;
  void ChangeAnimation( FieldMoveModel* pModel, Work* pWork, u32 motionID, f32 stepFrame = 1.0f, bool isLoop = true) const;

  // コリジョン
  void UpdateCollisionShapeByJoint( FieldMoveModel* pModel, Work* pWork) const;
  void UpdateCollisionShapeByDisableParameter( FieldMoveModel* pModel) const;

  /**
  *  @brief 視線トレーナーの状態
  */
  enum State
  {
    STATE_NONE,               ///< なし(未初期化)
    STATE_NOT_MOVE_SEARCH,    ///< 探索中(エンカウント可、移動しない)
    STATE_BREAK,              ///< 探索していない(エンカウント不可)
    STATE_WAIT,               ///< 待機(エンカウント後)

    STATE_MAX
  };

  // 各状態メインプロセス
  void MainProcess_StateNone( FieldMoveModel* pModel, Work* pWork) const;
  void MainProcess_StateNotMoveSearch( FieldMoveModel* pModel, Work* pWork) const;
  void MainProcess_StateBreak( FieldMoveModel* pModel, Work* pWork) const;
  void MainProcess_StateWait( FieldMoveModel* pModel, Work* pWork) const;

  typedef void (FieldMoveCodeTrainerEye::*StateFunc)( FieldMoveModel* pModel, Work* pWork) const;
  static const StateFunc m_scStateFunc[ STATE_MAX ];


#ifdef ENABLE_TEST_EYE_DRAW
  void UpdateEyeModel( FieldMoveModel* pModel, Work* pWork) const;
#endif

};  // end of class FieldMoveCodeTrainerEye;
  
}; //end of namespace mmodel
}; //end of namespace fld
