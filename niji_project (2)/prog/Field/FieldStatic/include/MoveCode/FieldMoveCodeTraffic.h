/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMoveCodeTraffic.h
 *  @brief  往来NPC専用動作コード
 *  @author miyachi_soichi
 *  @date   2015/06/26
 */

#if !defined(__FIELD_MOVECODE_TRAFFIC_H__)
#define __FIELD_MOVECODE_TRAFFIC_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// Field
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeBase.h"

GFL_NAMESPACE_BEGIN(Field)
// 前方宣言
class TrafficActor;
class TrafficPath;
GFL_NAMESPACE_BEGIN(MoveModel)

class FieldMoveCodeTraffic : public FieldMoveCodeBase
{
public:
  struct Work
  {
    u32           state;      //!< 状態
    u32           next;       //!< 次の状態予約
    u32           motion_id;  //!< 予約時のモーション
    f32           life;       //!< 時間
    f32           delta_rot;  //!< 角速度
    f32           motionSpeedRatio; //!< モーション再生速度
    TrafficActor  *actor;     //!< 演者
    TrafficPath   *path;      //!< パス
  }; // struct Work
  enum State
  {
    STATE_NONE,           //!< 未初期化
    STATE_MOVE_FORWORD,   //!< すすむ
    STATE_STAY_FORWORD,   //!< すすむ待機
    STATE_MOVE_BACKWORD,  //!< もどる
    STATE_STAY_BACKWORD,  //!< もどる待機
    STATE_GOAL,           //!< 到着後処理
    STATE_SUICIDE,        //!< 自殺待ち
    STATE_EMOTE_ROT,      //!< エモート前回転
    STATE_EMOTE,          //!< エモート
    STATE_ROTATE,         //!< 回転
    STATE_ROTATE_QUICK,   //!< 高速回転

    STATE_MAX       //!< 最大
  }; // enum State

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldMoveCodeTraffic( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveCodeTraffic( void );

  /**
   *  @brief  対応している動作コードの取得
   *  @return 動作コード
   */
  virtual FIELD_MOVE_CODE_ID GetMVCode( void ) const { return FIELD_MOVE_CODE_TRAFFIC; }

  /**
   *  @brief  動作コード処理　初期化
   */
  virtual void InitProcess( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理　メイン
   */
  virtual void MainProcess( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理　再開
   */
  virtual void Resume( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理　中断
   */
  virtual void Suspend( FieldMoveCodeWork *pMoveCodeWork ) const;
  /**
   *  @brief  動作コード処理  復帰
   */
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const;

private:
  /**
   *  @brief  状態処理：未初期化
   */
  void MainProcess_StateNone( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：すすむ
   */
  void MainProcess_StateMoveForword( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：到着後
   */
  void MainProcess_StateGoal( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：自殺待ち
   */
  void MainProcess_StateSuicide( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：すすむ待機
   */
  void MainProcess_StateStayForword( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：もどる
   */
  void MainProcess_StateMoveBackword( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：もどる待機
   */
  void MainProcess_StateStayBackword( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：エモート前回転
   */
  void MainProcess_StateEmoteRot( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：エモート
   */
  void MainProcess_StateEmote( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：回転
   */
  void MainProcess_StateRotate( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  状態処理：高速回転
   */
  void MainProcess_StateRotateQuick( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  反転設定
   */
  void SetReverse( FieldMoveModel *pMoveModel, Work *pWork ) const;
  /**
   *  @brief  プレイヤー方向を向く設定
   */
  void CalcRotationPlayer( FieldMoveModel *pMoveModel, Work *pWork ) const;

  /**
   *  @brief  話しかけよう復帰処理
   */
  void ResumeTalker( FieldMoveCodeWork *pMoveCodeWork ) const;

}; // class FieldMoveCodeTraffic : public FieldMoveCodeBase

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)

#endif // !defined(__FIELD_MOVECODE_TRAFFIC_H__)
