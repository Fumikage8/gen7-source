//======================================================================
/**
 * @file PlayerController.cpp
 * @date 2016/09/13 12:20:41
 * @author fang_yicheng
 * @brief プレイヤーコントローラー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "PlayerController.h"
#include "../MantainSurfAppManager.h"
#include <GameSys/include/GameManager.h>

#include <ui/include/gfl2_UI_Button.h>
#include <math/include/gfl2_math_control.h>

using namespace App::MantainSurf::SurfSound;

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Player)

// 入力
const f32 VALID_INPUT_STICK_VAL = 0.2f;       // 有効な入力量
const f32 INPUT_STICK_VAL_MIN   = 0.5f;       // 入力量の最小値
const f32 INPUT_STICK_VAL_MAX   = 0.8f;       // 入力量最大と判定する入力量
const f32 NEUTRAL_STICK_VAL     = 0.4f;       // ニュートラルと判定する入力量

// 歩く／走る
const f32 SPEED_WALK_MAX  = 10.0f * 100.0f;   // 歩く速度の最大値
const f32 SPEED_RUN_MAX   = 25.0f * 100.0f;   // 走る速度の最大値
const f32 ACCEL_WALK      = 20.0f * 100.0f;   // 歩く加速度
const f32 ACCEL_RUN       = 50.0f * 100.0f;   // 走る加速度

// 前進速度
const u32 SPEED_LEVEL_MAX             = 4;                    // 速度の段階
const f32 SPEED_MAX[SPEED_LEVEL_MAX]  = { 10.0f * 100.0f, 15.0f * 100.0f, 20.0f * 100.0f, 25.0f * 100.0f };
const f32 SPEED_MAX_BASE              = SPEED_MAX[0];
const f32 SPEED_MAX_FORWARD           = SPEED_MAX[SPEED_LEVEL_MAX - 1];     // 前進の最大速度(cm/s)

// ジャンプ速度
const f32 PlayerController::JUMP_SPEED_MIN[SPEED_LEVEL_MAX] = { 710.0f, 2720.0f, 4250.0f, 6300.0f };
const f32 PlayerController::ACCEL_AERIAL = 23.0f * 100.0f;     // エリアル時の加速度(cm/s^2)

const f32 SPEED_MIN_JUMP = PlayerController::JUMP_SPEED_MIN[0];                       // ジャンプ開始時の最小速度(cm/s)
const f32 SPEED_MAX_JUMP = PlayerController::JUMP_SPEED_MIN[SPEED_LEVEL_MAX - 1];     // ジャンプ開始時の最大速度(cm/s)

// 最小/最大速度
const f32 SPEED_MAX_L             =  6.0f * 100.0f;     // 左移動の最大速度(cm/s)
const f32 SPEED_MAX_R             =  8.0f * 100.0f;     // 右移動の最大速度(cm/s)
const f32 SPEED_MAX_GRAVITY       =  6.0f * 100.0f;     // 重力の最大速度(cm/s)
const f32 SPEED_MIN_LANDING       =  4.0f * 100.0f;     // 着水時のX軸移動最小スピード
const f32 SPEED_MAX_LANDING       = 28.0f * 100.0f;     // 着水時のX軸移動最大スピード
const f32 SPEED_MIN_LANDING_MISS  = 10.0f * 100.0f;     // 着水ミス時のY軸移動最小スピード
const f32 SPEED_MAX_LANDING_MISS  = 15.0f * 100.0f;     // 着水ミス時のY軸移動最大スピード

// 加速度
const f32 ACCEL_BASE           =  8.0f * 100.0f;     // 前進の基本加速度(cm/s^2)
const f32 ACCEL_SPEED_DOWN     =  5.0f * 100.0f;     // スピードダウン時の減速度(cm/s^2)
const f32 ACCEL_L              = 18.0f * 100.0f;     // 左移動の加速度(cm/s^2)
const f32 ACCEL_R              = 20.0f * 100.0f;     // 右移動の加速度(cm/s^2)
const f32 ACCEL_GRAVITY        = 24.0f * 100.0f;     // 重力加速度(cm/s^2)
const f32 ACCEL_TURN_OVER      = 10.0f * 100.0f;     // 障害物衝突時の加(減)速度(cm/s^2)
const f32 ACCEL_LANDING_MISS_Y = 10.0f * 100.0f;     // 着水失敗時のY軸加(減)速度(cm/s^2)
const f32 ACCEL_LANDING_MISS_Z = 40.0f * 100.0f;     // 着水失敗時のY軸加(減)速度(cm/s^2)

// 着水失敗
const f32 AERIAL_MISS_MIN_LENGTH = 400.0f;
const f32 AERIAL_MISS_MAX_LENGTH = 770.0f;
const f32 AERIAL_MISS_MIN_TIME = 0.7f;
const f32 AERIAL_MISS_MAX_TIME = 1.2f;

// 角度
const f32 TURN_ANGLE_MAX_L    = 45.0f;   // 左移動時向きの最大回転角度
const f32 TURN_ANGLE_MAX_R    = 30.0f;   // 右移動時向きの最大回転角度
const f32 TURN_ANGLE_MIN_JUMP = 30.0f;   // ジャンプ時の最小回転角度
const f32 TURN_ANGLE_MAX_JUMP = 75.0f;   // ジャンプ時の最大回転角度

// 角度補間率
const f32 TURN_RATE_WALK    = 0.5f;   // 向き回転時の補完率(歩く)
const f32 TURN_RATE_RUN     = 0.25f;  // 向き回転時の補完率(走る)
const f32 TURN_RATE_ACCEL   = 0.15f;  // 向き回転時の補完率(加速)
const f32 TURN_RATE_DOWN    = 0.01f;  // 向き回転時の補完率(下り)

// タイマー
const f32 ACCEL_START_TIME        = 0.2f;   // 加速開始時間(s)
const f32 ACCEL_FORBIDDON_TIME    = 0.6f;   // 加速操作禁止時間(s)
const f32 AERIAL_MISS_TIME        = 0.2f;   // エアリアル着水失敗の処理時間(s)
const f32 SPEED_DOWN_TIME_MIN     = 2.0f;   // 減速タイマー最小値(s)
const f32 SPEED_DOWN_TIME_MAX     = 6.0f;   // 減速タイマー最大値(s)
const f32 TURN_OVER_RECOVER_TIME  = 0.5f;   // 転倒から復帰までの所要時間(s)
const f32 SHAKY_SAFE_TIME         = 0.2f;   // よろめき安全期間(s)
const u32 SCORE_DIPLAY_TIME       = 60;     // スコアの表示時間

// エフェクト
const f32 JET_EFFECT_SCALE_CHANGE_RATE = 0.2f; // ジェットエフェクトスケールの変化率

// 加速度
const f32 PlayerController::ACCEL_FORWARD = 240.0f;    // 前進加速度(cm/s^2)

// 回転パラメータ
const f32 PlayerController::TURN_RADIUS_UPWARD               = 200.0f;    // 回転半径(上り)
const f32 PlayerController::TURN_RADIUS_DOWNWARD             = 200.0f;    // 回転半径(下り)
const f32 PlayerController::TURN_SPEED_UPWARD                = 600.0f;    // 回転速度(上り)
const f32 PlayerController::TURN_SPEED_DOWNWARD              = 600.0f;    // 回転速度(下り)
const f32 PlayerController::TURN_ANGLE_UPWARD                = -90.0f;    // 回転角度(上り)
const f32 PlayerController::TURN_ANGLE_DOWNWARD              = 90.0f;     // 回転角度(下り)
const f32 PlayerController::TURN_FACTOR_UPWARD               = 1.0f;      // 回転のやすさ(上り)
const f32 PlayerController::TURN_FACTOR_DOWNWARD             = 1.0f;      // 回転のやすさ(下り)
const f32 PlayerController::TURN_SPEED_ACCEL                 = 600.0f;    // 回転速度の加(減)速度
const f32 PlayerController::TURN_ACCEL_RADIUS_MAG            = 1.1f;      // ボトムターン加速時間の獲得倍率
const f32 PlayerController::TURN_ACCEL_RADIUS_MAG_START      = 0.5f;      // ボトムターン可能な回転半径倍率の閾値
const f32 PlayerController::LANDING_CONTROL_AGILITY          = 2.0f;      // 着水操作の機敏性

// 得点パラメータ
const f32 PlayerController::SCORE_RATE_BASE    = 0.5f;   // 確定得点係数
const f32 PlayerController::SCORE_RATE_INPUT   = 0.4f;   // 技入力の得点係数
const f32 PlayerController::SCORE_RATE_SPEED   = 0.1f;   // 自機速度の得点係数
const f32 PlayerController::SCORE_RATE_LANDING = 0.07f;  // 着水成功ボーナス係数

// 技入力
const f32 MAX_SKILL_FRAME_ERROR             = 15.0f;  // 技発動フレームの最大誤差
const s32 SKILL_END_EFFECT_FRAME_THREDSHOLD = 16;     // 技終了エフェクトの再生タイミング

// SE
const f32 SE_MIN_PITCH_AERIAL_NORMAL = 1.0f;
const f32 SE_MAX_PITCH_AERIAL_NORMAL = 1.5f;
const f32 SURF_SE_END_MAX_DISTANCE   = 12000.0f;   // 終了演出時サーフSEが消えるまでの最大走行距離

// モーション
const u32 SURF_RIGHT_MOTION_SWITCH_TIME_OFF_TO_ON = 8;    // 右移動モーションの切り替え時間(海面→波の上)
const u32 SURF_RIGHT_MOTION_SWITCH_TIME_ON_TO_OFF = 32;   // 右移動モーションの切り替え時間(波の上→海面)

// 技発動時のZ回転ブレンドテーブル(アシレーヌフリップ用)(モーション長さ：34フレーム)
const f32 PlayerController::SKILL_Z_ROTATION_BLEND_TABLE_EX04[] = {
  // キーフレーム
  0.0f,                          6.0f,                          30.0f,                         34.0f,
  // Z軸回転値
  -90.0f,                        0.0f,                          0.0f,                          -90.0f,
  // 補間方式
  SKILL_ROT_Z_BLEND_TYPE_CURVE0, SKILL_ROT_Z_BLEND_TYPE_LINEAR, SKILL_ROT_Z_BLEND_TYPE_CURVE2, SKILL_ROT_Z_BLEND_TYPE_LINEAR,
};

// 技発動時のZ回転ブレンドテーブル(オーバーザギャラドス用)(モーション長さ：45フレーム)
const f32 PlayerController::SKILL_Z_ROTATION_BLEND_TABLE_EX05[] = {
  // キーフレーム
  0.0f,                          6.0f,                          39.0f,                         45.0f,
  // Z軸回転値
  -90.0f,                        0.0f,                          0.0f,                          -90.0f,
  // 補間方式
  SKILL_ROT_Z_BLEND_TYPE_CURVE0, SKILL_ROT_Z_BLEND_TYPE_LINEAR, SKILL_ROT_Z_BLEND_TYPE_CURVE0, SKILL_ROT_Z_BLEND_TYPE_LINEAR,
};

// 技発動時のZ回転ブレンドテーブル(スターミートルネード用)(モーション長さ：38フレーム)
const f32 PlayerController::SKILL_Z_ROTATION_BLEND_TABLE_EX06[] = {
  // キーフレーム
  0.0f,                          14.0f,                         29.0f,                         36.0f,
  // Z軸回転値
  -90.0f,                        0.0f,                          0.0f,                          -90.0f,
  // 補間方式
  SKILL_ROT_Z_BLEND_TYPE_CURVE0, SKILL_ROT_Z_BLEND_TYPE_LINEAR, SKILL_ROT_Z_BLEND_TYPE_CURVE2, SKILL_ROT_Z_BLEND_TYPE_LINEAR,
};

// 技発動時のZ回転ブレンドテーブル(コイキングスペシャル用)(モーション長さ：30フレーム)
const f32 PlayerController::SKILL_Z_ROTATION_BLEND_TABLE_EX07[] = {
  // キーフレーム
  0.0f,                          6.0f,                          21.0f,                         27.0f,
  // Z軸回転値
  -90.0f,                        0.0f,                          0.0f,                          -90.0f,
  // 補間方式
  SKILL_ROT_Z_BLEND_TYPE_CURVE0, SKILL_ROT_Z_BLEND_TYPE_LINEAR, SKILL_ROT_Z_BLEND_TYPE_CURVE0, SKILL_ROT_Z_BLEND_TYPE_LINEAR,
};

const f32 DELTA_TIME = 1.0f / 30.0f;  // 毎フレームの時間間隔

PlayerController::PlayerController() :
  m_pAppManager(NULL),
  m_pPlayerModel(NULL),
  m_stateNow(ST_NONE),
  m_stateNext(ST_NONE),
  m_autoPlayStateNow(AUTO_PLAY_ST_NONE),
  m_autoPlayStateNext(AUTO_PLAY_ST_MOVE_UP),
  m_stateSeq(0),
  m_showScoreRestTime(0),
  m_turnRateWalk(TURN_RATE_WALK),
  m_turnRateRun(TURN_RATE_RUN),
  m_dirInput(gfl2::math::Vector3(0.0f, 0.0f, 0.0f)),
  m_dirMove(gfl2::math::Vector3(0.0f, 0.0f, 0.0f)),
  m_velocity(gfl2::math::Vector3(0.0f, 0.0f, 0.0f)),
  m_prevMoveDir(gfl2::math::Vector3(0.0f, 0.0f, 0.0f)),
  m_isAccel(true),
  m_isJump(true)
{
#if PM_DEBUG
  m_skillCounter = 0;
#endif
}

PlayerController::~PlayerController()
{
}

void PlayerController::Initialize(MantainSurfAppManager* pAppManager, PlayerModelSet* pPlayerModel)
{
  m_pAppManager = pAppManager;
  m_pPlayerModel = pPlayerModel;
  m_nowNamiInfo = m_pAppManager->GetNamiController()->GetNamiPointInfo(m_pPlayerModel->GetPosition());
}

void PlayerController::Update()
{
  // 入力の更新
  UpdateInput();
#if PM_DEBUG
  if(m_pAppManager->GetDebugMenu()->m_dbvIsAutoPlayMode && !m_pAppManager->IsTutorialMode())
  {
    UpdateAutoPlayState();
  }
#endif

  // エアリアルスキル入力の更新
  UpdateAerialInput();

  // 状態の更新
  UpdateState();

  // プレイヤー位置の更新
  UpdatePosition();

  // ダッシュエフェクトの更新
  UpdateDashEffect();

  // プレイヤーモデルの更新
  m_pPlayerModel->Update();

  // スコア表示時間を更新
  if(m_showScoreRestTime > 0 && !m_isJump)
  {
    m_showScoreRestTime--;
  }
}

void PlayerController::UpdateInput()
{
  // 入力情報を初期化
  m_Input.Initialize();
  m_moveVec = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);

  // 操作可能かをチェック
  if(m_pAppManager->GetGameState() != MantainSurfAppManager::ST_SURFING || m_pAppManager->GetNamiController()->IsFinish())
  {
    // サーフィン状態中以外の場合は操作できない
    return;
  }

  if(m_pAppManager->IsTutorialMode() && !m_pAppManager->GetTutorialManager()->IsControllEnable())
  {
    // チュートリアル中の操作不可状態
    return;
  }

  // 入力情報を更新
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::VectorDevice* pStick = pGameManager->GetUiDeviceManager()->GetStick(0);

#if PM_DEBUG
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && pButton != NULL && pButton->IsHold(gfl2::ui::BUTTON_R) && !IsAerialState())
  {
    // カメラ操作中はプレイヤーを操作できない
    return;
  }
  if(pButton != NULL)
  {
    m_Input.m_isHoldRunButton = pButton->IsHold(gfl2::ui::BUTTON_B);
    m_Input.m_isAccelButtonDown = pButton->IsTrigger(gfl2::ui::BUTTON_X);
  }
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && m_Input.m_isAccelButtonDown && !IsAerialState())
  {
    // サーフ状態切り替え
    m_isAccel = !m_isAccel;
  }
#endif

  if(pStick != NULL)
  {
    m_Input.m_fStickXVal = gfl2::math::Clamp(pStick->GetX(), -1.0f, 1.0f);
    m_Input.m_fStickYVal = gfl2::math::Clamp(pStick->GetY(), -1.0f, 1.0f);
    if(gfl2::math::FAbs(m_Input.m_fStickXVal) < VALID_INPUT_STICK_VAL)
    {
      m_Input.m_fStickXVal = 0.0f;
    }
    if(gfl2::math::FAbs(m_Input.m_fStickYVal) < VALID_INPUT_STICK_VAL)
    {
      m_Input.m_fStickYVal = 0.0f;
    }
  }
  if(!m_Input.m_isReturnNeutral)
  {
    m_Input.m_isReturnNeutral = (gfl2::math::FAbs(m_Input.m_fStickXVal) < NEUTRAL_STICK_VAL &&
                                 gfl2::math::FAbs(m_Input.m_fStickYVal) < NEUTRAL_STICK_VAL);
  }

#if PM_DEBUG
  // 入力の方向
  m_dirInput = GetInputDirection(m_Input.m_fStickXVal, m_Input.m_fStickYVal);
#endif
}

void PlayerController::UpdateAerialInput()
{
  // チュートリアル中にスキル発動不可の段階
  if(m_pAppManager->IsTutorialMode() && !m_pAppManager->GetTutorialManager()->IsAerialSkillEnable()) return;

  // エアリアル状態ではない
  if(!IsAerialState() || m_stateNow == ST_AERIAL_MISS) return;

  if(m_AerialParam.m_skillIdNext != MantainSurfAppData::SKILL_ID_NONE)
  {
    // 既に発動待機中の技が存在
    m_AerialParam.m_passedFrame++;
    return;
  }

  // スティックがニュートラルに戻ってない
  if(!m_Input.m_isReturnNeutral) return;

  // 着水判定
  if(m_stateNow == ST_AERIAL_NORMAL && CheckLanding(m_pPlayerModel->GetPosition()))
  {
    // 現在フレームで着水するため、入力不可
    return;
  }

  if(m_stateNow == ST_AERIAL_SKILL && m_AerialParam.m_isSkillWillLanding)
  {
    // 現在技完了後に着水するため、入力不可
    return;
  }

  // 先行入力タイミングチェック
  if(m_stateNow == ST_AERIAL_SKILL && m_pPlayerModel->GetAnimationEndFrame() - m_pPlayerModel->GetAnimationFrame() > SKILL_END_EFFECT_FRAME_THREDSHOLD)
  {
    // 先行入力タイミング内ではない
    return;
  }

  // 発動スキルの判定
  MantainSurfAppData::SkillId skillId = MantainSurfAppData::SKILL_ID_NONE;
  f32 absX = gfl2::math::FAbs(m_Input.m_fStickXVal);
  f32 absY = gfl2::math::FAbs(m_Input.m_fStickYVal);
  if(absX >= NEUTRAL_STICK_VAL && absX >= absY)
  {
    // 左右ターン
    if(m_Input.m_fStickXVal > 0.0f)
    {
      // 右ターン
      skillId = MantainSurfAppData::SKILL_ID_NORMAL_04;
    }
    else
    {
      // 左ターン
      skillId = MantainSurfAppData::SKILL_ID_NORMAL_03;
    }
  }
  else if(absY >= NEUTRAL_STICK_VAL && absY > absX)
  {
    // 上下ターン
    if(m_Input.m_fStickYVal > 0.0f)
    {
      // 上ターン
      skillId = MantainSurfAppData::SKILL_ID_NORMAL_01;
    }
    else
    {
      // 下ターン
      skillId = MantainSurfAppData::SKILL_ID_NORMAL_02;
    }
  }

#if PM_DEBUG
  // ボタン１つで大技発動デバッグ機能
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && !m_pAppManager->GetDebugMenu()->m_dbvIsAutoPlayMode)
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
    if(pButton->IsHold(gfl2::ui::BUTTON_A))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_01;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_Y))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_02;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_X))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_03;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_B))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_04;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_UP))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_05;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_RIGHT))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_06;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_LEFT))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_06;
    }
    else if(pButton->IsHold(gfl2::ui::BUTTON_DOWN))
    {
      skillId = MantainSurfAppData::SKILL_ID_EX_07;
    }
    if(skillId != MantainSurfAppData::SKILL_ID_NONE && skillId < MantainSurfAppData::SKILL_ID_NORMAL_01)
    {
      m_SkillSelector.Reset();
    }
  }
#endif

  if(skillId != MantainSurfAppData::SKILL_ID_NONE)
  {
    // SE再生
    SurfSoundManager::PlaySE(SurfSoundManager::SE_SKILL_END);
    m_AerialParam.m_skillIdNext = m_SkillSelector.SelectSkill(skillId);
    m_AerialParam.m_passedFrame = 0;
    m_Input.m_isReturnNeutral = false;
  }
}

#if PM_DEBUG

enum SkillCommand { CMD_EMPTY = -1, CMD_UP, CMD_DOWN, CMD_LEFT, CMD_RIGHT, CMD_MAX };

// スキルテーブル
static const SkillCommand SKILL_COMMAND_TABLE[][4] = {
  { CMD_LEFT, CMD_LEFT, CMD_LEFT, CMD_EMPTY },      // スクリューハンテール
  { CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_EMPTY },   // スクリューサクラビス
  { CMD_UP, CMD_DOWN, CMD_UP, CMD_EMPTY },          // ランターン360
  { CMD_DOWN, CMD_DOWN, CMD_DOWN, CMD_EMPTY },      // アシレーヌフリップ
  { CMD_LEFT, CMD_RIGHT, CMD_LEFT, CMD_UP },        // スターミートルネード
  { CMD_RIGHT, CMD_LEFT, CMD_RIGHT, CMD_UP },       // スターミートルネード
  { CMD_UP, CMD_LEFT, CMD_DOWN, CMD_RIGHT },        // オーバーザギャラドス
  { CMD_LEFT, CMD_DOWN, CMD_RIGHT, CMD_UP },        // オーバーザギャラドス
  { CMD_DOWN, CMD_RIGHT, CMD_UP, CMD_LEFT },        // オーバーザギャラドス
  { CMD_RIGHT, CMD_UP, CMD_LEFT, CMD_DOWN },        // オーバーザギャラドス
  { CMD_UP, CMD_RIGHT, CMD_DOWN, CMD_LEFT },        // オーバーザギャラドス
  { CMD_RIGHT, CMD_DOWN, CMD_LEFT, CMD_UP },        // オーバーザギャラドス
  { CMD_DOWN, CMD_LEFT, CMD_UP, CMD_RIGHT },        // オーバーザギャラドス
  { CMD_LEFT, CMD_UP, CMD_RIGHT, CMD_DOWN },        // オーバーザギャラドス
  { CMD_EMPTY, CMD_EMPTY, CMD_EMPTY, CMD_EMPTY },   // なし
};
static const u32 SKILL_COMMAND_TABLE_SIZE = GFL_NELEMS(SKILL_COMMAND_TABLE);

void PlayerController::UpdateAutoPlayState()
{
  m_Input.m_fStickXVal = 0.0f;
  m_Input.m_fStickYVal = 0.0f;

  // 操作可能かをチェック
  if(m_pAppManager->GetGameState() != MantainSurfAppManager::ST_SURFING || m_pAppManager->GetNamiController()->IsFinish())
  {
    // サーフィン状態中以外の場合は操作できない
    return;
  }

  // 状態変化チェック
  switch(m_autoPlayStateNow)
  {
  case AUTO_PLAY_ST_MOVE_UP:
    if(IsAerialState())
    {
      // エアリアル状態へ
      m_autoPlayStateNext = AUTO_PLAY_ST_AERIAL;
    }
    else if(m_velocity.z >= SPEED_MAX_FORWARD)
    {
      // 最大スピードなら何もしない(ジャンプさせる)
    }
    else if(m_pPlayerModel->GetPosition().y >= m_AutoPlayParam.m_targetHeight)
    {
      // ある程度波を登ったら下りへ
      m_autoPlayStateNext = AUTO_PLAY_ST_MOVE_DOWN;
    }
    break;

  case AUTO_PLAY_ST_MOVE_DOWN:
    if(IsAerialState())
    {
      // エアリアル状態へ
      m_autoPlayStateNext = AUTO_PLAY_ST_AERIAL;
    }
    else if(m_pPlayerModel->GetPosition().y <= 0.0f)
    {
      // ある程度波を降ったら上りへ
      m_autoPlayStateNext = AUTO_PLAY_ST_MOVE_UP;
    }
    break;

  case AUTO_PLAY_ST_AERIAL:
    if(!IsAerialState())
    {
      // エアリアル状態終了
      m_autoPlayStateNext = AUTO_PLAY_ST_MOVE_UP;
    }
    break;
  }

  // 状態変化時の初期化
  while(m_autoPlayStateNext != AUTO_PLAY_ST_NONE)
  {
    m_autoPlayStateNow = m_autoPlayStateNext;
    m_autoPlayStateNext = AUTO_PLAY_ST_NONE;

    switch(m_autoPlayStateNow)
    {
    case AUTO_PLAY_ST_MOVE_DOWN:
      break;
    case AUTO_PLAY_ST_MOVE_UP:
      break;
    case AUTO_PLAY_ST_AERIAL:
      // 技テーブルから抽選
      m_AutoPlayParam.m_skillIndex = RandomSelectSkill();
      m_AutoPlayParam.m_skillCmdIndex = 0;
      break;
    }
  }

  // 各状態の実行
  switch(m_autoPlayStateNow)
  {
  case AUTO_PLAY_ST_MOVE_UP:
    m_Input.m_fStickXVal = 1.0f;
    m_Input.m_fStickYVal = 0.0f;
    m_AutoPlayParam.m_targetHeight = 600.0f + (1400.0f * gfl2::math::Lerp(0.0f, 1.0f, System::GflUse::GetPublicRand(101) / 100.0f));
    break;

  case AUTO_PLAY_ST_MOVE_DOWN:
    m_Input.m_fStickXVal = -1.0f;
    m_Input.m_fStickYVal = 0.0f;
    break;

  case AUTO_PLAY_ST_AERIAL:
    m_Input.m_isReturnNeutral = true;
    if(m_stateNow == ST_AERIAL_SKILL && m_stateSeq == SEQ_END)
    {
      if(MantainSurfAppData::IsHighScoreSkill(m_AerialParam.m_skillIdNow))
      {
        // 大技が出たら再抽選
        m_AutoPlayParam.m_skillIndex = RandomSelectSkill();
        m_AutoPlayParam.m_skillCmdIndex = 0;
      }
    }
    if((m_stateNow == ST_AERIAL_START || m_stateNow == ST_AERIAL_SKILL) && m_stateSeq == SEQ_END && m_AutoPlayParam.m_aerialTotalFrame >= 24.0f)
    {
      // 技発動
      SkillCommand cmd = SKILL_COMMAND_TABLE[m_AutoPlayParam.m_skillIndex][m_AutoPlayParam.m_skillCmdIndex];
      if(cmd == CMD_EMPTY)
      {
        // 空ならランダム選択
        cmd = static_cast<SkillCommand>(System::GflUse::GetPublicRand(CMD_MAX));
      }
      m_AutoPlayParam.m_skillCmdIndex++;
      switch(cmd)
      {
      case CMD_UP:     m_Input.m_fStickYVal = 1.0f;    break;
      case CMD_DOWN:   m_Input.m_fStickYVal = -1.0f;   break;
      case CMD_LEFT:   m_Input.m_fStickXVal = -1.0f;   break;
      case CMD_RIGHT:  m_Input.m_fStickXVal = 1.0f;    break;
      }
    }
    break;
  }
}

u32 PlayerController::RandomSelectSkill()
{
  u32 skillId = System::GflUse::GetPublicRand(static_cast<u32>(MantainSurfAppData::SKILL_ID_EX_07) + 1);
  switch(skillId)
  {
  case MantainSurfAppData::SKILL_ID_EX_01:
  case MantainSurfAppData::SKILL_ID_EX_02:
  case MantainSurfAppData::SKILL_ID_EX_03:
  case MantainSurfAppData::SKILL_ID_EX_04:
    return skillId;
  case MantainSurfAppData::SKILL_ID_EX_05:
    return 4 + System::GflUse::GetPublicRand(2);
  case MantainSurfAppData::SKILL_ID_EX_06:
    return 6 + System::GflUse::GetPublicRand(8);
  case MantainSurfAppData::SKILL_ID_EX_07:
  default:
    return SKILL_COMMAND_TABLE_SIZE - 1;
  }
}
#endif  // PM_DEBUG

void PlayerController::UpdateState()
{
  if(m_pAppManager->GetGameState() == MantainSurfAppManager::ST_ENDING)
  {
    UpdateForEnding();
    Camera::Camera* pCamera = m_pAppManager->GetCamera();
    if((m_pPlayerModel->GetPosition() - pCamera->GetPosition()).Length() > pCamera->GetCameraFar() * 2.0f)
    {
      // ゲーム終了時に、プレイヤーがカメラと大分かけ離れた場合はステート更新しない
      return;
    }
  }

  if(!m_Input.m_isRotationMode)
  {
    // 状態変化あるかをチェック
    StateCheckChange();

    // 状態変化時の初期化処理
    StateInitialize();

    // 各状態の実行
    StateExecute();
  }
}

// 状態変化あるかをチェック
void PlayerController::StateCheckChange()
{
  switch(m_stateNow)
  {
  case ST_PADDLING:
    break;
#if PM_DEBUG
  case ST_WAIT:
    if(m_isAccel)
    {
      m_stateNext = ST_ACCEL;
    }
    else if(m_dirInput.LengthSq() > 0.0f)
    {
      // 待機→移動
      if(m_Input.m_isHoldRunButton)
      {
        m_stateNext = ST_RUN;
      }
      else
      {
        m_stateNext = ST_WALK;
      }
    }
    break;

  case ST_WALK:
    if(m_isAccel)
    {
      m_stateNext = ST_ACCEL;
    }
    else if(m_pPlayerModel->IsHitObstacle())
    {
      // 衝突と衝突した
      //m_stateNext = ST_HIT_OB;
    }
    else if(m_dirInput.LengthSq() == 0.0f && m_velocity.z == 0.0f)
    {
      // 歩く→待機
      m_stateNext = ST_WAIT;
    }
    else if(m_Input.m_isHoldRunButton)
    {
      // 歩く→走る
      m_stateNext = ST_RUN;
    }
    break;

  case ST_RUN:
    if(m_isAccel)
    {
      m_stateNext = ST_ACCEL;
    }
    else if(m_pPlayerModel->IsHitObstacle())
    {
      // 障害物と衝突した
      Gimmick::GimmickBase* pGmmick = static_cast<Gimmick::GimmickBase*>(m_pPlayerModel->GetMainCollisionInfo()->m_pTargetModel);
      if(pGmmick->IsTurnOver())
      {
        m_stateNext = ST_TURN_OVER;
      }
      else
      {
        m_stateNext = ST_SHAKY;
      }
    }
    else if(m_dirInput.LengthSq() == 0.0f && m_velocity.z == 0.0f)
    {
      // 走る→待機
      m_stateNext = ST_WAIT;
    }
    else if(!m_Input.m_isHoldRunButton)
    {
      // 走る→歩く
      m_stateNext = ST_WALK;
    }
    break;
#endif

  case ST_ACCEL:
#if PM_DEBUG
    if(!m_isAccel)
    {
      m_stateNext = ST_WAIT;
      break;
    }
#endif
    if(m_pPlayerModel->IsHitObstacle())
    {
      // 障害物と衝突した
      Gimmick::GimmickBase* pGmmick = static_cast<Gimmick::GimmickBase*>(m_pPlayerModel->GetMainCollisionInfo()->m_pTargetModel);
      if(pGmmick->IsTurnOver())
      {
        m_stateNext = ST_TURN_OVER;
      }
      else
      {
        m_stateNext = ST_SHAKY;
      }
    }
    else if(m_velocity.z >= SPEED_MAX_BASE && m_nowNamiInfo.m_isJumpPoint && m_pAppManager->GetGameState() == MantainSurfAppManager::ST_SURFING)
    {
      if(m_pPlayerModel->GetLookDirection().x < 0.0f && (!m_pAppManager->IsTutorialMode() || m_pAppManager->GetTutorialManager()->IsJumpEnable()))
      {
        // プレイヤーが右に向いている場合のみジャンプできる
        m_stateNext = ST_AERIAL_START;
      }
    }
    break;

  case ST_AERIAL_START:
    if(m_stateSeq == SEQ_END)
    {
      // エアリアル状態
      m_stateNext = ST_AERIAL_NORMAL;
    }
    break;

  case ST_AERIAL_NORMAL:
    if(CheckLanding(m_pPlayerModel->GetPosition()))
    {
      // 着水
      m_stateNext = ST_ACCEL;
    }
    else if(CheckAerialSkill())
    {
      m_stateNext = ST_AERIAL_SKILL;
    }
    break;

  case ST_AERIAL_SKILL:
    if(m_stateSeq == SEQ_END)
    {
      // 技処理完了、エアリアル待機状態にする
      m_stateNext = ST_AERIAL_NORMAL;
    }
    else if(CheckLanding(m_pPlayerModel->GetPosition()))
    {
      // 技発動中に着水した場合
      if(m_pPlayerModel->GetAnimationEndFrame() - m_pPlayerModel->GetAnimationFrame() <= m_AerialParam.m_skillParam.m_safeFrame)
      {
        // 安全フレーム内なら成功と見なす
        m_stateNext = ST_AERIAL_NORMAL;
      }
      else
      {
        // 着水失敗
        m_stateNext = ST_AERIAL_MISS;
      }
    }
    break;

  case ST_AERIAL_MISS:
    if(m_stateSeq == SEQ_END)
    {
      m_stateNext = ST_TURN_OVER;
    }
    break;

  case ST_TURN_OVER:
    if(m_stateSeq == SEQ_END)
    {
      m_stateNext = ST_ACCEL;
    }
    break;

  case ST_SHAKY:
    if(m_pPlayerModel->IsHitObstacle())
    {
      // 障害物と衝突した
      Gimmick::GimmickBase* pGmmick = static_cast<Gimmick::GimmickBase*>(m_pPlayerModel->GetMainCollisionInfo()->m_pTargetModel);
      if(pGmmick->IsTurnOver())
      {
        m_stateNext = ST_TURN_OVER;
      }
      else if(m_ShakyParam.m_safeTimer == 0.0f || m_ShakyParam.m_pTargetModel != pGmmick)
      {
        m_stateNext = ST_SHAKY;
      }
    }
    else if(m_stateSeq == SEQ_END)
    {
      m_stateNext = ST_ACCEL;
    }
  }
}

// 状態変化時の初期化処理
void PlayerController::StateInitialize()
{
  while(m_stateNext != ST_NONE)
  {
    State prevState = m_stateNow;
    m_stateNow = m_stateNext;
    m_stateNext = ST_NONE;

    if(IsSurfingState() &&
      m_pAppManager->GetSeaBreadRider()->IsStandby() &&
      m_pPlayerModel->GetPosition().z >= m_pAppManager->GetSeaBreadRider()->GetPosition().z)
    {
      // サーフ中にゴールラインを超えた場合は操作をリセット(操作不能)
      m_Input.Initialize();
    }

    if(prevState == ST_ACCEL)
    {
      m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_L_LOOP);
      m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_R_LOOP);
    }
    
    switch(m_stateNow)
    {
    case ST_PADDLING:
      SurfSoundManager::ChangeSurfSE(GetSurfSEIndex());
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_JET, gfl2::math::Vector3::GetZero());
      m_pPlayerModel->ChangeRotationY(0.0f, 1.0f);
      m_AccelParam.m_turnRate = 0.5f;
      break;
#if PM_DEBUG
    case ST_WAIT:
      // 待機状態の初期化
      SurfSoundManager::ChangeSurfSE(SurfSoundManager::SURF_SE_NONE);
      m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_SURF01_STRAIGHT);
      m_pPlayerModel->EndAllEffect();
      m_pPlayerModel->StartEffect(ModelMantain::EFT_WAIT);
      m_pPlayerModel->SetColliderEnabled(true);
      m_velocity = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
      m_pAppManager->GetCameraController()->SetZoomLevelNormal();
      break;
    case ST_WALK:
      // 歩く状態の初期化
      SurfSoundManager::ChangeSurfSE(SurfSoundManager::SURF_SE_0);
      m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_SURF01_STRAIGHT);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN);
      m_pAppManager->GetCameraController()->SetZoomLevelNormal();
      break;
    case ST_RUN:
      // 走る状態の初期化
      SurfSoundManager::ChangeSurfSE(SurfSoundManager::SURF_SE_1);
      m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_SURF01_STRAIGHT);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_JET);
      m_pAppManager->GetCameraController()->SetZoomLevel(2.0f);
      break;
#endif
    case ST_ACCEL:
      // サーフ状態の初期化
      m_pPlayerModel->EndEffect(ModelMantain::EFT_WAIT);
      if(prevState != ST_PADDLING && prevState != ST_SHAKY)
      {
        SurfSoundManager::ChangeSurfSE(GetSurfSEIndex());
        m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN);
        m_pPlayerModel->StartEffect(ModelMantain::EFT_JET, gfl2::math::Vector3::GetZero());
      }
      m_velocity.y = 0.0f;
      m_pPlayerModel->SetColliderEnabled(true);
      m_AccelParam.m_stateNext = SURF_STATE_NORMAL;
      m_AccelParam.m_isAccel = false;
      m_AccelParam.m_isKeepHighSpeed = false;
      m_AccelParam.m_isBottomTurnEnable = false;
      m_AccelParam.m_bottomTurnRadiusMag = 0.0f;
      m_AccelParam.m_bottomTurnSpeed = 0.0f;
      if(prevState == ST_AERIAL_NORMAL)
      {
        // ジャンプ着水
        // SE停止
        SurfSoundManager::StopSE(SurfSoundManager::SE_AERIAL_NORMAL);
        // SE再生
        SurfSoundManager::PlaySE(SurfSoundManager::SE_LANDING);
        m_pAppManager->GetCameraController()->EndAerialCamera();
        m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_AERIAL_END);
        m_pPlayerModel->EndEffect(ModelMantain::EFT_GLIDE);
        m_pPlayerModel->EndEffect(ModelMantain::EFT_GLIDE_DROP_WATER);
        m_pPlayerModel->StartEffect(ModelMantain::EFT_WATER_HIT_M);
        m_AccelParam.m_turnAngle = 0.0f;
        m_AccelParam.m_turnRate = 0.5f;
        m_AccelParam.m_turnSpeed = TURN_SPEED_DOWNWARD;
#if PM_DEBUG
        // デバッグ値を適用
        m_AccelParam.m_turnSpeed = m_pAppManager->GetDebugMenu()->m_dbvSurfParamTurnSpeedDownward;
#endif
        m_AccelParam.m_turnSpeed += (m_AerialParam.m_startVelocity - SPEED_MIN_JUMP) / (SPEED_MAX_JUMP - SPEED_MIN_JUMP) * m_AccelParam.m_turnSpeed;
        m_AccelParam.m_accelForbiddonTimer = ACCEL_FORBIDDON_TIME;
        m_AccelParam.m_isControlProhibited = true;
        // スピードダウン
        m_AccelParam.m_speedDownStartSpeed = m_velocity.z;
        m_AccelParam.m_speedDownTargetSpeed = m_velocity.z;
        f32 speedLevelRate = CalcSpeedLevel();
        if(speedLevelRate > 0.0f)
        {
          // 前進スピードダウンの終了速度を計算
          s32 level1 = static_cast<s32>(gfl2::math::FCeil(speedLevelRate));
          s32 level2 = level1 - 1;
          m_AccelParam.m_speedDownTargetSpeed -= SPEED_MAX[level1] - SPEED_MAX[level2];
          m_AccelParam.m_speedDownTargetSpeed = gfl2::math::Clamp(m_AccelParam.m_speedDownTargetSpeed, SPEED_MAX_BASE, SPEED_MAX_FORWARD);
        }
        // 着水ボーナス加点
        f32 param_scoreRateLanding = SCORE_RATE_LANDING;
#if PM_DEBUG
        // デバッグ値を適用
        param_scoreRateLanding = m_pAppManager->GetDebugMenu()->m_dbvScoreRateLanding;
#endif
        u32 bonusScore = static_cast<u32>(gfl2::math::FCeil(param_scoreRateLanding * m_AerialParam.m_totalScore));
        // 着水成功UI表示
        if(bonusScore > 0)
        {
          m_AerialParam.m_totalScore += bonusScore;
          RegisterSkillResult(MantainSurfAppData::SKILL_ID_BONUS, bonusScore);
        }
        OnJumpEnd();
      }
      else
      {
        // 初期状態 or 転倒復帰 or よろめき
        m_AccelParam.m_turnAngle = 0.0f;
        m_AccelParam.m_turnSpeed = TURN_SPEED_DOWNWARD;
        m_AccelParam.m_isControlProhibited = false;
        m_AccelParam.m_accelForbiddonTimer = 0.0f;
      }
      m_AccelParam.m_speedDownTimer = 0.0f;
      m_AccelParam.m_accelTimer = 0.0f;
      m_AccelParam.m_prevSurfAnimeId = -1;
      m_AccelParam.m_timerOnWave = 0;
      m_AccelParam.m_timerOffWave = 0;
      m_pAppManager->GetCameraController()->SetZoomLevel(GetSpeedLevel());
      break;
    case ST_AERIAL_START:
      // SE再生
      SurfSoundManager::ChangeSurfSE(SurfSoundManager::SURF_SE_NONE);
      SurfSoundManager::PlaySE(SurfSoundManager::SE_AERIAL_START);
      // エアリアル状態の初期化処理
      m_pPlayerModel->EndEffect(ModelMantain::EFT_SKILL_END);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_SKILL_END);
      m_stateSeq = 0;
      m_AccelParam.m_isAccel = false;
      m_Input.m_isReturnNeutral = false;
      // 技セレクタをリセット
      m_SkillSelector.Reset();
      m_AerialParam.m_waitTimer = 0;
      m_AerialParam.m_totalScorePrev = m_AerialParam.m_totalScore;
      m_AerialParam.m_totalScore = 0;
      m_AerialParam.m_fowardVelocity = m_velocity.z;
      m_AerialParam.m_skillIdNext = MantainSurfAppData::SKILL_ID_NONE;
      m_AerialParam.m_skillIdNow = MantainSurfAppData::SKILL_ID_NONE;
#if PM_DEBUG
      m_AutoPlayParam.m_aerialTotalFrame = 0.0f;
#endif
      m_pAppManager->GetCameraController()->SetZoomLevelNormal();
      OnJumpStart();
      break;
    case ST_AERIAL_NORMAL:
      m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_AERIAL_LOOP);
      m_AerialParam.m_waitTimer = 0;
      if(prevState == ST_AERIAL_START)
      {
        // エアリアル開始状態から遷移した場合
        // ジャンプ用パラメータを設定
        m_AerialParam.m_startVelocity = m_velocity.y;
        m_AerialParam.m_accel = -ACCEL_AERIAL;
        m_AerialParam.m_endVelocity = -m_velocity.y;
      }
      else
      {
        // 技状態から遷移した場合
        if(CheckLanding(m_pPlayerModel->GetPosition()))
        {
          // 技終了した瞬間着水したため、サーフ状態に
          m_stateNext = ST_ACCEL;
          break;
        }
      }
      if(!CheckLanding(m_pPlayerModel->GetPosition() + CalcJumpMoveVect(1)) && CheckAerialSkill())
      {
        // 次フレームで着水しない場合は技発動状態にする
        m_stateNext = ST_AERIAL_SKILL;
      }
      break;
    case ST_AERIAL_SKILL:
      m_stateSeq = 0;
      m_AerialParam.m_skillIdNow = m_AerialParam.m_skillIdNext;
      m_AerialParam.m_skillIdNext = MantainSurfAppData::SKILL_ID_NONE;
      m_AerialParam.m_skillParam = m_SkillSelector.GetSkillParam(m_AerialParam.m_skillIdNow, true);
      m_AerialParam.m_skillZRotBlendIndex = 0;
      m_pPlayerModel->ChangeAnimation(static_cast<ModelPlayer::AnimeId>(m_AerialParam.m_skillParam.m_animeId));
#if PM_DEBUG
      m_AerialParam.m_isUseSkillZRotBlend = m_pAppManager->GetDebugMenu()->m_dbvIsUseSkillZRotBlend;
      m_AutoPlayParam.m_aerialTotalFrame -= m_pPlayerModel->GetAnimationEndFrame();
#endif
      StartSkillSuccessEffect();
      PlaySkillStartSE();
      if(m_AerialParam.m_skillIdNow < MantainSurfAppData::SKILL_ID_NORMAL_01)
      {
        m_pPlayerModel->EndEffect(ModelMantain::EFT_SKILL_START);
        m_pPlayerModel->StartEffect(ModelMantain::EFT_SKILL_START);
      }
      // 結果を格納
      {
        u32 score = CalcSkillScore();
        m_AerialParam.m_totalScore += score;
        RegisterSkillResult(m_AerialParam.m_skillIdNow, score);
      }
      // 着水判定
      {
        gfl2::math::Vector3 moveVec1 = CalcJumpMoveVect(m_pPlayerModel->GetAnimationEndFrame());
        gfl2::math::Vector3 moveVec2 = CalcJumpMoveVect(m_pPlayerModel->GetAnimationEndFrame() + 1.0f);
        bool isLanding1 = CheckLanding(m_pPlayerModel->GetPosition() + moveVec1);
        bool isLanding2 = CheckLanding(m_pPlayerModel->GetPosition() + moveVec2);
        m_AerialParam.m_isSkillWillLanding = (isLanding1 || isLanding2);
      }
      break;
    case ST_AERIAL_MISS:
      m_stateSeq = 0;
      m_velocity.x = 0.0f;
      m_AerialParam.m_accel = -ACCEL_AERIAL;    // Y時方向減速
      m_AerialParam.m_procTimer = 0.0f;
      m_AerialParam.m_missTimer = 0.0f;
      m_AerialParam.m_missPoint = m_pPlayerModel->GetPosition();
      m_pAppManager->GetCameraController()->EndAerialCamera();
      m_pAppManager->GetCameraController()->SetZoomLevelNormal();
      OnJumpEnd();
      break;
    case ST_TURN_OVER:
      // 転倒状態の初期化
      m_stateSeq = 0;
      m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_OVERTURN_START);
      m_AccelParam.m_isAccel = false;
      m_velocity.x = 0.0f;
      m_TurnOverParam.m_timer = 0;
      if(prevState == ST_AERIAL_MISS)
      {
        // エアリアル着水失敗
        m_TurnOverParam.m_moveDirection = gfl2::math::Vector3(0.0f, 0.0f, 1.0f);
        m_TurnOverParam.m_turnOverFrame = 0.0f;
        m_TurnOverParam.m_pTargetModel = NULL;
        m_pPlayerModel->SetPosition(m_AerialParam.m_missPoint);
        m_pAppManager->GetCamera()->SetTraceModeTarget(m_pPlayerModel->GetMantainModel());
      }
      else
      {
        // 障害物と衝突
        // SE再生
        SurfSoundManager::ChangeSurfSE(SurfSoundManager::SURF_SE_NONE);
        SurfSoundManager::PlaySE(SurfSoundManager::SE_HIT_GIMMICK);
        m_pPlayerModel->StartEffect(ModelMantain::EFT_HURDLE_HIT);
        Collision::ColliderBase::CollisionInfo* pCollisionInfo = m_pPlayerModel->GetMainCollisionInfo();
        f32 t = (m_velocity.z - SPEED_MAX_BASE) / (SPEED_MAX_FORWARD - SPEED_MAX_BASE);
        m_velocity.z = gfl2::math::Lerp(SPEED_MAX_BASE, SPEED_MAX_FORWARD * 0.6f, t);
        gfl2::math::Vector3 normal = pCollisionInfo->m_normal;
        normal.y = 0.0f;
        if(normal.Length() > 0.0f)
        {
          normal.NormalizeRet();
          m_TurnOverParam.m_moveDirection = m_prevMoveDir - normal * 2.0f * m_prevMoveDir.Dot(normal);
        }
        else
        {
          m_TurnOverParam.m_moveDirection = -m_prevMoveDir;
        }
        m_TurnOverParam.m_turnOverFrame = m_pPlayerModel->GetAnimationEndFrame();
        m_TurnOverParam.m_pTargetModel = pCollisionInfo->m_pTargetModel;
      }
      // コライダーを一時無効
      m_pPlayerModel->SetColliderEnabled(false);
      m_pAppManager->GetCameraController()->SetZoomLevelNormal();
      break;

    case ST_SHAKY:
      {
        m_stateSeq = 0;
        if(prevState == ST_SHAKY)
        {
          m_pPlayerModel->EndAnimation();
          m_pPlayerModel->EndEffect(ModelMantain::EFT_HURDLE_HIT);
          m_ShakyParam.m_shakyCount++;
        }
        else
        {
          m_ShakyParam.m_shakyCount = 1;
        }
        if(m_ShakyParam.m_shakyCount > 3)
        {
          // 一定回数以上連続してよろめきしたため、転倒とする
          m_stateNext = ST_TURN_OVER;
          break;
        }
        // SE再生
        SurfSoundManager::PlaySE(SurfSoundManager::SE_HIT_GIMMICK);
        m_AccelParam.m_isAccel = false;
        m_pPlayerModel->StartEffect(ModelMantain::EFT_HURDLE_HIT);
        m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_SHAKY);
        Collision::ColliderBase::CollisionInfo* pCollisionInfo = m_pPlayerModel->GetMainCollisionInfo();
        Gimmick::GimmickBase* pGmmick = static_cast<Gimmick::GimmickBase*>(pCollisionInfo->m_pTargetModel);
        gfl2::math::Vector3 reflect = -m_prevMoveDir;
        gfl2::math::Vector3 normal = pCollisionInfo->m_normal;
        normal.y = 0.0f;
        if(normal.Length() > 0.0f)
        {
          normal.NormalizeRet();
          reflect = m_prevMoveDir - normal * 2.0f * m_prevMoveDir.Dot(normal);
        }
        // サーフパラメータの更新
        m_AccelParam.m_turnAngle = gfl2::math::NormalizeDegreeAngle(gfl2::math::AsinDeg(gfl2::math::Clamp(reflect.x, -1.0f, 1.0f)));
        if(reflect.z < 0.0f)
        {
          // 角度を調整
          m_AccelParam.m_turnAngle = reflect.x > 0.0f ? (180.0f - m_AccelParam.m_turnAngle) : -(180.0f + m_AccelParam.m_turnAngle);
        }
        f32 downSpeed = (SPEED_MAX_FORWARD - SPEED_MAX_BASE) / (SPEED_LEVEL_MAX - 1);
        m_AccelParam.m_speedDownTargetSpeed = m_velocity.z - downSpeed * pGmmick->GetPenaltyRatio();
        if(m_AccelParam.m_speedDownTargetSpeed < SPEED_MAX_BASE)
        {
          m_AccelParam.m_speedDownTargetSpeed = SPEED_MAX_BASE;
        }
        // よろめきパラメータの更新
        m_ShakyParam.m_pTargetModel = pGmmick;
        m_ShakyParam.m_safeTimer = SHAKY_SAFE_TIME;
        m_ShakyParam.m_duration = m_pPlayerModel->GetAnimationEndFrame() * (GetSpeedRate() * 0.2f + 1.0f);
        m_ShakyParam.m_timer = 0.0f;
        m_ShakyParam.m_turnAngleSpeed = gfl2::math::FAbs(m_AccelParam.m_turnAngle) / m_ShakyParam.m_duration;
        m_ShakyParam.m_startVelocity = m_velocity.z;
        m_ShakyParam.m_startRotationY = m_pPlayerModel->GetRotationY();
        m_ShakyParam.m_roationAngleY = PI * 2.0f * (GetSpeedRate() + 1.0f);
        if(m_AccelParam.m_turnAngle > gfl2::math::ConvRadToDeg(m_ShakyParam.m_startRotationY))
        {
          m_ShakyParam.m_roationAngleY *= -1.0f;
        }
        // 0度になるまでの補正値を計算する
        m_ShakyParam.m_roationAngleY -= gfl2::math::NormalizeRadianAngle(m_ShakyParam.m_startRotationY + m_ShakyParam.m_roationAngleY);
        // 少し行き過ぎるようの回転値付加値
        f32 roationPlus = gfl2::math::ConvDegToRad(gfl2::math::Lerp(25.0f, 60.0f, GetSpeedRate()));
        if(m_ShakyParam.m_roationAngleY > 0.0f)
        {
          m_ShakyParam.m_roationAngleY += roationPlus;
        }
        else
        {
          m_ShakyParam.m_roationAngleY -= roationPlus;
        }
      }
      break;
    }
  }
}

// 各状態の実行
void PlayerController::StateExecute()
{
  switch(m_stateNow)
  {
  case ST_PADDLING:
    ExecutePaddling();
    break;
#if PM_DEBUG
  case ST_WAIT:
    break;
  case ST_WALK:
  case ST_RUN:
    ExecuteMove();
    break;
#endif
  case ST_ACCEL:
    ExecuteAccel();
    break;
  case ST_AERIAL_START:
    ExecuteAerialStart();
    break;
  case ST_AERIAL_NORMAL:
    ExecuteAerialNormal();
    break;
  case ST_AERIAL_SKILL:
    ExecuteAerialSkill();
    break;
  case ST_AERIAL_MISS:
    ExecuteAerialMiss();
    break;
  case ST_TURN_OVER:
    ExecuteTurnOver();
    break;
  case ST_SHAKY:
    ExecuteShaky();
    break;
  }
}

void PlayerController::ExecutePaddling()
{
  m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, SPEED_MAX_BASE, ACCEL_BASE);
  m_moveVec.z = CalcMoveLengthDeltaTime(m_velocity.z, 0.0f);
}

// 移動の実行
void PlayerController::ExecuteMove()
{
  // プレイヤーの向きを調整
  if(m_dirInput.LengthSq() > 0.0f)
  {
    // 現在の向きから目標向きへの補完
    f32 rate = 1.0f;
    switch(m_stateNow)
    {
    case ST_WALK:
      rate = TURN_RATE_WALK;
      break;
    case ST_RUN:
      rate = TURN_RATE_RUN;
      break;
    }
    m_pPlayerModel->ChangeLookDirection(m_dirInput, rate);
    m_dirMove = m_pPlayerModel->GetLookDirection();
  }
  else
  {
    m_dirMove = m_pPlayerModel->GetLookDirection();
  }

  m_dirMove.y = 0.0f;
  m_dirMove.NormalizeRet();

  // 移動速度を更新
  switch(m_stateNow)
  {
  case ST_WALK:
    m_velocity.z += (m_dirInput.LengthSq() > 0.0f ? ACCEL_WALK : -ACCEL_WALK) * DELTA_TIME;
    m_velocity.z = gfl2::math::Clamp(m_velocity.z, 0.0f, SPEED_WALK_MAX);
    break;
  case ST_RUN:
    m_velocity.z += (m_dirInput.LengthSq() > 0.0f ? ACCEL_RUN : -ACCEL_RUN) * DELTA_TIME;
    m_velocity.z = gfl2::math::Clamp(m_velocity.z, 0.0f, SPEED_RUN_MAX);
    break;
  }
  m_moveVec = m_dirMove * m_velocity.z * DELTA_TIME;
}

void PlayerController::ExecuteAccel()
{
  ExecuteAccelSub();
}

void PlayerController::ExecuteAccelSub()
{
  f32 stickXVal = m_Input.m_fStickXVal;
  if(m_AccelParam.m_isControlProhibited)
  {
    // 操作禁止中のため、0にする
    stickXVal = 0.0f;
  }

  // 状態変更のチェック
  switch(m_AccelParam.m_stateNow)
  {
  case SURF_STATE_NORMAL:
    if(stickXVal < 0.0f && m_AccelParam.m_turnAngle > 0.0f && m_nowNamiInfo.m_zone > Nami::NamiController::ZONE_UP_START)
    {
      // 左入力有り && 左へ向いている && 波の上に居る → 加速状態へ
      m_AccelParam.m_stateNext = SURF_STATE_ACCEL;
    }
    else if(stickXVal > 0.0f && m_AccelParam.m_turnAngle >= 0.0f && m_AccelParam.m_isBottomTurnEnable)
    {
      // 右入力有り && 左へ向いている && 回転半径倍率有り → ボトムターン加速状態へ
      m_AccelParam.m_stateNext = SURF_STATE_TURN_ACCEL;
    }
    break;

  case SURF_STATE_ACCEL:
    if(stickXVal > 0.0f && m_AccelParam.m_turnAngle >= 0.0f && m_AccelParam.m_isBottomTurnEnable)
    {
      // 右入力有り && 左へ向いている && 回転半径倍率有り → ボトムターン加速状態へ
      m_AccelParam.m_stateNext = SURF_STATE_TURN_ACCEL;
    }
    else if(stickXVal >= 0.0f || m_nowNamiInfo.m_zone < Nami::NamiController::ZONE_UP_START)
    {
      // 左入力無し or 波の上に居ない → 通常状態
      m_AccelParam.m_stateNext = SURF_STATE_NORMAL;
    }
    break;

  case SURF_STATE_TURN_ACCEL:
    if(stickXVal <= 0.0f || m_AccelParam.m_turnRate == 0.0f)
    {
      // 右入力なし or 右に向いた
      if(stickXVal < 0.0f && m_AccelParam.m_turnAngle > 0.0f && m_nowNamiInfo.m_zone > Nami::NamiController::ZONE_UP_START)
      {
        // 左入力有り && 左へ向いている && 波の上に居る → 加速状態へ
        m_AccelParam.m_stateNext = SURF_STATE_ACCEL;
      }
      else
      {
        // 通常状態へ
        m_AccelParam.m_stateNext = SURF_STATE_NORMAL;
      }
    }
    break;
  }

  // サーフ処理前の速度レベル
  s32 zoomLevelCur = GetSpeedLevel();

  // 状態変更の初期化
  while(m_AccelParam.m_stateNext != SURF_STATE_NONE)
  {
    SurfState prevSurfState = m_AccelParam.m_stateNow;
    m_AccelParam.m_stateNow = m_AccelParam.m_stateNext;
    m_AccelParam.m_stateNext = SURF_STATE_NONE;

    if(prevSurfState == SURF_STATE_TURN_ACCEL)
    {
      if(m_AccelParam.m_bottomTurnRadiusMag < TURN_ACCEL_RADIUS_MAG_START || m_AccelParam.m_turnAngle <= 0.0f)
      {
        // 速度がある程度落ちたらボトムターン不可に
        m_AccelParam.m_isBottomTurnEnable = false;
      }
    }

    switch(m_AccelParam.m_stateNow)
    {
    case SURF_STATE_NORMAL:
      m_AccelParam.m_speedDownTimer = 0.0f;
      m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_L_LOOP);
      m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_R_LOOP);
      m_pAppManager->GetCameraController()->SetZoomLevel(zoomLevelCur);
      break;
    case SURF_STATE_ACCEL:
      if(m_velocity.z < SPEED_MAX_FORWARD)
      {
        // SE再生
        SurfSoundManager::PlaySE(SurfSoundManager::SE_ACCEL);
        // 加速した瞬間にダッシュのエフェクトを再生
        m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_R_LOOP);
        m_pPlayerModel->StartEffect(Model::ModelMantain::EFT_TURN_L_LOOP);
        m_pAppManager->GetCamera()->StartEffect(Camera::Camera::EFT_WIND_DASH);
        m_pAppManager->GetCameraController()->SetZoomLevel(zoomLevelCur + 2.0f);
      }
      break;
    case SURF_STATE_TURN_ACCEL:
      if(m_velocity.z < SPEED_MAX_FORWARD)
      {
        // SE再生
        SurfSoundManager::PlaySE(SurfSoundManager::SE_ACCEL);
        // 加速した瞬間にダッシュのエフェクトを再生
        m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_L_LOOP);
        m_pPlayerModel->StartEffect(Model::ModelMantain::EFT_TURN_R_LOOP);
        m_pAppManager->GetCamera()->EndEffect(Camera::Camera::EFT_WIND_DASH);
        m_pAppManager->GetCamera()->StartEffect(Camera::Camera::EFT_WIND_DASH);
        m_pAppManager->GetCameraController()->SetZoomLevel(zoomLevelCur + 2.0f);
      }
      m_AccelParam.m_bottomTurnSpeed = m_AccelParam.m_turnSpeed;
      break;
    }
  }

  // サーフ処理
  ExecuteSurfNormal();

  // 速度レベル変化のチェック
  s32 zoomLevelNew = GetSpeedLevel();
  if(zoomLevelNew > zoomLevelCur)
  {
    // 速度レベルが上がった
    m_pAppManager->GetCameraController()->SetZoomLevel(zoomLevelNew + 2.0f, Camera::CameraController::ZOOM_DURATION_SHORT);
  }
  if(zoomLevelCur != zoomLevelNew)
  {
    SurfSoundManager::ChangeSurfSE(GetSurfSEIndex());
  }

  // ボーナス速度を更新
  if(m_velocity.z == SPEED_MAX_FORWARD)
  {
    m_AccelParam.m_topSpeedBonus += ACCEL_FORWARD / 10.0f * DELTA_TIME;
  }
  else
  {
    m_AccelParam.m_topSpeedBonus = 0.0f;
  }

#if PM_DEBUG
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && m_Input.m_isHoldRunButton)
  {
    m_pAppManager->GetCameraController()->SetZoomLevel(CalcSpeedLevel(), Camera::CameraController::ZOOM_DURATION_LONG);
  }
#endif
}

void PlayerController::ExecuteSurfNormal()
{
  enum MoveDir
  {
    DIR_NONE,
    DIR_UPWARD,   // 上り
    DIR_DOWNWARD, // 下り
  };

  // パラメータ
  f32 param_accelForward = ACCEL_FORWARD;
  f32 param_turnRadius[] = { 0.0f, TURN_RADIUS_UPWARD, TURN_RADIUS_DOWNWARD };
  f32 param_turnSpeed[] = { 0.0f, TURN_SPEED_UPWARD, TURN_SPEED_DOWNWARD };
  f32 param_turnAngle[] = { 0.0f, TURN_ANGLE_UPWARD, TURN_ANGLE_DOWNWARD };
  f32 param_turnFact[] = { 0.0f, TURN_FACTOR_UPWARD, TURN_FACTOR_DOWNWARD };
  f32 param_turnSpeedAccel = TURN_SPEED_ACCEL;
  f32 param_turnAccelRadiusMag = TURN_ACCEL_RADIUS_MAG;
  f32 param_landingAgility = LANDING_CONTROL_AGILITY;

#if PM_DEBUG
  // デバッグ値を適用
  Debug::DebugMenu* pDebugMenu = m_pAppManager->GetDebugMenu();
  param_accelForward = pDebugMenu->m_dbvSurfParamAccelForward;
  param_turnRadius[DIR_UPWARD] = pDebugMenu->m_dbvSurfParamTurnRadiusUpward;
  param_turnRadius[DIR_DOWNWARD] = pDebugMenu->m_dbvSurfParamTurnRadiusDownward;
  param_turnSpeed[DIR_UPWARD] = pDebugMenu->m_dbvSurfParamTurnSpeedUpward;
  param_turnSpeed[DIR_DOWNWARD] = pDebugMenu->m_dbvSurfParamTurnSpeedDownward;
  param_turnAngle[DIR_UPWARD] = pDebugMenu->m_dbvSurfParamTurnAngleUpward;
  param_turnAngle[DIR_DOWNWARD] = pDebugMenu->m_dbvSurfParamTurnAngleDownward;
  param_turnFact[DIR_UPWARD] = pDebugMenu->m_dbvSurfParamTurnFactorUpward;
  param_turnFact[DIR_DOWNWARD] = pDebugMenu->m_dbvSurfParamTurnFactorDownward;
  param_turnSpeedAccel = pDebugMenu->m_dbvSurfParamTurnSpeedAccel;
  param_turnAccelRadiusMag = pDebugMenu->m_dbvSurfParamTurnAccelRadiusMag;
  param_landingAgility = pDebugMenu->m_dbvSurfParamLandingAgility;
#endif

  f32 stickXVal = m_Input.m_fStickXVal;
  if(m_AccelParam.m_isControlProhibited)
  {
    // 操作禁止中
    m_AccelParam.m_speedDownTimer = 0.0f;
    m_AccelParam.m_isBottomTurnEnable = false;
    // パラメータ調整
    param_turnSpeedAccel         += param_landingAgility * param_turnSpeedAccel * gfl2::math::FAbs(stickXVal);
    param_turnFact[DIR_UPWARD]   += param_landingAgility * param_turnFact[DIR_UPWARD] * gfl2::math::FAbs(stickXVal);
    param_turnFact[DIR_DOWNWARD] += param_landingAgility * param_turnFact[DIR_DOWNWARD] * gfl2::math::FAbs(stickXVal);
    // 完了チェック
    if(m_AccelParam.m_turnSpeed == param_turnSpeed[DIR_DOWNWARD] && m_AccelParam.m_turnRate == 1.0f)
    {
      // 通常速度に戻ったら操作を解禁
      m_AccelParam.m_isControlProhibited = false;
    }
    else
    {
      // 入力を0にする
      stickXVal = 0.0f;
    }
    // スピードダウン
    f32 t = (m_AccelParam.m_turnRate - 0.5f) / 0.5f;
    m_velocity.z = gfl2::math::Lerp(m_AccelParam.m_speedDownStartSpeed, m_AccelParam.m_speedDownTargetSpeed, t);
  }

  // 入力量調整
  if(stickXVal != 0.0f)
  {
    f32 absX = gfl2::math::FAbs(stickXVal);
    absX = gfl2::math::Lerp(INPUT_STICK_VAL_MIN, 1.0f, (absX - INPUT_STICK_VAL_MIN) / (INPUT_STICK_VAL_MAX - INPUT_STICK_VAL_MIN));
    absX = gfl2::math::Clamp(absX, INPUT_STICK_VAL_MIN, 1.0f);
    stickXVal = (stickXVal > 0.0f ? absX : -absX);
  }

  // 前進の処理
  {
    if(m_velocity.z < SPEED_MAX_BASE)
    {
      m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, SPEED_MAX_BASE, ACCEL_BASE);
    }
  }

  // 入力量と移動方向を求める
  f32 force = 0.0f;
  f32 targetTurnAngle = 0.0f;
  MoveDir dir = DIR_NONE;
  {
    // 入力チェック
    force = -stickXVal;
    if(stickXVal <= 0.0f && (m_nowNamiInfo.m_zone > Nami::NamiController::ZONE_UP_START || m_AccelParam.m_isControlProhibited))
    {
      // 右入力無しかつ、波の上に居るの場合は下りのちから(重力)を加える
      force = force * 1.25f + 1.0f;
    }
    // 移動方向を決める
    if(force < 0.0f)
    {
      // 上りの力がある
      dir = DIR_UPWARD;
      targetTurnAngle = param_turnAngle[dir];
    }
    else if(force > 0.0f)
    {
      // 下りの力がある
      dir = DIR_DOWNWARD;
      targetTurnAngle = param_turnAngle[dir];
    }
    else
    {
      // 力なしの場合、前へ向くように移動方向修正
      if(m_AccelParam.m_turnAngle < 0.0f)
      {
        // 右に向いている場合は左(下り)へ修正
        force = 0.75f;
        dir = DIR_DOWNWARD;
      }
      else if(m_AccelParam.m_turnAngle > 0.0f)
      {
        // 左に向いている場合は右(上り)へ修正
        force = -0.75f;
        dir = DIR_UPWARD;
      }
    }
  }

  // 回転速度を求める
  f32 turnSpeedRate;      // 回転速度の倍率(%)
  f32 turnSpeedTarget;    // 目標の回転速度
  {
    m_AccelParam.m_turnRate = (m_AccelParam.m_turnAngle - param_turnAngle[DIR_UPWARD]) / (param_turnAngle[DIR_DOWNWARD] - param_turnAngle[DIR_UPWARD]);
    // 最大スピードを調整(速度や力を比率にその方向への最大スピードに加算する倍率)
    f32 rate = gfl2::math::Max(gfl2::math::FAbs(force) - 1.0f, 0.0f);
    f32 maxSpeedU = (dir == DIR_UPWARD ? param_turnSpeed[DIR_UPWARD] + rate * param_turnSpeed[DIR_UPWARD] : param_turnSpeed[DIR_UPWARD]);
    f32 maxSpeedD = (dir == DIR_DOWNWARD ? param_turnSpeed[DIR_DOWNWARD] + rate * param_turnSpeed[DIR_DOWNWARD] : param_turnSpeed[DIR_DOWNWARD]);
    // 目標速度を計算
    turnSpeedTarget = gfl2::math::Lerp(maxSpeedU, maxSpeedD, m_AccelParam.m_turnRate);
    m_AccelParam.m_turnSpeed = CalcVelocityDeltaTime(m_AccelParam.m_turnSpeed, turnSpeedTarget, param_turnSpeedAccel);
    // 回転速度の倍率を計算
    turnSpeedRate = 0.0f;
    if(param_turnAngle[dir] != 0.0f)
    {
      turnSpeedRate = gfl2::math::Max(m_AccelParam.m_turnSpeed / param_turnSpeed[dir] - 1.0f, 0.0f);
    }
    m_AccelParam.m_bottomTurnRadiusMag = turnSpeedRate;
  }

  // 角度回転
  f32 turnAngleOld = m_AccelParam.m_turnAngle;
  {
    f32 turnRadius = param_turnRadius[dir];
    if(m_AccelParam.m_stateNow == SURF_STATE_TURN_ACCEL)
    {
      // ボトムターン時の付加回転半径
      turnRadius += param_turnRadius[dir] * m_AccelParam.m_bottomTurnRadiusMag * param_turnAccelRadiusMag;
    }
    if(turnRadius != 0.0f)
    {
      // 角速度(rad/s)
      f32 w = m_AccelParam.m_turnSpeed / turnRadius;
      // 角度の移動量
      f32 value = param_turnFact[dir] * gfl2::math::ConvRadToDeg(w) * DELTA_TIME * force;
      // 回転角度計算
      m_AccelParam.m_turnAngle = CalcValueByStep(m_AccelParam.m_turnAngle, targetTurnAngle, value);
      m_AccelParam.m_turnAngle = gfl2::math::NormalizeDegreeAngle(m_AccelParam.m_turnAngle);
    }
  }

#if PM_DEBUG
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && m_Input.m_isHoldRunButton)
  {
    // Bボタンダッシュ
    m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, SPEED_MAX_FORWARD, param_accelForward * 4.0f);
    m_AccelParam.m_speedDownTimer = 0.0f;
  }
#endif

  // 各状態の処理
  m_AccelParam.m_isAccel = false;
  switch(m_AccelParam.m_stateNow)
  {
  case SURF_STATE_NORMAL:
    {
      // 回転半径倍率の計算
      if(m_AccelParam.m_bottomTurnRadiusMag <= 0.0f && m_AccelParam.m_isBottomTurnEnable)
      {
        // 速度がある程度落ちたらボトムターン不可に
        m_AccelParam.m_isBottomTurnEnable = false;
      }
      // 減速の判定
      if(m_nowNamiInfo.m_zone <= Nami::NamiController::ZONE_UP_START)
      {
        // 海面移動中は減速タイマーを更新
        m_AccelParam.m_speedDownTimer = CalcValueByStep(m_AccelParam.m_speedDownTimer, SPEED_DOWN_TIME_MAX, DELTA_TIME);
      }
      else
      {
        // 減速タイマーをリセット
        m_AccelParam.m_speedDownTimer = 0.0f;
      }
      if(m_AccelParam.m_speedDownTimer > SPEED_DOWN_TIME_MIN)
      {
        // 減速状態になる
        f32 accRate = (m_AccelParam.m_speedDownTimer - SPEED_DOWN_TIME_MIN) / (SPEED_DOWN_TIME_MAX - SPEED_DOWN_TIME_MIN);
        m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, SPEED_MAX_BASE, ACCEL_SPEED_DOWN * accRate);
        // 傾き調整
        m_pPlayerModel->ChangeRotationX(-PI / 6.0f * GetSpeedRate(), TURN_RATE_ACCEL);
      }
      else
      {
        // 傾き調整
        m_pPlayerModel->ChangeRotationX(0.0f, TURN_RATE_ACCEL);
      }
      // 回転時のエフェクト
      if(turnAngleOld < 0.0f && m_AccelParam.m_turnAngle > 0.0f)
      {
        // 右向きから左向き変化した
        m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_L);
        m_pPlayerModel->StartEffect(Model::ModelMantain::EFT_TURN_L);
      }
      else if(turnAngleOld > 0.0f && m_AccelParam.m_turnAngle < 0.0f)
      {
        // 左向きから右向き変化した
        m_pPlayerModel->EndEffect(Model::ModelMantain::EFT_TURN_R);
        m_pPlayerModel->StartEffect(Model::ModelMantain::EFT_TURN_R);
      }
      // カメラズーム更新
      if(!m_pAppManager->GetCameraController()->IsZooming())
      {
        m_pAppManager->GetCameraController()->SetZoomLevel(GetSpeedLevel());
      }
    }
    break;

  case SURF_STATE_ACCEL:
    {
      m_AccelParam.m_isAccel = true;
      // ターンエフェクトのスケール更新
      m_pPlayerModel->GetMantainModel()->SetEffectScale(Model::ModelMantain::EFT_TURN_L_LOOP, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * turnSpeedRate);
      // ボトムターン計算
      if(m_AccelParam.m_bottomTurnRadiusMag >= TURN_ACCEL_RADIUS_MAG_START && !m_AccelParam.m_isBottomTurnEnable)
      {
        // 回転速度がある程度到達したらボトムターン可能に
        m_AccelParam.m_isBottomTurnEnable = true;
      }
      if(m_velocity.z < SPEED_MAX_FORWARD)
      {
        // 加速処理 
        f32 acc = gfl2::math::FAbs(param_accelForward * stickXVal);  // 加速度
#if 0
        // 次の速度レベルの半分まで加速可能
        s32 speedLevel = GetSpeedLevel();
        f32 speedMax = gfl2::math::Lerp(SPEED_MAX[speedLevel], SPEED_MAX[speedLevel + 1], 0.5f);
        speedMax = gfl2::math::Max(m_velocity.z, speedMax);
#else
        f32 speedMax = SPEED_MAX_FORWARD;
#endif
        m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, speedMax, acc);
      }
      // 傾き調整
      m_pPlayerModel->ChangeRotationX(PI / 12.0f, TURN_RATE_ACCEL);
    }
    break;

  case SURF_STATE_TURN_ACCEL:
    {
      m_AccelParam.m_isAccel = true;
      // ターンエフェクトのスケール更新
      m_pPlayerModel->GetMantainModel()->SetEffectScale(Model::ModelMantain::EFT_TURN_R_LOOP, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * turnSpeedRate);
      if(turnAngleOld > 0.0f && m_AccelParam.m_turnAngle <= 0.0f)
      {
        // 向きが左から右へと変わる瞬間
        //m_pPlayerModel->StartEffect(Model::ModelMantain::EFT_TURN_R);
      }
      // 回転速度を保持する
      m_AccelParam.m_turnSpeed = m_AccelParam.m_bottomTurnSpeed;
      // 移動量加算
      m_moveVec.z += m_AccelParam.m_turnSpeed * gfl2::math::CosDeg(m_AccelParam.m_turnAngle) * DELTA_TIME;
      if(m_velocity.z < SPEED_MAX_FORWARD)
      {
        // 加速
        f32 acc = gfl2::math::FAbs(param_accelForward * stickXVal * turnSpeedRate);  // 加速度
#if 0
        // 次の速度レベルまで加速可能(回転角度が不足なら制限する)
        s32 speedLevel = GetSpeedLevel();
        f32 t = gfl2::math::Clamp(1.0f - m_AccelParam.m_turnRate, 0.0f, 0.5f);
        f32 speedMax = gfl2::math::Lerp(SPEED_MAX[speedLevel], SPEED_MAX[speedLevel + 1], 0.5f + t);
        speedMax = gfl2::math::Max(m_velocity.z, speedMax);
#else
        f32 speedMax = SPEED_MAX_FORWARD;
#endif
        m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, speedMax, acc);
      }
      // 傾き調整
      m_pPlayerModel->ChangeRotationX(PI / 12.0f, TURN_RATE_ACCEL);
    }
    break;
  }

  // 移動量の計算
  {
    // X軸の速度を更新
    m_velocity.x = m_AccelParam.m_turnSpeed * gfl2::math::SinDeg(m_AccelParam.m_turnAngle);
    // 回転の移動量
    m_moveVec.x += m_velocity.x * DELTA_TIME;
    // スピードアップによる付加速度の移動量
    m_moveVec.x += param_turnSpeed[dir] * GetSpeedRate() * gfl2::math::SinDeg(m_AccelParam.m_turnAngle) * DELTA_TIME * 0.3f;
    // 前進状態の移動量
    m_moveVec.z += CalcMoveLengthDeltaTime(m_velocity.z, 0.0f);
  }

  // 波の上に居る時間を更新
  {
    if(m_pPlayerModel->GetPosition().y > 0.0f)
    {
      // 波の上に居る
      m_AccelParam.m_timerOnWave++;
      m_AccelParam.m_timerOffWave = 0;
    }
    else
    {
      // 海面上に居る
      m_AccelParam.m_timerOnWave = 0;
      m_AccelParam.m_timerOffWave++;
    }
  }

  // 向きとモーション調整
  {
    f32 maxAngleR = gfl2::math::Max(
      gfl2::math::Lerp(TURN_ANGLE_MAX_R, TURN_ANGLE_MAX_R * 2.0f, GetSpeedRate()),
      gfl2::math::Lerp(TURN_ANGLE_MAX_R, TURN_ANGLE_MAX_R * 2.0f, turnSpeedRate));
    f32 maxAngleL = gfl2::math::Lerp(TURN_ANGLE_MAX_L, TURN_ANGLE_MAX_L * 2.0f, turnSpeedRate);
    f32 targetAngle;
    if(m_AccelParam.m_turnRate <= 0.5f)
    {
      targetAngle = gfl2::math::Lerp(-maxAngleR, 0.0f, m_AccelParam.m_turnRate * 2.0f);
    }
    else
    {
      targetAngle = gfl2::math::Lerp(0.0f, maxAngleL, (m_AccelParam.m_turnRate - 0.5f) * 2.0f);
    }

    s32 animeId = -1;
    if(m_Input.m_fStickXVal > 0.0f)
    {
      // 右入力有り
      if(m_AccelParam.m_stateNow == SURF_STATE_TURN_ACCEL)
      {
        animeId = ModelPlayer::ANIME_SURF04_WAVE_RIGHT;
      }
      else if(m_pPlayerModel->GetPosition().y == 0.0f)
      {
        // 海面に居る
        if(m_AccelParam.m_prevSurfAnimeId == ModelPlayer::ANIME_SURF04_WAVE_RIGHT && m_AccelParam.m_timerOffWave < SURF_RIGHT_MOTION_SWITCH_TIME_ON_TO_OFF)
        {
          // 少しの間前のモーションを維持する
          animeId = ModelPlayer::ANIME_SURF04_WAVE_RIGHT;
        }
        else
        {
          animeId = ModelPlayer::ANIME_SURF03_RIGHT;
        }
      }
      else
      {
        // 波の上に居る
        if(m_AccelParam.m_prevSurfAnimeId == ModelPlayer::ANIME_SURF03_RIGHT && m_AccelParam.m_timerOnWave < SURF_RIGHT_MOTION_SWITCH_TIME_OFF_TO_ON)
        {
          // 少しの間前のモーションを維持する
          animeId = ModelPlayer::ANIME_SURF03_RIGHT;
        }
        else
        {
          animeId = ModelPlayer::ANIME_SURF04_WAVE_RIGHT;
        }
      }
      targetAngle = gfl2::math::Min(targetAngle, -TURN_ANGLE_MAX_R);
    }
    else if(m_Input.m_fStickXVal < 0.0f && !m_AccelParam.m_isControlProhibited)
    {
      // 左入力有り
      animeId = ModelPlayer::ANIME_SURF02_LEFT;
      targetAngle = gfl2::math::Max(targetAngle, TURN_ANGLE_MAX_L);
    }
    else if(m_AccelParam.m_isControlProhibited)
    {
      // 入力禁止中
      targetAngle = maxAngleL;
      animeId = ModelPlayer::ANIME_SURF01_STRAIGHT;
    }
    else
    {
      // 入力無し
      animeId = ModelPlayer::ANIME_SURF01_STRAIGHT;
    }
    if(animeId >= 0 && m_pPlayerModel->IsAnimationEnd(ModelPlayer::ANIME_AERIAL_END))
    {
      if(animeId == ModelPlayer::ANIME_SURF04_WAVE_RIGHT)
      {
        if(m_AccelParam.m_stateNow == SURF_STATE_TURN_ACCEL)
        {
          // ターン中は短くブレンド
          m_pPlayerModel->GetPlayerModel()->SetSurfRightMotionBlendFrame(ModelPlayer::SURF_RIGHT_MOTION_BLEND_FRAME_SHORT);
        }
        else
        {
          // 通常は長くブレンド
          s32 frame = ModelPlayer::SURF_RIGHT_MOTION_BLEND_FRAME_LONG;
#if PM_DEBUG
          frame = m_pAppManager->GetDebugMenu()->m_dbvSurfRightBlendFrame;
#endif
          f32 t = gfl2::math::Clamp((m_AccelParam.m_turnRate - 0.5f) / 0.5f, 0.0f, 1.0f);
          frame = gfl2::math::Lerp(frame, ModelPlayer::SURF_RIGHT_MOTION_BLEND_FRAME_SHORT, t);
          m_pPlayerModel->GetPlayerModel()->SetSurfRightMotionBlendFrame(static_cast<ModelPlayer::SurfRightMotionBlendFrame>(frame));
        }
      }
      m_pPlayerModel->ChangeAnimation(static_cast<ModelPlayer::AnimeId>(animeId));
      m_AccelParam.m_prevSurfAnimeId = animeId;
    }
    targetAngle = gfl2::math::ConvDegToRad(targetAngle);
    m_pPlayerModel->ChangeRotationY(targetAngle, TURN_RATE_ACCEL);
  }
}

void PlayerController::ExecuteAerialStart()
{
  // 移動量を更新
  m_moveVec.x = 0;
  m_moveVec.z = m_velocity.z * DELTA_TIME;
  
  switch(m_stateSeq)
  {
  case 0:
    // エリアル開始状態
    m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_AERIAL_START);
    m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN);
    m_pPlayerModel->EndEffect(ModelMantain::EFT_JET);
    m_pPlayerModel->StartEffect(ModelMantain::EFT_WATER_HIT_M);
    m_pPlayerModel->StartEffect(ModelMantain::EFT_GLIDE);
    m_pPlayerModel->StartEffect(ModelMantain::EFT_GLIDE_DROP_WATER);
    m_moveVec.y = gfl2::math::FAbs(m_velocity.x * DELTA_TIME);
    // 飛び出す時のY軸速度を計算
    {
      s32 speedLevel = GetSpeedLevel();
      f32 base1 = JUMP_SPEED_MIN[speedLevel];
      f32 base2 = (speedLevel < SPEED_LEVEL_MAX - 1 ? JUMP_SPEED_MIN[speedLevel + 1] : JUMP_SPEED_MIN[SPEED_LEVEL_MAX - 1]);
      m_velocity.y = gfl2::math::Lerp(base1, base2, (CalcSpeedLevel() - speedLevel) * 0.5f) * (1.0f - m_AccelParam.m_turnRate);
      m_velocity.y = gfl2::math::Max(m_velocity.y, base1);
      f32 jumpLevel = (m_velocity.y - SPEED_MIN_JUMP) / (SPEED_MAX_JUMP - SPEED_MIN_JUMP);
      m_AerialParam.m_seMaxPitch = gfl2::math::Lerp(SE_MIN_PITCH_AERIAL_NORMAL, SE_MAX_PITCH_AERIAL_NORMAL, jumpLevel);
      m_pAppManager->GetCameraController()->BeginAerialCamera(jumpLevel, m_velocity.y / ACCEL_AERIAL);
    }
    // SE再生
    SurfSoundManager::PlaySE(SurfSoundManager::SE_AERIAL_NORMAL, m_pPlayerModel->GetAnimationEndFrame(), m_AerialParam.m_seMaxPitch);
    m_stateSeq++;
#if PM_DEBUG
    //---------------------
    // 滞空時間計算
    {
      f32 y1 = 0.0f;
      for(u32 i = 0; i < m_pPlayerModel->GetAnimationEndFrame(); i++)
      {
        y1 += i / m_pPlayerModel->GetAnimationEndFrame() * m_velocity.y * DELTA_TIME;
      }
      gfl2::math::Vector3 pos = m_pPlayerModel->GetPosition();
      Nami::NamiController::NamiPointInfo point = m_pAppManager->GetNamiController()->GetNamiPointInfo(pos);
      pos.y += y1;
      f32 vy = m_velocity.y;
      u32 frame = 0;
      while(pos.y > point.m_point.y)
      {
        pos.y += vy * DELTA_TIME + 0.5f * -ACCEL_AERIAL * DELTA_TIME * DELTA_TIME;
        vy += -ACCEL_AERIAL * DELTA_TIME;
        frame++;
      }
      m_AutoPlayParam.m_aerialTotalFrame = frame;
      m_pAppManager->GetDebugMenu()->m_aerialTime = frame;
      GFL_PRINT("[ExecuteAerialStart]JumpTime=%d\r\n", frame);
    }
    //---------------------
#endif
    /* Through */
  case 1:
    // 向きを変更
    {
      f32 rate = m_pPlayerModel->GetAnimationProgress();
      m_moveVec.y = m_velocity.y * rate * DELTA_TIME;
      // エリアル開始状態のチェック
      if(m_pPlayerModel->IsAnimationEnd(true))
      {
        m_stateSeq = SEQ_END;
        rate = 1.0f;
      }
      m_pPlayerModel->ChangeRotationX(0.0f, rate);
      m_pPlayerModel->ChangeRotationZ(-PI / 2.0f, rate);
      m_pPlayerModel->ChangeLookDirection(gfl2::math::Vector3(-m_velocity.y, 0.0f, m_velocity.z).Normalize(), rate);
    }
    break;
  }
}

