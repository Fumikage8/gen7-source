//======================================================================
/**
 * @file Light.h
 * @date 2016/09/08 13:35:11
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__LIGHT_H_INCLUDED__
#define __LUNA_SOL_DIVE__LIGHT_H_INCLUDED__
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

//forward declaration
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
//forward declaration
class RenderingPipeline;

class Light
{
  GFL_FORBID_COPY_AND_ASSIGN(Light);
public:
  // constructor
  Light();
  // initialize
  void Initialize(
              System::nijiAllocator* pAllocator,
              RenderingPipeline* pRenderingPipeline);
  // Terminate
  void Terminate();
private:
  // Set light set
  void SetLightSet(
              const u32& setNo,
              System::nijiAllocator* pAllocator);
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvironmentNode;
};
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__LIGHT_H_INCLUDED__
