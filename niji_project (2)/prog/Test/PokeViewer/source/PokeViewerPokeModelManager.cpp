//======================================================================
/**
 * @file PokeViewerPokeModelManager.cpp
 * @date 2015/12/03 16:23:25
 * @author araki_syo
 * @brief ポケビューア：ポケモンモデル管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include "System/include/GflUse.h"
#include "System/include/Screenshot.h"

#include "../include/PokeViewerPokeModelManager.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

#if defined(GF_PLATFORM_WIN32)
#define POKEVIEWER_WAITFRAME 1
#else
#define POKEVIEWER_WAITFRAME 2
#endif

static const s32 POKEVIEWER_POKEMODELMANAGER_MEM_PROC_DEV_MEM_SIZE    = 0x0A00000;

static void Rot_Deg( f32 &x , f32 &z , const f32 deg )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosDeg(&sin, &cos, deg);
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}

static f32 Deg_to_Fidx(f32 deg)
{
  return deg * (256.0f / 360.0f);
}

PokeViewerPokeModelManager::PokeViewerPokeModelManager() :
  m_pHeap(NULL),
  m_pNijiAllocator(NULL),
  m_pRenderingPipeline(NULL),
  m_pGameManager(NULL),
  m_AnimeType(PokeTool::MODEL_ANIMETYPE_BATTLE),
  m_NextAnimeType(PokeTool::MODEL_ANIMETYPE_BATTLE),
  m_PokeParam(),
  m_MotionIndex(0),
  m_MotionIsLoop(false),
  m_MotionSpeed(1.0),
  m_IsPlaying(false),
  m_StartMonsNo(1),
  m_EndMonsNo(1),
  m_UpdateType(UPDATE_TYPE_NORMAL),
  m_CaptureSeq(0),
  m_MotionFrame(0.0f),
  m_ShadowVisibility(false),
  m_IsPokeShaderEnable(false),
  m_pInframeSys(NULL),
  m_pInframeCamera(NULL),
  m_CameraMode(PokeViewerPokeModelManager::CAMERA_MODE_FREE),
  m_LeftEarValue(0x78),
  m_RightEarValue(0x66),
  m_LeftEyeValue(0x7a),
  m_RightEyeValue(0x3a),
  m_FileNamePattern(PokeViewerPokeModelManager::FILENAME_PATTERN_POKEMONCENTER),
  m_MeasurementStatus(MEASUREMENT_STATUS_NONE),
  m_MeasurementFrameCount(0),
  m_CurrentDirection(CAMERA_DIRECTION_FRONT),
  m_CameraDistance(1.0f),
  /*  debugTextDrawer*/
  m_DebugTextDrawerHeap(),
  m_DebugTextDrawer(),
  m_GuideRectPixelSize(0)
{
  m_PokeParam.monsNo = (MonsNo)1;
  SetSpindaPattern();

  // 背景色初期設定
  m_ClearColor = gfl2::gfx::Color(1.0f, 1.0f, 1.0f, 1.0f);

  // エッジ色初期設定
  m_EdgeColor = gfl2::gfx::Color(0.4f, 0.4f, 0.4f);

  // カラーシェーダプリセット
  m_PokeShaderColor = gfl2::gfx::Color(0.0f, 0.0f, 0.0f, 1.0f);

  ResetCameraPosition();
  ResetCameraFovyNearFar();
  ResetLightPosition();
}

PokeViewerPokeModelManager::~PokeViewerPokeModelManager()
{
}

void PokeViewerPokeModelManager::Initialize(GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* pHeap)
{
  m_pGameManager = pGameManager;

  m_pHeap = GFL_CREATE_LOCAL_HEAP(pHeap, POKEVIEWER_POKEMODELMANAGER_MEM_PROC_DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);

  m_pNijiAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);

  m_pRenderingPipeline = GFL_NEW(m_pHeap) PokeViewerRenderingPipeline();
  m_pRenderingPipeline->Initialize(pGameManager->GetAsyncFileManager(), m_pHeap, m_pNijiAllocator);
  m_pRenderingPipeline->SetClearColor(m_ClearColor);

  m_pPokeModelSys = GFL_NEW(m_pHeap) PokeTool::PokeModelSystem();
  m_pPokeModelSys->InitSystemAsync(m_pHeap, m_pHeap, m_pGameManager->GetAsyncFileManager(), 1);

  m_pInframeSys = GFL_NEW(m_pHeap) System::Camera::CModelViewerInFrame();
  m_pInframeSys->Init(System::Camera::CModelInFrame::SCREEN_UPPER, 90, 10, 310, 230, System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT);
  m_pInframeSys->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM);

  // とりあえずインフレームにNULLを設定
  m_pInframeSys->SetAndInitModel(NULL);

  m_pInframeCamera = GFL_NEW(m_pHeap) System::Camera::InFrameCamera();

  while(!m_pPokeModelSys->IsFinishInitSystemAsync())
  {
  }

  m_pPokeModelSys->CreateSystem(m_pNijiAllocator, true, false);

  //管理ヒープ作成
  {
    PokeTool::PokeModelSystem::HeapOption opt;
    opt.animeType = PokeTool::MODEL_ANIMETYPE_KAWAIGARI;
    opt.useIdModel = false;
    opt.useShadow = true;
    opt.useColorShader = true;
    m_pPokeModelSys->CreatePokeModelHeap(m_pHeap,&opt);
  }

  // カラーシェーダセットアップ
  m_pRenderingPipeline->SetPokeModelSys(m_pPokeModelSys);

  // デバッグメニュー作成
  InitDebug();

#if defined(GF_PLATFORM_CTR)
  // シングルコマンドリストモードに設定
  gfl2::gfx::GFGL::SetSigleCommandListMode(true);
#endif

  m_pPersonalData = GFL_NEW(m_pHeap)pml::personal::PersonalData(m_pHeap);

  {
    m_DebugTextDrawerHeap.LocalHeapCreate(m_pHeap, m_pHeap, 0x1000, 0x00100000);
    m_DebugTextDrawer.Initialize(&m_DebugTextDrawerHeap);
    m_DebugTextDrawer.Scale(1,1);
    m_DebugTextDrawer.COLOR(0xff, 0x10, 0x10);
  }
}

void PokeViewerPokeModelManager::Finalize()
{
  {
    m_DebugTextDrawer.Finalize();
    m_DebugTextDrawerHeap.Delete();
  }

  GFL_SAFE_DELETE(m_pPersonalData);

#if defined(GF_PLATFORM_CTR)
  // シングルコマンドリストモード終了
  gfl2::gfx::GFGL::SetSigleCommandListMode(false);
#endif

  // デバッグメニュー破棄
  TermDebug();

  GFL_SAFE_DELETE(m_pInframeCamera);
  GFL_SAFE_DELETE(m_pInframeSys);

  m_pPokeModelSys->DeletePokeModelHeap();
  m_pPokeModelSys->TermSystemAsync();

  while(!m_pPokeModelSys->IsFinishTermSystemAsync())
  {
  }

  GFL_SAFE_DELETE(m_pPokeModelSys);

  m_pRenderingPipeline->Finalize();
  GFL_SAFE_DELETE(m_pRenderingPipeline);

  GFL_SAFE_DELETE(m_pNijiAllocator);

  GFL_DELETE_HEAP(m_pHeap);
}

void PokeViewerPokeModelManager::Update()
{
  switch (m_UpdateType)
  {
  case UPDATE_TYPE_NORMAL:
    Update_Normal();
    break;

  case UPDATE_TYPE_CAPTURE:
    Update_Capture();
    break;

  case UPDATE_TYPE_CAPTURE_ALL:
    Update_CaptureAll();
    break;

  case UPDATE_TYPE_DIMENSIONAL_CAPTURE:
    Update_DimensionalCapture();
    break;

  case UPDATE_TYPE_LOAD_MEASUREMENT:
    Update_LoadMeasurement();
    break;
  }
}