void PlayerController::ExecuteAerialNormal()
{
  // タイマー更新
  m_AerialParam.m_waitTimer++;

#if PM_DEBUG
  m_pAppManager->GetDebugMenu()->m_aerialTime--;
#endif

  // 位置を更新
  m_moveVec.x = 0;
  m_moveVec.z = m_velocity.z * DELTA_TIME;

  f32 velocicyOld = m_velocity.y;
  f32 t_old = (m_velocity.y - m_AerialParam.m_startVelocity) / (m_AerialParam.m_endVelocity - m_AerialParam.m_startVelocity);
  m_moveVec.y = m_velocity.y * DELTA_TIME + 0.5f * m_AerialParam.m_accel * DELTA_TIME * DELTA_TIME;
  m_velocity.y += m_AerialParam.m_accel * DELTA_TIME;

  // 海面以下に行かないように移動量調整
  if(m_velocity.y < 0.0f)
  {
    gfl2::math::Vector3 position = m_pPlayerModel->GetPosition();
    if(position.y + m_moveVec.y < 0.0f)
    {
      m_moveVec.y = -position.y;
    }
  }

  // Y軸の速度が逆へ上がらないように調整
  if(velocicyOld <= 0.0f && m_velocity.y > 0.0f)
  {
    m_velocity.y = 0.0f;
  }

  f32 t = (m_velocity.y - m_AerialParam.m_startVelocity) / (m_AerialParam.m_endVelocity - m_AerialParam.m_startVelocity);
  // 至高点判定
  if(t_old < 0.45f && t >= 0.45f && m_AerialParam.m_startVelocity == SPEED_MAX_JUMP)
  {
    // 最大速度ジャンプエフェクトを再生
    m_pAppManager->GetCamera()->StartMaxJumpEffect(m_pAppManager->GetSkyBoxManager()->IsDayTime());
  }

  // 向きを更新
  m_pPlayerModel->ChangeLookDirection(gfl2::math::Vector3(-m_velocity.y, 0.0f, m_velocity.z).Normalize(), 1.0f);

  // カメラ更新
  m_pAppManager->GetCameraController()->SetAerialCameraProgress(t);

  // SEピッチ更新
  {
    f32 pitch;
    if(t < 0.5f)
    {
      pitch = gfl2::math::Lerp(m_AerialParam.m_seMaxPitch, 1.0f, t / 0.5f);
    }
    else
    {
      pitch = gfl2::math::Lerp(1.0f, m_AerialParam.m_seMaxPitch, (t - 0.5f) / 0.5f);
    }
    SurfSoundManager::ChangeSEPitch(SurfSoundManager::SE_AERIAL_NORMAL, pitch);
  }
}

