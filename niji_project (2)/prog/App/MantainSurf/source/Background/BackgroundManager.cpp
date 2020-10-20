//======================================================================
/**
 * @file BackgroundManager.cpp
 * @date 2016/10/14 16:31:14
 * @author fang_yicheng
 * @brief 背景マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "BackgroundManager.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Background)


const gfl2::math::Vector3 BackgroundManager::DEFAULT_ISLAND_OFFSET = gfl2::math::Vector3(0.0f, 0.0f, 30000.0f);  // 背景がプレイヤーとの相対距離
const f32 BackgroundManager::MAX_ISLAND_SCALE  = 6.0f;                    // 背景の最大大きさ
const f32 BackgroundManager::DEFAULT_SEA_SCALE = 30000.0f / 28415.0f;     // デフォルトの海スケール


const f32 POSITION_X_START    = 10000.0f;   // 開始時のX軸座標
const f32 POSITION_X_END      = 5000.0f;    // 終了時のX軸座標
const f32 POSITION_Y_START    = 0.0f;       // 開始時のY軸座標
const f32 POSITION_Y_END      = 0.0f;       // 終了時のY軸座標
const f32 DISPLAY_THREDSHOLD  = 0.5f;       // 背景が見えるようになった進行具合の閾値

BackgroundManager::BackgroundManager()
{
}

BackgroundManager::~BackgroundManager()
{
}

void BackgroundManager::Initialize(const SetupParam &setupParam)
{
  m_SetupParam = setupParam;

  // 島を作成
  {
    m_BackgroundModel.Initialize(m_SetupParam.m_pGfxAllocator, m_SetupParam.m_pHeap, m_SetupParam.m_pResourceManager, NULL);
    setupParam.m_pRenderingPipeLine->AddBgModel(m_BackgroundModel.GetModel());
    m_BackgroundModel.SetRotation(gfl2::math::Vector3(0.0f, PI, 0.0f));
    m_BackgroundModel.SetPosition(gfl2::math::Vector3(POSITION_X_START, POSITION_Y_START, 0.0f));
    m_BackgroundModel.SetScale(gfl2::math::Vector3::GetZero());
    m_BackgroundModel.StartAnimation();
  }

  // 海を作成
  {
    m_SeaModel.Initialize(m_SetupParam.m_pGfxAllocator, m_SetupParam.m_pHeap, m_SetupParam.m_pResourceManager, NULL);
    setupParam.m_pRenderingPipeLine->AddBgModel(m_SeaModel.GetModel());
    m_SeaModel.SetRotation(gfl2::math::Vector3(0.0f, PI, 0.0f));
    m_SeaModel.SetPosition(gfl2::math::Vector3::GetZero());
    m_SeaModel.SetScale(gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * DEFAULT_SEA_SCALE);
    m_SeaModel.StartAnimation();
  }
  UpdateBackgourndPosition();
  UpdateSeaPosition();
}

void BackgroundManager::Terminate()
{
  m_SeaModel.Terminate();
  m_BackgroundModel.Terminate();
}

void BackgroundManager::Update()
{
  UpdateBackgourndPosition();
  UpdateSeaPosition();
  m_BackgroundModel.Update();
  m_SeaModel.Update();
}

void BackgroundManager::UpdateTimeZone(s32 timeZone)
{
  m_BackgroundModel.UpdateTimeZone(timeZone);
  m_SeaModel.UpdateTimeZone(timeZone);
}

void BackgroundManager::UpdateBackgourndPosition()
{
  gfl2::math::Vector3 newPos = m_BackgroundModel.GetPosition();

  gfl2::math::Vector3 offset = DEFAULT_ISLAND_OFFSET;
  f32 maxScale = MAX_ISLAND_SCALE;
  f32 progress = (m_SetupParam.m_pNamiController->GetProgress() - DISPLAY_THREDSHOLD) / DISPLAY_THREDSHOLD;
  if(progress < 0.0f)
  {
    progress = 0.0f;
  }

#if PM_DEBUG
  // デバッグ値を適用
  offset.x = m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvBgIslandOffsetX;
  offset.y = m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvBgIslandOffsetY;
  offset.z = m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvBgIslandOffsetZ;
  maxScale = m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvBgIslandScale;
  if(m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvIsInfinityMode)
  {
    progress = 1.0f;
  }
#endif

  // 配置位置更新
  if(!m_SetupParam.m_pNamiController->IsFinish())
  {
    gfl2::math::Vector3 playerPos = m_SetupParam.m_pPlayerModelSet->GetPosition();
    newPos.z = playerPos.z + offset.z;
  }

  newPos.x = gfl2::math::Lerp(POSITION_X_START, POSITION_X_END, progress) + offset.x;
  newPos.y = gfl2::math::Lerp(POSITION_Y_START, POSITION_Y_END, progress) + offset.y;
  m_BackgroundModel.SetPosition(newPos);
  m_BackgroundModel.SetScale(gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * progress * maxScale);
}

void BackgroundManager::UpdateSeaPosition()
{
  // 海の位置を更新
  if(!m_SetupParam.m_pNamiController->IsFinish())
  {
    gfl2::math::Vector3 newPos = m_SeaModel.GetPosition();
    newPos.z = m_SetupParam.m_pAppManager->GetPlayerModelSet()->GetPosition().z;
    m_SeaModel.SetPosition(newPos);
  }
#if PM_DEBUG
  m_SeaModel.SetScale(gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvBgSeaScale);
#endif
}

GFL_NAMESPACE_END(Background)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
