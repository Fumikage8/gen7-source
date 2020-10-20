//======================================================================
/**
 * @file MantainSurfDrawManager.cpp
 * @date 2016/12/06 17:30:03
 * @author fang_yicheng
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "MantainSurfDrawManager.h"

#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#if defined(GF_PLATFORM_CTR)
#include <RenderingEngine/include/Renderer/gfl2_CombinerCtrShaderDriver.h>
#endif

#define DRAW_TEXT_ENABLE 0

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

// バッファクリア設定、初期値
static const GameSys::GameManager::BUFFER_CLEAR_SETTING sc_BufferClearDefaultSettingTable[] = 
{
  // 0.75
  {
    gfl2::gfx::Color( 0.75f, 0.75f, 0.75f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
  // 0.5
  {
    gfl2::gfx::Color( 0.5f, 0.5f, 0.5f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
  // 0.3
  {
    gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
  // 0.0
  {
    gfl2::gfx::Color( 0.0f, 0.0f, 0.0f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
};

MantainSurfDrawManager::MantainSurfDrawManager() :
  m_seq(0),
  m_pAppHeap(NULL),
  m_pNijiAllocator(NULL),
  m_pGameManager(NULL),
  m_pAppRenderingManager(NULL),
  m_pRenderingPipeLine(NULL),
  m_pTextRenderingPipeLine(NULL),
  m_pTextDrawUtil(NULL),
  m_pHeapAllocator(NULL),
  m_ClearColorType(0),
  m_pCamera(NULL),
  m_pDrawEnvNode(NULL),
  m_pDirectionalLightNode(NULL)
{
}

MantainSurfDrawManager::~MantainSurfDrawManager()
{
}

void MantainSurfDrawManager::Initialize(System::nijiAllocator* pNijiAllocator, app::util::Heap* pAppHeap, GameSys::GameManager* pGameManager)
{
  m_pNijiAllocator = pNijiAllocator;
  m_pAppHeap = pAppHeap;
  m_pGameManager = pGameManager;

#if DRAW_TEXT_ENABLE
  m_pTextRenderingPipeLine = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::util::UtilTextRenderingPipeLine(m_pNijiAllocator, 400.0f, 240.0f);

  m_pHeapAllocator = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::heap::NwAllocator(m_pAppHeap->GetDeviceHeap());

  m_pTextDrawUtil = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::util::DrawUtilText(m_pHeapAllocator, m_pHeapAllocator);
  m_pTextDrawUtil->Initialize(m_pHeapAllocator, m_pHeapAllocator, m_pTextRenderingPipeLine->GetUtilTextRenderPath());
  m_pTextDrawUtil->SetRenderPath(m_pTextRenderingPipeLine->GetUtilTextRenderPath());
  m_pTextDrawUtil->SetFont(GFL_SINGLETON_INSTANCE(print::SystemFont)->GetFont());
#endif
}

bool MantainSurfDrawManager::IsInitializeFinished()
{
  switch(m_seq)
  {
  case 0:
    // レンダリングマネージャ
    {
      m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
      app::util::AppRenderingManager::Description desc;
      desc.callGlobalTraverseModel = false;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      m_pAppRenderingManager->StartAsyncCreate(
        desc,
        m_pAppHeap,
        NULL,
        NULL
        );
      m_seq++;
    }

  case 1:
    if(!m_pAppRenderingManager->IsCreated())
    {
      m_pAppRenderingManager->Update();
    }
    if(!m_pAppRenderingManager->IsCreated())
    {
      return false;
    }
    m_seq++;
    
  case 2:
    // レンダリングパイプライン
    m_pRenderingPipeLine = GFL_NEW(m_pAppHeap->GetDeviceHeap()) MantainSurfRenderingPipeline();
    m_pRenderingPipeLine->Initialize(m_pGameManager->GetAsyncFileManager(), m_pAppHeap->GetDeviceHeap(), m_pNijiAllocator);
    m_seq++;

  case 3:
    if(!m_pRenderingPipeLine->IsInitializeFinished())
    {
      return false;
    }
    CreateDrawEnv();
    m_seq++;

  case 4:
    return true;

  default:
    return false;
  }
}

void MantainSurfDrawManager::CreateDrawEnv()
{
  // 画面のクリアカラー設定
  for(u32 i = 0; i < gfl2::gfx::CtrDisplayNo::DOWN; ++i)
  {
    m_pGameManager->GetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, &m_oldBufferClearSettingList[i]);
    m_pGameManager->SetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, sc_BufferClearDefaultSettingTable[m_ClearColorType]);
  }

  m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_pNijiAllocator);

  // ライト0
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode		*pLightSetNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pDirectionalLightNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pAmbientLightNode;

    pLightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_pNijiAllocator);
    pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);
    pAmbientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);

    {
      gfl2::math::Vector3 lightPos(0.0f, 500000.0f, 500000.0f);

      pDirectionalLightNode->SetNodeName("DirectionalLight01");
      gfl2::util::FnvHash				nameHash(reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName()));
      pDirectionalLightNode->SetNodeNameHash(nameHash.value);
      pDirectionalLightNode->SetLightType(gfl2::renderingengine::LightType::Directional);
      pDirectionalLightNode->SetPosition(lightPos);
      gfl2::math::Vector3 direction = -lightPos.Normalize();
      pDirectionalLightNode->SetDirection(direction);
      pDirectionalLightNode->SetColor(gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
    }

    {
      pAmbientLightNode->SetNodeName("AmbientLight01");
      gfl2::util::FnvHash				nameHash(reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()));
      pAmbientLightNode->SetNodeNameHash(nameHash.value);
      pAmbientLightNode->SetLightType(gfl2::renderingengine::LightType::Ambient);
      pAmbientLightNode->SetColor(gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
    }

    pLightSetNode->SetSetNo(0);
    pLightSetNode->AddLight(pDirectionalLightNode);//LightSetNodeの子となる
    pLightSetNode->AddLight(pAmbientLightNode);//LightSetNodeの子となる
    m_pDrawEnvNode->AddLightSet(pLightSetNode);//DrawEnvNodeの子となる
  }

  // ライト2
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode		*pLightSetNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pDirectionalLightNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pAmbientLightNode;

    pLightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_pNijiAllocator);
    pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);
    pAmbientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);

    {
      gfl2::math::Vector3 lightPos(0.0f, 500000.0f, 500000.0f);

      pDirectionalLightNode->SetNodeName("DirectionalLight02");
      gfl2::util::FnvHash				nameHash(reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName()));
      pDirectionalLightNode->SetNodeNameHash(nameHash.value);
      pDirectionalLightNode->SetLightType(gfl2::renderingengine::LightType::Directional);
      pDirectionalLightNode->SetPosition(lightPos);
      gfl2::math::Vector3 direction = -lightPos.Normalize();
      pDirectionalLightNode->SetDirection(direction);
      pDirectionalLightNode->SetColor(gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
    }

    {
      pAmbientLightNode->SetNodeName("AmbientLight02");
      gfl2::util::FnvHash				nameHash(reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()));
      pAmbientLightNode->SetNodeNameHash(nameHash.value);
      pAmbientLightNode->SetLightType(gfl2::renderingengine::LightType::Ambient);
      pAmbientLightNode->SetColor(gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
    }

    pLightSetNode->SetSetNo(2);
    pLightSetNode->AddLight(pDirectionalLightNode);//LightSetNodeの子となる
    pLightSetNode->AddLight(pAmbientLightNode);//LightSetNodeの子となる
    m_pDrawEnvNode->AddLightSet(pLightSetNode);//DrawEnvNodeの子となる

    m_pDirectionalLightNode = pDirectionalLightNode;
  }

  gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
  m_pDrawEnvNode->SetAABB(envSpace);
  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(m_pDrawEnvNode);

  m_pRenderingPipeLine->AddDrawEnv(m_pDrawEnvNode);
}

void MantainSurfDrawManager::Finalize()
{
#if defined(GF_PLATFORM_CTR)
  // シングルコマンドリストモード終了
  gfl2::gfx::GFGL::SetSigleCommandListMode(false);
#endif

  // UIレンダリングマネージャーを削除
  GFL_SAFE_DELETE(m_pAppRenderingManager);

  // レンダリングパイプライン削除
  m_pRenderingPipeLine->Finalize();
  GFL_SAFE_DELETE(m_pRenderingPipeLine);
  GFL_SAFE_DELETE(m_pTextRenderingPipeLine);

  if(m_pTextDrawUtil)
  {
    m_pTextDrawUtil->Terminate();
  }
  GFL_SAFE_DELETE(m_pTextDrawUtil);
  GFL_SAFE_DELETE(m_pHeapAllocator);

  // DrawEnvNode削除
  // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも殺されるらしい
  GFL_SAFE_DELETE(m_pDrawEnvNode);

  // 画面のクリアカラー設定
  for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::DOWN; ++i)
  {
    m_pGameManager->SetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, m_oldBufferClearSettingList[i]);
  }
}

void MantainSurfDrawManager::SetCamera(Camera::Camera* pCamera)
{
  m_pCamera = pCamera;
  //m_pAppRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pCamera->GetBaseCamera());
}

void MantainSurfDrawManager::Update()
{
  // シーンのトラバース
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

void MantainSurfDrawManager::UpdateLighting(const gfl2::math::Vector3 &color)
{
  gfl2::renderingengine::scenegraph::instance::LightSetNode		*pLightSetNode;
  gfl2::renderingengine::scenegraph::instance::LightNode			*pLightNode;

  // ライト0
  {
    pLightSetNode = m_pDrawEnvNode->GetLightSet(0);
    pLightNode = pLightSetNode->GetLightNode(1); // AmbientLight
    pLightNode->SetColor(color);
  }

  // ライト2
  {
    pLightSetNode = m_pDrawEnvNode->GetLightSet(2);
    pLightNode = pLightSetNode->GetLightNode(1); // AmbientLight
    pLightNode->SetColor(color);
  }
}

void MantainSurfDrawManager::PreDraw()
{
  //m_pAppRenderingManager->PreDraw();
}

void MantainSurfDrawManager::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
#if DRAW_TEXT_ENABLE
#ifdef GF_PLATFORM_CTR
  m_pTextDrawUtil->SetTextColor(gfl2::math::Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  m_pTextDrawUtil->DrawText(0, 0, L"Test Text");
#else
  m_pTextDrawUtil->DrawTextW(0, 0, L"Test Text");
#endif
  m_pTextDrawUtil->SetRenderPath(m_pTextRenderingPipeLine->GetUtilTextRenderPath());
  gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];
  u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);
  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer(System::GflUse::m_HandleList[displayNo]);
  gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[displayNo]);
  // 描画
  m_pTextRenderingPipeLine->SetRenderTarget(pBackBuffer);
  m_pTextRenderingPipeLine->SetDepthStencil(pDepthBuffer);
  m_pTextRenderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
  m_pTextRenderingPipeLine->Update();
  m_pTextRenderingPipeLine->Execute();
#endif

  if(displayNo == gfl2::gfx::CtrDisplayNo::LEFT)
  {
    DrawModel(displayNo);
  }
  DrawLayout(displayNo);
}

void MantainSurfDrawManager::DrawLayout(gfl2::gfx::CtrDisplayNo displayNo)
{
  m_pAppRenderingManager->Update();
  m_pAppRenderingManager->Draw(displayNo);
}

void MantainSurfDrawManager::DrawModel(gfl2::gfx::CtrDisplayNo displayNo)
{
  gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];
  u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);
  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer(System::GflUse::m_HandleList[displayNo]);
  gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[displayNo]);
  
  if(displayWidth > 0 && displayHeight > 0)
  {
    // 変換行列セットアップ
    f32 ratio = static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight);
    m_pCamera->SetupMatrix(ratio);
  }

  gfl2::math::Matrix44 projMatrix = m_pCamera->GetProjectionMatrix();
  m_pRenderingPipeLine->SetProjectionMatrix(projMatrix);
  m_pRenderingPipeLine->SetViewMatrix(m_pCamera->GetViewMatrix());

#if defined(GF_PLATFORM_CTR)

  gfl2::math::Matrix44 projMat[gfl2::gfx::CtrDisplayNo::NumberOf];
  gfl2::math::Matrix34 viewMat[gfl2::gfx::CtrDisplayNo::NumberOf];

  static f32 depthLevel = 30.0f;
  static f32 factor = 0.15f;

  if(nn::gx::CTR::IsStereoVisionAllowed() == false || gfl2::gfx::ctr::CTRGL::IsStereoModeEnable() == false)
  {
    factor = 0.0f;
  }
  else
  {
    // @fix GFNMCat[4537]Finalビルド対応
    factor = 0.15f;
  }


  // ビュースペースのベース用行列
  gfl2::gfx::GFGL::GetStereoMatrix(projMatrix,
    // @fix GFNMcat[1919]他、フィールドカメラが小刻みに震える、対処。ViewSpace描画ではGPUに単位行列×立体視差分が含まれた行列が渡されるが、その行列の生成計算式に問題があり、カメラ座標が大きい際に計算誤差が発生していた。
    // 立体視視差をとるだけなので、単位行列から単位行列×立体視差分行列を作成するように修正
    gfl2::math::Matrix34::GetIdentity(),
    depthLevel,
    factor,
    &projMat[gfl2::gfx::CtrDisplayNo::LEFT],
    &viewMat[gfl2::gfx::CtrDisplayNo::LEFT],
    &projMat[gfl2::gfx::CtrDisplayNo::RIGHT],
    &viewMat[gfl2::gfx::CtrDisplayNo::RIGHT]);


  gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(viewMat[gfl2::gfx::CtrDisplayNo::LEFT]);
#endif

  // 描画
  m_pRenderingPipeLine->SetRenderTarget(pBackBuffer);
  m_pRenderingPipeLine->SetDepthStencil(pDepthBuffer);
  m_pRenderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
  m_pRenderingPipeLine->Update();
  m_pRenderingPipeLine->Execute();
}

MantainSurfRenderingPipeline* MantainSurfDrawManager::GetRenderingPipeline()
{
  return m_pRenderingPipeLine;
}

app::util::AppRenderingManager* MantainSurfDrawManager::GetLayoutRenderingManager()
{
  return m_pAppRenderingManager;
}

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
