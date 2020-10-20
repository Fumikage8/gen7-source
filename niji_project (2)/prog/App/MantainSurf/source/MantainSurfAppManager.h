//======================================================================
/**
 * @file MantainSurfAppManager.h
 * @date 2016/09/13 12:20:58
 * @author fang_yicheng
 * @brief アプリケーションマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MANTAIN_SURF_APP_MANAGER_H_INCLUDED__
#define __MANTAIN_SURF_APP_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "../include/MantainSurfAppData.h"
#include "SurfSound/SurfSoundManager.h"
#include "UI/SurfUI.h"
#include "MantainSurfDrawManager.h"
#include "MantainSurfRenderingPipeline.h"
#include "Resource/ResourceManager.h"
#include "Course/CourseDataManager.h"
#include "Nami/NamiModelSet.h"
#include "Nami/NamiController.h"
#include "Effect/EffectManager.h"
#include "Player/PlayerModelSet.h"
#include "Player/PlayerController.h"
#include "NpcRider/NpcRiderSeaBread.h"
#include "Camera/Camera.h"
#include "Camera/CameraController.h"
#include "Model/ModelBackground.h"
#include "Model/Gimmick/GimmickManager.h"
#include "Background/BackgroundManager.h"
#include "Collision/CollisionManager.h"
#include "SkyBox/SkyBoxManager.h"
#include "Tutorial/TutorialManager.h"
#include "Debug/DebugMenu.h"

#include <App/AppEvent/include/MantainSurf/MantainSurfParam.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

class MantainSurfAppManager : public UI::SurfUI::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(MantainSurfAppManager);

public:

  static const f32 Z_POS_RESET_THREDSHOLD;

  enum GameState
  {
    ST_NONE = -1,
    ST_STARTING,      // 開始演出中
    ST_SURFING,       // サーフィン中
    ST_ENDING,        // 終了演出中
    ST_QUIT,          // APP終了
    ST_MAX
  };

public:

  MantainSurfAppManager();
  virtual ~MantainSurfAppManager();

  void Initialize(System::nijiAllocator* pNijiAllocator, gfl2::heap::HeapBase* pHeap, MantainSurfDrawManager* pDrawManager, UI::SurfUI* pSurfUI, App::Event::MANTAIN_SURF_PARAM* pAppParam);
  bool IsInitializeFinished();
  void Finalize();

  void Update();
  void UpdateAfterTraverse();

  void SetCourseId(s32 courseId);
  s32  GetCourseId();

  void AddScore(const u32 score);
  u32  GetTotalScore() { return m_totalScore; }

  void StopEffect();
  void ResumeEffect();

  void ShowSkillList();
  void HideSkillList();

  void ResetZPosition();

  void SetEnding();
  void SetQuit();

  bool IsTutorialMode();

  gfl2::math::Vector3               GetPlayerInitPosition();

  Camera::Camera*                   GetCamera();
  Camera::CameraController*         GetCameraController();
  Nami::NamiController*             GetNamiController();
  Nami::NamiModelSet*               GetNamiModelSet();
  Player::PlayerController*         GetPlayerController();
  Player::PlayerModelSet*           GetPlayerModelSet();
  UI::SurfUI*                       GetSurfUI();
  Model::Gimmick::GimmickManager*   GetGimmickManager();
  Effect::EffectManager*            GetEffectManager();
  SkyBox::SkyBoxManager*            GetSkyBoxManager();
  Background::BackgroundManager*    GetBackgroundManager();
  Course::CourseDataManager*        GetCourseDataManager();
  Tutorial::TutorialManager*        GetTutorialManager();
  SurfSound::SurfSoundManager*      GetSurfSoundManager();
  NpcRider::NpcRiderSeaBread*       GetSeaBreadRider();

  // 外部データ
  App::Event::MANTAIN_SURF_PARAM*   GetAppParam() const;
  GameSys::GameData*                GetGameData() const;

#if PM_DEBUG
  Debug::DebugMenu*         GetDebugMenu();
  f32                       GetTotalDistance();
  f32                       GetPlayTime() { return m_playTime; }
#endif

  GameState                 GetGameState();

private:

  virtual void SurfUI_OnEvent(UI::SurfUI* pSender, const EventCode eventCode);

private:

  bool LoadResource();
  void CreateResource();

  void UpdateState();
  void UpdateFinishTimer();
  void UpdatePosReset();

  void ExecuteStartingState();
  void ExecuteSurfingState();
  void ExecuteEndingState();

  bool IsGoal();

  u32  GetBestScore();

public:

  Nami::NamiModelSet::MapData m_MapData;

private:

  // リソースマネージャー群
  enum ResourceManagerId
  {
    RMID_CHARA_MODEL,   // キャラクターリソース
    RMID_FIELD_MODEL,   // フィールドリソース
    RMID_EFFECT,        // エフェクトリソース
    RMID_COURSE_DATA,   // コースデータリソース
    RMID_MAX,
  };

  enum StateSeq
  {
    SEQ_END = -1,
  };

  // Inputパラメータ
  System::nijiAllocator*          m_pNijiAllocator;
  gfl2::heap::HeapBase*           m_pHeap;
  MantainSurfDrawManager*         m_pDrawManager;
  App::Event::MANTAIN_SURF_PARAM* m_pAppParam;

  // 管理パラメータ
  UI::SurfUI*                 m_pSurfUI;
  MantainSurfAppData*         m_pAppData;

  gfl2::Effect::GFHeap*       m_pEffectHeap;
  gfl2::Effect::System*       m_pEffectSystem;
  gfl2::util::List<u32>*      m_pEffectGroupIDList;
  Effect::EffectManager       m_EffectManager;

  Resource::ResourceManager   m_ResourceManagerList[RMID_MAX];

  Model::Gimmick::GimmickManager  m_GimmickManager;
  Collision::CollisionManager     m_CollisionManager;
  Background::BackgroundManager   m_BackgroundManager;

  Nami::NamiModelSet         m_NamiModelSet;
  Player::PlayerModelSet     m_PlayerModelSet;
  NpcRider::NpcRiderSeaBread m_SeaBreadRider;

  Player::PlayerController  m_PlayerController;
  Nami::NamiController      m_NamiController;
  Camera::Camera            m_Camera;
  Camera::CameraController  m_CameraController;

  SkyBox::SkyBoxManager     m_SkyBoxManager;
  Tutorial::TutorialManager m_TutorialManager;

  SurfSound::SurfSoundManager  m_SurfSoundManager;

#if PM_DEBUG
  Debug::DebugMenu m_DebugMenu;
  f32              m_totalResetDistance;    // トータルでリセットした距離
  f32              m_playTime;              // コースのプレイ時間
  s32              m_restartTimer;          // 再起動テスト用タイマー
#endif

  // 非管理パラメータ
  Course::CourseDataManager m_CourseDataManager;

  s32       m_seq;
  s32       m_stateSeq;
  GameState m_stateNow;
  GameState m_stateNext;

  s32       m_courseId;
  u32       m_totalScore; // アプリ内部用累計スコア

  struct GameStateParam
  {
    f32   m_procTimer;    // 処理タイマー
    f32   m_finishTimer;  // 終了タイマー

    GameStateParam()
    {
      m_procTimer = 0.0f;
      m_finishTimer = 0.0f;
    }
  };
  GameStateParam m_stateParam;
  
};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MANTAIN_SURF_APP_MANAGER_H_INCLUDED__
