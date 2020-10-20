/**
 * @file ArakiTestProc.h
 * @date 2015/06/11
 * @author araki_syo
 * @brief ArakiTestProcヘッダーファイル
 */

#if (PM_DEBUG)

#ifndef	__ARAKI_TEST_PROC_H__
#define	__ARAKI_TEST_PROC_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>


#include <GameSys/include/GameProc.h>
#include <System/include/GflUse.h>
#include <system/include/nijiAllocator.h>

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(ArakiTest)

/**
 * キャラクターを順次着せ替えてスクリーンショットを撮影する
 */
class ArakiTestProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(ArakiTestProc); // forbid copy and assign

public:

  /**
   * @brief constructor
   * 
   */
  ArakiTestProc();

  /**
   * @brief destructor
   *
   */
  virtual ~ArakiTestProc();

  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);
  virtual void DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
  virtual GameSys::FrameMode GetFrameMode(void) const {return GameSys::FRAMEMODE_30;}

private:
  gfl2::heap::HeapBase* m_pHeap;
  System::nijiAllocator* m_pAllocator;

  u32 m_InitSeq;
  u32 m_UpdateSeq;
  u32 m_EndSeq;
  s32 m_ItemParamSeq;
  s32 m_BodyParamSeq;

  b32 m_TestFlag;

  s32 m_Counter;

  s32 m_StartFreeSize;

  void* buf;

  gfl2::renderingengine::renderer::DefaultRenderingPipeLine* m_pRenderingPipeLine;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;

  poke_3d::model::CharaModelFactory m_CharaModelFactory;
  poke_3d::model::DressUpModelResourceManager m_DressUpModelResourceManager;

  poke_3d::model::DressUpModel m_DressUpModel;
  poke_3d::model::DressUpParam m_DressUpParam;

private:
  inline void SetBodyParams();
  inline void SetBodyParams(s32 bodyColorID, s32 eyeColorID, s32 hairColorID, s32 ripColorID);
  inline void SetItemParams();
  inline void SetItemParams(s32 faceID, s32 hairStyleID, s32 badgeAccessoryID, s32 eyeAccessoryID, s32 hairAccessoryID, s32 bagID, s32 bangleID, s32 bodyID, s32 bottomsID, s32 hatID, s32 legsID, s32 shoesID, s32 topsID);
};

GFL_NAMESPACE_END(ArakiTest)
GFL_NAMESPACE_END(Test)

#endif	// __ARAKI_TEST_PROC_H__

#endif // PM_DEBUG