void PokeViewerPokeModelManager::Update_Normal()
{
  switch (m_State)
  {
  case STATE_LOAD_POKEMODEL:
    Update_LoadPokeModel();
    break;

  case STATE_UNLOAD_POKEMODEL:
    Update_UnloadPokeModel();
    break;

  default:
    if(m_pPokeModel)
    {
      // モーション再生速度を更新
      m_pPokeModel->SetAnimationStepFrame(m_IsPlaying? m_MotionSpeed : 0.0f);

      // モーションループ設定を更新
      m_pPokeModel->SetAnimationLoop(m_MotionIsLoop);

      // カラーシェーダを適用
      UpdatePokeShaderColor();

      m_pPokeModel->Update();
      m_pPokeModel->UpdateAnimation();

      m_MotionFrame = m_pPokeModel->GetAnimationFrame();

      if(!m_MotionIsLoop && m_pPokeModel->IsAnimationLastFrame())
      {
        m_IsPlaying = false;
      }
    }
    break;
  }
  
  UpdateCameraPosition();
  UpdateLightPosition();

  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

void PokeViewerPokeModelManager::Update_Capture()
{
  switch (m_State)
  {
  case STATE_LOAD_POKEMODEL:
    Update_LoadPokeModel();
    break;

  case STATE_UNLOAD_POKEMODEL:
    Update_UnloadPokeModel();
    break;

  default:
    // インフレームモードならカメラをUpdate
    if(m_CameraMode >= CAMERA_MODE_INFRAME_CTR)
    {
      m_pInframeSys->Update(m_pInframeCamera);
    }

    if(m_pPokeModel)
    {
      UpdatePokeShaderColor();

      // アニメーション更新
      m_pPokeModel->SetAnimationStepFrame(0.0f);

      m_pPokeModel->Update();
      m_pPokeModel->UpdateAnimation();

      m_pPokeModel->SetAnimationStepFrame(m_MotionSpeed);
    }

    if(CapturePokeModel())
    {
      LoadPokeModel();
      m_UpdateType = UPDATE_TYPE_NORMAL;
    }

    break;
  }

  UpdateLightPosition();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

void PokeViewerPokeModelManager::Update_CaptureAll()
{
  switch (m_State)
  {
  case STATE_LOAD_POKEMODEL:
    Update_LoadPokeModel();
    break;

  case STATE_UNLOAD_POKEMODEL:
    Update_UnloadPokeModel();
    break;

  default:
    // インフレームモードならカメラをUpdate
    if(m_CameraMode >= CAMERA_MODE_INFRAME_CTR)
    {
      m_pInframeSys->Update(m_pInframeCamera);
    }

    // 通常モードに戻る
    if(m_CurrentMonsNo < m_StartMonsNo || m_CurrentMonsNo > m_EndMonsNo)
    {
      LoadPokeModel();
      m_UpdateType = UPDATE_TYPE_NORMAL;
      break;
    }

    m_PokeParam.monsNo = (MonsNo)m_CurrentMonsNo;

    if(m_pPokeModel)
    {
      UpdatePokeShaderColor();

      // アニメーション更新
      m_pPokeModel->SetAnimationStepFrame(0.0f);

      m_pPokeModel->Update();
      m_pPokeModel->UpdateAnimation();

      m_pPokeModel->SetAnimationStepFrame(m_MotionSpeed);
    }

    if(CapturePokeModel())
    {
      if(!SetNextPokeParam())
      {
        ++m_CurrentMonsNo;
      }
    }

    break;
  }

  UpdateLightPosition();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

void PokeViewerPokeModelManager::Update_DimensionalCapture()
{
  switch (m_State)
  {
  case STATE_LOAD_POKEMODEL:
    Update_LoadPokeModel();
    break;

  case STATE_UNLOAD_POKEMODEL:
    Update_UnloadPokeModel();
    break;

  default:
    // 通常モードに戻る
    if(m_CurrentMonsNo < m_StartMonsNo || m_CurrentMonsNo > m_EndMonsNo)
    {
      ResetLightPosition();
      SetPresetCamera(CAMERA_TYPE_DEFAULT);
      LoadPokeModel();
      m_UpdateType = UPDATE_TYPE_NORMAL;
      break;
    }

    m_PokeParam.monsNo = (MonsNo)m_CurrentMonsNo;

    if(m_pPokeModel)
    {
      UpdatePokeShaderColor();
      AdjustCamera(m_CameraDistance, m_CurrentDirection);
      AdjustLight(m_CurrentDirection);

      // アニメーション更新
      m_pPokeModel->SetAnimationStepFrame(0.0f);

      m_pPokeModel->Update();
      m_pPokeModel->UpdateAnimation();

      m_pPokeModel->SetAnimationStepFrame(m_MotionSpeed);
    }

    if(CapturePokeModel())
    {
      if(!SetNextCameraDirection())
      {
        m_CurrentDirection = CAMERA_DIRECTION_FRONT;

        if(!SetNextPokeParam())
        {
          ++m_CurrentMonsNo;
        }
      }
    }

    break;
  }

  UpdateLightPosition();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

/**
 * @brief 負荷計測モードの更新処理
 */
void PokeViewerPokeModelManager::Update_LoadMeasurement()
{
  switch (m_State)
  {
  case STATE_LOAD_POKEMODEL:
    Update_LoadPokeModel();
    break;

  case STATE_UNLOAD_POKEMODEL:
    Update_UnloadPokeModel();
    break;

  default:
    ExecuteLoadMeasurement();
    break;
  }

  UpdateLightPosition();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

/**
 * @brief 負荷計測の実行
 */
void PokeViewerPokeModelManager::ExecuteLoadMeasurement()
{
  switch (m_MeasurementStatus)
  {
  case MEASUREMENT_STATUS_START_CALC_REF_VALUE:   // 基準値計測開始
    // NULLを渡して, コマンドのサイズを0としておく.
    m_LoadMeasurementData.CalculateNumberOfDrawCommands(NULL);
    // フレームカウンタを初期化し, 基準値計算に移行
    m_MeasurementFrameCount = 0;
    m_MeasurementStatus = MEASUREMENT_STATUS_WORKING_CALC_REF_VALUE;
    break;
    
  case MEASUREMENT_STATUS_WORKING_CALC_REF_VALUE: // 基準値計測中
    // 処理負荷を計算
    if (CalculateLoadMeasurement())
    {// 負荷計測が終了していたら, 計測終了状態へ移行
      m_MeasurementStatus = MEASUREMENT_STATUS_END_CALC_REF_VALUE;
    }
    break;

  case MEASUREMENT_STATUS_END_CALC_REF_VALUE:   // 基準値計測終了
    // 負荷計測の結果をコンソールに出力
    GFL_RELEASE_PRINT("MonsNo = %d, Sex = %d, FormNo = %d, ", 0, 0, 0);   // 何も描画していないので0にしておく
    m_LoadMeasurementData.OutputMeasurementToConsole();
    // 出力後, 負荷計測開始状態へ移行
    m_MeasurementStatus = MEASUREMENT_STATUS_START;
    break;

  case MEASUREMENT_STATUS_START:    // 負荷計測開始
    // 指定した範囲の番号の負荷計測が終了したら元に戻す
    if(m_CurrentMonsNo < m_StartMonsNo || m_CurrentMonsNo > m_EndMonsNo)
    {
      m_UpdateType = UPDATE_TYPE_NORMAL;
      m_MeasurementStatus = MEASUREMENT_STATUS_NONE;
      break;
    }
    m_PokeParam.monsNo = (MonsNo)m_CurrentMonsNo;

    if(LoadPokeModel())
    {
      // モデルの読み取りが完了したので, カメラの設定へ移行
      m_MeasurementStatus = MEASUREMENT_STATUS_CALC_COMMAND_SIZE;
    }
    break;

  case MEASUREMENT_STATUS_CALC_COMMAND_SIZE:
    // コマンドバッファサイズを計算
    m_LoadMeasurementData.CalculateNumberOfDrawCommands(m_pPokeModel->GetModelInstanceNode());
    m_MeasurementStatus = MEASUREMENT_STATUS_ADJUST_CAMERA;
    break;

  case MEASUREMENT_STATUS_ADJUST_CAMERA:    // 負荷計測設定
    if (AdjustCamera(1.0f))
    {// カメラの設定が成功したら負荷計測開始状態へ移行
      m_MeasurementStatus = MEASUREMENT_STATUS_WORKING;
    }
    break;

  case MEASUREMENT_STATUS_WORKING:  // 負荷計測中
    // 処理負荷を計算
    if (CalculateLoadMeasurement())
    {// 負荷計測が終了していたら, 計測終了状態へ移行
      m_MeasurementStatus = MEASUREMENT_STATUS_END;
    }
    break;

  case MEASUREMENT_STATUS_END:    // 負荷計測終了
    // 負荷計測の結果をコンソールに出力
    GFL_RELEASE_PRINT("MonsNo = %d, Sex = %d, FormNo = %d, ", m_PokeParam.monsNo, m_PokeParam.sex, m_PokeParam.formNo);
    m_LoadMeasurementData.OutputMeasurementToConsole();

    // 雌雄差があるモデルがあれば雌雄を変更して再度負荷計測
    if(ExistFemale() && m_PokeParam.sex == pml::SEX_MALE)
    {
      m_PokeParam.sex = pml::SEX_FEMALE;
      m_MeasurementStatus = MEASUREMENT_STATUS_START;
      break;
    }

    // ここまで到達したらまたオスに戻す
    m_PokeParam.sex = pml::SEX_MALE;

    // フォルム番号を更新
    ++m_PokeParam.formNo;
    if(m_PokeParam.formNo <= GetFormMax())
    {// 別フォルムがまだ存在するのであれば再度負荷計測
      m_MeasurementStatus = MEASUREMENT_STATUS_START;
      break;
    }

    // 雌雄差, フォルム違いの負荷計測が終わったら, フォルムを0に戻して, 番号を更新して負荷計測
    m_PokeParam.formNo = 0;
    ++m_CurrentMonsNo;
    m_MeasurementStatus = MEASUREMENT_STATUS_START;

    break;
  }

  if(m_pPokeModel)
  {
    m_pPokeModel->Update();
    m_pPokeModel->UpdateAnimation();
  }
}

/**
 * @brief 負荷計測で使用するカメラの調整
 * @return bool 成功/失敗
 */
bool PokeViewerPokeModelManager::AdjustCamera( f32 cameraScale, CAMERA_DIRECTION dir )
{
  if (m_pPokeModel == NULL)
  {// モデルが設定されていなければ終了
    return false;
  }

  // カメラの設定を行う
  f32 aspect = 400.0f / 240.0f;
  gfl2::math::AABB aabb;
  bool succeeded = m_pPokeModel->GetMotionCollision(&aabb);
  if (!succeeded)
  {// AABBが正常に取得できなければ終了
    return false;
  }

  // 正面から捉えた位置でAABBが収まるように調整
  gfl2::math::Vector4 min = aabb.GetMin();
  gfl2::math::Vector4 max = aabb.GetMax();

  // ボックスの中央の座標を求める
  gfl2::math::Vector4 center = (min + max) / 2.0f;

  // カメラがどれくらい引くのかを求める
  gfl2::math::Vector4 diff = max - min;
  f32 lengthW = ( (dir == CAMERA_DIRECTION_RIGHT || dir == CAMERA_DIRECTION_LEFT)? diff.z : diff.x ) / aspect;
  f32 lengthH = (dir == CAMERA_DIRECTION_TOP || dir == CAMERA_DIRECTION_BOTTOM)? diff.z : diff.y;
  

  // sangoの計測ではfovは30°なので30に設定
  m_CameraFovy = 30;
  f32 radianFov = gfl2::math::ConvDegToRad(m_CameraFovy);
  f32 cameraLength = std::max(lengthW, lengthH) / ( 2.0f * tanf(radianFov / 2.0f) ) * cameraScale;

  gfl2::math::Vector3 eyePosition;
  gfl2::math::Vector3 lookAtPosition;

  switch( dir )
  {
  default:
  case CAMERA_DIRECTION_FRONT:
    eyePosition = gfl2::math::Vector3(center.x, center.y, max.z + cameraLength);
    lookAtPosition = gfl2::math::Vector3(center.x, center.y, max.z);
    break;
  case CAMERA_DIRECTION_BACK:
    eyePosition = gfl2::math::Vector3(center.x, center.y, min.z - cameraLength);
    lookAtPosition = gfl2::math::Vector3(center.x, center.y, min.z);
    break;
  case CAMERA_DIRECTION_RIGHT:
    eyePosition = gfl2::math::Vector3(min.x - cameraLength, center.y, center.z);
    lookAtPosition = gfl2::math::Vector3(min.x, center.y, center.z);
    break;
  case CAMERA_DIRECTION_LEFT:
    eyePosition = gfl2::math::Vector3(max.x + cameraLength, center.y, center.z);
    lookAtPosition = gfl2::math::Vector3(max.x, center.y, center.z);
    break;
  case CAMERA_DIRECTION_TOP:
    eyePosition = gfl2::math::Vector3(center.x, max.y + cameraLength, center.z);
    lookAtPosition = gfl2::math::Vector3(center.x, max.y, center.z);
    break;
  case CAMERA_DIRECTION_BOTTOM:
    eyePosition = gfl2::math::Vector3(center.x, min.y - cameraLength, center.z);
    lookAtPosition = gfl2::math::Vector3(center.x, min.y, center.z);
    break;
  }

  m_CameraPosition = eyePosition;
  m_CameraTarget = lookAtPosition;
  m_CameraNear = 32.0f;
  m_CameraFar = 6000.0f;
  m_CameraLength = (eyePosition - lookAtPosition).Length();

  return true;
}

void PokeViewerPokeModelManager::AdjustLight(CAMERA_DIRECTION dir)
{
  switch(dir)
  {
  case CAMERA_DIRECTION_FRONT:
    m_LightRotate.x = 0.0f;
    m_LightRotate.y = 0.0f;
    break;
  case CAMERA_DIRECTION_BACK:
    m_LightRotate.x = 0.0f;
    m_LightRotate.y = 180.0f;
    break;
  case CAMERA_DIRECTION_RIGHT:
    m_LightRotate.x = 0.0f;
    m_LightRotate.y = 90.0f;
    break;
  case CAMERA_DIRECTION_LEFT:
    m_LightRotate.x = 0.0f;
    m_LightRotate.y = 270.0f;
    break;
  case CAMERA_DIRECTION_TOP:
    m_LightRotate.x = 90.0f;
    m_LightRotate.y = 0.0f;
    break;
  case CAMERA_DIRECTION_BOTTOM:
    m_LightRotate.x = -90.0f;
    m_LightRotate.y = 0.0f;
    break;
  }
}

/**
 * @brief 処理負荷の計測
 * @return bool 計測終了/計測中
 */
bool PokeViewerPokeModelManager::CalculateLoadMeasurement()
{
  // カウンタを更新
  ++m_MeasurementFrameCount;
  const u32 update = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
  const u32 draw   = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
  const u32 gpu    = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);
  m_LoadMeasurementData.AddMeasurementValue(LoadMeasurementData::MeasurementType::UPDATE_MEASUREMENT, update);
  m_LoadMeasurementData.AddMeasurementValue(LoadMeasurementData::MeasurementType::DRAW_MEASUREMENT, draw);
  m_LoadMeasurementData.AddMeasurementValue(LoadMeasurementData::MeasurementType::GPU_MEASUREMENT, gpu);

  // 負荷計測の時間を超えたら終了へ移行
  if(m_MeasurementFrameCount >= MEASUREMENT_FRAME_COUNT_MAX)
  {
    m_MeasurementFrameCount = 0;  // フレームを0に戻す
    return true;
  }

  return false;
}

void PokeViewerPokeModelManager::Update_LoadPokeModel()
{
  static u32 state = 0;

  switch (state)
  {
  case 0:

    // 既に読み込まれていたら解放する
    if(m_pPokeModel)
    {
      m_pRenderingPipeline->RemoveDrawEnv(m_pPokeModel->GetDrawEnvNode());

      m_pPokeModel->SetVisible(false);
      m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetModelInstanceNode());

      // 影モデルがあったらそれも解放
      if(m_pPokeModel->GetShadowModel() && m_ShadowVisibility)
      {
        m_pPokeModel->GetShadowModel()->SetVisible(false);
        m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
      }

      // IDモデルがあったらそれも解放
      if(m_pPokeModel->GetIdModel())
      {
        m_pPokeModel->GetIdModel()->SetVisible(false);
        m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetIdModel()->GetModelInstanceNode());
      }

      if(m_pPokeModel->GetDistortionModel())
      {
        m_pRenderingPipeline->RemoveDistortionNode(m_pPokeModel->GetDistortionModel()->GetModelInstanceNode(), m_pPokeModel->GetDrawEnvNode());
      }

      // カラーシェーダ解放
      m_pPokeModel->CleanColorShader();

      // インフレーム追従を切る
      m_pInframeSys->SetAndInitModel(NULL);
    }

    ++state;
    // break;

  case 1:

    if(m_pPokeModel)
    {
      if(m_pPokeModel->CanDestroy())
      {
        m_pPokeModelSys->DeleteModel(m_pPokeModel);
        m_pPokeModel = NULL;
      }
      else
      {
        break;
      }
    }

    ++state;
    // break;

  case 2:
    {
      PokeTool::PokeModel::SetupOption opt;
      opt.dataHeap = NULL;
      opt.workHeap = m_pHeap;
      opt.useShadow = true;
      opt.animeType = m_NextAnimeType;

      m_pPokeModel = m_pPokeModelSys->CreateModelSimple(&m_PokeParam, opt);
      m_AnimeType = m_NextAnimeType;

      ++state;
      // break;
    }

  case 3:

    if(!m_pPokeModel->IsFinishLoad())
    {
      break;
    }

    m_pPokeModel->Create();

    m_pRenderingPipeline->AddNode(m_pPokeModel->GetModelInstanceNode());

    if(m_pPokeModel->GetShadowModel() && m_ShadowVisibility)
    {
      m_pRenderingPipeline->AddNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
    }

    if(m_pPokeModel->GetDistortionModel())
    {
      m_pRenderingPipeline->AddDistortionNode(m_pPokeModel->GetDistortionModel()->GetModelInstanceNode(), m_pPokeModel->GetDrawEnvNode());
    }

    m_pRenderingPipeline->AddDrawEnv(m_pPokeModel->GetDrawEnvNode());

    // モーション再生速度を更新
    m_pPokeModel->SetAnimationStepFrame(m_IsPlaying? m_MotionSpeed : 0.0f);

    // モーションループ設定を更新
    m_pPokeModel->SetAnimationLoop(m_MotionIsLoop);

    // カラーシェーダセットアップ
    m_pPokeModel->SetupColorShader();

    // モーションのフレーム位置を設定
    m_pPokeModel->ChangeAnimationDirect(m_MotionIndex);
    m_pPokeModel->SetAnimationIsLoop(m_MotionIsLoop);
    m_pPokeModel->SetAnimationFrame(m_MotionFrame);

    // カラーシェーダ設定を適用
    UpdatePokeShaderColor();

    // アニメーション更新
    m_pPokeModel->SetAnimationStepFrame(0.0f);
    m_pPokeModel->UpdateAnimation();
    m_pPokeModel->SetAnimationStepFrame(m_MotionSpeed);

    // インフレーム設定
    m_pInframeSys->SetAndInitModel(m_pPokeModel);

    state = 0;
    m_State = STATE_NONE;
  }
}

void PokeViewerPokeModelManager::Update_UnloadPokeModel()
{
  static u32 state = 0;

  switch (state)
  {
  case 0:

    // 既に読み込まれていたら解放する
    if(m_pPokeModel)
    {
      m_pRenderingPipeline->RemoveDrawEnv(m_pPokeModel->GetDrawEnvNode());

      m_pPokeModel->SetVisible(false);
      m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetModelInstanceNode());

      // 影モデルがあったらそれも解放
      if(m_pPokeModel->GetShadowModel() && m_ShadowVisibility)
      {
        m_pPokeModel->GetShadowModel()->SetVisible(false);
        m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
      }

      // IDモデルがあったらそれも解放
      if(m_pPokeModel->GetIdModel())
      {
        m_pPokeModel->GetIdModel()->SetVisible(false);
        m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetIdModel()->GetModelInstanceNode());
      }

      if(m_pPokeModel->GetDistortionModel())
      {
        m_pPokeModel->GetDistortionModel()->SetVisible(false);
        m_pRenderingPipeline->RemoveDistortionNode(m_pPokeModel->GetDistortionModel()->GetModelInstanceNode(), m_pPokeModel->GetDrawEnvNode());
      }

      // カラーシェーダ解放
      m_pPokeModel->CleanColorShader();

      // インフレーム追従を切る
      m_pInframeSys->SetAndInitModel(NULL);
    }

    ++state;
    // break;

  case 1:

    if(m_pPokeModel)
    {
      if(m_pPokeModel->CanDestroy())
      {
        m_pPokeModelSys->DeleteModel(m_pPokeModel);
        m_pPokeModel = NULL;
      }
      else
      {
        break;
      }
    }

    state = 0;
    m_State = STATE_NONE;
    // break;
  }
}

