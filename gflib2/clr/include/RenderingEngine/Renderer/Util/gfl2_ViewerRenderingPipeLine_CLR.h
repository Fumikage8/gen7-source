#if !defined(GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_VIEWERRENDERINGPIPELINE_CLR_H_INCLUDED)
#define GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_VIEWERRENDERINGPIPELINE_CLR_H_INCLUDED
#pragma once

#include <clr/include/RenderingEngine/Renderer/Util/gfl2_ViewerRenderingPipeLineNative_CLR.h>
#include <clr/include/RenderingEngine/Renderer/Util/gfl2_ViewerEnums_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/gfx/gfl2_Color_CLR.h>
#include <clr/include/gfx/gfl2_Surface_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawEnvNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ModelInstanceNode_CLR.h>
#include <clr/include/poke_3d_lib/gfl2_PokemonModel_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace renderer { namespace util {

public ref class ViewerRenderingPipeLine
{
public:
  ViewerRenderingPipeLine(
    System::String^ colorShaderPlaneFilePath,
    System::String^ colorShaderPlaneVertexShaderPath,
    System::String^ colorShaderPlanePixelShaderPath,
    System::String^ colorShaderPlaneTexturePath,
    System::String^ edgeCheckPlaneFilePath,
    System::String^ edgeCheckPlaneVertexShaderPath,
    System::String^ edgeCheckPlanePixelShaderPath,
    System::String^ edgeCheckPlaneTexturePath,
    System::String^ edgeGfmdlPath,
    array<System::String^>^ edgeShaderPaths,
    array<System::String^>^ edgeTexturePaths,
    System::String^ gfbmdlPathNormal,
    array<System::String^>^ shaderPathsNormal,
    System::String^ texturePathNormal,
    System::String^ gfbmdlPathId,
    array<System::String^>^ shaderPathsId,
    System::String^ texturePathId,
    System::String^ dofPlaneFilePath,
    System::String^ dofPlaneVertexShaderPath,
    System::String^ dofFrontPlanePixelShaderPath,
    System::String^ dofBackPlanePixelShaderPath,
    System::String^ dofPlaneTexturePath,
    u32 screenWidth,
    u32 screenHeight,
    gfl2::clr::gfx::Surface^ surface);

  ~ViewerRenderingPipeLine()
  {
    this->!ViewerRenderingPipeLine();
  }
  !ViewerRenderingPipeLine();

  gfl2::renderingengine::renderer::util::ViewerRenderingPipeLine* GetNative()
  {
    return m_pNative;
  }

  void ChangeBackgroundVisibility(bool enable);
  void ChangeModelVisibility(bool enable);
  void ChangeWireFrameVisibility(bool enable);
  void ChangeJointVisibility(bool enable);
  void ChangeEffectLocatorVisibility(bool enable);
  void ChangeGridVisibility(bool enable);
  void ChangeGaugeVisibility(bool enable);
  void ChangeInterestRangeVisibility(bool enable);
  void ChangeOutLineVisibility(bool enable);
  void SetEnabledDof(bool enable);
  void ChangeFillTexture(s32 index);
  void SetClearColor(clr::gfx::Color^ color);
  void AddEdgeRenderTarget(clr::poke_3d_lib::PokemonModel^ pModel);
  void RemoveEdgeRenderTarget(clr::poke_3d_lib::PokemonModel^ pModel);
  void AddDrawableModel(clr::poke_3d_lib::PokemonModel^ pModel);
  void RemoveDrawableModel(clr::poke_3d_lib::PokemonModel^ pModel);
  void AddBackgroundModel(clr::poke_3d_lib::PokemonModel^ pModel);
  void RemoveBackgroundModel(clr::poke_3d_lib::PokemonModel^ pModel);
  void SetCameraDataToBackgroundPath(
    bool useCameraData,
    gfl2::clr::math::Matrix^ projection,
    gfl2::clr::math::Matrix^ view);
  s32 GetCpuTimeAverage();
  s32 GetGpuTimeAverage();

  void SetModelPosition(gfl2::clr::math::Vector^ pos);
  void SetInnerTubeVisible(bool visible);
  void SetInnerTubeColor(gfl2::clr::math::Vector^ col);
  void SetOuterTubeVisible(bool visible);
  void SetOuterTubeColor(gfl2::clr::math::Vector^ col);
  void SetPokemonSize(gfl2::clr::renderingengine::PokemonSize::Enum size);
  void SetFocusPointVisible(bool visible);
  void SetFocusPosition(gfl2::clr::math::Vector^ pos);
  void SetInterestPositionVisible(bool visible);
  void SetInterestPosition(gfl2::clr::math::Vector^ pos);
  void SetIsModelAABBVisible(bool visible);
  void SetModelAABBColor(gfl2::clr::math::Vector^ col);
  void SetLightPositionVisible(bool visible);
  void SetLightPosition(gfl2::clr::math::Vector^ pos);
  void SetModelAABB(gfl2::clr::math::Vector^ min, gfl2::clr::math::Vector^ max);
  void SetColorShaderTextureColor(clr::poke_3d_lib::PokemonModel^ pokemonModel);
  void SetProjectionMatrix(clr::math::Matrix^ mat);
  void SetViewMatrix(clr::math::Matrix^ mat);
  void SetRenderTarget(clr::gfx::Surface^ pSurface);
  void SetDepthStencil(clr::gfx::Surface^ pSurface);
  void AddColorTexture(clr::poke_3d_lib::PokemonModel^ pokemonModel);
  void RemoveColorTexture(clr::poke_3d_lib::PokemonModel^ pokemonModel);
  void ChangeFillScreenType(gfl2::clr::renderingengine::FillScreenConstantType::Enum fillScreenType);
  void ChangeDofType(gfl2::clr::renderingengine::DofType::Enum dofType);
  void Update();
  void Execute();

private:
  gfl2::renderingengine::renderer::util::ViewerRenderingPipeLine* m_pNative;
};

}}}}}

#endif