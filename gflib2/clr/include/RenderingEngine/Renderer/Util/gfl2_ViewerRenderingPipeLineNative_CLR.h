#if !defined(GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_VIEWERRENDERINGPIPELINENATIVE_CLR_H_INCLUDED)
#define GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_VIEWERRENDERINGPIPELINENATIVE_CLR_H_INCLUDED
#pragma once

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ColorShaderTextureRenderPath.h>
#include <renderingengine/include/renderer/Util/gfl2_UtilEffectLocatorRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_FillScreenRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <RenderingEngine/include/renderer/gfl2_IShaderDriver.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilMcnfRenderPath.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ClearRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ModelRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_MakeBlurImagePath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_MakeReductionImagePath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_DofRenderPath.h>
#include <clr/include/RenderingEngine/Renderer/Util/gfl2_ViewerEnumsNative_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_PokemonModel.h>
#include <renderer/include/gfl2_EdgeMapSceneRenderPath.h>
#include <renderer/include/gfl2_OutLinePostRenderPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {
//-----------------------------------------------------------
//! @brief ワイヤーフレーム描画用シェーダドライバ
//-----------------------------------------------------------
class WireFrameShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
{
public:
  WireFrameShaderDriver();
  virtual ~WireFrameShaderDriver();

	virtual b32 Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;

	gfl2::gfx::Shader*						m_pPixelShader;
private:
  
  gfl2::gfx::DepthStencilStateObject*	m_DepthStencilStateObject;
  gfl2::gfx::RasterizerStateObject*		m_RasterizerStateObject;
};

//-----------------------------------------------------------
//! @brief ワイヤーフレーム描画パスクラス
//-----------------------------------------------------------
class WireFrameRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
  WireFrameRenderPath();
  virtual ~WireFrameRenderPath();

  virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride();
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
private:
  WireFrameShaderDriver m_WireFrameShaderDriver;
};

//-----------------------------------------------------------
//! @brief ジョイント描画パスクラス
//-----------------------------------------------------------
class ViewerJointRenderPath : public RenderingPipeLine::IRenderPath
{
public:

  struct InitDescription
  {
    u32 maxJointCount;                  // 描画可能な最大ジョイント数
    gfl2::math::Vector4 jointColor;      // ジョイントカラー
    f32 xyzScale;                       // XYZ軸のスケール
    f32 boneScale;                      // ボーンのスケール

    InitDescription() :
      maxJointCount(1024),
      jointColor(0.6f, 0.8f, 0.6f),
      xyzScale(3.0f),
      boneScale(0.8f)
    {
    }
  };

  ViewerJointRenderPath();
  virtual ~ViewerJointRenderPath();

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  virtual const IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfl2::util::FixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );

  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

protected:

  InitDescription m_InitDescription;                                              // 初期化設定
  gfl2::gfx::DrawUtil::VertexFormat* m_pVertexFormatList;                         // 頂点フォーマットリスト
  u32 m_VertexFormatLimit;                                                        // 頂点フォーマットリスト確保数
};

//-----------------------------------------------------------
//! @brief グリッド描画パスクラス
//-----------------------------------------------------------
class GridRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

  struct InitDescription
  {
    u32 maxGridCount;                  // 描画可能な最大ジョイント数
    gfl2::math::Vector4 jointColor;     // グリッド線のカラー

    InitDescription() :
      maxGridCount(1024),
      jointColor(0.6f, 0.8f, 0.6f)
    {
    }
  };

  GridRenderPath();
  virtual ~GridRenderPath();

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

protected:

  InitDescription m_InitDescription;                                              // 初期化設定
  std::auto_ptr<gfl2::gfx::DepthStencilStateObject> m_DepthStencilStateObject;    // デプスステンシルオブジェクト
  gfl2::gfx::DrawUtil::VertexFormat* m_pVertexFormatList;                         // 頂点フォーマットリスト
  u32 m_VertexFormatLimit;                                                        // 頂点フォーマットリスト確保数
};

