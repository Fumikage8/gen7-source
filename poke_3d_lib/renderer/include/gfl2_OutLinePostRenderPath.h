#if !defined(POKE_3D_LIB_MODEL_OUTLINEPOSTRENDERPATH_H_INCLUDED)
#define POKE_3D_LIB_MODEL_OUTLINEPOSTRENDERPATH_H_INCLUDED
#pragma once

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <renderer/include/gfl2_EdgeMapSceneRenderPath.h>

namespace poke_3d { namespace renderer {

class OutLinePostRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  OutLinePostRenderPath();
  virtual ~OutLinePostRenderPath();

  void Initialize( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinaryNormal, c8** shaderBinariesNormal, s32 shaderNumNormal, c8* textureBinaryNormal, c8* gfbmdlBinaryId, c8** shaderBinariesId, s32 shaderNumId, c8* textureBinaryId );
  
  void SetEdgeMapTexture( u32 type, const gfl2::gfx::Texture* pTexture);

  void SetEnable( u32 edgeColor, b32 flag );
  b32 IsEnable( u32 edgeColor ) const;

  /**
  * @brief エッジのカラー調整
  * @param r   赤
  * @param g   緑
  * @param b   青
  * @param a   ＩＤカラー
  */
  void SetEdgeColor( u8 r, u8 g, u8 b );

private:
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

protected:
	const gfl2::gfx::Texture						*m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Max];    // テクスチャ

	gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Max];
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Max];
  b32                                                                              m_EdgeColorEnableTable[ EdgeMapSceneRenderPath::EdgeColor::Max ];
};

}}

#endif