void PlayerController::ExecuteAerialSkill()
{
  ExecuteAerialNormal();

  s32 frame = static_cast<s32>(m_pPlayerModel->GetAnimationEndFrame() - m_pPlayerModel->GetAnimationFrame());
  if(frame == SKILL_END_EFFECT_FRAME_THREDSHOLD && m_stateNow == ST_AERIAL_SKILL && !m_AerialParam.m_isSkillWillLanding)
  {
    // スキル完了エフェクトを再生
    m_pPlayerModel->EndEffect(ModelMantain::EFT_SKILL_END);
    m_pPlayerModel->StartEffect(ModelMantain::EFT_SKILL_END);
  }

  // Z軸回転ブレンド
  if(m_AerialParam.m_isUseSkillZRotBlend)
  {
    SkillZRotationBlend(m_AerialParam.m_skillIdNow, m_pPlayerModel->GetAnimationFrame() + 1.0f);
  }

  // SE更新
  f32 pan = m_pAppManager->GetCamera()->JudgeLeftRightSide(m_pPlayerModel->GetMantainModel()->GetBodyPosition());
  SurfSoundManager::ChangeSEPan(m_AerialParam.m_skillParam.m_seIndex, pan);

  // 技を実行する
  bool isSucceeded = m_pPlayerModel->IsAnimationEnd(m_AerialParam.m_skillParam.m_animeId, true);
  if(isSucceeded && m_stateNow != ST_AERIAL_SKILL)
  {
    // スキル状態で決めた技ではない場合は未成功とみなす
    isSucceeded = false;
  }

  if(isSucceeded)
  {
    // ステート終了
    m_stateSeq = SEQ_END;
  }
}

