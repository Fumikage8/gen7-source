//======================================================================
/**
 * @file Light.cpp
 * @date 2016/09/08 18:43:00
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightSetNode.h>
#include "./Assert.h"
#include "./Light.h"
#include "./RenderingPipeline.h"

namespace LunaSolDive{
// constructor
Light::Light()
    : m_pDrawEnvironmentNode(NULL){}
// Initialize
void Light::Initialize(
            System::nijiAllocator* pAllocator,
            RenderingPipeline* pRenderingPipeline)
{
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(pRenderingPipeline != NULL);
  using namespace gfl2::renderingengine::scenegraph;
  m_pDrawEnvironmentNode = instance::InstanceCreator::CreateDrawEnvNode(pAllocator);
  SetLightSet(0, pAllocator);
  SetLightSet(2, pAllocator);
  // set AABB
  m_pDrawEnvironmentNode->SetAABB(
              gfl2::math::AABB(
                      gfl2::math::Vector4(-5.0e+5f, -5.0e+5f, -5.0e+5f),
                      gfl2::math::Vector4(5.0e+5f, 5.0e+5f, 5.0e+5f)));
  // register RenderingPipeline
  SceneGraphManager::AddChild(m_pDrawEnvironmentNode);
  pRenderingPipeline->AddDrawEnv(m_pDrawEnvironmentNode);
}
// Terminate
void Light::Terminate(){
  GFL_SAFE_DELETE(m_pDrawEnvironmentNode);
}
// Set light set
void Light::SetLightSet(
            const u32& setNo,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  // Light Set Node
  instance::LightSetNode* const pLightSetNode =
          instance::InstanceCreator::CreateLightSetNode(pAllocator);
  // Directional Light
  const gfl2::math::Vector3 lightPosition(0.0f, 1.0e+7f, 1.0e+7f);
  instance::LightNode* const pDirectionalLightNode =
          instance::InstanceCreator::CreateLightNode(pAllocator);
  pDirectionalLightNode->SetNodeName("DirectionalLight");
  pDirectionalLightNode->SetNodeNameHash(
              gfl2::util::FnvHash(reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName())).value);
  pDirectionalLightNode->SetLightType(
              gfl2::renderingengine::LightType::Directional);
  pDirectionalLightNode->SetPosition(lightPosition);
  pDirectionalLightNode->SetDirection(- lightPosition.Normalize());
  pDirectionalLightNode->SetColor(
              gfl2::math::Vector3(0.7f, 0.7f, 0.7f));
  // Ambient Light
  instance::LightNode* const pAmbientLightNode =
          instance::InstanceCreator::CreateLightNode(pAllocator);
  pAmbientLightNode->SetNodeName("AmbientLight");
  pAmbientLightNode->SetNodeNameHash(
              gfl2::util::FnvHash(reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName())).value);
  pAmbientLightNode->SetLightType(
              gfl2::renderingengine::LightType::Ambient);
  pAmbientLightNode->SetColor(
              gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
  // add Light
  pLightSetNode->SetSetNo(setNo);
  pLightSetNode->AddLight(pDirectionalLightNode);
  pLightSetNode->AddLight(pAmbientLightNode);
  m_pDrawEnvironmentNode->AddLightSet(pLightSetNode);
}
}// namespace LunaSolDive
