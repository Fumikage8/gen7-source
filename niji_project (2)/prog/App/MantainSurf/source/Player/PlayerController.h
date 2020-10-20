//======================================================================
/**
 * @file PlayerController.h
 * @date 2016/09/13 12:20:58
 * @author fang_yicheng
 * @brief プレイヤーコントローラー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __PLAYER_CONTROLLER_H_INCLUDED__
#define __PLAYER_CONTROLLER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "../UI/SurfUI.h"
#include "PlayerModelSet.h"
#include "../../include/MantainSurfAppData.h"
#include "../Nami/NamiController.h"
#include "../Skill/SkillSelector.h"
#include "../SurfSound/SurfSoundManager.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Player)

class PlayerController
{
  GFL_FORBID_COPY_AND_ASSIGN(PlayerController);

public:

  // プレイヤーの状態
  enum State
  {
    ST_NONE = -1,               // なし
    ST_PADDLING,                // パドリング
    ST_WAIT,                    // 待機
    ST_WALK,                    // 歩く
    ST_RUN,                     // 走る
    ST_ACCEL,                   // サーフ状態
    ST_AERIAL_START,            // エアリアル開始状態
    ST_AERIAL_NORMAL,           // エアリアル待機状態
    ST_AERIAL_SKILL,            // エアリアル技発動状態
    ST_AERIAL_MISS,             // エアリアル着水失敗状態
    ST_TURN_OVER,               // 転倒状態
    ST_SHAKY,                   // よろめき
    ST_MAX,
  };

  // サーフ状態
  enum SurfState
  {
    SURF_STATE_NONE = -1,
    SURF_STATE_NORMAL,     // 通常状態
    SURF_STATE_ACCEL,      // 加速状態
    SURF_STATE_TURN_ACCEL, // ターン状態
    SURF_STATE_MAX,
  };

  // オートプレイ状態
  enum AutoPlayState
  {
    AUTO_PLAY_ST_NONE = -1,
    AUTO_PLAY_ST_MOVE_UP,      // 波上りへ移動中
    AUTO_PLAY_ST_MOVE_DOWN,    // 波下りへ移動中
    AUTO_PLAY_ST_AERIAL,       // エアリアル中
  };

  // エアリアル技Z回転補間方式
  enum SkillRotZBlendType
  {
    SKILL_ROT_Z_BLEND_TYPE_LINEAR,    // 線形
    SKILL_ROT_Z_BLEND_TYPE_CURVE0,    // カーブ0(急→緩)
    SKILL_ROT_Z_BLEND_TYPE_CURVE1,    // カーブ1(緩→急)
    SKILL_ROT_Z_BLEND_TYPE_CURVE2,    // カーブ1(緩→急→緩)
    SKILL_ROT_Z_BLEND_TYPE_MAX,
  };

  // サーフパラメータ
  struct AccelParam
  {
    SurfState m_stateNow;
    SurfState m_stateNext;
    bool m_isAccel;               // 加速状態か
    bool m_isKeepHighSpeed;       // 速度を保持状態か
    bool m_isControlProhibited;   // 操作禁止中か
    bool m_isBottomTurnEnable;    // ボトムターン可能か
    f32  m_accelTimer;            // 加速状態に入った時間
    f32  m_accelForbiddonTimer;   // 加速操作禁止時間
    f32  m_topSpeedBonus;         // 最大速度を超えたあとのボーナス値
    f32  m_speedDownTimer;        // 減速タイマー
    f32  m_speedDownStartSpeed;   // 減速時の開始速度
    f32  m_speedDownTargetSpeed;  // 減速時の目標速度
    f32  m_turnAngle;             // 回転角度(deg)
    f32  m_turnRate;              // 角度の回転具合(0.0(右)～1.0(左))
    f32  m_turnSpeed;             // 回転速度
    f32  m_bottomTurnRadiusMag;   // ボトムターン時付加の回転半径の倍率
    f32  m_bottomTurnSpeed;       // ボトムターン時の付加速度
    s32  m_prevSurfAnimeId;       // 前フレームのサーフモーションID
    u32  m_timerOnWave;           // 波の上に居る時間
    u32  m_timerOffWave;          // 波の上に以内時間

    AccelParam()
    {
      m_stateNow = SURF_STATE_NONE;
      m_stateNext = SURF_STATE_NONE;
      m_isAccel = false;
      m_isKeepHighSpeed = false;
      m_isControlProhibited = false;
      m_accelTimer = 0.0f;
      m_accelForbiddonTimer = 0.0f;
      m_topSpeedBonus = 0.0f;
      m_speedDownTimer = 0.0f;
      m_speedDownStartSpeed = 0.0f;
      m_speedDownTargetSpeed = 0.0f;
      m_turnAngle = 0.0f;
      m_turnRate = 0.0f;
      m_turnSpeed = 0.0f;
      m_isBottomTurnEnable = false;
      m_bottomTurnRadiusMag = 0.0f;
      m_bottomTurnSpeed = 0.0f;
      m_prevSurfAnimeId = -1;
      m_timerOnWave = 0;
      m_timerOffWave = 0;
    }
  };

  // エアリアルパラメータ
  struct AerialParam
  {
    f32   m_startVelocity;
    f32   m_endVelocity;
    f32   m_fowardVelocity;            // 飛んだ時の前進速度
    f32   m_accel;                     // ジャンプ時の加(減)速度
    f32   m_procTimer;
    f32   m_missTimer;
    f32   m_seMaxPitch;
    s32   m_waitTimer;                 // エアリアル待機タイマー
    s32   m_passedFrame;               // 技入力されてから経過したフレーム数
    u32   m_totalScore;                // 一回のエアリアル中に決めた技の総得点
    u32   m_totalScorePrev;            // 前回のエアリアル中に決めた技の総得点
    u32   m_skillZRotBlendIndex;       // 技Z回転ブレンド用インデックス
    bool  m_isUseSkillZRotBlend;       // 技のZ回転ブレンド使うか
    bool  m_isSkillWillLanding;        // 今回技発動後に着水するか
    gfl2::math::Vector3 m_missPoint;   // 着水失敗時のカメラ注視点
    MantainSurfAppData::SkillId      m_skillIdNow;
    MantainSurfAppData::SkillId      m_skillIdNext;
    Skill::SkillSelector::SkillParam m_skillParam;

    AerialParam()
    {
      m_startVelocity = 0.0f;
      m_endVelocity = 0.0f;
      m_fowardVelocity = 0.0f;
      m_accel = 0.0f;
      m_procTimer = 0.0f;
      m_missTimer = 0.0f;
      m_waitTimer = 0;
      m_seMaxPitch = 0.0f;
      m_passedFrame = 0;
      m_totalScore = 0;
      m_totalScorePrev = 0;
      m_skillZRotBlendIndex = 0;
      m_isUseSkillZRotBlend = true;
      m_missPoint = gfl2::math::Vector3::GetZero();
      m_skillIdNow = MantainSurfAppData::SKILL_ID_NONE;
      m_skillIdNext = MantainSurfAppData::SKILL_ID_NONE;
    }
  };

public:

  static const f32 ACCEL_FORWARD;
  static const f32 TURN_SPEED_UPWARD;
  static const f32 TURN_SPEED_DOWNWARD;
  static const f32 TURN_ANGLE_UPWARD;
  static const f32 TURN_ANGLE_DOWNWARD;
  static const f32 TURN_FACTOR_UPWARD;
  static const f32 TURN_FACTOR_DOWNWARD;
  static const f32 TURN_RADIUS_UPWARD;
  static const f32 TURN_RADIUS_DOWNWARD;
  static const f32 TURN_SPEED_ACCEL;
  static const f32 TURN_ACCEL_RADIUS_MAG;
  static const f32 TURN_ACCEL_RADIUS_MAG_START;
  static const f32 LANDING_CONTROL_AGILITY;

  static const f32 JUMP_SPEED_MIN[];
  static const f32 ACCEL_AERIAL;

  static const f32 SCORE_RATE_BASE;
  static const f32 SCORE_RATE_INPUT;
  static const f32 SCORE_RATE_SPEED;
  static const f32 SCORE_RATE_LANDING;

  static const f32 SKILL_Z_ROTATION_BLEND_TABLE_EX04[];
  static const f32 SKILL_Z_ROTATION_BLEND_TABLE_EX05[];
  static const f32 SKILL_Z_ROTATION_BLEND_TABLE_EX06[];
  static const f32 SKILL_Z_ROTATION_BLEND_TABLE_EX07[];

public:

  PlayerController();
  virtual ~PlayerController();

  void Initialize(MantainSurfAppManager* pAppManager, PlayerModelSet* pPlayerModel);
  void Update();
  void AdjustPlayerPosition();

  void SetState(State state);

  bool IsAccel();
  bool IsSurfingState();
  bool IsLandingState();
  bool IsAerialState();
  bool IsJumpOverTop();
  bool IsExSkillFinished();
  bool IsLandingSucceeded();

  f32 GetSpeedRate();
  f32 CalcSurfSEPitchLevel();
  f32 GetBaseSpeed();
  f32 GetCurrentSpeed();
  PlayerModelSet* GetPlayerModelSet();

  State               GetState();
  gfl2::math::Vector3 GetVelocity();

  void ResetZPosition(const f32 resetLength);
  void ResetPlayerState(bool isEnd);
  void SaveEndPosition();

  const AccelParam*  GetAccelParam();
  const AerialParam* GetAerialParam();

  bool GetSkillZRotationBlendTable(MantainSurfAppData::SkillId skillId, const f32 **ppBlendTableOut, u32 *pKeyFrameNumOut);

  u32  GetShowScoreRestTime() { return m_showScoreRestTime; }

  Skill::SkillSelector *GetSkillSelector() { return &m_SkillSelector; }

#if PM_DEBUG
  u32  GetSkillCounter() { return m_skillCounter; }
#endif

private:

  void UpdateInput();
  void UpdateState();
  void UpdatePosition();
  void UpdateDashEffect();
  void UpdateCameraTracing();
  void UpdateForEnding();
  void UpdateAerialInput();

  void StateCheckChange();
  void StateInitialize();
  void StateExecute();

  void ExecutePaddling();
  void ExecuteMove();
  void ExecuteAccel();
  void ExecuteAccelSub();
  void ExecuteSurfNormal();
  void ExecuteSurfAccel();
  void ExecuteSurfTurnAccel();
  void ExecuteAerialStart();
  void ExecuteAerialNormal();
  void ExecuteAerialSkill();
  void ExecuteAerialMiss();
  void ExecuteTurnOver();
  void ExecuteShaky();

  gfl2::math::Vector3 GetInputDirection(f32 fInputX, f32 fInputY);
  gfl2::math::Vector3 MakeDownwardMoveVec();

  bool CheckAerialSkill();
  bool CheckLanding(const gfl2::math::Vector3 &playerPosition);
  bool IsGoDown(gfl2::math::Vector3 lookDirection);

  f32 CalcVecAngleDeg(gfl2::math::Vector3 fromVec, gfl2::math::Vector3 toVec, gfl2::math::Vector3 fromVecRight);
  f32 CalcVelocityDeltaTime(f32 vStart, f32 vTarget, f32 acc);
  f32 CalcMoveLengthDeltaTime(f32 vStart, f32 acc);
  f32 CalcValueByStep(f32 curValue, f32 targetValue, f32 step);
  f32 CalcLandingVelocity(f32 minSpeed, f32 maxSpeed);
  f32 CalcSpeedLevel();
  gfl2::math::Vector3 CalcJumpMoveVect(u32 frame);

  u32 CalcSkillScore();

  s32 GetSpeedLevel();
  
  void StartSkillSuccessEffect();
  void PlaySkillStartSE();

  SurfSound::SurfSoundManager::SurfSEIndex GetSurfSEIndex();

  void SkillZRotationBlend(MantainSurfAppData::SkillId skillId, f32 curAnimationFrame);
  void RegisterSkillResult(MantainSurfAppData::SkillId skillId, u32 score);

  void OnJumpStart();
  void OnJumpEnd();

#if PM_DEBUG
  void UpdateAutoPlayState();
  u32  RandomSelectSkill();
#endif

private:

  MantainSurfAppManager* m_pAppManager;

  PlayerModelSet* m_pPlayerModel;

  State m_stateNow;     // 現在の状態
  State m_stateNext;    // 次の状態

  AutoPlayState m_autoPlayStateNow;
  AutoPlayState m_autoPlayStateNext;

  // 状態のシーケンス
  enum StateSeq
  {
    SEQ_START = 0,
    SEQ_END = -1,
  };
  s32 m_stateSeq;

  gfl2::math::Vector3 m_dirInput;     // 入力の方向
  gfl2::math::Vector3 m_dirMove;      // プレイヤーの移動方向
  gfl2::math::Vector3 m_velocity;     // プレイヤーの速度(x=横移動速度 z=前進速度 y=ジャンプ速度)
  gfl2::math::Vector3 m_moveVec;      // 移動量
  gfl2::math::Vector3 m_prevMoveDir;  // 前フレームの移動方向
  gfl2::math::Vector3 m_endPosition;  // 終了演出時の位置

  f32 m_turnRateWalk;
  f32 m_turnRateRun;
  
  Nami::NamiController::NamiPointInfo m_nowNamiInfo;

  Skill::SkillSelector                m_SkillSelector;

  bool m_isAccel;
  bool m_isJump;

  struct Input
  {
    bool m_isHoldRunButton;     // 走る(B)ボタンが押されているか
    bool m_isAccelButtonDown;   // 加速(A)ボタンが押下されているか
    bool m_isRotationMode;      // 回転(ZR)モードであるか
    bool m_isReturnNeutral;     // 前の入力からスティックがニュートラルに戻ったか
    f32  m_fStickXVal;          // 左スティックX軸の入力量
    f32  m_fStickYVal;          // 左スティックY軸の入力量

    Input()
    {
      Initialize();
      m_isReturnNeutral = false;
    }

    void Initialize()
    {
      m_isHoldRunButton   = false;
      m_isAccelButtonDown = false;
      m_isRotationMode    = false;
      m_fStickXVal        = 0.0f;
      m_fStickYVal        = 0.0f;
    }
  };
  Input m_Input;    // 入力情報

  struct TurnAccelInfo
  {
    gfl2::math::Vector3 m_targetDir;    // 目標向き
  };
  TurnAccelInfo m_TurnAccelInfo;

  AccelParam  m_AccelParam;
  AerialParam m_AerialParam;

  // 転倒のパラメータ
  struct TurnOverParam
  {
    gfl2::math::Vector3 m_moveDirection;          // 移動方向
    f32                 m_turnOverFrame;          // 転倒所要時間
    f32                 m_initVelocityX;          // X軸移動初速度
    f32                 m_timer;                  // 転倒処理用タイマー
    ModelBase*          m_pTargetModel;           // 衝突対象

    TurnOverParam()
    {
      m_moveDirection = gfl2::math::Vector3::GetZero();
      m_turnOverFrame = 0.0f;
      m_timer         = 0.0f;
      m_initVelocityX = 0.0f;
      m_pTargetModel  = NULL;
    }
  };
  TurnOverParam m_TurnOverParam;

  // よろめきパラメータ
  struct ShakyParam
  {
    f32  m_startVelocity;       // よろめき開始時の前進速度(cm/s)
    f32  m_startRotationY;      // よろめき開始時のY軸回転(rad)
    f32  m_roationAngleY;       // よろめきのY軸角度回転値(rad)
    f32  m_turnAngleSpeed;      // よろめきの回転角速度(deg/s)
    f32  m_duration;            // よろめき復帰所要時間(フレーム)
    f32  m_timer;               // 処理タイマー(フレーム)
    f32  m_safeTimer;           // 短時間内の同じ対象と連続して衝突するのを抑える用タイマー(秒)
    s32  m_shakyCount;          // 連続よろめき回数
    ModelBase* m_pTargetModel;  // 衝突対象

    ShakyParam()
    {
      m_startVelocity = 0.0f;
      m_startRotationY = 0.0f;
      m_roationAngleY = 0.0f;
      m_turnAngleSpeed = 0.0f;
      m_duration = 0.0f;
      m_timer = 0.0f;
      m_safeTimer = 0.0f;
      m_shakyCount = 0;
      m_pTargetModel = NULL;
    }
  };
  ShakyParam m_ShakyParam;

  u32 m_showScoreRestTime;        // スコア表示の残り時

#if PM_DEBUG
  // オートプレイ操作パラメータ
  struct AutoPlayParam
  {
    f32  m_targetHeight;      // 目標の波高さ
    f32  m_aerialTotalFrame;  // エアリアル状態の滞空時間
    s32  m_skillIndex;        // 技番号
    s32  m_skillCmdIndex;     // 技コマンド番号
  };
  AutoPlayParam m_AutoPlayParam;

  u32 m_skillCounter;         // 1回のプレイで出した技の回数
#endif
};

GFL_NAMESPACE_END(Player)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __PLAYER_CONTROLLER_H_INCLUDED__