void PlayerController::SkillZRotationBlend(MantainSurfAppData::SkillId skillId, f32 curAnimationFrame)
{
  const f32 *blendTable = NULL;
  u32 keyFrameNum = 0;
  if(!GetSkillZRotationBlendTable(skillId, &blendTable, &keyFrameNum))
  {
    // 対象外の技はブレンドしない
    return;
  }

  if(m_AerialParam.m_skillZRotBlendIndex == keyFrameNum - 1)
  {
    // 最後まで処理した
    return;
  }

#if PM_DEBUG
  // デバッグ値を適用
  GFL_ASSERT(keyFrameNum == Debug::DebugMenu::SRB_CONST_MAX_KEY_FRAME_NUM);
  u32 skillIndex = 0;
  switch(skillId)
  {
  case MantainSurfAppData::SKILL_ID_EX_04:  skillIndex = 0; break;
  case MantainSurfAppData::SKILL_ID_EX_05:  skillIndex = 1; break;
  case MantainSurfAppData::SKILL_ID_EX_06:  skillIndex = 2; break;
  case MantainSurfAppData::SKILL_ID_EX_07:  skillIndex = 3; break;
  }
  u32 startIndex = skillIndex * Debug::DebugMenu::SRB_CONST_MAX_KEY_FRAME_NUM * 3;
  blendTable = m_pAppManager->GetDebugMenu()->m_dbvSkillRotBlendTables + startIndex;
#endif
  
  // 回転補正処理
  f32 keyFrame1 = blendTable[m_AerialParam.m_skillZRotBlendIndex];
  f32 keyFrame2 = blendTable[m_AerialParam.m_skillZRotBlendIndex + 1];
  f32 rot1 = blendTable[m_AerialParam.m_skillZRotBlendIndex + keyFrameNum];
  f32 rot2 = blendTable[m_AerialParam.m_skillZRotBlendIndex + keyFrameNum + 1];
  s32 blendType = static_cast<s32>(blendTable[m_AerialParam.m_skillZRotBlendIndex + keyFrameNum * 2]);

  // 補正率を計算
  f32 t = gfl2::math::Clamp((curAnimationFrame - keyFrame1) / (keyFrame2 - keyFrame1), 0.0f, 1.0f);
  switch(blendType)
  {
  case SKILL_ROT_Z_BLEND_TYPE_CURVE0:
    // 急→緩
    t = gfl2::math::SinDeg(t * 90.0f);
    break;
  case SKILL_ROT_Z_BLEND_TYPE_CURVE1:
    // 緩→急
    t = gfl2::math::SinDeg(gfl2::math::Lerp(-90.0f, 0.0f, t)) + 1.0f;
    break;
  case SKILL_ROT_Z_BLEND_TYPE_CURVE2:
    // 緩→急→緩
    t = (gfl2::math::SinDeg(gfl2::math::Lerp(-90.0f, 90.0f, t)) + 1.0f) / 2.0f;
    break;
  }

  f32 rot = gfl2::math::Clamp(gfl2::math::Lerp(rot1, rot2, t), -180.0f, 180.0f);
  m_pPlayerModel->ChangeRotationZ(gfl2::math::ConvDegToRad(rot), 1.0f);

  // キーフレームインデックス更新
  if(curAnimationFrame == keyFrame2)
  {
    m_AerialParam.m_skillZRotBlendIndex++;
  }
}

