//======================================================================
/**
 * @file MantainSurfAppManager.cpp
 * @date 2016/09/13 12:20:41
 * @author fang_yicheng
 * @brief アプリケーションマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "MantainSurfAppManager.h"
#include <GameSys/include/GameManager.h>

#include <ui/include/gfl2_UI_Button.h>
#include <math/include/gfl2_math_control.h>

#include <Effect/include/gfl2_EffectRenderPath.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

const f32 MantainSurfAppManager::Z_POS_RESET_THREDSHOLD = 40000.0f;        // Z軸座標リセット閾値

const f32 ENDING_CAMERA_MIN_TRACING_DURATION = 70.0f;   // 終了演出時のカメラ最小追従時間(フレーム)
const f32 ENDING_CAMERA_MAX_TRACING_DURATION = 100.0f;  // 終了演出時のカメラ最大追従時間(フレーム)
const u32 ENDING_SE_FADE_OUT_FRAME           = 45;      // 終了時のSEフェードアウト時間(フレーム)

MantainSurfAppManager::MantainSurfAppManager() :
  m_seq(0),
  m_stateSeq(0),
  m_courseId(0),
  m_totalScore(0),
  m_stateNow(ST_NONE),
  m_stateNext(ST_NONE),
  m_pNijiAllocator(NULL),
  m_pHeap(NULL),
  m_pDrawManager(NULL),
  m_pSurfUI(NULL),
  m_pAppParam(NULL),
  m_pEffectHeap(NULL),
  m_pEffectGroupIDList(NULL),
  m_pEffectSystem(NULL)
{
#if PM_DEBUG
  m_totalResetDistance = 0.0f;
  m_playTime = 0.0f;
  m_restartTimer = 0;
#endif
}

MantainSurfAppManager::~MantainSurfAppManager()
{
}

void MantainSurfAppManager::Finalize()
{
  // 天球
  m_SkyBoxManager.Terminate();

  // カメラ
  m_CameraController.Terminate();
  m_Camera.Terminate();

  // モデル
  m_GimmickManager.Terminate();

  // プレイヤー
  m_PlayerModelSet.Terminate();

  // 海パン野郎
  m_SeaBreadRider.Terminate();
  
  // マップ
  m_NamiModelSet.Destory();

  // エフェクト
  GFL_SAFE_DELETE(m_pEffectGroupIDList);
  m_EffectManager.Terminate();
  GFL_SAFE_DELETE(m_pEffectSystem);
  GFL_SAFE_DELETE(m_pEffectHeap);

  // 背景
  m_BackgroundManager.Terminate();

  // コリジョン
  m_CollisionManager.Terminate();

  // リソース
  for(u32 i = 0; i < RMID_MAX; i++)
  {
    m_ResourceManagerList[i].Terminate();
  }
}

void MantainSurfAppManager::Initialize(System::nijiAllocator* pNijiAllocator, gfl2::heap::HeapBase* pHeap, MantainSurfDrawManager* pDrawManager, UI::SurfUI* pSurfUI, App::Event::MANTAIN_SURF_PARAM* pAppParam)
{
  m_seq = 0;
  m_pNijiAllocator = pNijiAllocator;
  m_pHeap = pHeap;
  m_pDrawManager = pDrawManager;
  m_pAppParam = pAppParam;
  m_pSurfUI = pSurfUI;
  m_pSurfUI->SetEventHandler(this);
  m_pDrawManager->GetRenderingPipeline()->SetAppManager(this);
}

bool MantainSurfAppManager::IsInitializeFinished()
{
  if(!LoadResource())
  {
    return false;
  }
  m_stateNext = ST_STARTING;
#if PM_DEBUG
  m_DebugMenu.Initialize(m_pHeap, this);
#endif
  return true;
}

bool MantainSurfAppManager::LoadResource()
{
  switch(m_seq)
  {
  case 0:
    {
      // リソースマネージャー初期化
      static const u32 PARAM_SIZE = 4;
      static const u32 PARAM[RMID_MAX * PARAM_SIZE] = {
        // ------------------------------------------------
        ARCID_MANTAIN_SURF_CHAR_DATA,       // ARCID
        GARC_mantain_chara_DATA_NUM,        // データ数
        false,                              // 圧縮フラグ
        128,                                // align
        // ------------------------------------------------
        ARCID_MANTAIN_SURF_FIELD_DATA,      // ARCID
        GARC_mantain_field_DATA_NUM,        // データ数
        false,                              // 圧縮フラグ
        128,                                // align
        // ------------------------------------------------
        ARCID_MANTAIN_SURF_EFFECT_DATA,     // ARCID
        GARC_mantain_effect_DATA_NUM,       // データ数
        true,                               // 圧縮フラグ
        128,                                // align
        // ------------------------------------------------
        ARCID_MANTAIN_SURF_COURSE_DATA,     // ARCID
        GARC_mantain_course_data_DATA_NUM,  // データ数
        false,                              // 圧縮フラグ
        128,                                // align
        // ------------------------------------------------
      };
      for(u32 i = 0; i < RMID_MAX; i++)
      {
        u32 offset = i * PARAM_SIZE;
        m_ResourceManagerList[i].Initialize(m_pHeap, PARAM[offset], PARAM[offset + 1], PARAM[offset + 2], PARAM[offset + 3]);
      }
      m_seq++;
    }
    /* Fall Through */

  case 1:
    {
      // リソースをロード
      bool isLoadFinished = true;
      for(u32 i = 0; i < RMID_MAX; i++)
      {
        isLoadFinished &= m_ResourceManagerList[i].LoadResource();
      }
      if(!isLoadFinished)
      {
        // リソースロード中
        return false;
      }
      m_seq++;
    }
    /* Fall Through */

  case 2:
    {
      // リソースを作成
      CreateResource();
      m_seq++;
    }
    /* Fall Through */

  case 4:
    // ローディング完了
    return true;

  default:
    return false;
  }
}

