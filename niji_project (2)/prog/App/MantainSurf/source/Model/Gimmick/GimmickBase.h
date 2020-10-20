//======================================================================
/**
 * @file GimmickBase.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief ベース障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_BASE_H_INCLUDED__
#define __GIMMICK_BASE_H_INCLUDED__
#pragma once

#include "../ModelBaseChara.h"
#include "../../Nami/NamiController.h"
#include "../../Player/PlayerController.h"
#include "../../Camera/Camera.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickBase : public ModelBaseChara
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickBase);

public:

  enum Type
  {
    TYPE_NONE = -1,
    TYPE_COMMON,      // 通用型
    TYPE_REEF_S,      // 岩礁（小）
    TYPE_REEF_M,      // 岩礁（中）＋ナマコブシ
    TYPE_REEF_L,      // 岩礁（大）＋サニーゴ
    TYPE_MENOKURAGE,  // メノクラゲ
    TYPE_HOERUKO,     // ホエルコ
    TYPE_SAMEHADER,   // サメハダー
    TYPE_HOERUOH,     // ホエルオー
    TYPE_MAX,
  };

  enum State
  {
    ST_NONE = -1,
    ST_APPEAR,      // 出現状態
    ST_WAIT,        // 待機状態
    ST_ACTION,      // アクション状態
    ST_INACTIVE,    // 非活動状態
    ST_DEAD,        // 消滅
    ST_MAX
  };

public:

  static f32 MakeRandRate();

public:

  GimmickBase();
  virtual ~GimmickBase();

  virtual void Update();
  virtual void UpdateTimeZone(s32 timeZone) {};

  virtual gfl2::math::Vector3 GetBodyPosition() { return GetPosition(); }

  Type GetType();

  void SetController(Nami::NamiController* pNamiController, Player::PlayerController* pPlayerController);
  void SetCamera(Camera::Camera* pCamera);
  void Reset(const gfl2::math::Vector3 &position);
  void Reset();

  void SetDrawEnable(bool isDrawEnable, u32 delay);

  void SetDead();
  bool IsDead();

  void SetState(const State state);

  bool IsActionState();
  bool IsTurnOver();

  f32  GetPenaltyRatio();

#if PM_DEBUG
  gfl2::math::Vector3 GetInitPosition() { return m_initPosition; }
  void SetInitPosition(const gfl2::math::Vector3 &position) { m_initPosition = position; }
#endif

protected:

  virtual void OnSetDead() {}

  gfl2::math::Vector3 GetPlayerPosition();
  f32                 GetPlayerSpeedRate();
  ModelBase*          GetPlayerCollidedModel();

  void UpdateSEPlayEnv(SurfSound::SurfSoundManager::SEIndex seIndex, const gfl2::math::Vector3 &position);

private:

  void UpdateState();
  void UpdateDrawableState();

  void StateCheckChange();
  void StateInitialize();
  void StateExecute();

  void OnStateAppearInitialize(State prevState);
  void OnStateAppearExecute();

protected:

  virtual void OnStateWaitInitialize(State prevState) {};
  virtual void OnStateWaitExecute() {};
  virtual void OnStateActionInitialize(State prevState) {};
  virtual void OnStateActionExecute() {};
  virtual bool CheckActionTrigger() { return false; };

protected:

  Type  m_type;

  State m_stateNow;     // 現在の状態
  State m_stateNext;    // 次の状態

  bool  m_isSyncWithWave;     // 波の高さと傾きに同期するか
  bool  m_isRandomDirection;  // 向きをランダムするか

  Nami::NamiController*     m_pNamiController;
  Player::PlayerController* m_pPlayerController;
  Camera::Camera*           m_pCamera;

  gfl2::math::Vector3 m_initPosition;

private:

  enum StateSeq
  {
    SEQ_END = -1,
  };
  s32 m_stateSeq;

  struct StateParam
  {
    f32 m_procTimer;

    StateParam()
    {
      m_procTimer = 0.0f;
    }
  };
  StateParam m_stateParam;

  struct DrawableState
  {
    bool isStateWillChange;    // 描画状態が変更しようとしているか
    s32  stateNext;            // 次に変更する描画状態(-1:初期状態 0:描画しない 1以上:描画する)
    u32  delayTimer;           // 描画状態変更の遅延フレーム数

    DrawableState()
    {
      Initialize();
    }

    void Initialize()
    {
      isStateWillChange = false;
      stateNext = -1;
      delayTimer = 0;
    }
  };
  DrawableState m_drawableState;
};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_BASE_H_INCLUDED__