// Z回転ブレンドテーブルを取得
bool PlayerController::GetSkillZRotationBlendTable(MantainSurfAppData::SkillId skillId, const f32 **ppBlendTableOut, u32 *pKeyFrameNumOut)
{
  switch(skillId)
  {
  case MantainSurfAppData::SKILL_ID_EX_04:
    *ppBlendTableOut = SKILL_Z_ROTATION_BLEND_TABLE_EX04;
    *pKeyFrameNumOut = GFL_NELEMS(SKILL_Z_ROTATION_BLEND_TABLE_EX04) / 3;
    break;
  case MantainSurfAppData::SKILL_ID_EX_05:
    *ppBlendTableOut = SKILL_Z_ROTATION_BLEND_TABLE_EX05;
    *pKeyFrameNumOut = GFL_NELEMS(SKILL_Z_ROTATION_BLEND_TABLE_EX05) / 3;
    break;
  case MantainSurfAppData::SKILL_ID_EX_06:
    *ppBlendTableOut = SKILL_Z_ROTATION_BLEND_TABLE_EX06;
    *pKeyFrameNumOut = GFL_NELEMS(SKILL_Z_ROTATION_BLEND_TABLE_EX06) / 3;
    break;
  case MantainSurfAppData::SKILL_ID_EX_07:
    *ppBlendTableOut = SKILL_Z_ROTATION_BLEND_TABLE_EX07;
    *pKeyFrameNumOut = GFL_NELEMS(SKILL_Z_ROTATION_BLEND_TABLE_EX07) / 3;
    break;
  default:
    // 対象外の技
    return false;
  }
  return true;
}

