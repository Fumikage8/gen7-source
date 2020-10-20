/**
 *  GAME FREAK inc.
 *
 *  @file   EventFishingSpot.h
 *  @brief  釣りイベント
 *  @author Masanori Kanamaru
 *  @date   2015.10.26
 *
 */

#if !defined(__EVENT_FISHING_SPOT_H__)
#define __EVENT_FISHING_SPOT_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameEvent.h"

#include "Field/FieldStatic/include/FieldLocation.h"

// 前方宣言
namespace Field
{
  class FieldFishingSpotActor;

  namespace MoveModel
  {
    class FieldMoveModelPlayer;
    class FieldMoveModelManager;
  }
 
  namespace Effect
  {
    class EffectFishingBuoy;
    class EffectExclamation2;
  }
}

GFL_NAMESPACE_BEGIN( Field );

class EventFishingAction : public GameSys::GameEvent
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN(EventFishingAction);

public:
#if PM_DEBUG
  enum DebugStabilizingTargetFlag
  {
    Disable,
    ForceEnount,
    ForceItem,
  };

  static int stabilizingTargetFlag;
#endif

  static GameSys::GameEvent* Call(GameSys::GameEventManager* pGameEventManager, FieldFishingSpotActor* pActor);

  /**
   *  @brief コンストラクタ
   */
  EventFishingAction(gfl2::heap::HeapBase* pHeap);

  /**
   *  @brief デストラクタ
   */
  ~EventFishingAction();

private:
  class PlayerState
  {
  public:
    enum State
    {
      STATE_WAIT,

      STATE_START_CAST,
      STATE_LOADING_ROD,
      STATE_CAST,
      STATE_CASTING,
      STATE_WAIT_WITH_ROD,

      STATE_START_HOOK,
      STATE_START_HOOKING,
      STATE_REELING,

      STATE_PULL_ROD_FAIL,
      STATE_PULL_ROD_FAIL_WAIT,
      STATE_PULL_ROD_SUCCESS,
      STATE_PULL_ROD_SUCCESS_WAIT,
      STATE_PULL_ROD_SUCCESS_END,
      STATE_PULL_ROD_SUCCESS_PACK,
      STATE_PULLING_ROD,

      STATE_START_DISPOSE_ROD,
      STATE_DISPOSE_ROD,
      STATE_DISPOSE_END,
    };

    PlayerState()
      : state(STATE_WAIT)
    {}

    inline void Set(State state);
    inline State Get() const;

    inline void Update(MoveModel::FieldMoveModelPlayer* pPlayer, MoveModel::FieldMoveModelManager* pMoveModelManager);

  private:
    State state;
    inline void ChangeAnimation(MoveModel::FieldMoveModelPlayer* pPlayer, u32 singleAnimationID, u32 rideAnimationID);
    inline u32 GetRodModelID(MoveModel::FieldMoveModelPlayer* pPlayer);
  };

  static const f32 BuoyYOffset;

  FieldFishingSpotActor* pActor;

  Effect::EffectExclamation2* pExclamationIcon;
  Effect::EffectFishingBuoy*  pBuoy;

  PlayerState playerState;

  u32 waitFrame;
  u32 pullFrame;

  u32 elapsedFrame;

  bool isSuccess;

  Location playerLocation;

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

  static u32 LotFrame(f32 minSec, f32 maxSec);

  static const u32 HitPercentage = 50;

  static bool LotHit(GameSys::GameManager* pGameManager);
};

GFL_NAMESPACE_END( Field );

#endif // !defined(__EVENT_FISHING_SPOT_H__)