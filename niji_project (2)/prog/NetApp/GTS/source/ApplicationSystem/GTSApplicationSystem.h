// ============================================================================
/*
 * @file GTSApplicationSystem.h
 * @brief GTSのアプリケーションシステムです。
 * @date 2015.06.01
 */
// ============================================================================
#if !defined( GTSAPPLICATIONSYSTEM_H_INCLUDE )
#define GTSAPPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"
#include "AppLib/include/Util/app_util_heap.h"
#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_BaseCamera.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class GTSWork;
class GTSBuilder;
class GTSViewObjects;

class GTSApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSApplicationSystem );

public:
  GTSApplicationSystem();
  virtual~GTSApplicationSystem();

  void Build( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork, NetApp::GTS::ApplicationSystem::GTSBuilder* pApplicationSystemBuilder );

#if PM_DEBUG
  void  DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo);
#endif


private:

  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual bool IsCreateViewComplete();

  virtual void AddModel();
  virtual void RemoveModel();
  virtual void SetupCamera();
  virtual void UnsetCamera();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();


private:

  static const u32    MESSAGE_NUM   = 3;

  NetApp::GTS::ApplicationSystem::GTSWork*                            m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                     m_pGTSViewObjects;
  NetAppLib::Message::MessageUtility*                                 m_pMessageUtility;
  NetAppLib::Message::MessageUtility*                                 m_pJapanMonsNameMessageUtility;
  NetAppLib::Message::MessageUtility*                                 m_pEnglishMonsNameMessageUtility;
  NetAppLib::UI::NetAppEmptyView*                                     m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                                     m_pRootLowerView;


  gfl2::renderingengine::scenegraph::resource::ResourceNode*          m_pModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*          m_pMotionResourceNode;
  poke_3d::model::BaseModel*                                          m_pBackgroundModel;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*           m_pBoxUpperDrawEnvNode;
  poke_3d::model::BaseCamera*                                         m_pBoxUpperCamera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*           m_pUpperDrawEnvNodeOfCamera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*           m_pUpperDrawEnvNode;
  poke_3d::model::BaseCamera*                                         m_pUpperCamera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*           m_pLowerDrawEnvNode;
  poke_3d::model::BaseCamera*                                         m_pLowerCamera;

  NetAppLib::Util::NetAppPokeModelUtility*                            m_pPokeModelUtility;

  gfl2::math::VEC3                                                    m_vBGCameraUpperPosition;
  gfl2::math::VEC3                                                    m_vBGCameraUpperTarget;
  gfl2::math::VEC3                                                    m_vBGCameraUpperUpVector;
  float                                                               m_fBGCameraUpperFovY;
  float                                                               m_fBGCameraUpperNear;
  float                                                               m_fBGCameraUpperFar;
  gfl2::math::VEC3                                                    m_vBGCameraLowerPosition;
  gfl2::math::VEC3                                                    m_vBGCameraLowerTarget;
  gfl2::math::VEC3                                                    m_vBGCameraLowerUpVector;
  float                                                               m_fBGCameraLowerFovY;
  float                                                               m_fBGCameraLowerNear;
  float                                                               m_fBGCameraLowerFar;

  int                                                                 m_nPokeInFrameTop;
  int                                                                 m_nPokeInFrameBottom;
  int                                                                 m_nPokeInFrameLeft;
  int                                                                 m_nPokeInFrameRight;

#if PM_DEBUG
  u32                                                                 m_FourceVisibleFlag;
#endif // PM_DEBUG

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)


#endif // GTSAPPLICATIONSYSTEM_H_INCLUDE
