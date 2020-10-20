//======================================================================
/**
 * @file TutorialManager.cpp
 * @date 2017/2/6 20:42:00
 * @author fang_yicheng
 * @brief チュートリアルマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Fade/include/gfl2_FadeManager.h>

#include "TutorialManager.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Tutorial)

const s32 DEFAULT_RETRY_COUNT         = 1;          // リトライ回数
const s32 DEFAULT_PRACTICE_DURATION   = 30 * 30;    // 練習用時間(フレーム)

const gfl2::math::Vector4 COLOR_FADEOUT_START(0.0f, 0.0f, 0.0f, 0.0f);
const gfl2::math::Vector4 COLOR_FADEOUT_END(0.0f, 0.0f, 0.0f, 255.0f);

const u32 FADE_DURATION_SHORT = 15;
const u32 FADE_DURATION_LONG  = 30;
const u32 FADE_WAIT           = 15;
const u32 PRACTICE_END_WAIT_SHORT  = 15;
const u32 PRACTICE_END_WAIT_LONG   = 300;
const u32 SE_FADE_DURATION_SHORT = 20;
const u32 SE_FADE_DURATION_LONG  = 35;
const u32 SE_STOP_WAIT_MAX       = 3 * 30; // SE停止待ちの最大待ち時間

TutorialManager::TutorialManager() :
  m_pAppManager(NULL),
  m_stateNow(ST_NONE),
  m_stateNext(ST_NONE),
  m_stateSeq(SEQ_NONE),
  m_isResumeEffect(false),
  m_waitTimer(0)
{
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Initialize(MantainSurfAppManager* pAppManager)
{
  m_pAppManager = pAppManager;
}

void TutorialManager::StartTutorial()
{
  m_stateNext = ST_START;
}

void TutorialManager::Update()
{
  StateCheckChange();
  StateInitialize();
  StateExecute();
}

void TutorialManager::StateCheckChange()
{
  if(m_stateNow == ST_NONE)
  {
    return;
  }

  // 終了状態
  if(m_stateSeq == SEQ_END)
  {
    m_stateNext = static_cast<State>(static_cast<s32>(m_stateNow) + 1);
    if(m_stateNext == ST_MAX)
    {
      // 最後まで進んだため、完了状態に
      m_stateNow = ST_NONE;
      m_stateNext = ST_NONE;
    }
  }
}

void TutorialManager::StateInitialize()
{
  while(m_stateNext != ST_NONE)
  {
    State prevState = m_stateNow;
    m_stateNow = m_stateNext;
    m_stateNext = ST_NONE;

    if(prevState == ST_GIMMICK_AFTER)
    {
      // 障害物を消去
      m_pAppManager->GetGimmickManager()->ClearGimmickForTutorial();
    }
    if(m_stateNow == ST_GIMMICK_BEFORE)
    {
      // 障害物を作成
      m_pAppManager->GetGimmickManager()->MakeGimmickForTutorial();
    }
    if(m_stateNow >= ST_AERIAL_BEFORE)
    {
      // 技リストを表示
      m_pAppManager->ShowSkillList();
    }

    m_stateSeq = SEQ_NONE;
    m_stateParam.m_retryCount = DEFAULT_RETRY_COUNT;
    m_waitTimer = SE_STOP_WAIT_MAX;

    switch(m_stateNow)
    {
    case ST_SPEED_UP_BEFORE:
    case ST_END:
      // Infoウィンドウ表示中へ
      m_stateSeq = SEQ_INFO_OPENED;
      break;

    case ST_START:
    case ST_SPEED_UP_AFTER:
    case ST_GIMMICK_AFTER:
    case ST_JUMP_AFTER:
    case ST_AERIAL_AFTER:
    case ST_LANDING_AFTER:
      // Infoウィンドウオープンへ
      m_stateSeq = SEQ_INFO_OPEN;
      break;

    case ST_GIMMICK_BEFORE:
    case ST_JUMP_BEFORE:
    case ST_AERIAL_BEFORE:
    case ST_LANDING_BEFORE:
      // 画面フェイドインへ
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, FADE_DURATION_LONG);
      SurfSound::SurfSoundManager::ChangeMasterSEVolume(1.0f, FADE_DURATION_LONG);
      SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_SURF_ENV);
      m_stateSeq = SEQ_START_FADING_IN;
      break;
    }
  }
}

void TutorialManager::StateExecute()
{
  if(m_stateNow == ST_NONE)
  {
    return;
  }

  if(m_isResumeEffect)
  {
    m_pAppManager->ResumeEffect();
    m_isResumeEffect = false;
  }

  switch(m_stateSeq)
  {
  case SEQ_START_FADING_IN:
    if(!GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE))
    {
      // フェードイン中
      break;
    }
    m_stateSeq = SEQ_INFO_OPEN;
    /* Fall Through */

  case SEQ_INFO_OPEN:
    if(!SurfSound::SurfSoundManager::StopSoundForTutorial(false))
    {
      // サウンド停止待ち中
      m_waitTimer--;
      if(m_waitTimer > 0)
      {
        break;
      }
      // 一定時間以上待っても停止しない場合は強制停止
      SurfSound::SurfSoundManager::StopSoundForTutorial(true);
    }
    // Infoウィンドウ表示
    m_pAppManager->GetSurfUI()->StartTutorial(GetTutorialId(m_stateNow));
    m_pAppManager->StopEffect();
    m_stateSeq = SEQ_INFO_OPENED;
    break;

  case SEQ_PRACTICE:
    // 練習
    UpdatePractice();
    break;

  case SEQ_PRACTICE_END:
    // 練習完了
    if(m_waitTimer > 0)
    {
      m_waitTimer--;
    }
    if(m_pAppManager->GetPlayerController()->GetShowScoreRestTime() > 0 && m_waitTimer > 0)
    {
      // スコア表示中なら待ち合わせる(最大m_waitTimerまで待つ)
      break;
    }
    m_stateSeq = SEQ_END;
    break;

  case SEQ_RETRY_START:
    // リトライ
    if(m_pAppManager->GetPlayerController()->GetShowScoreRestTime() > 0)
    {
      // UI表示完了まで待つ
      break;
    }
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestOut(
      gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &COLOR_FADEOUT_START, &COLOR_FADEOUT_END, FADE_DURATION_SHORT);
    m_waitTimer = FADE_WAIT;
    SurfSound::SurfSoundManager::ChangeMasterSEVolume(0.0f, SE_FADE_DURATION_SHORT);
    m_stateSeq = SEQ_RETRY_FADING_OUT;
    break;

  case SEQ_RETRY_FADING_OUT:
    if(!GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_UPPER))
    {
      // フェードアウト中
      break;
    }
    if(m_waitTimer == FADE_WAIT)
    {
      m_pAppManager->GetPlayerController()->ResetPlayerState(false);
      SurfSound::SurfSoundManager::StopAllSE();
    }
    if(m_waitTimer > 0)
    {
      // 少し待つ
      m_waitTimer--;
      break;
    }
    // フェイドイン
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, FADE_DURATION_SHORT);
    SurfSound::SurfSoundManager::ChangeMasterSEVolume(1.0f, FADE_DURATION_SHORT);
    SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_SURF_ENV);
    m_stateSeq = SEQ_RETRY_FADING_IN;
    break;

  case SEQ_RETRY_FADING_IN:
    if(!GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_UPPER))
    {
      break;
    }
    // フェイドイン完了
    m_waitTimer = SE_STOP_WAIT_MAX;
    m_stateSeq = SEQ_INFO_OPEN;
    break;

  case SEQ_END_FADING_OUT:
    if(!GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE))
    {
      // フェードアウト中
      break;
    }
    if(m_waitTimer == FADE_WAIT)
    {
      m_pAppManager->GetPlayerController()->ResetPlayerState(m_stateNow == ST_END);
      SurfSound::SurfSoundManager::StopAllSE();
    }
    if(m_waitTimer > 0)
    {
      // 少し待つ
      m_waitTimer--;
      break;
    }
    m_stateSeq = SEQ_END;
    break;
  }
}

