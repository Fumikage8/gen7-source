//======================================================================
/**
 * @file OteireTestProc.h
 * @date 2015/10/29 8:38:05
 * @author araki_syo
 * @brief お手入れ用テスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if (PM_DEBUG)

#ifndef OTEIRE_TEST_PROC_H
#define OTEIRE_TEST_PROC_H
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include "System/include/nijiAllocator.h"

#include "System/include/RenderPath/ModelRenderPath.h"

#include "Poketool/include/PokeModelSystem.h"

#include "OteireTestRenderingPipeline.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(OteireTest)

/**
 * ポケモンを順次入れ替えてスクリーンショットを撮影する
 */
class OteireTestProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(OteireTestProc); // forbid copy and assign

public:

  /**
   * @brief constructor
   * 
   */
  OteireTestProc();

  /**
   * @brief destructor
   *
   */
  virtual ~OteireTestProc();

public:
  // proc virtual functions
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);
  virtual void DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
  virtual GameSys::FrameMode GetFrameMode(void) const {return GameSys::FRAMEMODE_30;}

public:
  // debug functions
  void InitDebug();
  void TermDebug();

private:
  // private functions
  void AddDefaultDrawEnv();

private:
  // sequence counter
  u32 m_InitSeq;
  u32 m_UpdateSeq;
  u32 m_EndSeq;

  // private variables
  gfl2::heap::HeapBase* m_pHeap;
  System::nijiAllocator* m_pAllocator;
  Test::OteireTest::OteireTestRenderingPipeline* m_pRenderingPipeLine;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::LightNode* m_pDirectionalLightNode;

  static const u32 POKE_NUM = 2;
  PokeTool::PokeModelSystem* m_pPokeModelSys;
  PokeTool::PokeModel* m_pPokeModel;

  poke_3d::model::CharaModel m_CharaModel;
  poke_3d::model::CharaModelFactory m_CharaModelFactory;

  int m_MonsNo;
  int m_FormNo;
  int m_Sex;
  bool m_Rare;
  static const u32 m_AnimeType = 0;
  static const u32 m_PokeIdx = 0;
  static const u32 m_MotionIdx = 0;
};

GFL_NAMESPACE_END(OteireTest)
GFL_NAMESPACE_END(Test)

#endif // OTEIRE_TEST_PROC_H

#endif // PM_DEBUG