void MantainSurfAppManager::CreateResource()
{
  MantainSurfRenderingPipeline* pRenderingPipeLine = m_pDrawManager->GetRenderingPipeline();

  // サウンドマネージャーを初期化
  {
    m_SurfSoundManager.Initialize(this);
  }

  // マップ初期情報の設定
  {
    m_MapData.m_mapDepth = 10000.0f;     // マップ奥行きの長さ(cm)
    m_MapData.m_mapWidth = 4000.0f;
    m_MapData.m_horizonHeight = 0.0f;
    m_MapData.m_mapInitPos = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
    m_MapData.m_playerInitPos = gfl2::math::Vector3(m_MapData.m_mapWidth / 2.0f * 1.125f, 0.0f, m_MapData.m_mapDepth / 2.0f);
  }

  // コースデータマネージャーを初期化
  {
    m_CourseDataManager.Initialize(m_pHeap, &m_ResourceManagerList[RMID_COURSE_DATA]);
    m_CourseDataManager.ChangeCourse(m_courseId);
  }

  // コリジョンマネージャーを初期化
  {
    m_CollisionManager.Initialize(m_pHeap, this);
  }

  // エフェクトシステムを初期化
  {
    static const u32 EMITTER_SET_NUM = 64;
    static const u32 EMITTER_NUM = EMITTER_SET_NUM * 8;
    static const u32 PARTICLE_NUM = 2048;
    static const u32 STRIPE_NUM = 64;

    m_pEffectHeap = GFL_NEW(m_pHeap) gfl2::Effect::GFHeap(m_pHeap);
    gfl2::Effect::Config effect_config;
    effect_config.SetEffectHeap(m_pEffectHeap);
#if defined(GF_PLATFORM_CTR)
    effect_config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
#endif
    effect_config.SetEmitterNum(EMITTER_NUM);
    effect_config.SetEmitterSetNum(EMITTER_SET_NUM);
    effect_config.SetParticleNum(PARTICLE_NUM);
    effect_config.SetStripeNum(STRIPE_NUM);
    effect_config.SetResourceNum(GARC_mantain_effect_DATA_NUM);
    m_pEffectSystem = GFL_NEW(m_pHeap) gfl2::Effect::System(effect_config);

    Effect::EffectManager::SetupParam setupParam;
    setupParam.m_pGfxAllocator = m_pNijiAllocator;
    setupParam.m_pEffectHeap = m_pEffectHeap;
    setupParam.m_pEffectSystem = m_pEffectSystem;
    setupParam.m_pResourceManager = &m_ResourceManagerList[RMID_EFFECT];
    setupParam.m_pRenderingPipeline = pRenderingPipeLine;
    setupParam.m_pAppManager = this;
    m_EffectManager.Initialize(setupParam);

    // 描画システムに登録
    m_pEffectGroupIDList = GFL_NEW(m_pHeap) gfl2::util::List<u32>(m_pHeap, 1);
    m_pEffectGroupIDList->PushFront(0);
    gfl2::Effect::EffectRenderPath::Config config;
    config.m_pSystem = m_pEffectSystem;
    config.m_cpGroupIDList = m_pEffectGroupIDList;
    pRenderingPipeLine->SetupEffectRenderPathConfig(config);
  }

  // マップを作成
  {
    m_NamiModelSet.Create(m_pNijiAllocator, m_pHeap, &m_ResourceManagerList[RMID_FIELD_MODEL], &m_EffectManager, this, m_MapData);
    m_NamiModelSet.AddToRenderingPipeLine(pRenderingPipeLine, true);
  }

  // プレイヤーを作成
  {
    m_PlayerModelSet.CreateModelSet(m_pNijiAllocator, m_pHeap, this, &m_ResourceManagerList[RMID_CHARA_MODEL], &m_EffectManager);
    m_PlayerModelSet.AddToRenderingPipeLine(pRenderingPipeLine, true);
    // プレイヤー初期位置の設定
    m_PlayerModelSet.SetPosition(m_MapData.m_playerInitPos);
    // コリジョンマネージャーに登録
    m_CollisionManager.AddRootModel(m_PlayerModelSet.GetMantainModel());
  }

  // 海パン野郎
  {
    m_SeaBreadRider.Create(m_pNijiAllocator, m_pHeap, this, &m_ResourceManagerList[RMID_CHARA_MODEL], &m_ResourceManagerList[RMID_FIELD_MODEL], &m_EffectManager);
    m_SeaBreadRider.AddToRenderingPipeLine(pRenderingPipeLine, true);
    m_SeaBreadRider.SetVisible(false);
  }

  // 障害物マネージャーを初期化
  {
    Model::Gimmick::GimmickManager::SetupData setupData;
    setupData.m_pGfxAllocator = m_pNijiAllocator;
    setupData.m_pHeap = m_pHeap;
    setupData.m_pRenderingPipeLine = pRenderingPipeLine;
    setupData.m_pAppManager = this;
    setupData.m_pResourceManager = &m_ResourceManagerList[RMID_FIELD_MODEL];
    setupData.m_pEffectManager = &m_EffectManager;
    setupData.m_pCourseDataManager = &m_CourseDataManager;
    setupData.m_pCollisionManager = &m_CollisionManager;
    setupData.m_pNamiController = &m_NamiController;
    setupData.m_pPlayerController = &m_PlayerController;
    setupData.m_pPlayerModelSet = &m_PlayerModelSet;
    m_GimmickManager.Initialize(setupData);
  }

  // 波コントローラーを初期化
  {
    f32 startPos = m_MapData.m_mapInitPos.x + m_MapData.m_mapWidth / 2.0f;
    f32 endPos = m_MapData.m_mapInitPos.x - m_MapData.m_mapWidth / 2.0f;
    m_NamiController.Initialize(this, &m_CourseDataManager, &m_NamiModelSet, m_MapData.m_mapDepth, m_MapData.m_mapWidth, startPos, endPos);
  }

  // プレイヤーコントローラーを初期化
  {
    m_PlayerController.Initialize(this, &m_PlayerModelSet);
  }

  // 背景マネージャーを初期化
  {
    Background::BackgroundManager::SetupParam setupParam;
    setupParam.m_pGfxAllocator = m_pNijiAllocator;
    setupParam.m_pHeap = m_pHeap;
    setupParam.m_pAppManager = this;
    setupParam.m_pRenderingPipeLine = pRenderingPipeLine;
    setupParam.m_pResourceManager = &m_ResourceManagerList[RMID_FIELD_MODEL];
    setupParam.m_pNamiController = &m_NamiController;
    setupParam.m_pPlayerModelSet = &m_PlayerModelSet;
    m_BackgroundManager.Initialize(setupParam);
  }

  // 天球マネージャーを初期化
  {
    // 天球を初期化
    SkyBox::SkyBoxManager::SetupParam setupParam;
    setupParam.m_pAppManager = this;
    setupParam.m_pGfxAllocator = m_pNijiAllocator;
    setupParam.m_pHeap = m_pHeap;
    setupParam.m_pPlayerController = &m_PlayerController;
    setupParam.m_pResourceManager = &m_ResourceManagerList[RMID_FIELD_MODEL];
    setupParam.m_pPlayerModelSet = &m_PlayerModelSet;
    setupParam.m_pRenderingPipeLine = m_pDrawManager->GetRenderingPipeline();
    m_SkyBoxManager.Initialize(setupParam);
  }

  // チュートリアマネージャーを初期化
  {
    m_TutorialManager.Initialize(this);
  }

  // カメラの設定
  {
    m_Camera.Initialize(this, &m_EffectManager);
    m_Camera.SetTraceModeTarget(m_PlayerModelSet.GetMantainModel());
    m_CameraController.Initialize(&m_Camera, this);
    m_CameraController.ResetCameraPosition();
  }
}