/**
 * @brief ポケモンをキャプチャ
 * @return bool キャプチャ終了するとtrueを返す
 */
bool PokeViewerPokeModelManager::CapturePokeModel()
{
  static u32 state = 0;

  switch(state)
  {
  case 0:
    {
      if(LoadPokeModel())
      {
        ++state;
      }
    }
    break;

  case 1:
    {
      static u32 counter = 0;
      if(counter >= POKEVIEWER_WAITFRAME)
      {
        counter = 0;
        ++state;
      }
      else
      {
        ++counter;
      }
    }
    break;

  case 2:
    {
      PrintScreen();
      m_IsPokeShaderEnable = true;
      ++state;
    }
    break;

  case 3:
    {
      static u32 counter = 0;
      if(counter >= POKEVIEWER_WAITFRAME)
      {
        counter = 0;
        ++state;
      }
      else
      {
        ++counter;
      }
    }
    break;

  case 4:
    {
      PrintScreen();
      m_IsPokeShaderEnable = false;

      // 色違いのキャプチャが終わっていれば終了
      if(m_PokeParam.isRare)
      {
        m_PokeParam.isRare = false;
        state = 0;
        return true;
      }

      // 終わっていなければ色違いにして最初から実行
      m_PokeParam.isRare = true;
      state = 0;
    }
    break;
  }

  return false;
}

