#if !defined(SCENE3D_H_INCLUDED)
#define SCENE3D_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   Scene3D.h
 * @date   2016/09/06 Tue. 19:22:39
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
#include  <model/include/gfl2_BaseCamera.h>

#include  <System/include/nijiAllocator.h>
#include  <AppLib/include/Util/app_util_heap.h>
#include  <AppLib/include/Util/AppRenderingManager.h>
#include  <AppLib/source/Tool/GeneralView/Module/ResourceLoader.h>

#include  "./EasyCamera.h"
#include  "./EasyLight.h"
#include  "./EasyModel.h"

namespace Test  {
namespace MutoTest {
// =============================================================================
/**/
//==============================================================================
class Scene3D
{
  GFL_FORBID_COPY_AND_ASSIGN(Scene3D);

public:
  Scene3D(void);
  virtual ~Scene3D();

  void  Initialize(app::util::Heap* pUtilHeap);
  void  Finalize(void);

  void  OnUpdate(void);
  void  OnPreDraw(void);
  void  OnDraw(const gfl2::gfx::CtrDisplayNo displayNo);

  void  Quit(void)      {m_bQuitRequest = true;}
  bool  IsStable(void) const;

  /*
    camera
  */
  void  SetCamera(EasyCamera& rCamera);
  void  UnsetCamera(void);

  /*
    light
  */
  void  SetLight(EasyLight& rLight);
  void  UnsetLight(void);

  /*
    model
  */
  void  AddModel_U(EasyModel& rModel);
  void  RemoveModel_U(EasyModel& rModel);

  void  AddModel_L(EasyModel& rModel);
  void  RemoveModel_L(EasyModel& rModel);

  void  AddModel(   const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo=0, bool enableEdgeRenderPath=true);
  void  RemoveModel(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo=0);



protected:
  app::util::Heap*                                            m_pHeap;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;
//  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode_Light;
  app::util::AppRenderingManager*                             m_pRenderingManager;
  System::nijiAllocator*                                      m_pNijiAllocator;
  /*  camera  */
  EasyCamera                                                  m_DefaultCamera;
  EasyCamera*                                                 m_pCamera;
  /*  light  */
  EasyLight*                                                  m_pLight;

  u32                                                         m_QuitCount;
  bool                                                        m_bQuitRequest;

  /*  resources  */
  enum
  {
    MODEL_Grid,
    
    MODEL_MAX
  };

  app::tool::ResourceLoader                                   m_ResourceLoaders[MODEL_MAX];
  EasyModel                                                   m_EasyModels[MODEL_MAX];
};



} /*  namespace MutoTest */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */

#endif  /*  #if !defined(SCENE3D_H_INCLUDED)  */
