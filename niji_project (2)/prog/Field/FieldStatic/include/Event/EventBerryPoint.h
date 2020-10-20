/**
 *  GAME FREAK inc.
 *
 *  @file   EventBerryPoint.h
 *  @brief  きのみイベント
 *  @author Masanori Kanamaru
 *  @date   2015.10.08
 *
 *  きのみを調べてからきのみが消えるまで。
 *
 *  1) ポケモンとのエンカウント判定
 *  1-1) (エンカウントしていたら)バトル
 *  2) きのみを獲得可能個数分抽選し、取得処理。
 */

#if !defined(__EVENT_BERRY_POINT_H__)
#define __EVENT_BERRY_POINT_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameEvent.h"

#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"

GFL_NAMESPACE_BEGIN( Field );

class EventBerryPoint : public GameSys::GameEvent
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( EventBerryPoint );

public:
  /**
   *  @brief きのみイベントの実行
   */
  static GameSys::GameEvent* CallBerryPointEvent(GameSys::GameEventManager* pGameEventManager, const FieldBerryLottedData* pData, const gfl2::math::Vector3* pPosition);

  /**
   *  @brief コンストラクタ
   */
  EventBerryPoint(gfl2::heap::HeapBase* pHeap);

  /**
   *  @brief デストラクタ
   */
  ~EventBerryPoint();

private:
  const gfl2::math::Vector3* pPosition;
  FieldBerryLottedData desc;
  u32 gottenAmount;

  /**
   * @brief 実行に必要なデータの設定
   */
  void SetData(const FieldBerryLottedData* pData);

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   pGameManager       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* pGameManager);


  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   pGameManager       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* pGameManager);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   pGameManager       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* pGameManager);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   pGameManager       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* pGameManager);
};

GFL_NAMESPACE_END( Field );

#endif // !defined(__EVENT_BERRY_POINT_H__)