void PokeViewerPokeModelManager::UpdatePokeShaderColor()
{
  if(m_pPokeModel)
  {
    gfl2::gfx::Color color = m_PokeShaderColor;

    if(!m_IsPokeShaderEnable)
    {
      color.SetA(0.0f);
    }

    m_pPokeModel->SetShaderColor(color);
  }
}

bool PokeViewerPokeModelManager::CheckedForDimensionalCapture()
{
  for(u32 i = CAMERA_DIRECTION_FRONT; i < CAMERA_DIRECTION_NUM; ++i)
  {
    if(m_CameraDirectionFlags[i])
    {
      return true;
    }
  }

  return false;
}

void PokeViewerPokeModelManager::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(displayNo == gfl2::gfx::CtrDisplayNo::DOWN)
  {
    return;
  }

  gfl2::gfx::DisplayDeviceHandle hDisplay = System::GflUse::m_HandleList[displayNo];
  u32 width = gfl2::gfx::GFGL::GetDisplayWidth(hDisplay);
  u32 height = gfl2::gfx::GFGL::GetDisplayHeight(hDisplay);

  gfl2::math::Matrix44 projMatrix;
  gfl2::math::Matrix34 viewMatrix;

  switch (m_CameraMode)
  {
  case test::pokeviewer::PokeViewerPokeModelManager::CAMERA_MODE_FREE:
    {
      projMatrix = gfl2::math::Matrix44::GetPerspective(
        gfl2::math::ConvDegToRad(m_CameraFovy),
        static_cast<f32>(width) / static_cast<f32>(height),
        m_CameraNear,
        m_CameraFar,
        true);

      // ビュー変換行列
      gfl2::math::Vector3 upDirection(0.0f, 1.0f, 0.0f);

      // 三面図撮影モード、上面・下面のみ上方向ベクトルを変更
      if( m_UpdateType == UPDATE_TYPE_DIMENSIONAL_CAPTURE && ( m_CurrentDirection == CAMERA_DIRECTION_TOP || m_CurrentDirection == CAMERA_DIRECTION_BOTTOM ) )
      {
        upDirection = gfl2::math::Vector3(0.0f, 0.0f, -1.0f);
      }

      viewMatrix = gfl2::math::Matrix34::GetLookAt(m_CameraPosition, m_CameraTarget, upDirection);
    }
    break;

  case test::pokeviewer::PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_CTR:
  case test::pokeviewer::PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_BTM:
    {
      projMatrix = m_pInframeCamera->GetProjectionMatrix();
#if defined(GF_PLATFORM_CTR)
      nn::math::MTX44Pivot((nn::math::MTX44*)&projMatrix, nn::math::PIVOT_UPSIDE_TO_TOP);
#endif
      viewMatrix = m_pInframeCamera->GetViewMatrix();
    }
    break;
  }

  m_pRenderingPipeline->SetProjectionMatrix(projMatrix);
  m_pRenderingPipeline->SetViewMatrix(viewMatrix);

  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer(hDisplay);
  gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(hDisplay);

  // 描画
  m_pRenderingPipeline->SetRenderTarget(pBackBuffer);
  m_pRenderingPipeline->SetDepthStencil(pDepthBuffer);
  m_pRenderingPipeline->SetDisplaySize(width, height);
  m_pRenderingPipeline->Update();
  m_pRenderingPipeline->Execute();


  {
    if (displayNo == gfl2::gfx::CtrDisplayNo::LEFT)
    {
      m_DebugTextDrawer.CLS();

      if (m_pPokeModel && (!m_pPokeModel->IsAvailableAnimationDirect(m_MotionIndex)))
      {
        m_DebugTextDrawer.PrintLn(L"invalid motionID:%d", m_MotionIndex);
      }

      m_DebugTextDrawer.Draw(displayNo);
      DrawGuideRect(m_GuideRectPixelSize);
    }
  }
}

PokeViewerPokeModelManager::CAMERA_MODE PokeViewerPokeModelManager::GetCameraMode() const
{
  return m_CameraMode;
}

