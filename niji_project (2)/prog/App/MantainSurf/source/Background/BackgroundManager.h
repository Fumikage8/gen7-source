//======================================================================
/**
 * @file BackgroundManager.h
 * @date 2016/10/14 16:31:43
 * @author fang_yicheng
 * @brief 背景マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BACKGROUND_MANAGER_H_INCLUDED__
#define __BACKGROUND_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include "../Model/ModelBackground.h"
#include "../Model/ModelSea.h"
#include "../Player/PlayerModelSet.h"
#include "../Nami/NamiController.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(Background)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Background)

class BackgroundManager
{
  GFL_FORBID_COPY_AND_ASSIGN(BackgroundManager);

public:

  static const gfl2::math::Vector3 DEFAULT_ISLAND_OFFSET;
  static const f32 MAX_ISLAND_SCALE;
  static const f32 DEFAULT_SEA_SCALE;

  struct SetupParam
  {
    gfl2::gfx::IGLAllocator*              m_pGfxAllocator;
    gfl2::heap::HeapBase*                 m_pHeap;
    MantainSurfAppManager*                m_pAppManager;
    MantainSurfRenderingPipeline*         m_pRenderingPipeLine;
    Resource::ResourceManager*            m_pResourceManager;
    Nami::NamiController*                 m_pNamiController;
    Player::PlayerModelSet*               m_pPlayerModelSet;


    SetupParam()
    {
      m_pGfxAllocator = NULL;
      m_pHeap = NULL;
      m_pAppManager = NULL;
      m_pRenderingPipeLine = NULL;
      m_pResourceManager = NULL;
      m_pNamiController = NULL;
      m_pPlayerModelSet = NULL;
    }
  };

  BackgroundManager();
  virtual ~BackgroundManager();

  void Initialize(const SetupParam &setupParam);
  void Terminate();
  void Update();
  void UpdateTimeZone(s32 timeZone);

private:

  void UpdateBackgourndPosition();
  void UpdateSeaPosition();

private:

  SetupParam  m_SetupParam;

  Model::ModelBackground m_BackgroundModel;
  Model::ModelSea        m_SeaModel;
};

GFL_NAMESPACE_END(Background)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __BACKGROUND_MANAGER_H_INCLUDED__
