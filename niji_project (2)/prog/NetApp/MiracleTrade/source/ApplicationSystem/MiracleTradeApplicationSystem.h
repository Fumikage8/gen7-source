// ============================================================================
/*
 * @file MiracleTradeApplicationSystem.h
 * @brief ミラクル交換のアプリケーションシステムです。
 * @date 2015.06.01
 */
// ============================================================================
#if !defined( MIRACLETRADEAPPLICATIONSYSTEM_H_INCLUDE )
#define MIRACLETRADEAPPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppEffectBG.h"
#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_BaseCamera.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class MiracleTradeWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class MiracleTradeBuilder;
class MiracleTradeViewObjects;

class MiracleTradeApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeApplicationSystem );

public:
  MiracleTradeApplicationSystem();
  virtual~MiracleTradeApplicationSystem();

  void Build( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeBuilder* pApplicationSystemBuilder );
#if PM_DEBUG
  void  DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo);
#endif


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();

  virtual void AddModel();
  virtual void RemoveModel();
  virtual void SetupCamera();
  virtual void UnsetCamera();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 2;

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*                  m_pMiracleTradeWork;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects*           m_pMiracleTradeViewObjects;
  NetAppLib::Message::MessageUtility*                                         m_pMessageUtility;
  NetAppLib::UI::NetAppEmptyView*                                             m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                                             m_pRootLowerView;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*                  m_pModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*                  m_pMotionResourceNode;
  poke_3d::model::BaseModel*                                                  m_pBackgroundModel;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*                   m_pUpperDrawEnvNodeOfCamera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*                   m_pUpperDrawEnvNode;
  poke_3d::model::BaseCamera*                                                 m_pUpperCamera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*                   m_pLowerDrawEnvNode;
  poke_3d::model::BaseCamera*                                                 m_pLowerCamera;

  NetAppLib::Util::NetAppPokeModelUtility*                                    m_pPokeModelUtility;

  NetAppLib::Util::NetAppEffectBG                                             m_EffectBG;

  gfl2::math::VEC3                                                            m_vBGCameraUpperPosition;
  gfl2::math::VEC3                                                            m_vBGCameraUpperTarget;
  gfl2::math::VEC3                                                            m_vBGCameraUpperUpVector;
  float                                                                       m_fBGCameraUpperFovY;
  float                                                                       m_fBGCameraUpperNear;
  float                                                                       m_fBGCameraUpperFar;
  gfl2::math::VEC3                                                            m_vBGCameraLowerPosition;
  gfl2::math::VEC3                                                            m_vBGCameraLowerTarget;
  gfl2::math::VEC3                                                            m_vBGCameraLowerUpVector;
  float                                                                       m_fBGCameraLowerFovY;
  float                                                                       m_fBGCameraLowerNear;
  float                                                                       m_fBGCameraLowerFar;

  gfl2::math::VEC3                                                            m_vEffectCameraUpperPosition;
  gfl2::math::VEC3                                                            m_vEffectCameraUpperTarget;
  gfl2::math::VEC3                                                            m_vEffectCameraUpperUpVector;
  float                                                                       m_fEffectCameraUpperFovY;
  float                                                                       m_fEffectCameraUpperNear;
  float                                                                       m_fEffectCameraUpperFar;
  gfl2::math::VEC3                                                            m_vEffectCameraLowerPosition;
  gfl2::math::VEC3                                                            m_vEffectCameraLowerTarget;
  gfl2::math::VEC3                                                            m_vEffectCameraLowerUpVector;
  float                                                                       m_fEffectCameraLowerFovY;
  float                                                                       m_fEffectCameraLowerNear;
  float                                                                       m_fEffectCameraLowerFar;

  int                                                                         m_nPokeInFrameTop;
  int                                                                         m_nPokeInFrameBottom;
  int                                                                         m_nPokeInFrameLeft;
  int                                                                         m_nPokeInFrameRight;

#if PM_DEBUG
  u32                                                                         m_FourceVisibleFlag;
#endif // PM_DEBUG

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)


#endif // MIRACLETRADEAPPLICATIONSYSTEM_H_INCLUDE