void PokeViewerPokeModelManager::SetCameraMode(PokeViewerPokeModelManager::CAMERA_MODE mode)
{
  m_CameraMode = mode;

  if(mode == CAMERA_MODE_INFRAME_BTM)
  {
    m_pInframeSys->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM);
  }

  if(mode == CAMERA_MODE_INFRAME_CTR)
  {
    m_pInframeSys->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER);
  }
}

void PokeViewerPokeModelManager::UpdateCameraPosition()
{
  // インフレームカメラ更新
  if(m_CameraMode >= CAMERA_MODE_INFRAME_CTR)
  {
    m_pInframeSys->Update(m_pInframeCamera);
  }

  // スティック操作
  if (true)
  {
    gfl2::ui::VectorDevice* pStick = m_pGameManager->GetUiDeviceManager()->GetStick(0);
    gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);

    // 左スティックで回転
    if (!pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      static const f32 STIC_MIN_VAL = 0.3f;
      static const f32 STIC_MID_VAL = 0.8f;
      static const f32 STIC_ROT_SPD = 2.0f;
      static const f32 STIC_ROT_X_MAX = 80.0f;

      f32 rotX = 0;
      f32 rotY = 0;
      if (gfl2::math::Abs(pStick->GetX()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetX()) > STIC_MID_VAL)
        {
          rotY += pStick->GetX() * 3;
        }
        else
        {
          rotY += pStick->GetX();
        }
      }
      if (gfl2::math::Abs(pStick->GetY()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetY()) > STIC_MID_VAL)
        {
          rotX += pStick->GetY() * 3;
        }
        else
        {
          rotX += pStick->GetY();
        }
      }

      m_CameraRotate.x += rotX * STIC_ROT_SPD;
      m_CameraRotate.x = gfl2::math::Clamp(m_CameraRotate.x, -STIC_ROT_X_MAX, STIC_ROT_X_MAX);
      m_CameraRotate.y += rotY * STIC_ROT_SPD;
    }

    // 距離の操作
    if (pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        m_CameraLength += 5;
      }
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        m_CameraLength -= 5;
        if (m_CameraLength < 10)
        {
          m_CameraLength = 10;
        }
      }
    }
    else
    {
      // 注視点の操作
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        m_CameraTarget.y += 5;
      }
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        m_CameraTarget.y -= 5;
      }
      f32 slideValX = 0;
      if (pButton->IsHold(gfl2::ui::DIR_LEFT))
      {
        slideValX = 5;
      }
      else
      if (pButton->IsHold(gfl2::ui::DIR_RIGHT))
      {
        slideValX = -5;
      }
      
      if (slideValX != 0)
      {
        f32 slideValZ = 0;
        
        Rot_Deg( slideValX , slideValZ , m_CameraRotate.y );
        
        m_CameraTarget.x += slideValX;
        m_CameraTarget.z += slideValZ;
      }
    }

    // リセット
    if (!pButton->IsHold(gfl2::ui::BUTTON_R) && pButton->IsTrigger(gfl2::ui::BUTTON_X))
    {
      this->ResetCameraPosition();
    }
  }

  gfl2::math::Vector3 camPos(0, 0, m_CameraLength);

  Rot_Deg( camPos.z , camPos.y , m_CameraRotate.x );
  Rot_Deg( camPos.x , camPos.z , m_CameraRotate.y );

  camPos += m_CameraTarget;
  
  m_CameraPosition = camPos;
}

void PokeViewerPokeModelManager::ResetCameraPosition()
{
  m_CameraPosition.Set(0.0f, 0.0f, 0.0f);
  m_CameraTarget.Set(0.0f, 60.0f, 0.0f);
  m_CameraRotate.Set(0.0f, 0.0f, 0.0f);
  m_CameraLength = 150.0f;
}

void PokeViewerPokeModelManager::ResetCameraFovyNearFar()
{
  SetCameraFovyType(CAMERA_TYPE_DEFAULT);
  SetCameraNearFarType(CAMERA_TYPE_DEFAULT);
}

u32 PokeViewerPokeModelManager::GetCameraFovy() const
{
  return m_CameraFovy;
}

void PokeViewerPokeModelManager::SetCameraFovyType(PokeViewerPokeModelManager::CAMERA_TYPE type)
{
  m_CameraFovyType = type;

  if (m_CameraFovyType == CAMERA_TYPE_KAWAIGARI)
  {
    m_CameraFovy = 20;
  }
  else
  {
    m_CameraFovy = 40;
  }
}

void PokeViewerPokeModelManager::SetCameraNearFarType(PokeViewerPokeModelManager::CAMERA_TYPE type)
{
  m_CameraNearFarType = type;

  // ポケモンモデルはNear/Farは全て同じ設定
  m_CameraNear = 32.0f;
  m_CameraFar = 6000.0f;
}

void PokeViewerPokeModelManager::SetPresetCamera(PokeViewerPokeModelManager::CAMERA_TYPE type)
{
  SetCameraFovyType(type);
  SetCameraNearFarType(type);
  SetPokeModelPosition(type);

  switch(type)
  {
  case CAMERA_TYPE_DEFAULT:
    {
      m_CameraTarget.Set(0.0f, 60.0f, 0.0f);
      m_CameraRotate.Set(0.0f, 0.0f, 0.0f);
      m_CameraLength = 150.0f;
      break;
    }

  case CAMERA_TYPE_STATUS:
    {
      if(!m_pPokeModel)
      {
        break;
      }

      m_CameraRotate.Set(0.0f, 0.0f, 0.0f);

      // 目標高さ
      {
        const f32 rate = 1.0f / std::powf(m_pPokeModel->GetAdjustHeight(), 0.73f);
        m_CameraTarget.Set(0.0f, 45.2f / rate, 0.0f);
        m_CameraLength = 292.9f / rate;
      }
    }
    break;

  case CAMERA_TYPE_KAWAIGARI:
    {
      const f32 size = m_pPokeModel? m_pPokeModel->GetAdjustHeight() : 1.0f;

      m_CameraTarget.Set(0.0f, 50.0f * size, 0.0f);
      m_CameraRotate.Set(Deg_to_Fidx(10.0f), Deg_to_Fidx(0.0f), 0.0f);
      m_CameraLength = 250.0f * size;
    }
    break;
  }
}

void PokeViewerPokeModelManager::SetPokeModelPosition(PokeViewerPokeModelManager::CAMERA_TYPE type)
{
  if(!m_pPokeModel)
  {
    return;
  }

  switch (type)
  {
  case test::pokeviewer::PokeViewerPokeModelManager::CAMERA_TYPE_DEFAULT:
    m_pPokeModel->SetRotation(gfl2::math::Vector3(0.0f, 0.0f, 0.0f));
    break;
  case test::pokeviewer::PokeViewerPokeModelManager::CAMERA_TYPE_STATUS:
    m_pPokeModel->SetRotation(gfl2::math::Vector3(0.0f, 0.44f, 0.0f));
    break;
    // かわいがりのプリセットは見た目がkujira/sangoのPokeViewerと違う（原因不明）
  case test::pokeviewer::PokeViewerPokeModelManager::CAMERA_TYPE_KAWAIGARI:
    m_pPokeModel->SetRotation(gfl2::math::Vector3(0.0f, 0.0f, 0.0f));
    break;
  }
}

void PokeViewerPokeModelManager::UpdateLightPosition()
{
  gfl2::ui::VectorDevice* pStick = m_pGameManager->GetUiDeviceManager()->GetStick(0);
  gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);

  // Rボタンが押されていなかったら何もしない
  if (pButton->IsHold(gfl2::ui::BUTTON_R) && m_UpdateType == UPDATE_TYPE_NORMAL)
  {
    // 左スティックで回転
    {
      static const f32 STIC_MIN_VAL = 0.3f;
      static const f32 STIC_MID_VAL = 0.8f;
      static const f32 STIC_ROT_SPD = 1.0f;
      static const f32 STIC_ROT_X_MAX = 90.0f;

      f32 rotX = 0;
      f32 rotY = 0;
      if (gfl2::math::Abs(pStick->GetX()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetX()) > STIC_MID_VAL)
        {
          rotY += pStick->GetX() * 3;
        }
        else
        {
          rotY += pStick->GetX();
        }
      }
      if (gfl2::math::Abs(pStick->GetY()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetY()) > STIC_MID_VAL)
        {
          rotX += pStick->GetY() * 3;
        }
        else
        {
          rotX += pStick->GetY();
        }
      }

      m_LightRotate.x += rotX * STIC_ROT_SPD;
      m_LightRotate.x = gfl2::math::Clamp(m_LightRotate.x, -STIC_ROT_X_MAX, STIC_ROT_X_MAX);
      m_LightRotate.y += rotY * STIC_ROT_SPD;
    }

    // リセット
    if (pButton->IsTrigger(gfl2::ui::BUTTON_X))
    {
      this->ResetLightPosition();
    }
  }

  gfl2::math::Vector3 lightPos(0, 0, 1000000);

  Rot_Deg( lightPos.z , lightPos.y , m_LightRotate.x );
  Rot_Deg( lightPos.x , lightPos.z , m_LightRotate.y );

  if(m_pPokeModel && m_State == STATE_NONE)
  {
    m_pPokeModel->GetDrawEnvNode()->GetLightSet(0)->GetLightNode(0)->SetPosition(lightPos);
    gfl2::math::Vector3 direction = -lightPos.Normalize();
    m_pPokeModel->GetDrawEnvNode()->GetLightSet(0)->GetLightNode(0)->SetDirection(direction);
  }
}