void MantainSurfAppManager::Update()
{
  // 状態更新
  UpdateState();
#if PM_DEBUG
  UpdateFinishTimer();
#endif

  // チュートリアルチェック
  if(IsTutorialMode() && m_TutorialManager.IsInfoWindowOpen())
  {
    // Infoウィンドウ表示中はアプリ更新停止
    return;
  }

  // コリジョン前更新
  m_CollisionManager.PreUpdate();

  // 波更新
  m_NamiController.Update();

  // プレイヤー更新
  m_PlayerController.Update();

  // 海パン更新
  m_SeaBreadRider.Update();

  // 障害物マネージャー更新
  m_GimmickManager.Update();

  // コリジョン更新
  m_CollisionManager.Update();
  m_PlayerController.AdjustPlayerPosition();

  // 背景更新
  m_BackgroundManager.Update();

  // マップ更新
  m_NamiModelSet.Update(m_PlayerModelSet.GetPosition());

  // 天球更新
  m_SkyBoxManager.Update();

  // カメラ更新
  m_CameraController.Update();
}

void MantainSurfAppManager::UpdateAfterTraverse()
{
  // トラバース後更新
  m_PlayerModelSet.UpdateAfterTraverse();
  m_SeaBreadRider.UpdateAfterTraverse();
  m_GimmickManager.UpdateAfterTraverse();
  m_NamiModelSet.UpdateAfterTraverse();

  // エフェクト更新
  m_EffectManager.Update();

  // サウンド更新
  m_SurfSoundManager.Update();

#if PM_DEBUG
  m_EffectManager.UpdateForDebug(m_PlayerModelSet.GetMantainModel()->GetSrtForEffect(0));
#endif
}

