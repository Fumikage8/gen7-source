//======================================================================
/**
 * @file DebugMenu.cpp
 * @date 2016/12/21 21:41:11
 * @author fang_yicheng
 * @brief マンタインサーフのデバッグメニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "DebugMenu.h"
#include "../MantainSurfAppManager.h"
#include <GameSys/include/NijiTime.h>

using namespace App::MantainSurf::SurfSound;

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Debug)

const wchar_t* ROOT_GROUP_NAME = L"MantainSurf";

DebugMenu::DebugMenu() :
  m_pHeap(NULL),
  m_pAppManager(NULL),
  m_pRootDebugWinGroup(NULL)
{
  m_dbvIsHotKeyEnable = false;
#ifndef GF_CONFIG_RELEASE
  m_dbvIsHotKeyEnable = true;
#endif
  m_dbvIsUIDrawEnable = true;
  m_dbvIsUIUppderDrawEnable = true;
  m_dbvIsUILowerDrawEnable = true;
  m_dbvIsCameraOffsetMode = false;
  m_dbvIsShowGimmickAll = true;
  m_dbvNextCourseId = -1;
  m_dbvPlayingEffectResId = -1;

  m_dbvSurfParamAccelForward = Player::PlayerController::ACCEL_FORWARD;
  m_dbvSurfParamTurnRadiusUpward = Player::PlayerController::TURN_RADIUS_UPWARD;
  m_dbvSurfParamTurnRadiusDownward = Player::PlayerController::TURN_RADIUS_DOWNWARD;
  m_dbvSurfParamTurnSpeedUpward = Player::PlayerController::TURN_SPEED_UPWARD;
  m_dbvSurfParamTurnSpeedDownward = Player::PlayerController::TURN_SPEED_DOWNWARD;
  m_dbvSurfParamTurnAngleUpward = Player::PlayerController::TURN_ANGLE_UPWARD;
  m_dbvSurfParamTurnAngleDownward = Player::PlayerController::TURN_ANGLE_DOWNWARD;
  m_dbvSurfParamTurnFactorUpward = Player::PlayerController::TURN_FACTOR_UPWARD;
  m_dbvSurfParamTurnFactorDownward = Player::PlayerController::TURN_FACTOR_DOWNWARD;
  m_dbvSurfParamTurnSpeedAccel = Player::PlayerController::TURN_SPEED_ACCEL;
  m_dbvSurfParamTurnAccelRadiusMag = Player::PlayerController::TURN_ACCEL_RADIUS_MAG;
  m_dbvSurfParamLandingAgility = Player::PlayerController::LANDING_CONTROL_AGILITY;

  m_dbvSkyBoxRotationY = SkyBox::SkyBoxManager::DEFAULT_ROTATION_Y;
  m_dbvSkyBoxPositionY = SkyBox::SkyBoxManager::DEFAULT_POSITION_Y;
  m_timeZone = 0;

  for(u32 i = 0; i < Model::Gimmick::GimmickBase::TYPE_MAX; i++)
  {
    m_dbvIsShowGimmick[i] = (i != Model::Gimmick::GimmickBase::TYPE_COMMON);
  }

  for(u32 i = 0; i < MantainSurfRenderingPipeline::VIEWER_PATH_COUNT; i++)
  {
    m_dbvDrawEnable[i] = true;
  }

  m_effectResId = 0;
}

DebugMenu::~DebugMenu()
{
  gfl2::debug::DebugWin_RemoveGroupByName(ROOT_GROUP_NAME);
}

// 基本情報定義
enum EnumBasicInfo
{
  BI_MODE,          // ゲームモード
  BI_POSITION,      // プレイヤー位置
  BI_STATE,         // プレイヤー状態
  BI_SURF_STATE,    // サーフ状態
  BI_TURN_SPEED,    // 回転速度
  BI_TURN_ANGLE,    // 回転半径
  BI_ACCEL_RADIUS,  // ボトムターン回転半径倍率
  BI_SPEED_X,       // X軸移動速度
  BI_SPEED_Y,       // Y軸移動速度
  BI_SPEED_Z,       // Z軸移動速度
  BI_SPEED_BONUS,   // 最大速度ボーナス
  BI_AERIAL_TIME,   // 残り滞空時間
  BI_SKILL_NAME,    // 技名
  BI_SKILL_TIMER,   // 入力タイマー
  BI_WAIT_TIMER,    // 待機タイマー
  BI_SCORE_SKILL_NAME,    // 発動技名
  BI_SCORE_SKILL_TIMER,   // 技発動時の入力時間
  BI_SCORE_WAIT_TIMER,    // 技発動時の待機時間
  BI_SCORE_INPUT,   // 入力得点
  BI_SKILL_COUNT,   // 技発動回数
  BI_TOTAL_SCORE,   // 累計スコア
  BI_CAMERA_POS,    // カメラ位置
  BI_CAMERA_ROT,    // カメラ回転
  BI_CAMERA_OFFSET, // カメラオフセット
  BI_CAMERA_LEN,    // カメラ距離
  BI_WAVE_HEIGHT,   // 波の高さ
  BI_PROGRESS,      // コース進捗率
  BI_DISTANCE,      // 進んだ距離
  BI_PLAY_TIME,     // プレイ時間
  BI_CAMERA_NEAR,   // カメラビュー平面距離(z=-1)
  BI_MAX
};

static void UpdateBasicInfo(void* userWork, gfl2::debug::DebugWinItem* item)
{
}

static wchar_t* GetSkillName(MantainSurfAppData::SkillId skillId)
{
  switch(skillId)
  {
  case MantainSurfAppData::SKILL_ID_EX_01:       return L"スクリューハンテール";
  case MantainSurfAppData::SKILL_ID_EX_02:       return L"スクリューサクラビス";
  case MantainSurfAppData::SKILL_ID_EX_03:       return L"ランターン３６０";
  case MantainSurfAppData::SKILL_ID_EX_04:       return L"アシレーヌフリップ";
  case MantainSurfAppData::SKILL_ID_EX_05:       return L"オーバーザギャラドス";
  case MantainSurfAppData::SKILL_ID_EX_06:       return L"スターミートルネード";
  case MantainSurfAppData::SKILL_ID_EX_07:       return L"コイキングスペシャル";
  case MantainSurfAppData::SKILL_ID_NORMAL_01:   return L"上回転";
  case MantainSurfAppData::SKILL_ID_NORMAL_02:   return L"下回転";
  case MantainSurfAppData::SKILL_ID_NORMAL_03:   return L"左回転";
  case MantainSurfAppData::SKILL_ID_NORMAL_04:   return L"右回転";
  default:                                       return L"なし";
  }
}

static wchar_t* DrawBasicInfo(void* userWork, gfl2::debug::DebugWinItem* item)
{
  MantainSurfAppManager* pAppManager = static_cast<MantainSurfAppManager*>(userWork);
  wchar_t*  strWork = gfl2::debug::DebugWin_GetWorkStr();
  const s32 strLen = gfl2::debug::DEBUGWIN_NAME_LEN;

  switch(item->GetScrId())
  {
  case BI_MODE:
    {
      wchar_t* strMode = L"通常モード";
      if(pAppManager->GetDebugMenu()->m_dbvIsInfinityMode)
      {
        strMode = L"無限モード";
      }
      swprintf(strWork, strLen, L"[ゲームモード]%ls", strMode);
    }
    break;

  case BI_POSITION:
    {
      gfl2::math::Vector3 pos = pAppManager->GetPlayerModelSet()->GetPosition();
      swprintf(strWork, strLen, L"[プレイヤー位置]X=%.1f Y=%.1f Z=%.1f", pos.x, pos.y, pos.z);
    }
    break;

  case BI_STATE:
    {
      wchar_t* strBase = L"[プレイヤー状態]%ls";
      wchar_t* strState = L"";
      Player::PlayerController::State state = pAppManager->GetPlayerController()->GetState();
      switch(state)
      {
      case Player::PlayerController::ST_WAIT:           strState = L"待機";               break;
      case Player::PlayerController::ST_WALK:           strState = L"自由移動(低速)";     break;
      case Player::PlayerController::ST_RUN:            strState = L"自由移動(高速)";     break;
      case Player::PlayerController::ST_ACCEL:          strState = L"サーフ";             break;
      case Player::PlayerController::ST_AERIAL_START:   strState = L"エアリアル(開始)";   break;
      case Player::PlayerController::ST_AERIAL_NORMAL:  strState = L"エアリアル(待機)";   break;
      case Player::PlayerController::ST_AERIAL_SKILL:   strState = L"エアリアル(技発動)"; break;
      case Player::PlayerController::ST_AERIAL_MISS:    strState = L"エアリアル(技失敗)"; break;
      case Player::PlayerController::ST_TURN_OVER:      strState = L"転覆";               break;
      case Player::PlayerController::ST_SHAKY:          strState = L"よろめき";           break;
      default:                                          strState = L"なし";               break;
      }
      swprintf(strWork, strLen, strBase, strState);
    }
    break;

  case BI_SURF_STATE:
    {
      wchar_t* strBase = L"[サーフ状態]%ls";
      wchar_t* strState = L"";
      Player::PlayerController::SurfState state = pAppManager->GetPlayerController()->GetAccelParam()->m_stateNow;
      switch(state)
      {
      case Player::PlayerController::SURF_STATE_NORMAL:       strState = L"通常";     break;
      case Player::PlayerController::SURF_STATE_ACCEL:        strState = L"加速";     break;
      case Player::PlayerController::SURF_STATE_TURN_ACCEL:   strState = L"ターン";   break;
      default:                                                strState = L"なし";     break;
      }
      swprintf(strWork, strLen, strBase, strState);
    }
    break;

  case BI_TURN_SPEED:
    {
      const Player::PlayerController::AccelParam* pParam = pAppManager->GetPlayerController()->GetAccelParam();
      swprintf(strWork, strLen, L"[回転速度(cm/s)]%.1f", pParam->m_turnSpeed);
    }
    break;

  case BI_TURN_ANGLE:
    {
      const Player::PlayerController::AccelParam* pParam = pAppManager->GetPlayerController()->GetAccelParam();
      swprintf(strWork, strLen, L"[回転角度(deg)]%.1f", pParam->m_turnAngle);
    }
    break;

  case BI_ACCEL_RADIUS:
    {
      const Player::PlayerController::AccelParam* pParam = pAppManager->GetPlayerController()->GetAccelParam();
      swprintf(strWork, strLen, L"[回転半径倍率]%.1f", pParam->m_bottomTurnRadiusMag);
    }
    break;

  case BI_SPEED_X:
    {
      swprintf(strWork, strLen, L"[X軸移動速度(cm/s)]%.1f", pAppManager->GetPlayerController()->GetVelocity().x);
    }
    break;

  case BI_SPEED_Y:
    {
      swprintf(strWork, strLen, L"[Y軸移動速度(cm/s)]%.1f", pAppManager->GetPlayerController()->GetVelocity().y);
    }
    break;

  case BI_SPEED_Z:
    {
      swprintf(strWork, strLen, L"[Z軸移動速度(cm/s)]%.1f", pAppManager->GetPlayerController()->GetVelocity().z);
    }
    break;
    
  case BI_AERIAL_TIME:
    {
      swprintf(strWork, strLen, L"[残り滞空時間(frame)]%.0f", pAppManager->GetDebugMenu()->m_aerialTime);
    }
    break;

  case BI_SPEED_BONUS:
    {
      swprintf(strWork, strLen, L"[最大速度ボーナス]%.1f", pAppManager->GetPlayerController()->GetAccelParam()->m_topSpeedBonus);
    }
    break;

  case BI_SKILL_NAME:
    {
      wchar_t* strBase = L"[入力技名]%ls";
      wchar_t* strName = GetSkillName(pAppManager->GetPlayerController()->GetAerialParam()->m_skillIdNext);
      swprintf(strWork, strLen, strBase, strName);
    }
    break;

  case BI_SKILL_TIMER:
    {
      swprintf(strWork, strLen, L"[入力技経過時間(フレーム)]%d", pAppManager->GetPlayerController()->GetAerialParam()->m_passedFrame);
    }
    break;

  case BI_WAIT_TIMER:
    {
      swprintf(strWork, strLen, L"[待機経過時間(フレーム)]%d", pAppManager->GetPlayerController()->GetAerialParam()->m_waitTimer);
    }
    break;

  case BI_SCORE_SKILL_NAME:
    {
      wchar_t* strBase = L"[発動技名]%ls";
      wchar_t* strName = GetSkillName(pAppManager->GetPlayerController()->GetAerialParam()->m_skillIdNow);
      swprintf(strWork, strLen, strBase, strName);
    }
    break;

  case BI_SCORE_SKILL_TIMER:
    {
      swprintf(strWork, strLen, L"[技発動時の経過時間(フレーム)]%d", pAppManager->GetDebugMenu()->m_scoreSkillTimer);
    }
    break;

  case BI_SCORE_WAIT_TIMER:
    {
      swprintf(strWork, strLen, L"[技発動時の待機時間(フレーム)]%d", pAppManager->GetDebugMenu()->m_scoreWaitTimer);
    }
    break;

  case BI_SCORE_INPUT:
    {
      swprintf(strWork, strLen, L"[技入力得点率]%.2f", pAppManager->GetDebugMenu()->m_scoreRateInput);
    }
    break;

  case BI_SKILL_COUNT:
    {
      swprintf(strWork, strLen, L"[技発動回数]%d", pAppManager->GetPlayerController()->GetSkillCounter());
    }
    break;

  case BI_TOTAL_SCORE:
    {
      swprintf(strWork, strLen, L"[累計スコア]%d", pAppManager->GetTotalScore());
    }
    break;

  case BI_CAMERA_POS:
    {
      gfl2::math::Vector3 pos = pAppManager->GetCamera()->GetPosition();
      swprintf(strWork, strLen, L"[カメラ位置]X=%.1f Y=%.1f Z=%.1f", pos.x, pos.y, pos.z);
    }
    break;

  case BI_CAMERA_ROT:
    {
      gfl2::math::Vector3 rot = pAppManager->GetCamera()->GetRotation();
      swprintf(strWork, strLen, L"[カメラ回転値]X=%.1f Y=%.1f Z=%.1f", rot.x, rot.y, rot.z);
    }
    break;

  case BI_CAMERA_OFFSET:
    {
      gfl2::math::Vector3 offset = pAppManager->GetCamera()->GetTargetOffset();
      swprintf(strWork, strLen, L"[カメラオフセット]X=%.1f Y=%.1f Z=%.1f", offset.x, offset.y, offset.z);
    }
    break;

  case BI_CAMERA_LEN:
    {
      swprintf(strWork, strLen, L"[カメラ距離(cm)]%.1f", pAppManager->GetCamera()->GetLength());
    }
    break;

  case BI_WAVE_HEIGHT:
    {
      swprintf(strWork, strLen, L"[波の高さ(％)]%.1f", pAppManager->GetNamiController()->GetNamiHeightRate() * 100.0f);
    }
    break;

  case BI_PROGRESS:
    {
      swprintf(strWork, strLen, L"[コース進捗率(％)]%.1f", pAppManager->GetNamiController()->GetProgress() * 100.0f);
    }
    break;
  case BI_DISTANCE:
    {
      swprintf(strWork, strLen, L"[進んだ距離(cm)]%.1f", pAppManager->GetTotalDistance());
    }
    break;
  case BI_PLAY_TIME:
    {
      swprintf(strWork, strLen, L"[プレイ時間(s)]%.1f", pAppManager->GetPlayTime() / 30.0f);
    }
    break;
  case BI_CAMERA_NEAR:
    {
      Camera::Camera::ViewPlane plane = pAppManager->GetCamera()->GetNearViewPlane();
      f32 len = (plane.center - pAppManager->GetCamera()->GetPosition()).Length();
      swprintf(strWork, strLen, L"[ビュー平面距離]%.2f", len);
    }
    break;
  }
  return strWork;
}

enum EnumCourseSetting
{
  CS_RESTART,
  CS_START_TUTORIAL,
  CS_SET_GOAL_LINE,
  CS_SET_GOAL,
  CS_SET_QUIT,
};

static void UpdateCourseSetting(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    switch(item->GetScrId())
    {
    case CS_RESTART:
      pDebugMenu->m_dbvIsCourseRestart = true;
      gfl2::debug::DebugWin_CloseWindow();
      break;
    case CS_START_TUTORIAL:
      pDebugMenu->m_dbvIsCourseRestart = true;
      pDebugMenu->m_dbvCourseIdNow = Course::CourseDataManager::TUTORIAL_COURSE_ID;
      gfl2::debug::DebugWin_CloseWindow();
      break;
    case CS_SET_GOAL_LINE:
      if(!pDebugMenu->GetAppManager()->IsTutorialMode())
      {
        pDebugMenu->GetAppManager()->GetSeaBreadRider()->SetAppear();
      }
      break;
    case CS_SET_GOAL:
      if(!pDebugMenu->GetAppManager()->IsTutorialMode())
      {
        pDebugMenu->GetAppManager()->SetEnding();
      }
      break;
    case CS_SET_QUIT:
      pDebugMenu->GetAppManager()->SetQuit();
      gfl2::debug::DebugWin_CloseWindow();
      break;
    }
  }
}

enum EnumGimmickTest
{
  GT_MAKE_GIMMICK,
  GT_STATE_ACTION,
  GT_STATE_RESET
};

static void UpdateGimmickTest(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);

  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(!pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    return;
  }

  switch(item->GetScrId())
  {
  case GT_MAKE_GIMMICK:
    pDebugMenu->GetAppManager()->GetGimmickManager()->MakeGimmickForDebug();
    break;
    
  case GT_STATE_ACTION:
    pDebugMenu->GetAppManager()->GetGimmickManager()->SetState(Model::Gimmick::GimmickBase::ST_ACTION);
    break;

  case GT_STATE_RESET:
    pDebugMenu->GetAppManager()->GetGimmickManager()->Reset();
    break;
  }
}

static wchar_t* DrawCalcJumpTime(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();

  const f32 AERIAL_START_MOTION_DURATION = 15.0f;    // エアリアル開始モーションの長さ
  const f32 DELTA_TIME = 1.0f / 30.0f;
  const f32 ACC = Player::PlayerController::ACCEL_AERIAL;

  f32 jumpSpeed = pDebugMenu->m_jumpSpeedStart;

  f32 y1 = 0.0f;
  for(u32 i = 0; i < AERIAL_START_MOTION_DURATION; i++)
  {
    y1 += i / AERIAL_START_MOTION_DURATION * jumpSpeed * DELTA_TIME;
  }

  f32 pos_y = y1;
  u32 frame = 0;
  while(pos_y > 0.0f)
  {
    pos_y += jumpSpeed * DELTA_TIME + 0.5f * -ACC * DELTA_TIME * DELTA_TIME;
    jumpSpeed += -ACC * DELTA_TIME;
    frame++;
  }
  swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), frame);
  return strWork;
}

enum EnumSkillSetting
{
  SS_SKILL_UNLOCK_ALL,
  SS_SKILL_UNLOCK_01,
  SS_SKILL_UNLOCK_02,
  SS_SKILL_UNLOCK_03,
  SS_SKILL_UNLOCK_04,
};

static bool GetSkillSysFlg(EnumSkillSetting skillSettingId, DebugMenu* pDebugMenu)
{
  switch(skillSettingId)
  {
  case SS_SKILL_UNLOCK_ALL:
    return pDebugMenu->m_isAllSkillUnlocked;
  case SS_SKILL_UNLOCK_01:
    return MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_03);
  case SS_SKILL_UNLOCK_02:
    return MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_04);
  case SS_SKILL_UNLOCK_03:
    return MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_06);
  case SS_SKILL_UNLOCK_04:
    return MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_05);
  default:
    return false;
  }
}

static wchar_t* DrawSkillSetting(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();

  EnumSkillSetting skillSettingId = static_cast<EnumSkillSetting>(item->GetScrId());
  bool flg = GetSkillSysFlg(skillSettingId, pDebugMenu);

  wchar_t* strValue1 = L"解放";
  if(flg) strValue1 = L"閉鎖";
  wchar_t* strValue2 = L"閉鎖";
  if(flg) strValue2 = L"解放";

  switch(skillSettingId)
  {
  case SS_SKILL_UNLOCK_ALL:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), strValue1);
    break;
  case SS_SKILL_UNLOCK_01:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), L"[ランターン３６０]", strValue1, strValue2);
    break;
  case SS_SKILL_UNLOCK_02:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), L"[アシレーヌフリップ]", strValue1, strValue2);
    break;
  case SS_SKILL_UNLOCK_03:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), L"[スターミートルネード]", strValue1, strValue2);
    break;
  case SS_SKILL_UNLOCK_04:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), L"[オーバーザギャラドス]", strValue1, strValue2);
    break;
  }
  return strWork;
}

static void SetSkillSysFlg(EnumSkillSetting skillSettingId, bool flg)
{
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Field::EventWork *pEventWork = pGameManager->GetGameData()->GetEventWork();
  switch(skillSettingId)
  {
  case SS_SKILL_UNLOCK_ALL:
    flg ? pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_01) : pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_01);
    flg ? pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_02) : pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_02);
    flg ? pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_03) : pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_03);
    flg ? pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_04) : pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_04);
    break;
  case SS_SKILL_UNLOCK_01:
    if(flg)
    {
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_01);
    }
    else
    {
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_01);
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_02);
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_03);
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_04);
    }
    break;
  case SS_SKILL_UNLOCK_02:
    if(flg)
    {
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_01);
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_02);
    }
    else
    {
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_02);
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_03);
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_04);
    }
    break;
  case SS_SKILL_UNLOCK_03:
    if(flg)
    {
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_01);
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_02);
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_03);
    }
    else
    {
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_03);
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_04);
    }
    break;
  case SS_SKILL_UNLOCK_04:
    if(flg)
    {
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_01);
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_02);
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_03);
      pEventWork->SetEventFlag(SYS_FLAG_SURF_SKILL_04);
    }
    else
    {
      pEventWork->ResetEventFlag(SYS_FLAG_SURF_SKILL_04);
    }
    break;
  }
}

static void UpdateSkillSetting(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);

  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(!pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    return;
  }

  EnumSkillSetting skillSettingId = static_cast<EnumSkillSetting>(item->GetScrId());
  switch(skillSettingId)
  {
  case SS_SKILL_UNLOCK_ALL:
    pDebugMenu->m_isAllSkillUnlocked = !pDebugMenu->m_isAllSkillUnlocked;
    SetSkillSysFlg(skillSettingId, pDebugMenu->m_isAllSkillUnlocked);
    break;
  default:
    SetSkillSysFlg(skillSettingId, !GetSkillSysFlg(skillSettingId, pDebugMenu));
    break;
  }

  if(!pDebugMenu->GetAppManager()->IsTutorialMode() ||
     pDebugMenu->GetAppManager()->GetTutorialManager()->GetState() >= Tutorial::TutorialManager::ST_AERIAL_BEFORE)
  {
    pDebugMenu->GetAppManager()->ShowSkillList();
  }
}

enum EnumOther
{
  OTH_RESET_Z_POS,
};

static void UpdateOther(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);

  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(!pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    return;
  }

  switch(item->GetScrId())
  {
  case OTH_RESET_Z_POS:
    pDebugMenu->GetAppManager()->ResetZPosition();
    break;
  }
}

enum EnumEffectTest
{
  ET_PLAY_EFFECT,
  ET_STOP_EFFECT,
};

static void UpdateEffectTest(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  bool isConfirm = pButton->IsTrigger(gfl2::ui::BUTTON_A);

  switch(item->GetScrId())
  {
  case ET_PLAY_EFFECT:
    {
      if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
      {
        // ID変更
        pDebugMenu->m_effectResId--;
        if(pDebugMenu->m_effectResId < 0) pDebugMenu->m_effectResId = GARC_mantain_effect_DATA_NUM - 1;
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
      {
        // ID変更
        pDebugMenu->m_effectResId++;
        if(pDebugMenu->m_effectResId >= GARC_mantain_effect_DATA_NUM) pDebugMenu->m_effectResId = 0;
      }
      if(isConfirm)
      {
        pDebugMenu->m_dbvPlayingEffectResId = pDebugMenu->m_effectResId;
      }
    }
    break;
    
  case ET_STOP_EFFECT:
    {
      if(isConfirm)
      {
        pDebugMenu->GetAppManager()->GetEffectManager()->StopEffectForDebug();
      }
    }
    break;
  }
}

static wchar_t* DrawEffectTest(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
  switch(item->GetScrId())
  {
  case ET_PLAY_EFFECT:
    {
      swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), pDebugMenu->m_effectResId);
    }
    break;

  case ET_STOP_EFFECT:
    {
      strWork = item->GetName();
    }
    break;
  }
  return strWork;
}

static void UpdateSkyBoxTimeZone(void* userWork, gfl2::debug::DebugWinItem* item)
{
  s32 step = 0;
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    step = -1;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT) || pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    step = 1;
  }
  if(step != 0)
  {
    pDebugMenu->m_timeZone += step;
    if(pDebugMenu->m_timeZone < 0)
    {
      pDebugMenu->m_timeZone = SkyBox::SkyBoxManager::TIME_ZONE_MAX - 1;
    }
    else if(pDebugMenu->m_timeZone >= SkyBox::SkyBoxManager::TIME_ZONE_MAX)
    {
      pDebugMenu->m_timeZone = 0;
    }
    // タイムゾーンによって時間を変更
    SkyBox::SkyBoxManager::Time time = SkyBox::SkyBoxManager::TIMEZONE_START_TIME[pDebugMenu->m_timeZone];
    pDebugMenu->m_dbvSkyBoxTimeHour = time.m_hour;
    pDebugMenu->m_dbvSkyBoxTimeMinute = time.m_minute;
    pDebugMenu->m_dbvSkyBoxTimeSecond = time.m_second;
    // タイムゾーンによってライトRGBを変更
    gfl2::math::Vector3 color = pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetLightingColor(pDebugMenu->m_timeZone);
    pDebugMenu->m_dbvLightingColorR = color.x;
    pDebugMenu->m_dbvLightingColorG = color.y;
    pDebugMenu->m_dbvLightingColorB = color.z;
  }
}

static wchar_t* DrawSkyBoxTimeZone(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  SkyBox::SkyBoxManager::Time time;
  time.m_hour = pDebugMenu->m_dbvSkyBoxTimeHour;
  time.m_minute = pDebugMenu->m_dbvSkyBoxTimeMinute;
  time.m_second = pDebugMenu->m_dbvSkyBoxTimeSecond;

  pDebugMenu->m_timeZone = SkyBox::SkyBoxManager::GetTimeZone(time);
  switch(pDebugMenu->m_timeZone)
  {
  case SkyBox::SkyBoxManager::TIME_ZONE_NIGHT:
    return L"時間帯[夜]";
  case SkyBox::SkyBoxManager::TIME_ZONE_NOON:
    return L"時間帯[昼]";
  case SkyBox::SkyBoxManager::TIME_ZONE_NOON_EVENING:
    return L"時間帯[昼～夕]";
  case SkyBox::SkyBoxManager::TIME_ZONE_EVENING:
    return L"時間帯[夕]";
  case SkyBox::SkyBoxManager::TIME_ZONE_EVENING_NIGHT:
    return L"時間帯[夕～夜]";
  default:
    return L"";
  }
}

static void UpdateSkyBoxTimeReset(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    SkyBox::SkyBoxManager::Time time = pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetTimeDefault();
    pDebugMenu->m_dbvSkyBoxTimeHour = time.m_hour;
    pDebugMenu->m_dbvSkyBoxTimeMinute = time.m_minute;
    pDebugMenu->m_dbvSkyBoxTimeSecond = time.m_second;
    gfl2::math::Vector3 color = pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetLightingColor(pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetTimeZone(time));
    pDebugMenu->m_dbvLightingColorR = color.x;
    pDebugMenu->m_dbvLightingColorG = color.y;
    pDebugMenu->m_dbvLightingColorB = color.z;
  }
}

static void UpdateLightColorReset(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    gfl2::math::Vector3 color = pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetLightingColor(pDebugMenu->m_timeZone);
    pDebugMenu->m_dbvLightingColorR = color.x;
    pDebugMenu->m_dbvLightingColorG = color.y;
    pDebugMenu->m_dbvLightingColorB = color.z;
  }
}

static void UpdateSkyBoxMoon(void* userWork, gfl2::debug::DebugWinItem* item)
{
  s32 step = 0;
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    step = -1;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT) || pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    step = 1;
  }
  if(step != 0)
  {
    pDebugMenu->m_moonPattern += step;
    if(pDebugMenu->m_moonPattern < 0)
    {
      pDebugMenu->m_moonPattern = SkyBox::SkyBoxManager::MOON_PATTERN_MAX - 1;
    }
    else if(pDebugMenu->m_moonPattern >= SkyBox::SkyBoxManager::MOON_PATTERN_MAX)
    {
      pDebugMenu->m_moonPattern = 0;
    }
    // MoonPatternよって日付を変更
    pDebugMenu->m_dbvSkyBoxDay = SkyBox::SkyBoxManager::GetDayByMoonPattern(pDebugMenu->m_moonPattern);
  }
}

static wchar_t* DrawSkyBoxMoon(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  pDebugMenu->m_moonPattern = pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetMoonPattern(pDebugMenu->m_dbvSkyBoxDay);
  switch(pDebugMenu->m_moonPattern)
  {
  case SkyBox::SkyBoxManager::MOON_PATTERN_NONE_DRAW:
    return L"月相[新月]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_CRESCENT_BEGIN:
    return L"月相[三日月前半]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_HALF_BEGIN:
    return L"月相[上弦]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_3_4_BEGIN:
    return L"月相[3/4月前半]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_FULL:
    return L"月相[満月]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_3_4_END:
    return L"月相[3/4月後半]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_HALF_END:
    return L"月相[下弦]";
  case SkyBox::SkyBoxManager::MOON_PATTERN_CRESCENT_END:
    return L"月相[三日月後半]";
  default:
    return L"";
  }
}

static void UpdateSkyBoxMoonReset(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pDebugMenu->m_dbvSkyBoxDay = pDebugMenu->GetAppManager()->GetSkyBoxManager()->GetDayDefault();
  }
}

enum EnumSoundTest
{
  ST_PLAY_SE1,
  ST_PLAY_SE2,
  ST_SE_CHANGE_PITCH1,
  ST_SE_CHANGE_PITCH2,
  ST_STOP_SE1,
  ST_STOP_SE2,
  ST_STOP_ALL_SE,
  ST_PLAY_BGM,
  ST_STOP_BGM
};

static wchar_t* GetSeLabelName(s32 seIndex)
{
  switch(seIndex)
  {
  case SurfSoundManager::SE_SURF_ENV:
    return L"サーフ環境音";
  case SurfSoundManager::SE_SURF_LV_0:
    return L"サーフ移動Lv0";
  case SurfSoundManager::SE_SURF_LV_1:
    return L"サーフ移動Lv1";
  case SurfSoundManager::SE_SURF_LV_2:
    return L"サーフ移動Lv2";
  case SurfSoundManager::SE_ACCEL:
    return L"加速";
  case SurfSoundManager::SE_HIT_GIMMICK:
    return L"障害物衝突";
  case SurfSoundManager::SE_TURN_OVER:
    return L"転覆";
  case SurfSoundManager::SE_AERIAL_START:
    return L"エアリアル開始";
  case SurfSoundManager::SE_AERIAL_NORMAL:
    return L"エアリアル待機";
  case SurfSoundManager::SE_LANDING:
    return L"着水";
  case SurfSoundManager::SE_RECOVERING:
    return L"転覆移動";
  case SurfSoundManager::SE_RECOVERED:
    return L"転覆復帰";
  case SurfSoundManager::SE_GIMMICK_ACTION_1:
    return L"サメハダー/ホエルコアクション";
  case SurfSoundManager::SE_GIMMICK_ACTION_2:
    return L"ホエルオーアクション";
  case SurfSoundManager::SE_GIMMICK_ACTION_3:
    return L"障害物着水音";
  case SurfSoundManager::SE_PADDLING_END:
    return L"パドリング完了";
  case SurfSoundManager::SE_SKILL_EX_01:
    return L"スクリューハンテール";
  case SurfSoundManager::SE_SKILL_EX_02:
    return L"スクリューサクラビス";
  case SurfSoundManager::SE_SKILL_EX_03:
    return L"ランターン360";
  case SurfSoundManager::SE_SKILL_EX_04:
    return L"アシレーヌフリップ";
  case SurfSoundManager::SE_SKILL_EX_05:
    return L"オーバーザギャラドス";
  case SurfSoundManager::SE_SKILL_EX_06:
    return L"スターミートルネード";
  case SurfSoundManager::SE_SKILL_EX_07:
    return L"コイキングスペシャル";
  case SurfSoundManager::SE_SKILL_NORMAL_01:
    return L"左回転/右回転";
  case SurfSoundManager::SE_SKILL_NORMAL_02:
    return L"上回転/下回転";
  case SurfSoundManager::SE_SUNSHINE:
    return L"日光";
  case SurfSoundManager::SE_SHOOTING_START:
    return L"流れ星";
  case SurfSoundManager::SE_SKILL_END:
    return L"技完了";
  case SurfSoundManager::SE_SKILL_START:
    return L"大技開始";
  case SurfSoundManager::SE_UI_SCORE_LOW:
    return L"技低得点";
  case SurfSoundManager::SE_UI_SCORE_HIGH:
    return L"技高得点";
  default:
    return L"なし";
  }
}

static void UpdateSoundTest(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  bool isConfirm = pButton->IsTrigger(gfl2::ui::BUTTON_A);

  switch(item->GetScrId())
  {
  case ST_PLAY_SE1:
    {
      if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
      {
        // ID変更
        pDebugMenu->m_seIndex1--;
        if(pDebugMenu->m_seIndex1 < 0)
        {
          pDebugMenu->m_seIndex1 = SurfSoundManager::SE_MAX - 1;
        }
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
      {
        // ID変更
        pDebugMenu->m_seIndex1++;
        if(pDebugMenu->m_seIndex1 == SurfSoundManager::SE_MAX)
        {
          pDebugMenu->m_seIndex1 = 0;
        }
      }
      if(isConfirm)
      {
        SurfSoundManager::StopSE(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndexNow1));
        SurfSoundManager::PlaySE(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndex1), 0, pDebugMenu->m_sePitch1);
        pDebugMenu->m_seIndexNow1 = pDebugMenu->m_seIndex1;
      }
    }
    break;

  case ST_PLAY_SE2:
    {
      if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
      {
        // ID変更
        pDebugMenu->m_seIndex2--;
        if(pDebugMenu->m_seIndex2 < 0)
        {
          pDebugMenu->m_seIndex2 = SurfSoundManager::SE_MAX - 1;
        }
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
      {
        // ID変更
        pDebugMenu->m_seIndex2++;
        if(pDebugMenu->m_seIndex2 == SurfSoundManager::SE_MAX)
        {
          pDebugMenu->m_seIndex2 = 0;
        }
      }
      if(isConfirm)
      {
        SurfSoundManager::StopSE(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndexNow2));
        SurfSoundManager::PlaySE(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndex2), 0, pDebugMenu->m_sePitch2);
        pDebugMenu->m_seIndexNow2 = pDebugMenu->m_seIndex2;
      }
    }
    break;

  case ST_STOP_SE1:
    {
      if(isConfirm)
      {
        SurfSoundManager::StopSE(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndexNow1));
      }
    }
    break;

  case ST_STOP_SE2:
    {
      if(isConfirm)
      {
        SurfSoundManager::StopSE(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndexNow2));
      }
    }
    break;

  case ST_STOP_ALL_SE:
    {
      if(isConfirm)
      {
        SurfSoundManager::StopAllSE();
      }
    }
    break;

  case ST_SE_CHANGE_PITCH1:
    {
      f32 pitchOld = pDebugMenu->m_sePitch1;
      f32 changeValue = 0.0f;
      if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
      {
        // Pitch変更
        changeValue = -0.01f;
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
      {
        // Pitch変更
        changeValue = 0.01f;
      }
      if(pButton->IsHold(gfl2::ui::BUTTON_R))
      {
        changeValue *= 10.0f;
      }
      pDebugMenu->m_sePitch1 = gfl2::math::Clamp(pDebugMenu->m_sePitch1 + changeValue, 0.0f, 100.0f);
      if(pitchOld != pDebugMenu->m_sePitch1)
      {
        SurfSoundManager::ChangeSEPitch(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndex1), pDebugMenu->m_sePitch1);
      }
    }
    break;

  case ST_SE_CHANGE_PITCH2:
    {
      f32 pitchOld = pDebugMenu->m_sePitch2;
      f32 changeValue = 0.0f;
      if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
      {
        // Pitch変更
        changeValue = -0.01f;
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
      {
        // Pitch変更
        changeValue = 0.01f;
      }
      if(pButton->IsHold(gfl2::ui::BUTTON_R))
      {
        changeValue *= 10.0f;
      }
      pDebugMenu->m_sePitch2 = gfl2::math::Clamp(pDebugMenu->m_sePitch2 + changeValue, 0.0f, 100.0f);
      if(pitchOld != pDebugMenu->m_sePitch2)
      {
        SurfSoundManager::ChangeSEPitch(static_cast<SurfSoundManager::SEIndex>(pDebugMenu->m_seIndex2), pDebugMenu->m_sePitch2);
      }
    }
    break;

  case ST_PLAY_BGM:
    {
      if(isConfirm)
      {
        SurfSoundManager::StartBGM();
      }
    }
    break;

  case ST_STOP_BGM:
    {
      if(isConfirm)
      {
        SurfSoundManager::EndBGM();
      }
    }
    break;
  }
}

static wchar_t* DrawSoundTest(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
  switch(item->GetScrId())
  {
  case ST_PLAY_SE1:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), pDebugMenu->m_seIndex1 + 1, GetSeLabelName(pDebugMenu->m_seIndex1));
    break;

  case ST_PLAY_SE2:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), pDebugMenu->m_seIndex2 + 1, GetSeLabelName(pDebugMenu->m_seIndex2));
    break;

  case ST_SE_CHANGE_PITCH1:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), pDebugMenu->m_sePitch1);
    break;

  case ST_SE_CHANGE_PITCH2:
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), pDebugMenu->m_sePitch2);
    break;
  }
  return strWork;
}

static void UpdateSkillRotBlend(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);

  s32 step = 0;
  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    step = -1;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    step = 1;
  }

  if(step == 0) return;

  if(item->GetScrId() < 0)
  {
    // 技設定更新
    pDebugMenu->m_curSkillRotBlendTableIndex += step;
    if(pDebugMenu->m_curSkillRotBlendTableIndex < 0)
    {
      pDebugMenu->m_curSkillRotBlendTableIndex = DebugMenu::SRB_CONST_MAX_SKILL_NUM - 1;
    }
    else if(pDebugMenu->m_curSkillRotBlendTableIndex >= DebugMenu::SRB_CONST_MAX_SKILL_NUM)
    {
      pDebugMenu->m_curSkillRotBlendTableIndex = 0;
    }

    // 設定テーブル更新
    pDebugMenu->m_curSkillRotBlendTable = pDebugMenu->m_dbvSkillRotBlendTables + pDebugMenu->m_curSkillRotBlendTableIndex * DebugMenu::SRB_CONST_MAX_KEY_FRAME_NUM * 3;
  }
  else
  {
    // 設定値更新
    u32 n = item->GetScrId() / 3;
    u32 mod = item->GetScrId() % 3;
    f32 valMin = 0.0f;
    f32 valMax = (mod < 2 ? 1000.0f : 3.0f);
    u32 index = (mod < 2 ? n + mod : n + DebugMenu::SRB_CONST_MAX_KEY_FRAME_NUM * 2);
    f32 val = pDebugMenu->m_curSkillRotBlendTable[index] + step;
    if(val > valMax)
    {
      val = valMin;
    }
    else if(val < valMin)
    {
      val = valMax;
    }
    pDebugMenu->m_curSkillRotBlendTable[index] = val;
  }
}

static wchar_t* DrawSkillRotBlend(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();

  if(item->GetScrId() < 0)
  {
    // 技名描画
    wchar_t* strSkillName = L"";
    switch(pDebugMenu->m_curSkillRotBlendTableIndex)
    {
    case 0:   strSkillName = L"アシレーヌフリップ(34フレーム)";      break;
    case 1:   strSkillName = L"オーバーザギャラドス(45フレーム)";    break;
    case 2:   strSkillName = L"スターミートルネード(38フレーム)";    break;
    case 3:   strSkillName = L"コイキングスペシャル(30フレーム)";    break;
    }
    swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), pDebugMenu->m_curSkillRotBlendTableIndex + 1, strSkillName);
  }
  else
  {
    // 設定値描画
    u32 n = item->GetScrId() / 3;
    u32 mod = item->GetScrId() % 3;
    u32 index = (mod < 2 ? n + mod : n + DebugMenu::SRB_CONST_MAX_KEY_FRAME_NUM * 2);
    s32 val = static_cast<s32>(pDebugMenu->m_curSkillRotBlendTable[index]);
    if(mod < 2)
    {
      // 数値
      if(mod == 0)  val++;
      swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), val);
    }
    else
    {
      // 補間方式
      wchar_t* strName = L"";
      switch(val)
      {
      case 0:   strName = L"線形";                break;
      case 1:   strName = L"カーブ1(急→緩)";     break;
      case 2:   strName = L"カーブ2(緩→急)";     break;
      case 3:   strName = L"カーブ3(緩→急→緩)"; break;
      }
      swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), strName);
    }
  }
  return strWork;
}

static wchar_t* DrawSkillInfo(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);
  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
  wchar_t* strSkillName = GetSkillName(static_cast<MantainSurfAppData::SkillId>(item->GetScrId()));
  Skill::SkillSelector::SkillParam skillParam = pDebugMenu->GetAppManager()->GetPlayerController()->GetSkillSelector()->GetSkillParam(static_cast<MantainSurfAppData::SkillId>(item->GetScrId()), false);
  swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), strSkillName, skillParam.m_rank + 1, skillParam.m_counter);
  return strWork;
}

static wchar_t* DrawGimmickInfo(void* userWork, gfl2::debug::DebugWinItem* item)
{
  DebugMenu* pDebugMenu = static_cast<DebugMenu*>(userWork);

  Model::Gimmick::GimmickBase** ppGimmickList = pDebugMenu->GetAppManager()->GetGimmickManager()->GetGimmickList();
  u32 size = pDebugMenu->GetAppManager()->GetGimmickManager()->GetMaxGimmickSize();

  u32 activeSize = 0;
  u32 displaySize = 0;

  for(u32 i = 0; i < size; i++)
  {
    Model::Gimmick::GimmickBase* pGimmick = ppGimmickList[i];
    if(item->GetScrId() != Model::Gimmick::GimmickBase::TYPE_MAX && pGimmick->GetType() != item->GetScrId()) continue;
    if(!pGimmick->IsDead())   activeSize++;
    if(pGimmick->IsVisible()) displaySize++;
  }

  wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName(), activeSize, displaySize);
  return strWork;
}

void DebugMenu::Initialize(gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager)
{
  m_pHeap = pHeap;
  m_pAppManager = pAppManager;

  m_pRootDebugWinGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, ROOT_GROUP_NAME, NULL);

  // 1.基本情報
  {
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"1.基本情報", m_pRootDebugWinGroup);
    for(u32 i = 0; i < BI_MAX; i++)
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"", m_pAppManager, UpdateBasicInfo, DrawBasicInfo, i);
    }
  }

  // 2.コース設定
  {
    m_dbvCourseIdNow = m_pAppManager->GetCourseId();
    m_dbvIsCourseRestart = false;
    m_dbvIsInfinityMode = false;
    m_dbvIsWaveHeightFix = false;
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"2.コース設定", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"コースID", &m_dbvCourseIdNow, 0, Course::CourseDataManager::COURSE_DATA_MAX - 1, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"設定したコースIDでリスタート", this, UpdateCourseSetting, 0, CS_RESTART);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"チュートリアルコースでリスタート", this, UpdateCourseSetting, 0, CS_START_TUTORIAL);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"波高さ固定", &m_dbvIsWaveHeightFix);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"無限モード", &m_dbvIsInfinityMode);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"直ちにゴールラインを配置する", this, UpdateCourseSetting, 0, CS_SET_GOAL_LINE);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"直ちにゴールする", this, UpdateCourseSetting, 0, CS_SET_GOAL);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"直ちに終了する", this, UpdateCourseSetting, 0, CS_SET_QUIT);
  }
  
  // 3.描画機能
  {
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"3.描画機能", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"天球描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_SKY]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"背景描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_BG]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"エッジマップ描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_EDGE]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"波モデル描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_NAMI]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"メインモデル描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_MAIN]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"アウトライン描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_OUTLINE]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"エフェクト(パーティクル型)描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_EFFECT]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"エフェクト(モデル型)描画", &m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_MODEL_EFFECT]);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"UI描画(全体)", &m_dbvIsUIDrawEnable);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"UI描画(上画面)", &m_dbvIsUIUppderDrawEnable);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"UI描画(下画面)", &m_dbvIsUILowerDrawEnable);
  }

  // 4.障害物テスト
  {
    m_dbvIsCollisionCheckOn = true;
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"4.障害物テスト", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"障害物一式を作成する", this, UpdateGimmickTest, 0, GT_MAKE_GIMMICK);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"障害物のアクションモーションを再生する", this, UpdateGimmickTest, 0, GT_STATE_ACTION);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"障害物の状態をリセットする", this, UpdateGimmickTest, 0, GT_STATE_RESET);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"障害物当たり判定", &m_dbvIsCollisionCheckOn);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"障害物表示_全体", &m_dbvIsShowGimmickAll);
    for(u32 i = 0; i < Model::Gimmick::GimmickBase::TYPE_MAX; i++)
    {
      if(i == Model::Gimmick::GimmickBase::TYPE_COMMON)
      {
        continue;
      }
      wchar_t* strName = L"";
      wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
      switch(i)
      {
      case Model::Gimmick::GimmickBase::TYPE_COMMON:      strName = L"通用型";       break;
      case Model::Gimmick::GimmickBase::TYPE_REEF_S:      strName = L"岩礁(小)";     break;
      case Model::Gimmick::GimmickBase::TYPE_REEF_M:      strName = L"岩礁(中)";     break;
      case Model::Gimmick::GimmickBase::TYPE_REEF_L:      strName = L"岩礁(大)";     break;
      case Model::Gimmick::GimmickBase::TYPE_MENOKURAGE:  strName = L"メノクラゲ";   break;
      case Model::Gimmick::GimmickBase::TYPE_HOERUKO:     strName = L"ホエルコ";     break;
      case Model::Gimmick::GimmickBase::TYPE_SAMEHADER:   strName = L"サメハダー";   break;
      case Model::Gimmick::GimmickBase::TYPE_HOERUOH:     strName = L"ホエルオー";   break;
      }
      swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, L"障害物表示_%ls", strName);
      gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, strWork, &m_dbvIsShowGimmick[i]);
    }
  }

  // 5.エフェクトテスト
  {
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"5.エフェクトテスト", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"エフェクトを再生する(Index=[%d])", this, UpdateEffectTest, DrawEffectTest, ET_PLAY_EFFECT);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"エフェクトを停止する", this, UpdateEffectTest, DrawEffectTest, ET_STOP_EFFECT);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"    [←→]Index変更 [A]決定", 0, 0, 0);
  }

  // 6.天球設定
  {
    SkyBox::SkyBoxManager::Time time = m_pAppManager->GetSkyBoxManager()->GetTimeDefault();
    m_dbvSkyBoxTimeHour = time.m_hour;
    m_dbvSkyBoxTimeMinute = time.m_minute;
    m_dbvSkyBoxTimeSecond = time.m_second;
    gfl2::math::Vector3 color = m_pAppManager->GetSkyBoxManager()->GetLightingColor(m_pAppManager->GetSkyBoxManager()->GetTimeZone(time));
    m_dbvLightingColorR = color.x;
    m_dbvLightingColorG = color.y;
    m_dbvLightingColorB = color.z;
    m_dbvSkyBoxDay = m_pAppManager->GetSkyBoxManager()->GetDayDefault();
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"6.天球設定", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"時間帯", this, UpdateSkyBoxTimeZone, DrawSkyBoxTimeZone);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"時間(時)", &m_dbvSkyBoxTimeHour, 0, 23, 1);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"時間(分)", &m_dbvSkyBoxTimeMinute, 0, 59, 1);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"時間(秒)", &m_dbvSkyBoxTimeSecond, 0, 59, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"サーフ開始時の時間にリセット", this, UpdateSkyBoxTimeReset);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"ライトR", &m_dbvLightingColorR, 0.0f, 1.00f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"ライトG", &m_dbvLightingColorG, 0.0f, 1.00f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"ライトB", &m_dbvLightingColorB, 0.0f, 1.00f, 0.01f);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"ライトRGBをリセット", this, UpdateLightColorReset);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"月相", this, UpdateSkyBoxMoon, DrawSkyBoxMoon);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"日付", &m_dbvSkyBoxDay, 1, 31, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"日付をリセット", this, UpdateSkyBoxMoonReset);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"Y軸回転(deg)", &m_dbvSkyBoxRotationY, -180, 180, 1);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"Y軸座標(cm)", &m_dbvSkyBoxPositionY, -100000.0f, 100000.0f, 10.0f);
  }

  // 7.背景設定
  {
    m_dbvBgSeaScale = Background::BackgroundManager::DEFAULT_SEA_SCALE;
    m_dbvBgIslandScale = Background::BackgroundManager::MAX_ISLAND_SCALE;
    m_dbvBgIslandOffsetX = Background::BackgroundManager::DEFAULT_ISLAND_OFFSET.x;
    m_dbvBgIslandOffsetY = Background::BackgroundManager::DEFAULT_ISLAND_OFFSET.y;
    m_dbvBgIslandOffsetZ = Background::BackgroundManager::DEFAULT_ISLAND_OFFSET.z;
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"7.背景設定", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"海のスケール", &m_dbvBgSeaScale, 0.0f, 10000.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"島のスケール", &m_dbvBgIslandScale, 0.0f, 10000.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"島のオフセットX", &m_dbvBgIslandOffsetX, 0.0f, 1000000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"島のオフセットY", &m_dbvBgIslandOffsetY, 0.0f, 1000000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"島のオフセットZ", &m_dbvBgIslandOffsetZ, 0.0f, 1000000.0f, 10.0f);
  }

  // 8.パラメータ設定
  {
    m_dbvSurfRightBlendFrame = Model::ModelPlayer::SURF_RIGHT_MOTION_BLEND_FRAME_LONG;
    m_dbvScoreRateBase = Player::PlayerController::SCORE_RATE_BASE;
    m_dbvScoreRateInput = Player::PlayerController::SCORE_RATE_INPUT;
    m_dbvScoreRateSpeed = Player::PlayerController::SCORE_RATE_SPEED;
    m_dbvScoreRateLanding = Player::PlayerController::SCORE_RATE_LANDING;
    m_jumpSpeedStart = Player::PlayerController::JUMP_SPEED_MIN[0];
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"8.パラメータ設定", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"加速度(cm/s^2)",   &m_dbvSurfParamAccelForward, 0.0f, 100000.0f, 1.0f);
    for(u32 i = 0; i < MantainSurfAppData::SKILL_SCORE_RANK_MAX; i++)
    {
      wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
      swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, L"ベース得点率(ランク%d)", i + 1);
      m_dbvScoreRateRank[i] = MantainSurfAppData::GetSkillScoreRate(i);
      gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, strWork, &m_dbvScoreRateRank[i], 0.0f, 100.0f, 0.01f);
    }
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"得点係数(確定)", &m_dbvScoreRateBase, 0.0f, 100.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"得点係数(入力)", &m_dbvScoreRateInput, 0.0f, 100.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"得点係数(速度)", &m_dbvScoreRateSpeed, 0.0f, 100.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"得点係数(着水)", &m_dbvScoreRateLanding, 0.0f, 100.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転半径(上り)", &m_dbvSurfParamTurnRadiusUpward, 50.0f, 100000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転半径(下り)", &m_dbvSurfParamTurnRadiusDownward, 50.0f, 100000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転速度(上り)", &m_dbvSurfParamTurnSpeedUpward, 50.0f, 100000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転速度(下り)", &m_dbvSurfParamTurnSpeedDownward, 50.0f, 100000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転角度(上り)", &m_dbvSurfParamTurnAngleUpward, -180.0f, 0.0f, 1.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転角度(下り)", &m_dbvSurfParamTurnAngleDownward, 0.0f, 180.0f, 1.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転係数(上り)", &m_dbvSurfParamTurnFactorUpward, 0.0f, 100.0f, 0.05f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転係数(下り)", &m_dbvSurfParamTurnFactorDownward, 0.0f, 100.0f, 0.05f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"回転加(減)速度", &m_dbvSurfParamTurnSpeedAccel, 0.0f, 100000.0f, 1.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"ボトムターン回転半径の倍率", &m_dbvSurfParamTurnAccelRadiusMag, 0.0f, 100.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"着水時操作の機敏性", &m_dbvSurfParamLandingAgility, 0.0f, 100.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"波右移動モーションブレンドフレーム数", &m_dbvSurfRightBlendFrame, 0, 100, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"▼滞空時間計算▼", 0, 0);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"　ジャンプ初速度(cm/s)", &m_jumpSpeedStart, 0.0f, 99999999.0f, 1.0f);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　滞空時間=%d(frame)", this, 0, DrawCalcJumpTime);
  }

  // 9.カメラ設定
  {
    m_dbvCameraFovy = gfl2::math::ConvRadToDeg(Camera::Camera::DEFAULT_FOVY);
    m_dbvCameraNear = Camera::Camera::DEFAULT_NEAR;
    m_dbvCameraFar = Camera::Camera::DEFAULT_FAR;
    m_dbvCameraNormalLength = Camera::CameraController::ZOOM_LENGTH_NORMAL;
    m_dbvCameraZoomStep = Camera::CameraController::ZOOM_LENGTH_STEP;
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"9.カメラ設定", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"画角(度)", &m_dbvCameraFovy, 0.0f, 180.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"Near(cm)", &m_dbvCameraNear, 0.0f, 1000000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"Far(cm)", &m_dbvCameraFar, 0.0f, 1000000.0f, 10.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"通常距離(cm)", &m_dbvCameraNormalLength, 0.0f, 1000000.0f, 1.0f);
    gfl2::debug::DebugWin_AddItemValueFloat(pWindowGroup, m_pHeap, L"ズーム単位距離(cm)", &m_dbvCameraZoomStep, 0.0f, 1000000.0f, 1.0f);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"カメラ操作で十字キーオフセットモード", &m_dbvIsCameraOffsetMode);
  }

  // 10.サウンドテスト
  {
    m_seIndex1 = m_seIndex2 = 0;
    m_seIndexNow1 = m_seIndexNow2 = 0;
    m_sePitch1 = m_sePitch2 = 1.0f;
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"10.サウンドテスト", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"SE1を再生する[%02d.%ls]", this, UpdateSoundTest, DrawSoundTest, ST_PLAY_SE1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"SE2を再生する[%02d.%ls]", this, UpdateSoundTest, DrawSoundTest, ST_PLAY_SE2);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"SE1_PITCH[%.2f]", this, UpdateSoundTest, DrawSoundTest, ST_SE_CHANGE_PITCH1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"SE2_PITCH[%.2f]", this, UpdateSoundTest, DrawSoundTest, ST_SE_CHANGE_PITCH2);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"SE1を停止する", this, UpdateSoundTest, 0, ST_STOP_SE1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"SE2を停止する", this, UpdateSoundTest, 0, ST_STOP_SE2);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"すべてのSEを停止する", this, UpdateSoundTest, 0, ST_STOP_ALL_SE);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"BGMを再生する", this, UpdateSoundTest, 0, ST_PLAY_BGM);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"BGMを停止する", this, UpdateSoundTest, 0, ST_STOP_BGM);
  }

  // 11.技解放フラグ設定
  {
    m_isAllSkillUnlocked = false;
#ifndef GF_CONFIG_RELEASE
    m_isAllSkillUnlocked = true;
    SetSkillSysFlg(SS_SKILL_UNLOCK_ALL, true);
#endif
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"11.技解放フラグ設定", m_pRootDebugWinGroup);
    // 技解放設定
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"すべての技を%lsする", this, UpdateSkillSetting, DrawSkillSetting, SS_SKILL_UNLOCK_ALL);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"%lsまでを%lsする[%ls中]", this, UpdateSkillSetting, DrawSkillSetting, SS_SKILL_UNLOCK_01);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"%lsまでを%lsする[%ls中]", this, UpdateSkillSetting, DrawSkillSetting, SS_SKILL_UNLOCK_02);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"%lsまでを%lsする[%ls中]", this, UpdateSkillSetting, DrawSkillSetting, SS_SKILL_UNLOCK_03);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"%lsまでを%lsする[%ls中]", this, UpdateSkillSetting, DrawSkillSetting, SS_SKILL_UNLOCK_04);
  }

  // 12.その他設定
  {
    m_dbvIsAutoPlayMode = false;
    m_dbvIsSyncWavePosition = true;
    m_dbvIsFixSkillInputFactor = false;
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"12.その他設定", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"オートプレイモード", &m_dbvIsAutoPlayMode);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"波位置同期", &m_dbvIsSyncWavePosition);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"技入力係数を常に最大", &m_dbvIsFixSkillInputFactor);
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"繰り返し起動テスト", &m_dbvIsRestartTest);
    gfl2::debug::DebugWin_AddItemValueInt(pWindowGroup, m_pHeap, L"繰り返し起動間隔(frame)", &m_dbvRestartInterval, 150, 10000000, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"Z座標をリセットする", this, UpdateOther, 0, OTH_RESET_Z_POS);
  }

  // 13.ホットキー
  {
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"13.ホットキー", m_pRootDebugWinGroup);
    gfl2::debug::DebugWin_AddItemValueBool (pWindowGroup, m_pHeap, L"ホットキー有効", &m_dbvIsHotKeyEnable);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[通常ホットキー一覧(3DSボタンベース)]", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Bボタン：押下している間は常に加速状態", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Xボタン：サーフ状態/自由移動状態の切り替え", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Yボタン：通常モード/無限モードの切り替え", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Lボタン：カメラリセット", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Rボタン：押しながら左スティックでカメラ回転", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  STARTボタン：障害物有無の切り替え", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  STARTボタン長押し：コース終了", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[エアリアルホットキー一覧(3DSボタンベース)]", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Aボタン：スクリューハンテール", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Bボタン：アシレーヌフリップ", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Xボタン：ランターン360", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  Yボタン：スクリューサクラビス", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  十字キー↑：オーバーザギャラドス", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  十字キー↓：コイキングスペシャル", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  十字キー←：スターミートルネード", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"  十字キー→：スターミートルネード", 0, 0);
  }

  // 14.技回転ブレンド設定
  {
    m_dbvIsUseSkillZRotBlend = true;
    InitSkillRotBlendTable();
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"14.技回転ブレンド設定", m_pRootDebugWinGroup);
    // 技Z軸回転ブレンド
    gfl2::debug::DebugWin_AddItemValueBool(pWindowGroup, m_pHeap, L"技回転ブレンド", &m_dbvIsUseSkillZRotBlend);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"設定する技[%d.%ls]", this, UpdateSkillRotBlend, DrawSkillRotBlend, -1);
    // 段階1
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[垂直→水平]", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　開始フレーム[%d]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　終了フレーム[%d]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　補間方式[%ls]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 2);
    // 段階2
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[水平保持]", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　開始フレーム[%d]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 3);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　終了フレーム[%d]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 4);
    // 段階3
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[水平→垂直]", 0, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　開始フレーム[%d]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 6);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　終了フレーム[%d]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 7);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"　補間方式[%ls]", this, UpdateSkillRotBlend, DrawSkillRotBlend, 8);
  }

  // 15.技発動情報
  {
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"15.技発動情報", m_pRootDebugWinGroup);
    for(u32 i = 0; i < MantainSurfAppData::SKILL_ID_MAX; i++)
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[%ls]得点ランク%d 発動回数%d", this, 0, DrawSkillInfo, i);
    }
  }

  // 16.障害物情報
  {
    gfl2::debug::DebugWinGroup* pWindowGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"16.障害物情報", m_pRootDebugWinGroup);
    for(u32 i = 0; i < Model::Gimmick::GimmickBase::TYPE_MAX; i++)
    {
      if(i == Model::Gimmick::GimmickBase::TYPE_COMMON)
      {
        continue;
      }
      wchar_t* strName = L"";
      wchar_t* strWork = gfl2::debug::DebugWin_GetWorkStr();
      switch(i)
      {
      case Model::Gimmick::GimmickBase::TYPE_COMMON:      strName = L"通用型";       break;
      case Model::Gimmick::GimmickBase::TYPE_REEF_S:      strName = L"岩礁(小)";     break;
      case Model::Gimmick::GimmickBase::TYPE_REEF_M:      strName = L"岩礁(中)";     break;
      case Model::Gimmick::GimmickBase::TYPE_REEF_L:      strName = L"岩礁(大)";     break;
      case Model::Gimmick::GimmickBase::TYPE_MENOKURAGE:  strName = L"メノクラゲ";   break;
      case Model::Gimmick::GimmickBase::TYPE_HOERUKO:     strName = L"ホエルコ";     break;
      case Model::Gimmick::GimmickBase::TYPE_SAMEHADER:   strName = L"サメハダー";   break;
      case Model::Gimmick::GimmickBase::TYPE_HOERUOH:     strName = L"ホエルオー";   break;
      }
      swprintf(strWork, gfl2::debug::DEBUGWIN_NAME_LEN, L"[%ls]%ls", strName, L"活動数[%d] 表示数[%d]");
      gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, strWork, this, 0, DrawGimmickInfo, i);
    }
    gfl2::debug::DebugWin_AddItemUpdateFunc(pWindowGroup, m_pHeap, L"[全体]活動数[%d] 表示数[%d]", this, 0, DrawGimmickInfo, Model::Gimmick::GimmickBase::TYPE_MAX);
  }
}

void DebugMenu::InitSkillRotBlendTable()
{
  for(u32 skillIndex = 0; skillIndex < SRB_CONST_MAX_SKILL_NUM; skillIndex++)
  {
    u32 startIndex = skillIndex * SRB_CONST_MAX_KEY_FRAME_NUM * 3;
    MantainSurfAppData::SkillId skillId = MantainSurfAppData::SKILL_ID_NONE;
    switch(skillIndex)
    {
    case 0: skillId = MantainSurfAppData::SKILL_ID_EX_04; break;
    case 1: skillId = MantainSurfAppData::SKILL_ID_EX_05; break;
    case 2: skillId = MantainSurfAppData::SKILL_ID_EX_06; break;
    case 3: skillId = MantainSurfAppData::SKILL_ID_EX_07; break;
    }
    const f32 *blendTable;
    u32 keyFrameCount;
    m_pAppManager->GetPlayerController()->GetSkillZRotationBlendTable(skillId, &blendTable, &keyFrameCount);
    GFL_ASSERT(keyFrameCount == SRB_CONST_MAX_KEY_FRAME_NUM);
    for(u32 i = 0; i < keyFrameCount * 3; i++)
    {
      m_dbvSkillRotBlendTables[startIndex + i] = blendTable[i];
    }
  }
  m_curSkillRotBlendTable = m_dbvSkillRotBlendTables + m_curSkillRotBlendTableIndex * SRB_CONST_MAX_KEY_FRAME_NUM * 3;
}

MantainSurfAppManager* DebugMenu::GetAppManager()
{
  return m_pAppManager;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // PM_DEBUG