void TutorialManager::UpdatePractice()
{
  m_stateParam.m_duration--;
  if(m_stateParam.m_duration < 0)
  {
    m_stateParam.m_duration = 0;
  }
  m_stateParam.m_isSucceeded = IsPracticeSucceeded();
  // 結果チェック
  if(m_stateParam.m_isSucceeded || (m_stateParam.m_duration == 0 && m_stateParam.m_retryCount == 0))
  {
    // 練習終了
    switch(m_stateNow)
    {
    case ST_AERIAL_BEFORE:
      m_waitTimer = PRACTICE_END_WAIT_LONG;
      m_stateSeq = SEQ_PRACTICE_END;
      break;
    case ST_LANDING_BEFORE:
      m_waitTimer = PRACTICE_END_WAIT_SHORT;
      m_stateSeq = SEQ_PRACTICE_END;
      break;
    default:
      m_stateSeq = SEQ_END;
      break;
    }
  }
  else if(m_stateParam.m_duration == 0 && m_stateParam.m_retryCount > 0)
  {
    // リトライへ
    if(m_pAppManager->GetPlayerController()->IsSurfingState())
    {
      m_stateParam.m_retryCount--;
      m_stateSeq = SEQ_RETRY_START;
    }
  }
}

bool TutorialManager::IsPracticeSucceeded()
{
#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && !gfl2::debug::DebugWin_IsOpen())
  {
    if(pButton->IsTrigger(gfl2::ui::BUTTON_Y))
    {
      // 即リトライ
      m_stateParam.m_duration = 0;
      return false;
    }
    else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      // 即成功
      return true;
    }
  }
