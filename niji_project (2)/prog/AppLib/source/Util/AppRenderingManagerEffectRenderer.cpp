//==============================================================================
/**
 @file    AppRenderingManagerEffectRenderer.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.07
 @brief   KawadaTest
 */
//==============================================================================


// gfl2のインクルード
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

// nijiのインクルード
#include "AppRenderingManagerEffectRenderer.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 


EffectRenderingPipeLine::EffectRenderingPipeLine(void)
  : gfl2::renderingengine::renderer::RenderingPipeLine(),
    m_EffectRenderPath(),
    m_RenderPathCnt(0),
    m_IsEffectRenderPathConfigSet(false),
    m_EffectRenderPathConfig(),
    m_pAppCamera(NULL),
    m_pCamera(NULL)
{}
EffectRenderingPipeLine::~EffectRenderingPipeLine()
{}


b32 EffectRenderingPipeLine::StartRenderPath()
{
  m_RenderPathCnt = 0;
  return true;
}
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* EffectRenderingPipeLine::GetRenderPath()
{
  switch( m_RenderPathCnt ){
  case 0:
    return &m_EffectRenderPath;
    //returnするからbreak要らない
  }
  return NULL;
}

b32 EffectRenderingPipeLine::NextRenderPath()
{
  m_RenderPathCnt++;
  if ( m_RenderPathCnt == 1 )
    return false;
  return true;
}

//gfl2::Effect::EffectRenderPath* EffectRenderingPipeLine::GetEffectRenderPath(void)
//{
//  return &m_EffectRenderPath;
//}

void EffectRenderingPipeLine::SetEffectRenderPathConfig(const gfl2::Effect::EffectRenderPath::Config& effectRenderPathConfig)
{
  m_EffectRenderPathConfig      = effectRenderPathConfig;
  m_EffectRenderPath.SetConfig(m_EffectRenderPathConfig);
  m_IsEffectRenderPathConfigSet = true;
}
void EffectRenderingPipeLine::UnsetEffectRenderPathConfig(void)
{
  m_EffectRenderPathConfig      = gfl2::Effect::EffectRenderPath::Config();
  m_IsEffectRenderPathConfigSet = false;
}
bool EffectRenderingPipeLine::CanExecute(void) const
{
  return m_IsEffectRenderPathConfigSet;
}

const app::util::AppCamera* EffectRenderingPipeLine::GetAppCamera(void) const
{
  return m_pAppCamera;
}
void EffectRenderingPipeLine::SetAppCamera(const app::util::AppCamera* pCamera)
{
  if(pCamera)
  {
    m_pAppCamera = pCamera;
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void EffectRenderingPipeLine::UnsetAppCamera(void)
{
  m_pAppCamera = NULL;
}

const poke_3d::model::BaseCamera* EffectRenderingPipeLine::GetCamera(void) const
{
  return m_pCamera;
}
void EffectRenderingPipeLine::SetCamera(const poke_3d::model::BaseCamera* pCamera)
{
  if(pCamera)
  {
    m_pCamera = pCamera;
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void EffectRenderingPipeLine::UnsetCamera(void)
{
  m_pCamera = NULL;
}


GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