void PokeViewerPokeModelManager::ResetLightPosition()
{
  m_LightRotate.Set(60, 15, 0);
}

f32 PokeViewerPokeModelManager::GetLightRotationV() const
{
  return m_LightRotate.x;
}

void PokeViewerPokeModelManager::SetLightRotationV(f32 RotV)
{
  m_LightRotate.x = RotV;
}

f32 PokeViewerPokeModelManager::GetLightRotationH() const
{
  return m_LightRotate.y;
}

void PokeViewerPokeModelManager::SetLightRotationH(f32 RotH)
{
  m_LightRotate.y = RotH;
}

void PokeViewerPokeModelManager::SetLightRotation(f32 RotV, f32 RotH)
{
  m_LightRotate.x = RotV;
  m_LightRotate.y = RotH;
}

bool PokeViewerPokeModelManager::LoadPokeModel()
{
  if (m_State == STATE_NONE)
  {
    m_State = STATE_LOAD_POKEMODEL;
    return true;
  }

  return false;
}

bool PokeViewerPokeModelManager::UnloadPokeModel()
{
  if(m_State == STATE_NONE)
  {
    m_State = STATE_UNLOAD_POKEMODEL;
    return true;
  }

  return false;
}

b32 PokeViewerPokeModelManager::GetShadowVisibility() const
{
  return m_ShadowVisibility;
}

void PokeViewerPokeModelManager::SetShadowVisibility(b32 visibility)
{
  m_ShadowVisibility = visibility;

  if(m_pPokeModel && m_pPokeModel->GetShadowModel())
  {
    if(m_ShadowVisibility)
    {
      m_pRenderingPipeline->AddNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
    }
    else
    {
      m_pRenderingPipeline->RemoveNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
    }
  }
}

void PokeViewerPokeModelManager::SetClearColor(gfl2::gfx::Color color)
{
  m_ClearColor = color;
  m_pRenderingPipeline->SetClearColor(m_ClearColor);
}

gfl2::gfx::Color PokeViewerPokeModelManager::GetClearColor() const
{
  return m_ClearColor;
}

gfl2::gfx::Color PokeViewerPokeModelManager::GetEdgeColor() const
{
  return m_EdgeColor;
}

void PokeViewerPokeModelManager::SetEdgeColor(gfl2::gfx::Color color)
{
  m_EdgeColor = color;
  m_pRenderingPipeline->SetEdgeColor(color);
}

gfl2::gfx::Color PokeViewerPokeModelManager::GetPokeShaderColor() const
{
  return m_PokeShaderColor;
}

void PokeViewerPokeModelManager::SetPokeShaderColor(gfl2::gfx::Color color)
{
  m_PokeShaderColor = color;
}

b32 PokeViewerPokeModelManager::IsPokeShaderEnable() const
{
  return m_IsPokeShaderEnable;
}

void PokeViewerPokeModelManager::SetPokeShaderEnable(b32 isEnable)
{
  m_IsPokeShaderEnable = isEnable;
}

void PokeViewerPokeModelManager::GetMotionName(char * buf) const
{
  static const char *const motionName[2][36] =
  {
    {
      "ba10_waitA01",
      "ba10_waitB01",
      "ba10_waitC01",
      "ba01_landA01",
      "ba01_landB01",
      "ba01_landC01",
      "ba01_land",
      "ba02_appeal01",
      "ba20_buturi01",
      "ba20_buturi02",
      "ba20_buturi03",
      "ba20_buturi04",
      "ba21_tokusyu01",
      "ba21_tokusyu02",
      "ba21_tokusyu03",
      "ba21_tokusyu04",
      "ba30_damageS01",
      "ba41_down01",
      "loop1",
      "loop2",
      "loop3",
      "loop4",
      "eye01",
      "eye02",
      "eye03",
      "mouth01",
      "mouth02",
      "mouth03",
    },
    {
      "kw01_wait01",
      "kw10_respond01",
      "kw11_turnA01",
      "kw11_turnB01",
      "kw20_drowseA01",
      "kw20_drowseB01",
      "kw20_drowseC01",
      "kw21_sleepA01",
      "kw21_sleepB01",
      "kw30_hate01",
      "kw31_question01",
      "kw32_happyA01",
      "kw32_happyB01",
      "kw32_happyC01",
      "kw33_moveA01",
      "kw33_moveB01",
      "kw33_moveC01",
      "kw33_moveD01",
      "kw34_lonely01",
      "kw35_playA01",
      "kw35_playB01",
      "kw36_mad01",
      "kw50_eatA01",
      "kw51_eatB01",
      "kw52_eatC01",
      "kw60_touch01",
      "loop1",
      "loop2",
      "loop3",
      "loop4",
      "eye01",
      "eye02",
      "eye03",
      "mouth01",
      "mouth02",
      "mouth03",
    }
  };

  u32 index = 0;
  if(m_pPokeModel)
  {
    index = m_AnimeType == PokeTool::MODEL_ANIMETYPE_BATTLE? 0 : 1;
  }

#if defined(GF_PLATFORM_WIN32)
  strcpy_s(buf, 16, motionName[index][m_MotionIndex]);
#else
  strcpy(buf, motionName[index][m_MotionIndex]);
#endif
}

u32 PokeViewerPokeModelManager::GetMotionIndex() const
{
  return m_MotionIndex;
}

void PokeViewerPokeModelManager::SetMotionIndex(s32 index)
{
  u32 motionNum = m_AnimeType == PokeTool::MODEL_ANIMETYPE_BATTLE? 28 : 36;

  // C++では剰余算で負数が返ってくるので正数になるよう適当に対処
  if(index < 0)
  {
    index += ( -index / motionNum + 1 ) * motionNum; 
  }

  m_MotionIndex = index % motionNum;

  if(m_pPokeModel)
  {
    m_pPokeModel->ChangeAnimationDirect(m_MotionIndex);
    m_pPokeModel->SetAnimationIsLoop(m_MotionIsLoop);
  }
}

f32 PokeViewerPokeModelManager::GetMotionMaxFrame() const
{
  if(m_pPokeModel && m_pPokeModel->GetAnimationController())
  {
    return m_pPokeModel->GetAnimationEndFrame();
  }
  else
  {
    return 0.0f;
  }
}

f32 PokeViewerPokeModelManager::GetCurrentMotionFrame() const
{
  if(m_pPokeModel && m_pPokeModel->GetAnimationController())
  {
    return m_pPokeModel->GetAnimationFrame();
  }
  else
  {
    return 0.0f;
  }
}

b32 PokeViewerPokeModelManager::GetMotionLoop() const
{
  return m_MotionIsLoop;
}

void PokeViewerPokeModelManager::SetMotionLoop(b32 isEnable)
{
  m_MotionIsLoop = isEnable;

  if(m_pPokeModel)
  {
    m_pPokeModel->SetAnimationIsLoop(isEnable);
  }
}

f32 PokeViewerPokeModelManager::GetMotionSpeed() const
{
  return m_MotionSpeed;
}

void PokeViewerPokeModelManager::SetMotionSpeed(f32 speed)
{
  m_MotionSpeed = speed;
}

void PokeViewerPokeModelManager::PlayMotion(b32 isEnable)
{
  m_IsPlaying = isEnable;

  if(m_IsPlaying && m_pPokeModel && m_MotionFrame == m_pPokeModel->GetAnimationEndFrame())
  {
    m_pPokeModel->SetAnimationFrame(0.0f);
  }
}

b32 PokeViewerPokeModelManager::IsPlaying() const
{
  return m_IsPlaying;
}

void PokeViewerPokeModelManager::PlaySingleFrame(b32 isForward)
{
  PlayMotion(false);

  if(m_pPokeModel)
  {

    s32 frame = m_pPokeModel->GetAnimationFrame();
    frame += (isForward) ? 1 : -1;
    if (frame >= static_cast<s32>(m_pPokeModel->GetAnimationEndFrame() + (m_MotionIsLoop? 0 : 1) ))
    {
      frame = 0;
    }
    else if (frame <= 0)
    {
      frame = m_pPokeModel->GetAnimationEndFrame();
    }

    m_pPokeModel->SetAnimationFrame(frame);
    m_pPokeModel->SetAnimationStepFrame(0.0f);
    m_pPokeModel->UpdateAnimation();
    m_pPokeModel->SetAnimationStepFrame(m_MotionSpeed);
  }
}

