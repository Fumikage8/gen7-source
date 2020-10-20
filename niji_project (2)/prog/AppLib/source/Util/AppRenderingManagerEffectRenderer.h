#if !defined(NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGEREFFECTRENDERER_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGEREFFECTRENDERER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    AppRenderingManagerEffectRenderer.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.07
 @brief   KawadaTest
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

// nijiのインクルード
#include <AppLib/include/Util/AppCamera.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 


//==============================================================================
/**
 @class     EffectRenderingPipeLine
 */
//==============================================================================
class EffectRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRenderingPipeLine);

public:
  EffectRenderingPipeLine(void);
  virtual ~EffectRenderingPipeLine();

private:
  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  virtual b32 NextRenderPath();

public:
  //gfl2::Effect::EffectRenderPath* GetEffectRenderPath(void);
  void SetEffectRenderPathConfig(const gfl2::Effect::EffectRenderPath::Config& effectRenderPathConfig);
  void UnsetEffectRenderPathConfig(void);
  bool CanExecute(void) const;


  const app::util::AppCamera* GetAppCamera(void) const;
  void SetAppCamera(const app::util::AppCamera* pCamera);
  void UnsetAppCamera(void);

  const poke_3d::model::BaseCamera* GetCamera(void) const;
  void SetCamera(const poke_3d::model::BaseCamera* pCamera);
  void UnsetCamera(void);

private:
  gfl2::Effect::EffectRenderPath          m_EffectRenderPath;
  u32                                     m_RenderPathCnt;
  bool                                    m_IsEffectRenderPathConfigSet;
  gfl2::Effect::EffectRenderPath::Config  m_EffectRenderPathConfig;
  const app::util::AppCamera*             m_pAppCamera;
  const poke_3d::model::BaseCamera*       m_pCamera;
};


GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGEREFFECTRENDERER_H_INCLUDED