//-----------------------------------------------------------
//! @brief ゲージ描画パスクラス
//-----------------------------------------------------------
class GaugeRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  static const s32 FRAME_COUNT_RESOLUTION = 10;

  struct InitDescription
  {
    u32 maxGaugeLength;                 // 描画可能な最大ジョイント数
    gfl2::math::Vector4 cpuGaugeColor;   // CPUゲージのカラー
    gfl2::math::Vector4 gpuGaugeColor;   // GPUゲージのカラー
    s32 cpuExecutionTime[FRAME_COUNT_RESOLUTION];
    s32 gpuExecutionTime[FRAME_COUNT_RESOLUTION];
    s32 frameCount;
    s32 cpuTimeAverage;
    s32 gpuTimeAverage;

    InitDescription() :
      maxGaugeLength(800),
      cpuGaugeColor(0.0f, 1.0f, 0.0f),
      gpuGaugeColor(1.0f, 1.0f, 0.0f),
      frameCount(0),
      cpuTimeAverage(0),
      gpuTimeAverage(0)
    {
      for (s32 i = 0; i < FRAME_COUNT_RESOLUTION; ++i)
      {
        cpuExecutionTime[i] = 0;
        gpuExecutionTime[i] = 0;
      }
    }
  };

  GaugeRenderPath();
  virtual ~GaugeRenderPath();

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  void SetCpuFrame(s32 frame);
  void SetGpuFrame(s32 frame);
  void ChangeVisibility(b32 enable);
  s32 GetCpuTimeAverage();
  s32 GetGpuTimeAverage();

protected:
  InitDescription m_InitDescription;                  // 初期化設定
  b32 m_IsGaugeVisible;
};

//-----------------------------------------------------------
//! @brief 判定用図形描画パスクラス
//-----------------------------------------------------------
class DebugFigureRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

  struct InitDescription
  {
    u32 maxDebugFigureCount;                  // 描画可能な最大ジョイント数
    gfl2::math::Vector4 jointColor;     // グリッド線のカラー

    InitDescription() :
      maxDebugFigureCount(1024),
      jointColor(0.6f, 0.8f, 0.6f)
    {
    }
  };

  DebugFigureRenderPath();
  virtual ~DebugFigureRenderPath();

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  void SetModelPosition(gfl2::math::Vector4 pos) { m_ModelPosition = pos; }
  void SetInnerTubeVisible(b32 visible) { m_IsInnerTubeVisible = visible; }
  void SetInnerTubeColor(gfl2::math::Vector4 col) { m_InnerTubeColor = col; }
  void SetOuterTubeVisible(b32 visible) { m_IsOuterTubeVisible = visible; }
  void SetOuterTubeColor(gfl2::math::Vector4 col) { m_OuterTubeColor = col; }
  void SetPokemonSize(gfl2::renderingengine::PokemonSize size) { m_PokemonSize = size; }
  void SetFocusPointVisible(b32 visible) { m_IsFocusPointVisible = visible; }
  void SetFocusPosition(gfl2::math::Vector4 pos) { m_FocusPosition = pos; }
  void SetInterestPositionVisible(b32 visible) { m_IsInterestPositionVisible = visible; }
  void SetInterestPosition(gfl2::math::Vector4 pos) { m_InterestPosition = pos; }
  void SetModelAABBVisible(b32 visible) { m_IsModelAABBVisible = visible; }
  void SetModelAABBColor(gfl2::math::Vector4 col) { m_ModelAABBColor = col; }
  void SetModelAABB(gfl2::math::AABB aabb) { m_ModelAABB = aabb; }
  void SetLightPositionVisible(b32 visible) { m_LightPositionVisible = visible; }
  void SetLightPosition(gfl2::math::Vector4 pos) { m_LightPosition = pos; }

protected:

  InitDescription m_InitDescription;                                              // 初期化設定
  std::auto_ptr<gfl2::gfx::DepthStencilStateObject> m_DepthStencilStateObject;    // デプスステンシルオブジェクト

  gfl2::math::Vector4 m_ModelPosition;
  b32 m_IsInnerTubeVisible;
  gfl2::math::Vector4 m_InnerTubeColor;
  b32 m_IsOuterTubeVisible;
  gfl2::math::Vector4 m_OuterTubeColor;
  gfl2::renderingengine::PokemonSize m_PokemonSize;
  f32 m_InnerTubeRadiuses[PokemonSize::NumberOf];
  f32 m_OuterTubeRadiuses[PokemonSize::NumberOf];

  b32 m_IsFocusPointVisible;
  gfl2::math::Vector4 m_FocusPosition;

  b32 m_IsInterestPositionVisible;
  gfl2::math::Vector4 m_InterestPosition;

  b32 m_IsModelAABBVisible;
  gfl2::math::Vector4 m_ModelAABBColor;
  gfl2::math::AABB m_ModelAABB;

  b32 m_LightPositionVisible;
  gfl2::math::Vector4 m_LightPosition;
};

class ViewerRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
  //!< @brief ビューア上で描画できる最大モデル数
  // ビューア上から最大数を確認する可能性が高いのでpublic
  static const s32 DRAWABLE_MODEL_MAX = 100;
  //!< @brief 背景データの読込可能最大数
  static const s32 DRAWABLE_BACKGROUND_MAX = 4;
  //!< @brief 被写界深度を調整するための描画パスの総数
  static const s32 MAKE_BLUR_IMAGE_PATH_NUM = 3;
  ViewerRenderingPipeLine(
      const c8* colorShaderPlaneFilePath,
      const c8* colorShaderPlaneVertexShaderPath,
      const c8* colorShaderPlanePixelShaderPath,
      const c8* colorShaderPlaneTexturePath,
      const c8* edgeCheckPlaneFilePath,
      const c8* edgeCheckPlaneVertexShaderPath,
      const c8* edgeCheckPlanePixelShaderPath,
      const c8* edgeCheckPlaneTexturePath,
      const c8*   edgeGfbmdlPath,
      const c8**  edgeShaderPaths,
      s32   edgeShaderNum,
      const c8**  edgeTexturePaths,
      s32   edgeTexNum,
      const c8* gfbmdlPathNormal,
      const c8** shaderPathsNormal,
      s32 shaderNumNormal,
      const c8* texturePathNormal,
      const c8* gfbmdlPathId,
      const c8** shaderPathsId,
      s32 shaderNumId,
      const c8* texturePathId,
      const c8* dofPlaneFilePath,
      const c8* dofPlaneVertexShaderPath,
      const c8* dofFrontPlanePixelShaderPath,
      const c8* dofBackPlanePixelShaderPath,
      const c8* dofPlaneTexturePath,
      u32 screenWidth,
      u32 screenHeight,
      gfl2::gfx::Surface* pSurface);
  virtual ~ViewerRenderingPipeLine();
  void ChangeBackgroundVisibility(b32 enable);
  void ChangeModelVisibility(b32 enable);
  void ChangeWireFrameVisibility(b32 enable);
  void ChangeJointVisibility(b32 enable);
  void ChangeEffectLocatorVisibility(b32 enable);
  void ChangeGridVisibility(b32 enable);
  void ChangeGaugeVisibility(b32 enable);
  void ChangeInterestRangeVisibility(b32 enable);
  void ChangeOutLineVisibility(b32 enable);
  void SetEnabledDof(b32 enable);
  void SetClearColor(const gfl2::gfx::Color& color);
  void AddEdgeRenderingTarget(poke_3d::model::BaseModel* pModel);
  void RemoveEdgeRenderingTarget(poke_3d::model::BaseModel* pModel);
  void SetCpuFrame(s32 frame);
  void SetGpuFrame(s32 frame);
  s32 GetCpuTimeAverage();
  s32 GetGpuTimeAverage();

  void SetModelPosition(gfl2::math::Vector4 pos)
  {
    m_DebugFigureRenderPath.SetModelPosition(pos);
  }
  void SetInnerTubeVisible(b32 visible)
  {
    m_DebugFigureRenderPath.SetInnerTubeVisible(visible); 
  }
  void SetInnerTubeColor(gfl2::math::Vector4 col)
  {
    m_DebugFigureRenderPath.SetInnerTubeColor(col);
  }
  void SetOuterTubeVisible(b32 visible)
  {
    m_DebugFigureRenderPath.SetOuterTubeVisible(visible);
  }
  void SetOuterTubeColor(gfl2::math::Vector4 col)
  {
    m_DebugFigureRenderPath.SetOuterTubeColor(col);
  }
  void SetPokemonSize(gfl2::renderingengine::PokemonSize size)
  {
    m_DebugFigureRenderPath.SetPokemonSize(size);
  }
  void SetFocusPointVisible(b32 visible)
  {
    m_DebugFigureRenderPath.SetFocusPointVisible(visible);
  }
  void SetFocusPosition(gfl2::math::Vector4 pos)
  {
    m_DebugFigureRenderPath.SetFocusPosition(pos);
  }
  void SetInterestPositionVisible(b32 visible)
  {
    m_DebugFigureRenderPath.SetInterestPositionVisible(visible);
  }
  void SetInterestPosition(gfl2::math::Vector4 pos)
  {
    m_DebugFigureRenderPath.SetInterestPosition(pos);
  }
  void SetIsModelAABBVisible(b32 visible)
  {
    m_DebugFigureRenderPath.SetModelAABBVisible(visible);
  }
  void SetModelAABBColor(gfl2::math::Vector4 col)
  {
    m_DebugFigureRenderPath.SetModelAABBColor(col);
  }
  void SetModelAABB(gfl2::math::Vector4 min, gfl2::math::Vector4 max)
  {
    m_DebugFigureRenderPath.SetModelAABB(gfl2::math::AABB(min, max));
  }
  void SetLightPositionVisible(b32 visible)
  {
    m_DebugFigureRenderPath.SetLightPositionVisible(visible);
  }
  void SetLightPosition(gfl2::math::Vector4 pos)
  {
    m_DebugFigureRenderPath.SetLightPosition(pos);
  }

  u32 GetColorShaderRenderPathNum()
  {
    return m_ColorShaderTextureRenderPaths.size();
  }

  void SetColorShaderRenderTargetPlane(scenegraph::instance::ModelInstanceNode* pNode);
  void AddColorShaderTexture(
    const gfl2::gfx::Texture* pOriginalColorTexture,
    const gfl2::gfx::Texture* pColorShaderTexture);
  void RemoveColorShaderTexture(u32 index);
  void SetColorShaderTextureColor(
    s32 startPassIndex,
    u32 passNum,
    const math::Vector4& color);

  s32 FindModelIndexOfModelRenderPath(
    poke_3d::model::PokemonModel* pModel,
    gfl2::renderingengine::renderer::util::ModelRenderPath* renderPathArray,
    s32 renderPathArrayLength);

  b32 AddDrawableModel(poke_3d::model::PokemonModel* pModel);
  b32 RemoveDrawableModel(poke_3d::model::PokemonModel* pModel);

  b32 AddBackgroundModel(poke_3d::model::PokemonModel* pModel);
  b32 RemoveBackgroundModel(poke_3d::model::PokemonModel* pModel);
  void SetCameraDataToBackgroundPath(
    b32 useCameraData,
    gfl2::math::Matrix44 projection,
    gfl2::math::Matrix34 view);

  void ChangeFillTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Enum edgeColor);
  void ChangeFillScreenType(gfl2::renderingengine::FillScreenConstantType::Enum fillScreenType);
  void ChangeDofType(gfl2::renderingengine::DofType::Enum dofType);