#endif

  switch(m_stateNow)
  {
  case ST_SPEED_UP_BEFORE:
    // 速度アップ練習の完了チェック
    return m_pAppManager->GetPlayerController()->GetSpeedRate() >= 0.7f;

  case ST_GIMMICK_BEFORE:
    // 障害物練習の完了チェック
    return m_pAppManager->GetGimmickManager()->IsReachTutorialEndPosition();

  case ST_JUMP_BEFORE:
    // ジャンプ練習の完了チェック
    return m_pAppManager->GetPlayerController()->IsJumpOverTop();

  case ST_AERIAL_BEFORE:
    // エアリアル練習の完了チェック
    return m_pAppManager->GetPlayerController()->IsExSkillFinished();

  case ST_LANDING_BEFORE:
    // 着水成功練習の完了チェック
    return m_pAppManager->GetPlayerController()->IsLandingSucceeded();

  default:
    return false;
  }
}

bool TutorialManager::IsTutorialFinished()
{
  return m_stateNow == ST_FINISH;
}

bool TutorialManager::IsInfoWindowOpen()
{
  return m_stateNow != ST_NONE && m_stateSeq == SEQ_INFO_OPENED;
}

bool TutorialManager::IsControllEnable()
{
  return m_stateNow != ST_NONE && m_stateSeq == SEQ_PRACTICE;
}

bool TutorialManager::IsJumpEnable()
{
  return m_stateNow >= ST_JUMP_BEFORE && m_stateSeq == SEQ_PRACTICE;
}

bool TutorialManager::IsAerialSkillEnable()
{
  if(m_stateSeq == SEQ_PRACTICE)
  {
    if(m_stateNow == ST_AERIAL_BEFORE)
    {
      MantainSurfAppData::SkillId skillId = m_pAppManager->GetPlayerController()->GetAerialParam()->m_skillIdNow;
      if(skillId == MantainSurfAppData::SKILL_ID_NONE || skillId >= MantainSurfAppData::SKILL_ID_NORMAL_01)
      {
        // エアリアル練習段階で、大技未発動の場合は技入力可能
        return true;
      }
    }
    else if(m_stateNow == ST_LANDING_BEFORE)
    {
      // 着水練習段階では常に技発動可能
      return true;
    }
  }
  return false;
}