void PokeViewerPokeModelManager::SetMotionFrame(const f32 frame)
{
  if (m_pPokeModel)
  {
    m_pPokeModel->SetAnimationFrame(frame);
    m_pPokeModel->UpdateAnimation();
  }
}


PokeTool::MODEL_ANIMETYPE PokeViewerPokeModelManager::GetAnimeType() const
{
  return m_NextAnimeType;
}

void PokeViewerPokeModelManager::SetAnimeType(PokeTool::MODEL_ANIMETYPE type)
{
  m_NextAnimeType = type;
  m_MotionIndex = 0;
}

u32 PokeViewerPokeModelManager::GetStartMonsNo() const
{
  return m_StartMonsNo;
}

void PokeViewerPokeModelManager::SetStartMonsNo(u32 monsNo)
{
  m_StartMonsNo = monsNo;
}

u32 PokeViewerPokeModelManager::GetEndMonsNo() const
{
  return m_EndMonsNo;
}

void PokeViewerPokeModelManager::SetEndMonsNo(u32 monsNo)
{
  m_EndMonsNo = monsNo;
}

b32 PokeViewerPokeModelManager::IsCaptureMode() const
{
  return m_UpdateType == UPDATE_TYPE_CAPTURE || m_UpdateType == UPDATE_TYPE_CAPTURE_ALL || m_UpdateType == UPDATE_TYPE_DIMENSIONAL_CAPTURE;
}

bool PokeViewerPokeModelManager::SetNextPokeParam()
{
  // 雌雄差があるならメスのモデルをキャプチャ
  if(ExistFemale() && m_PokeParam.sex == pml::SEX_MALE)
  {
    m_PokeParam.sex = pml::SEX_FEMALE;
    return true;
  }

  m_PokeParam.sex = pml::SEX_MALE;

  // フォルム番号をインクリメントして最大値を超えていたら次のポケモンへ
  if(++m_PokeParam.formNo > GetFormMax())
  {
    m_PokeParam.formNo = 0;
    return false;
  }

  return true;
}

f32 PokeViewerPokeModelManager::GetCameraDistance() const
{
  return m_CameraDistance;
}

void PokeViewerPokeModelManager::SetCameraDistance(f32 distance)
{
  m_CameraDistance = distance;
}

bool PokeViewerPokeModelManager::GetCameraDirectionFlag(CAMERA_DIRECTION dir) const
{
  return m_CameraDirectionFlags[dir];
}

void PokeViewerPokeModelManager::ToggleCameraDirectionFlag(CAMERA_DIRECTION dir)
{
  m_CameraDirectionFlags[dir] = !m_CameraDirectionFlags[dir];
}

bool PokeViewerPokeModelManager::SetNextCameraDirection()
{
  for(u32 dir = m_CurrentDirection + 1; dir < CAMERA_DIRECTION_NUM; ++dir)
  {
    if(m_CameraDirectionFlags[dir])
    {
      m_CurrentDirection = (CAMERA_DIRECTION)dir;
      return true;
    }
  }

  return false;
}

void PokeViewerPokeModelManager::PrintScreen() const
{
  char filename[64];
  pml::personal::SexVector vec = GetSexVector();
#if defined(GF_PLATFORM_WIN32)
  sprintf_s(filename, "%s/%s_%03d_%02d%s%s_%s%s%s.bmp",
    m_IsPokeShaderEnable? "alpha" : "color",
    m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "pokepic" : "zukan_poke_pic", 
    (u32)m_PokeParam.monsNo,
    (u32)m_PokeParam.formNo,
    m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "" : "_",
    vec == pml::personal::SEX_VECTOR_UNKNOWN? m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "n" : "u" : !ExistFemale()? "s" : m_PokeParam.sex == pml::SEX_FEMALE? "f" : "m",
    m_PokeParam.isRare? "r" : "n",
    m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "_picfl" : "",
    m_UpdateType == UPDATE_TYPE_DIMENSIONAL_CAPTURE? 
    (m_CurrentDirection == CAMERA_DIRECTION_FRONT? "_front" :
     m_CurrentDirection == CAMERA_DIRECTION_BACK? "_back" :
     m_CurrentDirection == CAMERA_DIRECTION_RIGHT? "_right" :
     m_CurrentDirection == CAMERA_DIRECTION_LEFT? "_left" :
     m_CurrentDirection == CAMERA_DIRECTION_TOP? "_top" : "_bottom") 
    : "");
#else
  sprintf(filename, "%s/%s_%03d_%02d%s%s_%s%s%s.bmp",
    m_IsPokeShaderEnable? "alpha" : "color",
    m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "pokepic" : "zukan_poke_pic", 
    (u32)m_PokeParam.monsNo,
    (u32)m_PokeParam.formNo,
    m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "" : "_",
    vec == pml::personal::SEX_VECTOR_UNKNOWN? m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "n" : "u" : !ExistFemale()? "s" : m_PokeParam.sex == pml::SEX_FEMALE? "f" : "m",
    m_PokeParam.isRare? "r" : "n",
    m_FileNamePattern == FILENAME_PATTERN_POKEMONCENTER? "_picfl" : "",
    m_UpdateType == UPDATE_TYPE_DIMENSIONAL_CAPTURE? 
    (m_CurrentDirection == CAMERA_DIRECTION_FRONT? "_front" :
    m_CurrentDirection == CAMERA_DIRECTION_BACK? "_back" :
    m_CurrentDirection == CAMERA_DIRECTION_RIGHT? "_right" :
    m_CurrentDirection == CAMERA_DIRECTION_LEFT? "_left" :
    m_CurrentDirection == CAMERA_DIRECTION_TOP? "_top" : "_bottom") 
    : "");
#endif
  System::Screenshot::PrintScreen(filename, gfl2::gfx::DisplayMode::CTR_LEFT);
/*
  filename[0] = 'd';
  filename[1] = 'e';
  filename[2] = 'p';
  filename[3] = 't';
  filename[4] = 'h';
  System::Screenshot::PrintDepthMap(filename, gfl2::gfx::DisplayMode::CTR_LEFT);

  filename[0] = 'a';
  filename[1] = 'b';
  filename[2] = 's';
  filename[3] = 'd';
  filename[4] = 'p';
  System::Screenshot::PrintAbsDepthMap(filename, gfl2::gfx::DisplayMode::CTR_LEFT);


  filename[0] = 's';
  filename[1] = 't';
  filename[2] = 'n';
  filename[3] = 'c';
  filename[4] = 'l';
  System::Screenshot::PrintStencil(filename, gfl2::gfx::DisplayMode::CTR_LEFT);
*/
}

/**
 * @brief 負荷計測モードかどうかの判定
 */
b32 PokeViewerPokeModelManager::IsLoadMeasurementMode() const
{
  return (m_UpdateType == UPDATE_TYPE_LOAD_MEASUREMENT);
}

void PokeViewerPokeModelManager::SetCaptureMode(PokeViewerPokeModelManager::UPDATE_TYPE type)
{
  switch (type)
  {
  case UPDATE_TYPE_NORMAL:
    m_UpdateType = UPDATE_TYPE_NORMAL;
    break;

  case UPDATE_TYPE_CAPTURE:
    m_UpdateType = UPDATE_TYPE_CAPTURE;
    m_PokeParam.isRare = false;
    m_IsPokeShaderEnable = false;
    break;

  case UPDATE_TYPE_CAPTURE_ALL:
    if(CheckedForDimensionalCapture())
    {
      m_UpdateType = UPDATE_TYPE_DIMENSIONAL_CAPTURE;
      m_CurrentMonsNo = m_StartMonsNo;
      m_PokeParam.sex = pml::SEX_MALE;
      m_PokeParam.isRare = false;
      m_IsPokeShaderEnable = false;

      for(u32 i = CAMERA_DIRECTION_FRONT; i < CAMERA_DIRECTION_NUM; ++i)
      {
        if(m_CameraDirectionFlags[i])
        {
          m_CurrentDirection = (CAMERA_DIRECTION)i;
          break;
        }
      }

      break;
    }
    else
    {
      m_UpdateType = UPDATE_TYPE_CAPTURE_ALL;
      m_CurrentMonsNo = m_StartMonsNo;
      m_PokeParam.sex = pml::SEX_MALE;
      m_PokeParam.isRare = false;
      m_IsPokeShaderEnable = false;
      break;  
    }

  case UPDATE_TYPE_LOAD_MEASUREMENT:
    m_UpdateType = UPDATE_TYPE_LOAD_MEASUREMENT;
    m_CurrentMonsNo = m_StartMonsNo;
    m_PokeParam.sex = pml::SEX_MALE;
    m_PokeParam.isRare = false;
    m_IsPokeShaderEnable = false;
    // 基準値計測開始の状態に変更
    m_MeasurementStatus = MEASUREMENT_STATUS_START_CALC_REF_VALUE;
    break;
  }
}