void MantainSurfAppManager::UpdateState()
{
  // ステート変更チェック
  switch(m_stateNow)
  {
  case ST_STARTING:
    if(m_stateSeq == SEQ_END)
    {
      m_stateNext = ST_SURFING;
    }
    break;

  case ST_SURFING:
    if(IsTutorialMode() && m_TutorialManager.IsTutorialFinished())
    {
      m_stateNext = ST_QUIT;
    }
    else if(IsGoal())
    {
#if PM_DEBUG
      if(m_DebugMenu.m_dbvIsInfinityMode)
      {
        // 無限モードの場合は何もしない
        if(m_NamiController.IsWaitFinish())
        {
          // 波が終わった場合はリセット
          m_NamiController.SetMain();
        }
        break;
      }
#endif
      m_stateNext = ST_ENDING;
    }
    break;

  case ST_ENDING:
    if(m_stateSeq == SEQ_END)
    {
      m_stateNext = ST_QUIT;
    }
    break;

  case ST_QUIT:
    break;
  }

#if PM_DEBUG
  // 終了判定
  if(m_stateNow != ST_QUIT)
  {
    if(m_stateParam.m_finishTimer >= 15.0f)
    {
      switch(m_stateNow)
      {
      case ST_STARTING:
      case ST_SURFING:
        if(IsTutorialMode())
        {
          m_stateNext = ST_QUIT;
        }
        else
        {
          m_stateNext = ST_ENDING;
        }
        break;
      case ST_ENDING:
        m_stateNext = ST_QUIT;
        break;
      }
    }
    if(GetDebugMenu()->m_dbvIsRestartTest)
    {
      if(m_restartTimer >= GetDebugMenu()->m_dbvRestartInterval)
      {
        GetDebugMenu()->m_dbvIsCourseRestart = true;
        GetDebugMenu()->m_dbvCourseIdNow = (GetDebugMenu()->m_dbvCourseIdNow + 1) % Course::CourseDataManager::COURSE_DATA_MAX;
      }
      m_restartTimer++;
    }
    if(GetDebugMenu()->m_dbvIsCourseRestart)
    {
      m_stateNext = ST_QUIT;
    }
  }
#endif

  // ステート変更時の初期化
  while(m_stateNext != ST_NONE)
  {
    m_stateNow = m_stateNext;
    m_stateNext = ST_NONE;

    switch(m_stateNow)
    {
    case ST_STARTING:
      // SE再生
      SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_SURF_ENV);
      // 開始演出
      if(m_pAppParam) m_pAppParam->out_score = 0;
      m_pSurfUI->SetBestScore(GetBestScore());
      if(!IsTutorialMode())
      {
        ShowSkillList();
        m_pSurfUI->SetCourseID(static_cast<MantainSurfAppData::CourseID>(m_courseId));
      }
      else
      {
        HideSkillList();
      }
      m_stateSeq = 0;
      break;
    case ST_SURFING:
      // BGM再生
      SurfSound::SurfSoundManager::StartBGM();
      if(IsTutorialMode())
      {
        m_TutorialManager.StartTutorial();
      }
      m_CameraController.SetState(Camera::CameraController::ST_NORMAL);
      m_PlayerController.SetState(Player::PlayerController::ST_ACCEL);
      m_NamiController.SetMain();
      break;
    case ST_ENDING:
      // 終了演出
      m_stateSeq = 0;
      m_stateParam.m_procTimer = 0.0f;
      m_stateParam.m_finishTimer = 0.0f;
      m_NamiController.SetFinish();
      m_Camera.StopTracingTarget();
      m_Camera.EndAllEffect();
      m_Camera.SetupTraceParam(gfl2::math::Vector3(0.0f, 0.0f, 1.0f), m_PlayerController.GetCurrentSpeed(), 0.0f,
        gfl2::math::Lerp(ENDING_CAMERA_MIN_TRACING_DURATION, ENDING_CAMERA_MAX_TRACING_DURATION, m_PlayerController.GetSpeedRate()));
      m_CameraController.SetState(Camera::CameraController::ST_ENDING);
      m_PlayerController.SaveEndPosition();
      break;
    case ST_QUIT:
      break;
    }
  }

  // 各ステートの実行
  switch(m_stateNow)
  {
  case ST_STARTING:
    ExecuteStartingState();
    break;
  case ST_SURFING:
    ExecuteSurfingState();
    break;
  case ST_ENDING:
    ExecuteEndingState();
    break;
  case ST_QUIT:
    break;
  }
}

