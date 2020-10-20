
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyLight.cpp
 * @date   2016/09/14 Wed. 11:41:04
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include  <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include  "./EasyLight.h"

namespace
{
}



namespace Test      {
namespace MutoTest {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyLight::EasyLight(void)
  : m_pDrawEnvNode(NULL)
  , m_pUtilHeap(NULL)
  , m_RenderPlace(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY)
{
  for(u32 lightSetNo = 0; lightSetNo < LIGHTSET_MAX; ++lightSetNo)
  {
    for(u32 index = 0; index < LIGHT_NUM; ++index)
    {
      m_pLights[lightSetNo][index]  = NULL;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyLight::~EasyLight()
{
  Release();
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyLight::Create(app::util::Heap* pUtilHeap, const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace)
{
  gfl2::util::GLHeapAllocator*  pGLAllocator  = pUtilHeap->GetDevGLHeapAllocator();

  Release();
  m_RenderPlace = renderPlace;

  /*  env  */
  {
    const gfl2::math::AABB  envSpace(
                              gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f),
                              gfl2::math::Vector4( 500000.0f,  500000.0f,  500000.0f)
                            );

    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(pGLAllocator);
    m_pDrawEnvNode->SetAABB(envSpace);
  }

  /*  lightset  */
  for(u32 lightSetNo = 0; lightSetNo < LIGHTSET_MAX; ++lightSetNo)
  {
    using namespace gfl2::renderingengine::scenegraph::instance;

    LightSetNode*           pLightSet = InstanceCreator::CreateLightSetNode(pGLAllocator);


    pLightSet->SetSetNo(lightSetNo);

    /*  LIGHT_Directional_0  */
    {
      LightNode*                  pLight  = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pGLAllocator);
      const char*                 pName   = "DirectionalLight";
      const gfl2::util::FnvHash   nameHash(reinterpret_cast<const u8*>(pName));
      const gfl2::math::Vector3   pos(-10000000.0f, 10000000.0f, 10000000.0f); 
      const gfl2::math::Vector3   col( 0.7f, 0.7f, 0.7f );

      pLight->SetNodeName( pName );
      pLight->SetNodeNameHash( nameHash.value );
      pLight->SetLightType( gfl2::renderingengine::LightType::Directional );
      pLight->SetPosition( pos );
      pLight->SetDirection(-pos.Normalize());
      pLight->SetColor(col);

      pLightSet->AddLight(pLight);
      m_pLights[lightSetNo][LIGHT_Directional_0]  = pLight;
    }

    /*  LIGHT_Ambient  */
    {
      LightNode*                  pLight  = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pGLAllocator);
      const char*                 pName   = "AmbientLight";
      const gfl2::util::FnvHash   nameHash(reinterpret_cast<const u8*>(pName));

      pLight->SetNodeName( pName );
      pLight->SetNodeNameHash( nameHash.value );
      pLight->SetLightType( gfl2::renderingengine::LightType::Ambient );
      pLight->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );

      pLightSet->AddLight(pLight);
      m_pLights[lightSetNo][LIGHT_Ambient]  = pLight;
    }


    m_pDrawEnvNode->AddLightSet(pLightSet);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyLight::Release(void)
{
  GFL_SAFE_DELETE(m_pDrawEnvNode);

  for(u32 lightSetNo = 0; lightSetNo < LIGHTSET_MAX; ++lightSetNo)
  {
    for(u32 index = 0; index < LIGHT_NUM; ++index)
    {
      m_pLights[lightSetNo][index]  = NULL;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyLight::Reset(void)
{
}









} /*  namespace MutoTest */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