b32 PokeViewerPokeModelManager::IsDisposable() const
{
  return (!m_pPokeModel && m_State == STATE_NONE);
}

void PokeViewerPokeModelManager::GetMonsName(pml::STRCODE* buf) const
{
  pml::personal::GetMonsName(buf,m_PokeParam.monsNo);
}

u32 PokeViewerPokeModelManager::GetMonsNo() const
{
  return static_cast<u32>(m_PokeParam.monsNo);
}

void PokeViewerPokeModelManager::SetMonsNo(s32 value)
{
  m_PokeParam.monsNo = (MonsNo)value;
}

u32 PokeViewerPokeModelManager::GetFormNo() const
{
  return m_PokeParam.formNo;
}

void PokeViewerPokeModelManager::ChangeFormNo(s32 value)
{
  m_PokeParam.formNo += value;
  const PokeTool::PokeModelSystem::POKE_MNG_DATA* MngData = m_pPokeModelSys->GetMngData(m_PokeParam.monsNo);

  u32 formMax = GetFormMax();

  m_PokeParam.formNo = (MonsNo)gfl2::math::clamp((u32)m_PokeParam.formNo, (u32)0, formMax);
}

u32 PokeViewerPokeModelManager::GetFormMax() const
{
  const PokeTool::PokeModelSystem::POKE_MNG_DATA* mngData = m_pPokeModelSys->GetMngData(m_PokeParam.monsNo);

  int formMax = 0;
  if(mngData->mFlgData & PokeTool::PokeModelSystem::POKE_MNG_FLG_EXIST_FORM_CHANGE)
  {
    formMax = mngData->mDataNum - 1;
    if(mngData->mFlgData & PokeTool::PokeModelSystem::POKE_MNG_FLG_EXIST_FEMALE)
    {
      --formMax;
    }
  }

  return formMax;
}

b32 PokeViewerPokeModelManager::ExistFemale() const
{
  const PokeTool::PokeModelSystem::POKE_MNG_DATA* mngData = m_pPokeModelSys->GetMngData(m_PokeParam.monsNo);

  return (mngData->mFlgData & PokeTool::PokeModelSystem::POKE_MNG_FLG_EXIST_FEMALE) && m_PokeParam.formNo == 0;
}

pml::Sex PokeViewerPokeModelManager::GetSex() const
{
  return m_PokeParam.sex;
}

pml::personal::SexVector PokeViewerPokeModelManager::GetSexVector() const
{
  m_pPersonalData->LoadData(m_PokeParam.monsNo, m_PokeParam.formNo);
  return static_cast<pml::personal::SexVector>(m_pPersonalData->GetParam(pml::personal::PARAM_ID_sex));
}

void PokeViewerPokeModelManager::ToggleSex()
{
  if(m_PokeParam.sex == pml::SEX_MALE)
  {
    m_PokeParam.sex = pml::SEX_FEMALE;
  }
  else if(m_PokeParam.sex == pml::SEX_FEMALE)
  {
    m_PokeParam.sex = pml::SEX_MALE;
  }
}

b32 PokeViewerPokeModelManager::IsRare() const
{
  return m_PokeParam.isRare;
}

void PokeViewerPokeModelManager::ToggleRare()
{
  m_PokeParam.isRare = !m_PokeParam.isRare;
}

f32 PokeViewerPokeModelManager::GetPokeBaseHeight() const
{
  if(m_pPokeModel)
  {
    return m_pPokeModel->GetBaseHeight();
  }
  else
  {
    return 0.0f;
  }
}

f32 PokeViewerPokeModelManager::GetPokeAdjustHeight() const
{
  if(m_pPokeModel)
  {
    return m_pPokeModel->GetAdjustHeight();
  }
  else
  {
    return 0.0f;
  }
}

char PokeViewerPokeModelManager::GetSizeCategory() const
{
  if(!m_pPokeModel)
  {
    return '-';
  }

  const char sizeChar[3] = {'S', 'M', 'L'};

  PokeTool::POKE_SIZE size = m_pPokeModel->GetPokeSize();

  if(size < PokeTool::POKE_SIZE_S || PokeTool::POKE_SIZE_L < size)
  {
    return '?';
  }

  return sizeChar[m_pPokeModel->GetPokeSize()];
}

void PokeViewerPokeModelManager::SetSpindaPattern()
{
  m_PokeParam.perRand = m_LeftEarValue + m_RightEarValue * 0x100 + m_LeftEyeValue * 0x10000 + m_RightEyeValue * 0x1000000;
}

void Callback_SpindaPattern(void* pWork);

void PokeViewerPokeModelManager::InitDebug()
{
  gfl2::debug::DebugWinGroup* topGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"PokeViewer", NULL);
  gfl2::debug::DebugWinItem* item;

  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"キャプチャファイル命名規則", reinterpret_cast<int*>(&m_FileNamePattern), 0, 1);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"パッチールぶち左耳", &m_LeftEarValue, 0x00, 0xff);
  item->SetChangedValueFunc(Callback_SpindaPattern, this);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"パッチールぶち右耳", &m_RightEarValue, 0x00, 0xff);
  item->SetChangedValueFunc(Callback_SpindaPattern, this);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"パッチールぶち左目", &m_LeftEyeValue, 0x00, 0xff);
  item->SetChangedValueFunc(Callback_SpindaPattern, this);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"パッチールぶち右目", &m_RightEyeValue, 0x00, 0xff);
  item->SetChangedValueFunc(Callback_SpindaPattern, this);

  gfl2::debug::DebugWin_SetCurrentItem(topGroup);
}

void PokeViewerPokeModelManager::TermDebug()
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"PokeViewer");
}

void Callback_SpindaPattern(void* pWork)
{
  PokeViewerPokeModelManager *pManager = static_cast<PokeViewerPokeModelManager*>(pWork);
  pManager->SetSpindaPattern();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  PokeViewerPokeModelManager::DrawGuideRect(const u32 rectPixelSize)
{
  if(!rectPixelSize)  return;

  const s32 screenW = 400;
  const s32 screenH = 240;


  gfl2::math::VEC2  screenRectVec[2];
  

  screenRectVec[0].x  = (screenW - static_cast<s32>(rectPixelSize))/2;
  screenRectVec[0].y  = (screenH - static_cast<s32>(rectPixelSize))/2;
  screenRectVec[1].x  = (screenW + static_cast<s32>(rectPixelSize))/2;
  screenRectVec[1].y  = (screenH + static_cast<s32>(rectPixelSize))/2;


  /*
    とりあえず上専用
  */
  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::DrawUtil::BeginRenderExtension();

  gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);
  gfl2::gfx::DrawUtil::SetMaterialColor(gfl2::math::Vector4(1,1,1,1));
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
  gfl2::gfx::DrawUtil::SetDepthTestEnable(false);


  gfl2::gfx::DrawUtil::SetScreenSize( 400, 240);
  gfl2::gfx::GFGL::SetViewport( 0, 0, 400, 240);
  gfl2::gfx::GFGL::SetScissor(0, 0, 400, 240);

  {
    const f32 w = screenRectVec[1].x - screenRectVec[0].x;
    const f32 h = screenRectVec[1].y - screenRectVec[0].y;
    const gfl2::math::Vector4 col( 0.8f, 0.3f, 0.3f, 0.5f);
    const f32 lineW = 2.0f;

    gfl2::gfx::DrawUtil::DrawRect(screenRectVec[0].x, screenRectVec[0].y, lineW,  h,      col);
    gfl2::gfx::DrawUtil::DrawRect(screenRectVec[0].x, screenRectVec[0].y, w,      lineW,  col);
    gfl2::gfx::DrawUtil::DrawRect(screenRectVec[1].x, screenRectVec[0].y, lineW,  h,      col);
    gfl2::gfx::DrawUtil::DrawRect(screenRectVec[0].x, screenRectVec[1].y, w,      lineW,  col);
  }

  gfl2::gfx::DrawUtil::EndRenderExtension();
  gfl2::gfx::GFGL::EndScene();
}


GFL_NAMESPACE_END(test)
GFL_NAMESPACE_END(pokeviewer)

#endif // PMM_DEBUG