void PlayerController::ExecuteAerialMiss()
{
  // ミス状態終了までスキルの処理は続行させる
  ExecuteAerialSkill();

  f32 ratio = (m_AerialParam.m_startVelocity - SPEED_MIN_JUMP) / (SPEED_MAX_JUMP - SPEED_MIN_JUMP);
  f32 timeMax = gfl2::math::Lerp(AERIAL_MISS_MIN_TIME, AERIAL_MISS_MAX_TIME, ratio);
  f32 lengthMax = gfl2::math::Lerp(AERIAL_MISS_MIN_LENGTH, AERIAL_MISS_MAX_LENGTH, ratio);

  {
    // タイマー更新
    m_AerialParam.m_missTimer = CalcValueByStep(m_AerialParam.m_missTimer, timeMax, DELTA_TIME);
    f32 x = m_AerialParam.m_missTimer / timeMax;
    f32 t = 1.0f - (x - 1.0f) * (x - 1.0f);

    // ミスポイントを更新
    m_AerialParam.m_missPoint.z += m_moveVec.z;
    gfl2::math::Vector3 point = m_AerialParam.m_missPoint;
    point.y -= gfl2::math::Lerp(0, lengthMax, t);
    if(point.y < 0.0f)
    {
      point.y = 0.0f;
    }
    point.x = m_pAppManager->GetNamiController()->GetNamiPointInfo(point.y).m_point.x;

    // カメラ追従更新
    m_pAppManager->GetCamera()->SetTraceModePoint(point);
    // 速度更新
    m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, 0.0f, ACCEL_LANDING_MISS_Z);
    // エフェクト位置を更新
    m_pPlayerModel->GetMantainModel()->SetEffectPosition(ModelMantain::EFT_WATER_HIT_AERIAL_MISS, point);
  }

  switch(m_stateSeq)
  {
  case 0:
    // 着水判定
    if(!CheckLanding(m_pPlayerModel->GetMantainModel()->GetBodyPosition() + m_moveVec))
    {
      break;
    }
    // 着水した
    // SE停止
    SurfSoundManager::StopSE(SurfSoundManager::SE_AERIAL_NORMAL);
    // SE再生
    SurfSoundManager::PlaySE(SurfSoundManager::SE_TURN_OVER);
    m_pAppManager->GetCamera()->EndAllEffect();
    m_pPlayerModel->EndAllEffect();
    m_pPlayerModel->StartEffect(ModelMantain::EFT_WATER_HIT_AERIAL_MISS);
    m_stateSeq++;
    /* Through */

  case 1:
    // ミス状態の処理
    m_AerialParam.m_procTimer = CalcValueByStep(m_AerialParam.m_procTimer, AERIAL_MISS_TIME * 2.0f, DELTA_TIME);
    if(m_AerialParam.m_procTimer >= AERIAL_MISS_TIME)
    {
      m_pPlayerModel->SetVisible(false);
    }
    if(m_AerialParam.m_missTimer >= timeMax)
    {
      // ミス状態終了
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_JET, gfl2::math::Vector3::GetZero());
      m_pPlayerModel->GetMantainModel()->SetEffectScale(ModelMantain::EFT_RUN, gfl2::math::Vector3(0.0f, 0.0f, 0.0f));
      m_AerialParam.m_missPoint = m_pAppManager->GetCamera()->GetTracingPosition();
      m_stateSeq = SEQ_END;
    }
    break;
  }
}