void TutorialManager::OnInfoWindowClose()
{
  m_isResumeEffect = true;
  switch(m_stateNow)
  {
  case ST_START:
    // Infoウィンドウ表示
    m_pAppManager->GetSurfUI()->StartTutorial(GetTutorialId(ST_SPEED_UP_BEFORE));
    m_isResumeEffect = false;
    m_stateSeq = SEQ_END;
    break;
  case ST_LANDING_AFTER:
    // Infoウィンドウ表示
    m_pAppManager->GetSurfUI()->StartTutorial(GetTutorialId(ST_END));
    m_isResumeEffect = false;
    m_stateSeq = SEQ_END;
    break;

  case ST_SPEED_UP_BEFORE:
  case ST_GIMMICK_BEFORE:
  case ST_JUMP_BEFORE:
  case ST_AERIAL_BEFORE:
  case ST_LANDING_BEFORE:
    // サウンド再開
    SurfSound::SurfSoundManager::ResumeSoundForTutorial();
    // 練習へ
    InitializePracticeParam();
    m_stateSeq = SEQ_PRACTICE;
    break;

  case ST_SPEED_UP_AFTER:
  case ST_GIMMICK_AFTER:
  case ST_JUMP_AFTER:
  case ST_AERIAL_AFTER:
  case ST_END:
    // サウンド再開
    SurfSound::SurfSoundManager::ResumeSoundForTutorial();
    // 全画面暗転へ
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestOut(
      gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &COLOR_FADEOUT_START, &COLOR_FADEOUT_END, FADE_DURATION_LONG);
    m_waitTimer = FADE_WAIT;
    SurfSound::SurfSoundManager::ChangeMasterSEVolume(0.0f, SE_FADE_DURATION_LONG);
    m_stateSeq = SEQ_END_FADING_OUT;
    break;
  }
}

void TutorialManager::InitializePracticeParam()
{
  m_stateParam.m_duration = GetPracticeDuration();
  m_stateParam.m_isSucceeded = false;
}

s32 TutorialManager::GetPracticeDuration()
{
  // 練習用時間を取得(フレーム)
  switch(m_stateNow)
  {
  case ST_SPEED_UP_BEFORE:
  case ST_GIMMICK_BEFORE:
  case ST_JUMP_BEFORE:
  case ST_LANDING_BEFORE:
    return DEFAULT_PRACTICE_DURATION;
  case ST_AERIAL_BEFORE:
    return DEFAULT_PRACTICE_DURATION * 2;
  default:
    GFL_ASSERT(0);
    return 0;
  }
}

MantainSurfAppData::TutorialID TutorialManager::GetTutorialId(State state)
{
  switch(state)
  {
  case ST_START:
    HOU_PRINT("[Tutorial]全体説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info01;
  case ST_SPEED_UP_BEFORE:
    HOU_PRINT("[Tutorial]速度アップ説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info02;
  case ST_SPEED_UP_AFTER:
    HOU_PRINT("[Tutorial]速度アップ補足説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info03;
  case ST_GIMMICK_BEFORE:
    HOU_PRINT("[Tutorial]障害物説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info04;
  case ST_GIMMICK_AFTER:
    HOU_PRINT("[Tutorial]障害物補足説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info05;
  case ST_JUMP_BEFORE:
    HOU_PRINT("[Tutorial]ジャンプ説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info06;
  case ST_JUMP_AFTER:
    HOU_PRINT("[Tutorial]ジャンプ補足説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info07;
  case ST_AERIAL_BEFORE:
    HOU_PRINT("[Tutorial]エアリアル説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info08;
  case ST_AERIAL_AFTER:
    HOU_PRINT("[Tutorial]エアリアル補足説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info09;
  case ST_LANDING_BEFORE:
    HOU_PRINT("[Tutorial]着水説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info10;
  case ST_LANDING_AFTER:
    HOU_PRINT("[Tutorial]着水補足説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info11;
  case ST_END:
    HOU_PRINT("[Tutorial]チュートリアル終了説明\r\n");
    return MantainSurfAppData::TUTORIAL_Info12;
  default:
    GFL_ASSERT(0);
    return static_cast<MantainSurfAppData::TutorialID>(0);
  }
}

GFL_NAMESPACE_END(Tutorial)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