void MantainSurfAppManager::UpdateFinishTimer()
{
  // 終了チェック
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsHold(gfl2::ui::BUTTON_START))
  {
    m_stateParam.m_finishTimer++;
  }
  else
  {
    m_stateParam.m_finishTimer = 0.0f;
  }
}

void MantainSurfAppManager::ExecuteStartingState()
{
  switch(m_stateSeq)
  {
  case 0:
    // カメラワーク開始
    m_CameraController.SetState(Camera::CameraController::ST_STARTING);
    m_PlayerController.SetState(Player::PlayerController::ST_PADDLING);
    m_PlayerModelSet.ChangeAnimation(Model::ModelPlayer::ANIME_PADDLING_LOOP);
    m_stateSeq++;
    break;

  case 1:
    if(m_CameraController.IsStartingFinished())
    {
      // カメラワーク完了
      m_stateSeq++;
      m_PlayerModelSet.ChangeAnimation(Model::ModelPlayer::ANIME_PADDLING_END);
    }
    break;

  case 2:
    if(m_PlayerModelSet.GetAnimationProgress() >= 0.5)
    {
      // アニメーション半分進んだらSE再生
      SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_PADDLING_END);
      m_stateSeq++;
    }
    break;

  case 3:
    if(m_PlayerModelSet.IsAnimationEnd(true))
    {
      // パドリングモーション完了
      m_stateSeq = SEQ_END;
    }
    break;
  }

#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(GetDebugMenu()->m_dbvIsHotKeyEnable && !gfl2::debug::DebugWin_IsOpen() && pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    // 開始演出をスキップ
    SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_PADDLING_END);
    m_stateSeq = SEQ_END;
  }