void PlayerController::ExecuteTurnOver()
{
  // 下りへの移動量を加える
  m_moveVec += MakeDownwardMoveVec();

  f32 oldRate = 0.0f;
  f32 turnRate = 0.0f;
  switch(m_stateSeq)
  {
  case 0:
    // 移動方向での減速
    if(m_velocity.z > 0.0f)
    {
      m_moveVec += m_TurnOverParam.m_moveDirection * CalcMoveLengthDeltaTime(m_velocity.z, -ACCEL_TURN_OVER);
      m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, 0.0f, ACCEL_TURN_OVER);
    }
    // タイマー更新
    if(m_TurnOverParam.m_turnOverFrame > 0.0f)
    {
      oldRate = m_TurnOverParam.m_timer / m_TurnOverParam.m_turnOverFrame;
      m_TurnOverParam.m_timer = CalcValueByStep(m_TurnOverParam.m_timer, m_TurnOverParam.m_turnOverFrame, 1.0f);
      turnRate = m_TurnOverParam.m_timer / m_TurnOverParam.m_turnOverFrame;
    }
    // 転倒処理
    if(oldRate < 0.5f && turnRate >= 0.5f)
    {
      // 半分くらい転倒したら着水エフェクト表示
      m_pPlayerModel->StartEffect(ModelMantain::EFT_WATER_HIT_L);
      // SE再生
      SurfSoundManager::PlaySE(SurfSoundManager::SE_TURN_OVER);
    }
    if(turnRate == 1.0f || m_TurnOverParam.m_turnOverFrame == 0.0f)
    {
      m_pPlayerModel->SetVisible(false);
      if(m_velocity.z == 0.0f)
      {
        // 完全に転倒した
        m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_OVERTURN_LOOP);
        // SE再生
        SurfSoundManager::PlaySE(SurfSoundManager::SE_RECOVERING);
        m_stateSeq++;
      }
    }
    break;

  case 1:
    // ベース速度まで加速
    m_pPlayerModel->ChangeLookDirection(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), TURN_RATE_ACCEL);
    m_pPlayerModel->ChangeUpDirection(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), TURN_RATE_ACCEL, 0.0f);
    m_moveVec.z += CalcMoveLengthDeltaTime(m_velocity.z, ACCEL_BASE);
    m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, SPEED_MAX_BASE, ACCEL_BASE);
    // 下り移動するのを待つ
    if(m_velocity.x == 0.0f && m_velocity.z == SPEED_MAX_BASE)
    {
      m_TurnOverParam.m_timer = 0.0f;
      m_stateSeq++;
    }
    break;

  case 2:
    // 復帰処理開始
    m_pPlayerModel->ChangeLookDirection(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), TURN_RATE_ACCEL);
    m_pPlayerModel->ChangeUpDirection(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), TURN_RATE_ACCEL, 0.0f);
    m_moveVec.z += CalcMoveLengthDeltaTime(m_velocity.z, 0.0f);
    m_TurnOverParam.m_timer += DELTA_TIME;
    if(m_TurnOverParam.m_pTargetModel != NULL)
    {
      Gimmick::GimmickBase* pGimmick = static_cast<Gimmick::GimmickBase*>(m_TurnOverParam.m_pTargetModel);
      if(pGimmick->IsActionState() && m_pPlayerModel->GetPosition().z < pGimmick->GetPosition().z + 300.0f)
      {
        // アクション実行中の障害物と一定距離を超えるのを待つ
        break;
      }
    }
    if(m_TurnOverParam.m_timer >= TURN_OVER_RECOVER_TIME)
    {
      // 復帰可能になる
      m_pPlayerModel->ChangeAnimation(ModelPlayer::ANIME_OVERTURN_END);
      m_pPlayerModel->SetVisible(true);
      m_TurnOverParam.m_timer = 0;
      m_TurnOverParam.m_turnOverFrame = m_pPlayerModel->GetAnimationEndFrame();
      m_stateSeq++;
    }
    break;

  case 3:
    // 復帰する
    m_pPlayerModel->ChangeLookDirection(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), TURN_RATE_ACCEL);
    m_pPlayerModel->ChangeUpDirection(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), TURN_RATE_ACCEL, 0.0f);
    m_moveVec.z = CalcMoveLengthDeltaTime(m_velocity.z, 0.0f);
    oldRate = m_TurnOverParam.m_timer / m_TurnOverParam.m_turnOverFrame;
    m_TurnOverParam.m_timer = CalcValueByStep(m_TurnOverParam.m_timer, m_TurnOverParam.m_turnOverFrame, 1.0f);
    turnRate = m_TurnOverParam.m_timer / m_TurnOverParam.m_turnOverFrame;
    if(oldRate < 0.1f && turnRate >= 0.1f)
    {
      m_pPlayerModel->StartEffect(ModelMantain::EFT_WATER_HIT_M);
      // SE停止
      SurfSoundManager::StopSE(SurfSoundManager::SE_RECOVERING);
      // SE再生
      SurfSoundManager::PlaySE(SurfSoundManager::SE_RECOVERED);
      //m_pPlayerModel->SetVisible(true);
    }
    if(m_pPlayerModel->IsAnimationEnd(true))
    {
      // 完全に復帰した
      m_stateSeq = SEQ_END;
    }
    break;
  }
  m_pPlayerModel->GetMantainModel()->SetEffectScale(ModelMantain::EFT_RUN, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * m_velocity.z / SPEED_MAX_BASE);
}

void PlayerController::ExecuteShaky()
{
  // ゾーンチェック
  f32 turnAngleSpeed = m_ShakyParam.m_turnAngleSpeed;
  if(m_nowNamiInfo.m_zone > Nami::NamiController::ZONE_UP_START)
  {
    // 下りゾーンに居るため、回転スピードを調整
    turnAngleSpeed *= 1.25f;
  }

  // タイマー更新
  f32 t;
  {
    m_ShakyParam.m_safeTimer = CalcValueByStep(m_ShakyParam.m_safeTimer, 0.0f, DELTA_TIME);
    m_ShakyParam.m_timer++;
    t = gfl2::math::Clamp(m_ShakyParam.m_timer / m_ShakyParam.m_duration, 0.0f, 1.0f);
  }

  // 角度回転
  {
    m_AccelParam.m_turnAngle = CalcValueByStep(m_AccelParam.m_turnAngle, 0.0f, m_ShakyParam.m_turnAngleSpeed);
  }

  // 減速
  {
    s32 speedLevelOld = GetSpeedLevel();
    m_velocity.z = gfl2::math::Lerp(m_ShakyParam.m_startVelocity, m_AccelParam.m_speedDownTargetSpeed, t);
    s32 speedLevelNew = GetSpeedLevel();
    m_pAppManager->GetCameraController()->SetZoomLevel(speedLevelNew);
    if(speedLevelOld != speedLevelNew)
    {
      // サーフSE切り替え
      SurfSoundManager::ChangeSurfSE(GetSurfSEIndex());
    }
  }

  // 移動量計算
  {
    f32 velocityX = gfl2::math::Lerp(SPEED_MAX_BASE, SPEED_MAX_BASE * 1.2f, GetSpeedRate());
    m_velocity.x = velocityX * gfl2::math::SinDeg(m_AccelParam.m_turnAngle);
    m_moveVec.x = m_velocity.x * DELTA_TIME;
    m_moveVec.z = m_velocity.z * gfl2::math::CosDeg(m_AccelParam.m_turnAngle) * DELTA_TIME;
  }

  // 向き調整
  {
    f32 turnAngle = gfl2::math::SinDeg(t * 90.0f) * m_ShakyParam.m_roationAngleY;
    f32 rotationY = gfl2::math::NormalizeRadianAngle(m_ShakyParam.m_startRotationY + turnAngle);
    m_pPlayerModel->ChangeRotationY(rotationY, 1.0f);
    m_pPlayerModel->ChangeRotationX(0.0f, TURN_RATE_ACCEL);
  }

  // 完了判定
  if(m_ShakyParam.m_timer >= m_ShakyParam.m_duration)
  {
    m_stateSeq = SEQ_END;
  }
}

void PlayerController::UpdatePosition()
{
  // ジェットスケールを設定
  m_pPlayerModel->GetMantainModel()->SetEffectScale(ModelMantain::EFT_JET, gfl2::math::Vector3(1.5f, 1.5f, 1.5f) * GetSpeedRate());

  // 位置を移動
  gfl2::math::Vector3 oldPosition = m_pPlayerModel->GetPosition();
  gfl2::math::Vector3 newPosition = oldPosition + m_moveVec;
  if(m_moveVec.Length() > 0.0f)
  {
    m_prevMoveDir = m_moveVec.Normalize();
  }
  
  // 海面との高さを更新
  Nami::NamiController *pNamiController = m_pAppManager->GetNamiController();
  Nami::NamiController::NamiPointInfo oldNamiInfo = m_nowNamiInfo;
  m_nowNamiInfo = pNamiController->GetNamiPointInfo(newPosition);
  if(oldNamiInfo.m_zone <= Nami::NamiController::ZONE_TOP && m_nowNamiInfo.m_zone >= Nami::NamiController::ZONE_TOP)
  {
    m_nowNamiInfo = pNamiController->GetNamiPointInfo(Nami::NamiController::ZONE_TOP);
    newPosition.x = m_nowNamiInfo.m_point.x;
  }
  else if(oldNamiInfo.m_zone >= Nami::NamiController::ZONE_LEFT_END && m_nowNamiInfo.m_zone < Nami::NamiController::ZONE_LEFT_END)
  {
    m_nowNamiInfo = pNamiController->GetNamiPointInfo(Nami::NamiController::ZONE_LEFT_END);
    newPosition.x = m_nowNamiInfo.m_point.x;
  }

  // 傾きを設定
  gfl2::math::Vector3 normal = m_nowNamiInfo.m_normal;
  if(!IsAerialState())
  {
    // エアリアル状態以外の場合
    newPosition.y = m_nowNamiInfo.m_point.y;
    m_pPlayerModel->ChangeInclination(normal, 0.3f);
  }
  else if(CheckLanding(newPosition))
  {
    // エアリアル待機状態でかつ着水した場合は波頂上より以下へ行かないように調整
    if(m_stateNow == ST_AERIAL_NORMAL ||
      (m_stateNow == ST_AERIAL_SKILL && m_pPlayerModel->GetAnimationEndFrame() - m_pPlayerModel->GetAnimationFrame() - 1.0f <= m_AerialParam.m_skillParam.m_safeFrame))
    {
      newPosition.y = m_nowNamiInfo.m_point.y;
    }
  }
  m_pPlayerModel->SetPosition(newPosition);
}

void PlayerController::UpdateForEnding()
{
  m_AccelParam.m_isAccel = false;

  // 常に減速
  {
    m_AccelParam.m_speedDownTimer = 0.0f;
    s32 speedLevelOld = GetSpeedLevel();
    m_velocity.z = CalcVelocityDeltaTime(m_velocity.z, SPEED_MAX_BASE, ACCEL_SPEED_DOWN);
    s32 speedLevelNew = GetSpeedLevel();
    if(speedLevelOld != speedLevelNew)
    {
      // サーフSE切り替え
      SurfSoundManager::ChangeSurfSE(GetSurfSEIndex());
    }
    // 傾き調整
    m_pPlayerModel->ChangeRotationX(-PI / 6.0f * GetSpeedRate(), TURN_RATE_ACCEL);
  }

  // サーフSEボリューム調整
  {
    gfl2::math::Vector3 position = m_pPlayerModel->GetPosition();
    f32 t = (position.z - m_endPosition.z) / SURF_SE_END_MAX_DISTANCE;
    t = gfl2::math::Clamp(t, 0.0f, 1.0f);
    f32 volume = gfl2::math::Lerp(1.0f, 0.0f, t);
    SurfSoundManager::ChangeSurfSEVolume(volume);
  }
}

void PlayerController::AdjustPlayerPosition()
{
  if(m_pPlayerModel->IsHitObstacle())
  {
    // 障害物衝突時に、海面との高さを調整
    Nami::NamiController::NamiPointInfo point = m_pAppManager->GetNamiController()->GetNamiPointInfo(m_pPlayerModel->GetPosition());
    m_pPlayerModel->SetPosition(point.m_point);
  }
}

void PlayerController::UpdateDashEffect()
{
  // スピードレベル変化チェック
  s32 speedLevel = GetSpeedLevel();
  Camera::Camera* pCamera = m_pAppManager->GetCamera();
  if(m_pAppManager->GetGameState() == MantainSurfAppManager::ST_SURFING && m_stateNow == ST_ACCEL)
  {
    // カメラエフェクトの更新
    switch(speedLevel)
    {
    case 0:
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_01_LO);
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_02_LO);
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_03_LO);
      break;

    case 1:
      pCamera->StartEffect(Camera::Camera::EFT_WIND_DASH_01_LO);
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_02_LO);
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_03_LO);
      break;

    case 2:
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_01_LO);
      pCamera->StartEffect(Camera::Camera::EFT_WIND_DASH_02_LO);
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_03_LO);
      break;

    case 3:
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_01_LO);
      pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_02_LO);
      pCamera->StartEffect(Camera::Camera::EFT_WIND_DASH_03_LO);
      break;
    }
  }
  else
  {
    pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_01_LO);
    pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_02_LO);
    pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH_03_LO);
  }

  // ダッシュエフェクト更新
  if(m_AccelParam.m_isAccel)
  {
    pCamera->StartEffect(Camera::Camera::EFT_ZOOM);
  }
  else
  {
    pCamera->EndEffect(Camera::Camera::EFT_ZOOM);
    pCamera->EndEffect(Camera::Camera::EFT_WIND_DASH);
  }

  if(m_stateNow == ST_ACCEL)
  {
    switch(speedLevel)
    {
    case 0:
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP1);
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP2);
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP3);
      break;
    case 1:
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN_DASH01_LOOP1);
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP2);
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP3);
      break;
    case 2:
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP1);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN_DASH01_LOOP2);
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP3);
      break;
    case 3:
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP1);
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP2);
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN_DASH01_LOOP3);
      break;
    }

    if(m_AccelParam.m_isBottomTurnEnable && speedLevel < SPEED_LEVEL_MAX - 1)
    {
      // ボトムターン可能時の衝撃波
      m_pPlayerModel->StartEffect(ModelMantain::EFT_RUN_DASH02_LOOP);
    }
    else
    {
      m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH02_LOOP);
    }
  }
  else
  {
    m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP1);
    m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP2);
    m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH01_LOOP3);
    m_pPlayerModel->EndEffect(ModelMantain::EFT_RUN_DASH02_LOOP);
  }
}

// スティックの入力値を取得
gfl2::math::Vector3 PlayerController::GetInputDirection(f32 fInputX, f32 fInputY)
{
  gfl2::math::Vector3 direction(-fInputX, 0.0f, fInputY);
  if(direction.LengthSq() > 0.0f)
  {
    direction = m_pAppManager->GetCamera()->TransformDirection(direction);
    direction.y = 0.0f;
    direction.NormalizeRet();
  }
  return direction;
}

gfl2::math::Vector3 PlayerController::MakeDownwardMoveVec()
{
  gfl2::math::Vector3 moveVec;
  f32 maxSpeed = 0.0f;
  if(m_nowNamiInfo.m_zone >= Nami::NamiController::ZONE_UP_START)
  {
    maxSpeed = SPEED_MAX_GRAVITY;
  }
  if(maxSpeed != 0.0f || m_velocity.x != maxSpeed)
  {
    gfl2::math::Vector3 down(1.0f, 0.0f, 0.0f);
    moveVec = down * CalcMoveLengthDeltaTime(m_velocity.x, ACCEL_GRAVITY);
  }
  m_velocity.x = CalcVelocityDeltaTime(m_velocity.x, maxSpeed, ACCEL_GRAVITY);
  return moveVec;
}

bool PlayerController::CheckAerialSkill()
{
  return m_AerialParam.m_skillIdNext != MantainSurfAppData::SKILL_ID_NONE;
}

