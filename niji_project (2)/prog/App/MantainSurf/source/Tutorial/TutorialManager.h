//======================================================================
/**
 * @file TutorialManager.h
 * @date 2017/2/6 20:42:00
 * @author fang_yicheng
 * @brief チュートリアルマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TUTORIAL_MANAGER_H_INCLUDED__
#define __TUTORIAL_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include "../../include/MantainSurfAppData.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Tutorial)

/**
 * @class TutorialManager
 * @brief チュートリアルマネージャー
 */
class TutorialManager
{
  GFL_FORBID_COPY_AND_ASSIGN(TutorialManager);

public:

  // 状態
  enum State
  {
    ST_NONE = -1,
    ST_START,               // 全体流れの説明
    ST_SPEED_UP_BEFORE,     // 速度アップ説明
    ST_SPEED_UP_AFTER,      // 速度アップ補足説明
    ST_JUMP_BEFORE,         // ジャンプ説明
    ST_JUMP_AFTER,          // ジャンプ補足説明
    ST_AERIAL_BEFORE,       // エアリアル説明
    ST_AERIAL_AFTER,        // エアリアル補足説明
    ST_LANDING_BEFORE,      // 着水説明
    ST_LANDING_AFTER,       // 着水補足説明
    ST_END,                 // チュートリアル終了説明
    ST_FINISH,              // 完了
    ST_MAX,
    // これ以降は不要なチュートリアル項目
    ST_GIMMICK_BEFORE,      // 障害物説明
    ST_GIMMICK_AFTER,       // 障害物補足説明
  };

  enum StateSeq
  {
    SEQ_NONE = -1,
    SEQ_START_FADING_IN,  // 開始のフェイドイン中
    SEQ_INFO_OPEN,        // Infoウィンドウ表示
    SEQ_INFO_OPENED,      // Infoウィンドウ表示中
    SEQ_PRACTICE,         // 練習中
    SEQ_PRACTICE_END,     // 練習終了後処理
    SEQ_RETRY_START,      // リトライ開始
    SEQ_RETRY_FADING_OUT, // リトライのフェードアウト中
    SEQ_RETRY_FADING_IN,  // リトライのフェードイン中
    SEQ_END_FADING_OUT,   // 終了のフェードアウト中
    SEQ_END,              // 終了
  };

public:

  TutorialManager();
  virtual ~TutorialManager();

  void Initialize(MantainSurfAppManager* pAppManager);
  void StartTutorial();

  void Update();

  State GetState() { return m_stateNow; }

  bool IsTutorialFinished();
  bool IsInfoWindowOpen();
  bool IsControllEnable();
  bool IsJumpEnable();
  bool IsAerialSkillEnable();

  void OnInfoWindowClose();

private:

  void StateCheckChange();
  void StateInitialize();
  void StateExecute();

  void UpdatePractice();

  void InitializePracticeParam();
  bool IsPracticeSucceeded();

  s32  GetPracticeDuration();
  MantainSurfAppData::TutorialID GetTutorialId(State state);

private:

  MantainSurfAppManager*    m_pAppManager;

  State m_stateNow;
  State m_stateNext;
  s32   m_stateSeq;
  s32   m_waitTimer;

  struct StateParam
  {
    s32  m_duration;      // 練習用時間(フレーム)
    s32  m_retryCount;    // リトライ回数
    bool m_isSucceeded;   // 練習成功したか

    StateParam()
    {
      m_duration    = 0;
      m_retryCount  = 0;
      m_isSucceeded = false;
    }
  };
  StateParam m_stateParam;

  bool m_isResumeEffect;
};

GFL_NAMESPACE_END(Tutorial)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __TUTORIAL_MANAGER_H_INCLUDED__