#endif
}

void MantainSurfAppManager::ExecuteSurfingState()
{
  // チュートリアル更新
  m_TutorialManager.Update();

  // 座標リセット更新
  UpdatePosReset();

#if PM_DEBUG
  m_playTime++;
#endif
}

void MantainSurfAppManager::UpdatePosReset()
{
  // チュートリアルチェック
  if(IsTutorialMode() && m_TutorialManager.IsInfoWindowOpen())
  {
    // Infoウィンドウ表示中はアプリ更新停止
    return;
  }

  /*
   *  座標リセットする必要なもの
   *  ・プレイヤー(主人公+マンタイン)
   *  ・障害物
   *  ・波
   */
  if(m_PlayerModelSet.GetPosition().z > Z_POS_RESET_THREDSHOLD)
  {
    ResetZPosition();
#if PM_DEBUG
    m_totalResetDistance += Z_POS_RESET_THREDSHOLD;
#endif
  }
}

void MantainSurfAppManager::ResetZPosition()
{
  m_PlayerController.ResetZPosition(Z_POS_RESET_THREDSHOLD);
  m_SeaBreadRider.ResetZPosition(Z_POS_RESET_THREDSHOLD);
  m_GimmickManager.ResetZPosition(Z_POS_RESET_THREDSHOLD);
  m_NamiModelSet.ResetZPosition(Z_POS_RESET_THREDSHOLD);
  HOU_PRINT("[UpdatePosReset]Position Reset!\n");
}

void MantainSurfAppManager::SetEnding()
{
  if(m_stateNow < ST_ENDING)
  {
    m_stateNext = ST_ENDING;
  }
}

void MantainSurfAppManager::SetQuit()
{
  if(m_stateNow < ST_QUIT)
  {
    m_stateNext = ST_QUIT;
  }
}

void MantainSurfAppManager::ExecuteEndingState()
{
  switch(m_stateSeq)
  {
  case 0:
    // しばらく待つ
    m_stateParam.m_procTimer++;
    if(m_stateParam.m_procTimer >= 70.0f && m_Camera.IsTracingStabled())
    {
      m_stateSeq++;
    }
    break;

  case 1:
    // 結果画面表示
    m_pSurfUI->ShowResult();
    m_stateParam.m_procTimer = 0.0f;
    m_stateSeq++;
    break;

  case 2:
    // 画面表示完了待ち(完了したらSurfUI_OnEventが通知される)
    break;

  case 3:
    // しばらく待つ
    m_stateParam.m_procTimer++;
    if(m_stateParam.m_procTimer >= ENDING_SE_FADE_OUT_FRAME)
    {
      // 終了へ
      m_stateSeq = SEQ_END;
    }
    break;
  }
}

void MantainSurfAppManager::SurfUI_OnEvent(UI::SurfUI* pSender, const EventCode eventCode)
{
  switch(eventCode)
  {
  case EVENT_ResultEnd:
    // 結果表示終了
    if(m_stateNow == ST_ENDING)
    {
      SurfSound::SurfSoundManager::ChangeMasterSEVolume(0.0f, ENDING_SE_FADE_OUT_FRAME);
      m_stateSeq++;
    }
    break;
  case EVENT_TutorialEnd:
    // チュートリアル表示終了
    m_TutorialManager.OnInfoWindowClose();
    break;
  }
}

void MantainSurfAppManager::StopEffect()
{
  m_Camera.StopEffect();
  m_NamiModelSet.StopEffect();
  m_PlayerModelSet.StopEffect();
  m_GimmickManager.StopEffect();
}

void MantainSurfAppManager::ResumeEffect()
{
  m_Camera.ResumeEffect();
  m_NamiModelSet.ResumeEffect();
  m_PlayerModelSet.ResumeEffect();
  m_GimmickManager.ResumeEffect();
}

void MantainSurfAppManager::ShowSkillList()
{
  MantainSurfAppData::SkillListMode mode = MantainSurfAppData::SKILLLIST_MODE_0;
  if(MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_05) || 
     MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_06))
  {
    // 3スキルまで解放済み
    mode = MantainSurfAppData::SKILLLIST_MODE_3;
  }
  else if(MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_04))
  {
    // 2スキルまで解放済み
    mode = MantainSurfAppData::SKILLLIST_MODE_2;
  }
  else if(MantainSurfAppData::IsSkillUnlocked(MantainSurfAppData::SKILL_ID_EX_03))
  {
    // 1スキルまで解放済み
    mode = MantainSurfAppData::SKILLLIST_MODE_1;
  }
  m_pSurfUI->ShowTrickList(mode);
}

