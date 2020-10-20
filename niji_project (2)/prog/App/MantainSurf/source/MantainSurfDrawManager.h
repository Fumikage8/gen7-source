//======================================================================
/**
 * @file MantainSurfDrawManager.h
 * @date 2016/12/06 17:30:03
 * @author fang_yicheng
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MANTAIN_SURF_DRAW_MANAGER_H__
#define __MANTAIN_SURF_DRAW_MANAGER_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <Debug/include/gfl2_Assert.h>
#include <Debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

// niji
#include <GameSys/include/GameProc.h>
#include <System/include/GflUse.h>
#include <System/include/nijiAllocator.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include "Camera/Camera.h"

#include "MantainSurfRenderingPipeline.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

/**
 * @class MantainSurfDrawManager
 * @brief 描画管理
 */
class MantainSurfDrawManager
{
  GFL_FORBID_COPY_AND_ASSIGN(MantainSurfDrawManager);

public:

  MantainSurfDrawManager();
  virtual ~MantainSurfDrawManager();

  void Initialize(System::nijiAllocator* pNijiAllocator, app::util::Heap* pAppHeap, GameSys::GameManager* pGameManager);
  bool IsInitializeFinished();
  void Finalize();

  void SetCamera(Camera::Camera* pCamera);

  MantainSurfRenderingPipeline*   GetRenderingPipeline();
  app::util::AppRenderingManager* GetLayoutRenderingManager();

  void Update();
  void UpdateLighting(const gfl2::math::Vector3 &color);

  void PreDraw();
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:

  void CreateDrawEnv();

  void DrawLayout(gfl2::gfx::CtrDisplayNo displayNo);
  void DrawModel(gfl2::gfx::CtrDisplayNo displayNo);

private:

  app::util::Heap* m_pAppHeap;
  System::nijiAllocator* m_pNijiAllocator;

  app::util::AppRenderingManager*         m_pAppRenderingManager;
	MantainSurfRenderingPipeline*           m_pRenderingPipeLine;
  gfl2::util::UtilTextRenderingPipeLine*  m_pTextRenderingPipeLine;
  gfl2::util::DrawUtilText*               m_pTextDrawUtil;

  gfl2::heap::NwAllocator*		m_pHeapAllocator;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::LightNode* m_pDirectionalLightNode;

  GameSys::GameManager* m_pGameManager;

  Camera::Camera* m_pCamera;
  GameSys::GameManager::BUFFER_CLEAR_SETTING m_oldBufferClearSettingList[gfl2::gfx::CtrDisplayNo::NumberOf];
  u32 m_ClearColorType;

  s32 m_seq;
  
};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MANTAIN_SURF_DRAW_MANAGER_H__