private:
  s32 AddDrawableModelToRenderPath(
    poke_3d::model::PokemonModel* pModel,
    gfl2::renderingengine::renderer::util::ModelRenderPath* renderPathArray,
    s32 renderPathArrayLength);
  s32 RemoveDrawableModelToRenderPath(
    poke_3d::model::PokemonModel* pModel,
    gfl2::renderingengine::renderer::util::ModelRenderPath* renderPathArray,
    s32 renderPathArrayLength);

  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  virtual b32 NextRenderPath();

  //!< @brief 各パスを表した列挙型
  enum PathType
  {
    Clear,
    Background,
    Edge,
    Model,
    OutLine,
    StretchBltFrameBuffer,
    Distortion,
    WireFrame,
    Grid,
    MakeBlurImage,
    MakeReductionImage,
    Dof,
    Joint,
    EffectLocator,
    Gauge,
    DebugFigure,
    Mcnf,
    FillScreen,
    Present,
    NumberOf
  };

  b32                                                                   m_IsBackgroundVisible;
  b32                                                                   m_IsModelVisible;
  b32                                                                   m_IsWireFrameVisible;
  b32                                                                   m_IsJointVisible;
  b32                                                                   m_IsGridVisible;
  b32                                                                   m_IsEffectLocatorVisible;
  b32                                                                   m_IsInterestRangeVisible;
  b32                                                                   m_IsOutLineVisible;
  b32                                                                   m_IsDofEnable;
  u32                                                                   m_RenderPathCnt;
  
  gfl2::gfx::GLFixedSizeContainer<ColorShaderTextureRenderPath*>        m_ColorShaderTextureRenderPaths;

  //!< @brief 各パスの開始インデックス
  s32 m_PathStartIndex[PathType::NumberOf + 1];

  scenegraph::resource::ResourceNode*                           m_ColorShaderRenderTargetPlaneResourceNode;
  scenegraph::instance::ModelInstanceNode*                      m_ColorShaderRenderTargetPlaneInstanceNode;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData* m_pColorShaderModelData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pColorShaderVertexShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pColorShaderPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData* m_pColorShaderTextureData;

  scenegraph::resource::ResourceNode*                           m_EdgeCheckRenderTargetPlaneResourceNode;
  scenegraph::instance::ModelInstanceNode*                      m_EdgeCheckRenderTargetPlaneInstanceNode;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData* m_pEdgeCheckModelData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pEdgeCheckVertexShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pEdgeCheckPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData* m_pEdgeCheckTextureData;

  scenegraph::resource::ResourceNode*                           m_DofRenderTargetPlaneResourceNode;
  scenegraph::instance::ModelInstanceNode*                      m_DofRenderTargetPlaneInstanceNode;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData* m_pDofModelData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pDofVertexShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pDofFrontPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData* m_pDofBackPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData* m_pDofTextureData;

  //!< @brief カラーシェーダパス以外の全てのパスを格納した配列(最後の+1はNULL用)
  IRenderPath*                                                          m_AllPaths[DRAWABLE_BACKGROUND_MAX + (DRAWABLE_MODEL_MAX * 2) + MAKE_BLUR_IMAGE_PATH_NUM + 14 + 1];
  gfl2::renderingengine::renderer::util::ClearRenderPath                m_ClearRenderPath;
  gfl2::renderingengine::renderer::util::ModelRenderPath                m_BackgroundRenderPath[DRAWABLE_BACKGROUND_MAX];
  poke_3d::renderer::EdgeMapSceneRenderPath                             m_EdgeMapSceneRenderPath;
  gfl2::renderingengine::renderer::util::ModelRenderPath                m_ModelRenderPath[DRAWABLE_MODEL_MAX];
  poke_3d::renderer::OutLinePostRenderPath                              m_OutLinePostRenderPath;
  gfl2::renderingengine::renderer::util::MakeBlurImagePath              m_MakeBlurImagePath;
  gfl2::renderingengine::renderer::util::MakeReductionImagePath         m_MakeReductionImagePath;
  gfl2::renderingengine::renderer::util::DofRenderPath                  m_DofRenderPath;
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath      m_StretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath                           m_DistortionPostRenderPath[DRAWABLE_MODEL_MAX];
  WireFrameRenderPath                                                   m_WireFrameRenderPath;
  GridRenderPath                                                        m_GridRenderPath;
  gfl2::renderingengine::renderer::util::ViewerJointRenderPath          m_JointRenderPath;
  gfl2::renderingengine::renderer::util::EffectLocatorRenderPath        m_EffectLocatorRenderPath;
  gfl2::renderingengine::renderer::util::GaugeRenderPath                m_GaugeRenderPath;
  gfl2::renderingengine::renderer::util::DebugFigureRenderPath          m_DebugFigureRenderPath;
  gfl2::renderingengine::renderer::util::McnfRenderPath                 m_McnfRenderPath;
  gfl2::renderingengine::renderer::util::FillScreenRenderPath           m_FillScreenRenderPath;
  gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath       m_PresentPath;
  u64 m_CpuStartFrame;
  u64 m_CpuEndFrame;
  u64 m_GpuEndFrame;
};
}}}}

#endif