void MantainSurfAppManager::HideSkillList()
{
  m_pSurfUI->ShowTrickList(MantainSurfAppData::SKILLLIST_MODE_NOTHING);
}

void MantainSurfAppManager::SetCourseId(s32 courseId)
{
  m_courseId = courseId;
}

s32 MantainSurfAppManager::GetCourseId()
{
  return m_courseId;
}

bool MantainSurfAppManager::IsTutorialMode()
{
  return m_courseId == Course::CourseDataManager::TUTORIAL_COURSE_ID;
}

bool MantainSurfAppManager::IsGoal()
{
  // ゴールの条件 = 海パンスタンバイ && プレイヤーサーフ状態 && プレイヤーがゴールラインを超えた
  if(!m_SeaBreadRider.IsStandby())          return false;
  if(!m_PlayerController.IsSurfingState())  return false;
  return m_PlayerModelSet.GetPosition().z >= m_SeaBreadRider.GetGoalLinePosition().z;
}

void MantainSurfAppManager::AddScore(const u32 score)
{
  if(m_pAppParam) m_pAppParam->out_score += score;
  m_totalScore += score;
}

gfl2::math::Vector3 MantainSurfAppManager::GetPlayerInitPosition()
{
  return m_MapData.m_playerInitPos;
}

MantainSurfAppManager::GameState MantainSurfAppManager::GetGameState()
{
  return m_stateNow;
}

Camera::Camera* MantainSurfAppManager::GetCamera()
{
  return &m_Camera;
}

Camera::CameraController* MantainSurfAppManager::GetCameraController()
{
  return &m_CameraController;
}

Player::PlayerController* MantainSurfAppManager::GetPlayerController()
{
  return &m_PlayerController;
}

Player::PlayerModelSet* MantainSurfAppManager::GetPlayerModelSet()
{
  return &m_PlayerModelSet;
}

Nami::NamiController* MantainSurfAppManager::GetNamiController()
{
  return &m_NamiController;
}

Nami::NamiModelSet* MantainSurfAppManager::GetNamiModelSet()
{
  return &m_NamiModelSet;
}

UI::SurfUI* MantainSurfAppManager::GetSurfUI()
{
  return m_pSurfUI;
}

Model::Gimmick::GimmickManager* MantainSurfAppManager::GetGimmickManager()
{
  return &m_GimmickManager;
}

Effect::EffectManager* MantainSurfAppManager::GetEffectManager()
{
  return &m_EffectManager;
}

#if PM_DEBUG
Debug::DebugMenu* MantainSurfAppManager::GetDebugMenu()
{
  return &m_DebugMenu;
}

f32 MantainSurfAppManager::GetTotalDistance()
{
  return m_PlayerModelSet.GetPosition().z - m_MapData.m_playerInitPos.z + m_totalResetDistance;
}
#endif

SkyBox::SkyBoxManager* MantainSurfAppManager::GetSkyBoxManager()
{
  return &m_SkyBoxManager;
}

Background::BackgroundManager* MantainSurfAppManager::GetBackgroundManager()
{
  return &m_BackgroundManager;
}

Course::CourseDataManager* MantainSurfAppManager::GetCourseDataManager()
{
  return &m_CourseDataManager;
}

Tutorial::TutorialManager* MantainSurfAppManager::GetTutorialManager()
{
  return &m_TutorialManager;
}

SurfSound::SurfSoundManager* MantainSurfAppManager::GetSurfSoundManager()
{
  return &m_SurfSoundManager;
}

NpcRider::NpcRiderSeaBread* MantainSurfAppManager::GetSeaBreadRider()
{
  return &m_SeaBreadRider;
}

GameSys::GameData* MantainSurfAppManager::GetGameData() const
{
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  return pGameManager->GetGameData();
}

App::Event::MANTAIN_SURF_PARAM* MantainSurfAppManager::GetAppParam() const
{
  return m_pAppParam;
}

u32 MantainSurfAppManager::GetBestScore()
{
  if(m_pAppParam)
  {
    return m_pAppParam->in_score;
  }
  return 0;
}

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
