#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

//#define DOF_ENABLE                 // 被写界深度を有効にするマクロ
#define FRAMEBUFFER_COPY_ENABLE    // ストレッチコピーでなく、従来の等倍コピーモード

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ColorShaderTextureRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ClearRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ModelRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_MakeBlurImagePath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_MakeReductionImagePath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_DofRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>

#include <renderer/include/gfl2_EdgeMapSceneRenderPath.h>
#include <renderer/include/gfl2_OutLinePostRenderPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>
#include <gfx/include/gfl2_GLMemory.h>

class MainRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:

	MainRenderPath(gfl2::gfx::IGLAllocator* allocator);
	virtual ~MainRenderPath();

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;

};

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine( gfl2::gfx::IGLAllocator* allocator, f32 screanWidth, f32 screanHeight, const gfl2::gfx::Surface *pFrameBuffer );
	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_MainRenderPath.AddDrawEnv(pNode);
    m_DistortionPostRenderPath.AddDrawEnv(pNode);
  }

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void SetDepthTexture( gfl2::gfx::Texture* pTexture );

	void AddColorShaderTexture(
    const gfl2::gfx::Texture* pOriginalColorTexture,
    const gfl2::gfx::Texture* pColorShaderTexture);

  void SetColorShaderTextureColor(const gfl2::math::Vector4& color);

	void ClearColorShaderPath();

  void SetCameraDataToBackgroundPath(
    b32 useCameraData,
    gfl2::math::Matrix44 projection,
    gfl2::math::Matrix34 view);

  void SetEnabledBackgroundVisible(b32 visible)
  {
    m_IsBackgroundVisible = visible;
  }

  void ChangeDofType()
  {
    m_DofType = (m_DofType == gfl2::renderingengine::DofType::Front) ? gfl2::renderingengine::DofType::Back : gfl2::renderingengine::DofType::Front;
    m_MakeBlurImagePath.ChangeDofType(m_DofType);
    m_DofRenderPath.ChangeDofType(m_DofType);
  }

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;

	static const u32																											s_EdgeMapFileNum = 6;
	poke_3d::renderer::EdgeMapSceneRenderPath															m_EdgeMapSceneRenderPath;
	c8																																		*m_pEdgeMapBinData[s_EdgeMapFileNum];

	MainRenderPath																												m_MainRenderPath;

	static const u32																											s_OutLineFileNum = 8;
	poke_3d::renderer::OutLinePostRenderPath															m_OutLinePostRenderPath;
	c8																																		*m_pOutLineBinData[s_OutLineFileNum];

  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath      m_StretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath                           m_DistortionPostRenderPath;
	gfl2::renderingengine::renderer::util::MakeReductionImagePath         m_CopySamplePath;

  gfl2::renderingengine::renderer::util::ClearRenderPath                m_ClearRenderPath;
  struct BackgroundResourceSet
  {
    c8 m_GfbmdlFilePath[256];                                                               //!< @brief gfbmdlのファイルパス
    c8 m_ShaderFilePaths[16][256];                                                          //!< @brief シェーダファイルパス
    c8 m_TextureFilePaths[48][256];                                                         //!< @brief テクスチャファイルパス
    s32 m_ShaderNum;                                                                        //!< @brief シェーダファイルの総数
    s32 m_TextureNum;                                                                       //!< @brief テクスチャファイルの総数
    c8 *m_pGfbmdlBinData;                                                                   //!< @brief 背景のバイナリデータ
    c8 *m_pShaderBinData[16];                                                               //!< @brief シェーダバイナリデータ
    c8 *m_pTextureBinData[48];                                                              //!< @brief テクスチャバイナリデータ
    gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResourceNode;        //!< @brief モデルリソースノード
    gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pShaderResourceNodes[16];  //!< @brief シェーダリソースノード
    gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pTextureResourceNodes[48]; //!< @brief テクスチャリソースノード
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;   //!< @brief モデルインスタンスノード
    gfl2::renderingengine::scenegraph::instance::LightNode* m_pLightNode;                   //!< @brief ライトノード
    gfl2::renderingengine::scenegraph::instance::LightSetNode* m_pLightSetNode;             //!< @brief ライトセットノード
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;               //!< @brief DrawEnvノード
  };
  static const u32                                                      s_BackgroundFileNum = 4;
  gfl2::renderingengine::renderer::util::ModelRenderPath                m_BackgroundRenderPath[s_BackgroundFileNum];
  BackgroundResourceSet                                                 m_BackgroundResourceSet[s_BackgroundFileNum];

  // 被写界深度用の描画パス
  gfl2::renderingengine::renderer::util::MakeBlurImagePath              m_MakeBlurImagePath;
  gfl2::renderingengine::renderer::util::MakeReductionImagePath         m_MakeReductionImagePath;
  gfl2::renderingengine::renderer::util::DofRenderPath                  m_DofRenderPath;

  // 被写界深度で使用する描画リソース
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_DofRenderTargetPlaneResourceNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_DofRenderTargetPlaneInstanceNode;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData*   m_pDofModelData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData*   m_pDofVertexShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData*   m_pDofFrontPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData*   m_pDofBackPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData*   m_pDofTextureData;

  // 被写界深度の表面/裏面設定
  gfl2::renderingengine::DofType m_DofType;

  gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath*>         m_ColorShaderTextureRenderPaths;

	gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_ColorShaderRenderTargetPlaneResourceNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_ColorShaderRenderTargetPlaneInstanceNode;
	c8*																																							m_pColorShaderData;

  b32 m_IsBackgroundVisible;
  
};

#endif