bool PlayerController::CheckLanding(const gfl2::math::Vector3 &playerPosition)
{
  Nami::NamiController::NamiPointInfo point = m_pAppManager->GetNamiController()->GetNamiPointInfo(playerPosition);
  return playerPosition.y <= point.m_point.y;
}

// 2つのベクトルの角度を計算(戻り値：-180度～180度)
// 右/左側判定するため、fromVecの右側方向ベクトル(fromVecRight)も引数として渡す必要がある
// 戻り値が正の値の場合は、toVecはfromVecの右側にある
// 戻り値が負の値の場合は、toVecはfromVecの左側にある
f32 PlayerController::CalcVecAngleDeg(gfl2::math::Vector3 fromVec, gfl2::math::Vector3 toVec, gfl2::math::Vector3 fromVecRight)
{
  f32 dot = fromVec.Dot(toVec);
  f32 angle = gfl2::math::ConvRadToDeg(acosf(dot));
  if(fromVecRight.Dot(toVec) < 0.0f)
  {
    angle = -angle;
  }
  return angle;
}

// 目標速度まで加速／減速
f32 PlayerController::CalcVelocityDeltaTime(f32 vStart, f32 vTarget, f32 acc)
{
  return CalcValueByStep(vStart, vTarget, acc * DELTA_TIME);
}

f32 PlayerController::CalcMoveLengthDeltaTime(f32 vStart, f32 acc)
{
  return vStart * DELTA_TIME + 0.5f * acc * DELTA_TIME * DELTA_TIME;
}

f32 PlayerController::CalcValueByStep(f32 curValue, f32 targetValue, f32 step)
{
  step = gfl2::math::FAbs(step);  // ステップ常に絶対値を取る
  if(curValue < targetValue)
  {
    // 目標より小さい場合はカウントアップ
    curValue += step;
    if(curValue > targetValue)
    {
      curValue = targetValue;
    }
  }
  else if(curValue > targetValue)
  {
    // 目標より大きい場合はカウントダウン
    curValue -= step;
    if(curValue < targetValue)
    {
      curValue = targetValue;
    }
  }
  return curValue;
}

// 着水時の速度を算出
f32 PlayerController::CalcLandingVelocity(f32 minSpeed, f32 maxSpeed)
{
  f32 t = (m_AerialParam.m_startVelocity - SPEED_MIN_JUMP) / (SPEED_MAX_JUMP - SPEED_MIN_JUMP);
  return gfl2::math::Lerp(minSpeed, maxSpeed, t);
}

u32 PlayerController::CalcSkillScore()
{
  f32 score = 0.0f;

  if(m_AerialParam.m_skillIdNow == MantainSurfAppData::SKILL_ID_EX_05)
  {
    // オーバーザギャラドスの特殊処理
    score = m_AerialParam.m_totalScorePrev;   // 前回ジャンプ時の累計スコア
    if(m_AerialParam.m_skillParam.m_counter == 0)
    {
      // 初回の場合は2倍
      score *= 2.0f;
    }
    else
    {
      // 2回目以降は0.1倍
      score *= 0.1f;
    }
    return static_cast<u32>(gfl2::math::FCeil(score));
  }
  
  f32 baseScore = MantainSurfAppData::GetSkillBaseScore(m_AerialParam.m_skillIdNow, m_AerialParam.m_skillParam.m_counter);

  // 特定技のベース得点修正
  switch(m_AerialParam.m_skillIdNow)
  {
  case MantainSurfAppData::SKILL_ID_EX_03:
    // ランターン360は波が高いほど得点が高い
    baseScore *= gfl2::math::Lerp(1.0f, 1.5f, m_pAppManager->GetNamiController()->GetNamiHeightRate());
    break;
  case MantainSurfAppData::SKILL_ID_EX_04:
    // アシレーヌフリップは波が低いほど得点が高い
    baseScore *= gfl2::math::Lerp(1.5f, 1.0f, m_pAppManager->GetNamiController()->GetNamiHeightRate());
    break;
  }

  // 得点パラメータ
  f32 param_scoreRateRank  = MantainSurfAppData::GetSkillScoreRate(m_AerialParam.m_skillParam.m_rank);
  f32 param_scoreRateBase  = SCORE_RATE_BASE;
  f32 param_scoreRateInput = SCORE_RATE_INPUT;
  f32 param_scoreRateSpeed = SCORE_RATE_SPEED;

#if PM_DEBUG
  // デバッグ値を適用
  param_scoreRateRank  = m_pAppManager->GetDebugMenu()->m_dbvScoreRateRank[m_AerialParam.m_skillParam.m_rank];
  param_scoreRateBase  = m_pAppManager->GetDebugMenu()->m_dbvScoreRateBase;
  param_scoreRateInput = m_pAppManager->GetDebugMenu()->m_dbvScoreRateInput;
  param_scoreRateSpeed = m_pAppManager->GetDebugMenu()->m_dbvScoreRateSpeed;
#endif

  // 得点ランク
  {
    baseScore *= param_scoreRateRank;
  }

  // 確定得点
  {
    score += baseScore * param_scoreRateBase;
  }

  // 技入力の得点
  {
    f32 frameDiff = static_cast<f32>(gfl2::math::Abs(m_AerialParam.m_passedFrame - m_AerialParam.m_waitTimer));
    f32 frameEffor = (MAX_SKILL_FRAME_ERROR - frameDiff) / MAX_SKILL_FRAME_ERROR;
    if(frameEffor < 0.0f)
    {
      frameEffor = 0.0f;
    }

#if PM_DEBUG
    // デバッグ用情報を更新
    m_pAppManager->GetDebugMenu()->m_scoreRateInput = frameEffor;
    m_pAppManager->GetDebugMenu()->m_scoreSkillTimer = m_AerialParam.m_passedFrame;
    m_pAppManager->GetDebugMenu()->m_scoreWaitTimer = m_AerialParam.m_waitTimer;
    if(m_pAppManager->GetDebugMenu()->m_dbvIsFixSkillInputFactor)
    {
      frameEffor = 1.0f;
    }
#endif

    score += baseScore * param_scoreRateInput * frameEffor;
  }

  // 自機速度
  {
    score += baseScore * param_scoreRateSpeed * GetSpeedRate();
  }

  // @fix GFMMCat[552] 誤差を取り除く(小数点以下3桁以降を切り捨てる)
  {
    score = static_cast<f32>(static_cast<u32>(score * 100.0f)) / 100.0f;
  }

  return static_cast<u32>(gfl2::math::FCeil(score));
}

// スキル結果を登録する
void PlayerController::RegisterSkillResult(MantainSurfAppData::SkillId skillId, u32 score)
{
#if PM_DEBUG
  static const u32 MAX_SKILL_COUNT = 1024;
  if(m_skillCounter >= MAX_SKILL_COUNT && m_pAppManager->GetDebugMenu()->m_dbvIsAutoPlayMode)
  {
    m_skillCounter++;
    return;
  }
  m_skillCounter++;
#endif
  m_pAppManager->GetSurfUI()->RegisterTrickResult(skillId, score);
  m_pAppManager->AddScore(score);
  m_showScoreRestTime = SCORE_DIPLAY_TIME;
}

// 指定フレームのジャンプ移動距離を計算
gfl2::math::Vector3 PlayerController::CalcJumpMoveVect(u32 frame)
{
  gfl2::math::Vector3 moveVect = gfl2::math::Vector3::GetZero();
  f32 velocity = m_velocity.y;
  for(u32 i = 0; i < frame; i++)
  {
    moveVect.y += velocity * DELTA_TIME + 0.5f * m_AerialParam.m_accel * DELTA_TIME * DELTA_TIME;
    velocity += m_AerialParam.m_accel * DELTA_TIME;
  }
  return moveVect;
}

bool PlayerController::IsGoDown(gfl2::math::Vector3 lookDirection)
{
  f32 angleY = gfl2::math::NormalizeRadianAngle(0.5f * PI - atan2(lookDirection.z, lookDirection.x));
  if(angleY > PI / 6.0f && m_Input.m_fStickXVal <= 0.0f)
  {
    return true;
  }
  return false;
}

bool PlayerController::IsAccel()
{
  return m_stateNow == ST_ACCEL && m_AccelParam.m_isAccel;
}

bool PlayerController::IsSurfingState()
{
#if PM_DEBUG
  return m_stateNow == ST_RUN || m_stateNow == ST_ACCEL;
#else
  return m_stateNow == ST_ACCEL;
#endif
}

bool PlayerController::IsLandingState()
{
#if PM_DEBUG
  return m_stateNow == ST_WAIT || m_stateNow == ST_WALK || m_stateNow == ST_RUN || m_stateNow == ST_ACCEL;
#else
  return m_stateNow == ST_ACCEL;
#endif
}

// -----------------------------------------
// 速度段階を取得(0～3)
// 0 : SPEED_MAX[1]未満
// 1 : SPEED_MAX[1]以上 SPEED_MAX[2]未満
// 2 : SPEED_MAX[2]以上 SPEED_MAX[3]未満
// 3 : SPEED_MAX[3]以上
// -----------------------------------------
s32 PlayerController::GetSpeedLevel()
{
  s32 level = 0;
  for(s32 i = 1; i < SPEED_LEVEL_MAX; i++)
  {
    if(m_velocity.z < SPEED_MAX[i])
    {
      break;
    }
    level++;
  }
  return level;
}

// 通常スピード以下の場合は0.0を返す、最大スピードの場合は1.0を返す
f32 PlayerController::GetSpeedRate()
{
  if(m_velocity.z <= SPEED_MAX_BASE)
  {
    return 0.0f;
  }
  return (m_velocity.z - SPEED_MAX_BASE) / (SPEED_MAX_FORWARD - SPEED_MAX_BASE);
}

// @fix MMCat[449] サーフ通常移動SE判定条件変更によりピッチ計算式を修正
// 現在スピードによりサーフSEのピッチレベルを計算する(0.0～1.0)
f32 PlayerController::CalcSurfSEPitchLevel()
{
  s32 level = GetSpeedLevel();
  if(level < 2)
  {
    // 速度段階0と1の場合
    return CalcSpeedLevel() * 0.5f;
  }
  else
  {
    // 速度段階2以上の場合
    return CalcSpeedLevel() - static_cast<f32>(level);
  }
}

f32 PlayerController::GetBaseSpeed()
{
  return SPEED_MAX_BASE;
}

f32 PlayerController::GetCurrentSpeed()
{
  return m_velocity.z;
}

PlayerModelSet* PlayerController::GetPlayerModelSet()
{
  return m_pPlayerModel;
}

bool PlayerController::IsAerialState()
{
  switch(m_stateNow)
  {
  case ST_AERIAL_START:
  case ST_AERIAL_NORMAL:
  case ST_AERIAL_SKILL:
  case ST_AERIAL_MISS:
    return true;
  default:
    return false;
  }
}

// ジャンプ至高点を通過したか
bool PlayerController::IsJumpOverTop()
{
  return m_stateNow == ST_AERIAL_NORMAL && m_velocity.y < 0.0f;
}

// 大技が発動完了か
bool PlayerController::IsExSkillFinished()
{
  if(m_stateNow == ST_AERIAL_SKILL && m_AerialParam.m_skillIdNow < MantainSurfAppData::SKILL_ID_NORMAL_01)
  {
    // @fix GFMMCat[850] チュートリアル側は着水まで待っているため大技発動後に即成功にしても結果は同じ
    return true;
  }
  return false;
}

// 着水成功か
bool PlayerController::IsLandingSucceeded()
{
  if(CheckLanding(m_pPlayerModel->GetPosition()))
  {
    // @fix GFMMCat[850] 技モーションが安全フレーム以内に着水した場合は着水成功とする
    if(m_stateNow == ST_AERIAL_NORMAL ||
      (m_stateNow == ST_AERIAL_SKILL && m_pPlayerModel->GetAnimationEndFrame() - m_pPlayerModel->GetAnimationFrame() <= m_AerialParam.m_skillParam.m_safeFrame))
    {
      return true;
    }
  }
  return false;
}

PlayerController::State PlayerController::GetState()
{
  return m_stateNow;
}

gfl2::math::Vector3 PlayerController::GetVelocity()
{
  return m_velocity;
}

void PlayerController::ResetZPosition(const f32 resetLength)
{
  m_pPlayerModel->ResetZPosition(resetLength);
  if(m_stateNow == ST_AERIAL_MISS)
  {
    m_AerialParam.m_missPoint.z -= resetLength;
  }
}

void PlayerController::ResetPlayerState(bool isEnd)
{
  m_stateNow = ST_NONE;
  m_stateNext = isEnd ? ST_NONE : ST_ACCEL;
  m_velocity = gfl2::math::Vector3(0.0f, 0.0f, SPEED_MAX_BASE);
  m_pPlayerModel->EndAllEffect();
  m_pPlayerModel->SetPosition(m_pAppManager->GetPlayerInitPosition());
  m_pPlayerModel->ChangeRotation(0.0f, 0.0f, 0.0f, 1.0f);
  m_pPlayerModel->SetVisible(true);
  m_pPlayerModel->SetColliderEnabled(true);
  m_pAppManager->GetCamera()->EndAllEffect();
  m_pAppManager->GetCameraController()->SetTraceModeTarget(m_pPlayerModel->GetMantainModel());
  m_pAppManager->GetCameraController()->SetZoomLevelNormal(0.0f);
  m_pAppManager->GetCameraController()->ResetCameraPosition();
  m_pAppManager->GetCameraController()->EndAerialCamera();
}

void PlayerController::SaveEndPosition()
{
  m_endPosition = m_pPlayerModel->GetPosition();
}

const PlayerController::AccelParam* PlayerController::GetAccelParam()
{
  return &m_AccelParam;
}

const PlayerController::AerialParam* PlayerController::GetAerialParam()
{
  return &m_AerialParam;
}

// 現在速度のレベルを計算
f32 PlayerController::CalcSpeedLevel()
{
  f32 t = gfl2::math::Clamp((m_velocity.z - SPEED_MAX_BASE) / (SPEED_MAX_FORWARD - SPEED_MAX_BASE), 0.0f, 1.0f);
  return gfl2::math::Lerp(0.0f, static_cast<f32>(SPEED_LEVEL_MAX - 1), t);
}

void PlayerController::SetState(State state)
{
  if(m_stateNow != state)
  {
    m_stateNext = state;
  }
}

void PlayerController::StartSkillSuccessEffect()
{
  if(m_AerialParam.m_skillParam.m_eftIdPlayer >= 0)
  {
    // プレイヤーエフェクト再生
    ModelMantain::EffectId eftId = static_cast<ModelMantain::EffectId>(m_AerialParam.m_skillParam.m_eftIdPlayer);
    m_pPlayerModel->EndEffect(eftId);
    m_pPlayerModel->StartEffect(eftId);
  }

  if(m_AerialParam.m_skillParam.m_eftIdCamera >= 0)
  {
    // カメラ再生
    Camera::Camera::EffectId eftId = static_cast<Camera::Camera::EffectId>(m_AerialParam.m_skillParam.m_eftIdCamera);
    m_pAppManager->GetCamera()->EndEffect(eftId);
    m_pAppManager->GetCamera()->StartEffect(eftId, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * m_AerialParam.m_skillParam.m_eftScaleCamera);
  }
}

void PlayerController::PlaySkillStartSE()
{
  if(MantainSurfAppData::IsHighScoreSkill(m_AerialParam.m_skillIdNow))
  {
    SurfSoundManager::PlaySE(SurfSoundManager::SE_SKILL_START);
  }
  SurfSoundManager::PlaySE(m_AerialParam.m_skillParam.m_seIndex);
}

SurfSoundManager::SurfSEIndex PlayerController::GetSurfSEIndex()
{
  // @fix MMCat[449] サーフ通常移動SEの判定条件を修正
  // 速度段階0と1の場合 → SurfSoundManager::SURF_SE_0
  // 速度段階2の場合    → SurfSoundManager::SURF_SE_1
  // 速度段階3の場合    → SurfSoundManager::SURF_SE_2
  s32 level = GetSpeedLevel();
  switch(level)
  {
  case 0:
  case 1:
    return SurfSoundManager::SURF_SE_0;
  case 2:
    return SurfSoundManager::SURF_SE_1;
  case 3:
    return SurfSoundManager::SURF_SE_2;
  default:
    return SurfSoundManager::SURF_SE_0;
  }
}

void PlayerController::OnJumpStart()
{
  m_pAppManager->GetSurfUI()->OnJumpStart();
  m_isJump = true;
}

void PlayerController::OnJumpEnd()
{
  m_pAppManager->GetSurfUI()->OnJumpEnd();
  m_isJump = false;
}

GFL_NAMESPACE_END(Player)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

