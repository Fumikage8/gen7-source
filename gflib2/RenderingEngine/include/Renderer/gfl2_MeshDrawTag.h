#ifndef GFLIB2_RENDERINGENGINE_RENDERER_MESHDRAWTAG_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_MESHDRAWTAG_H_INCLUDED


#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/renderer/gfl2_DrawTag.h>
#include <RenderingEngine/include/renderer/gfl2_IShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNode.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

class MeshDrawTag : public gfx::GLMemory, public DrawTag
{
public:

	//static MeshDrawTag* Create();
	static MeshDrawTag* Create(gfx::IGLAllocator* pAllocator);
	//static MeshDrawTag* CreateArray( u32 arrayNum );
	static MeshDrawTag* CreateArray( gfx::IGLAllocator* pAllocator, u32 arrayNum );

	static void CacheClear()
	{
		for( u32 i = 0; i < scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX; ++i )
		s_TextureCache[i] = 0;
	}
	
	virtual ~MeshDrawTag();

	void SetTexture( u32 no, const gfx::Texture* pTexture ){ m_pTextureList[no] = pTexture; }
	const gfx::Texture* GetTexture( u32 no ) const { return m_pTextureList[no]; }

	void SetSampler( u32 no, const gfx::SamplerStateObject* pSampler ){ m_pSamplerList[no] = pSampler; }
	const gfx::SamplerStateObject* GetSampler( u32 no ) const { return m_pSamplerList[no]; }

	void SetMaterialInstanceNode( const scenegraph::instance::MaterialInstanceNode* pMaterial );

	void SetEnabledColorShaderDriver( const IShaderDriver* pShader ){ GFL_ASSERT(pShader);m_pShaderDriver = pShader; }
	const IShaderDriver* GetShaderDriver() const { return m_pShaderDriver; }

#if defined(GF_PLATFORM_CTR)

	void SetIndexBuffer( const u8* pBuffer ){ m_pIndexBuffer = pBuffer; }
	const u8* GetIndexBuffer() const { return m_pIndexBuffer; }

	void SetVertexBuffer( u32 streamNo, const u8* pBuffer ){ m_pVertexBuffer[streamNo] = pBuffer; }
	const u8* GetVertexBuffer( u32 streamNo ) const { return m_pVertexBuffer[streamNo]; }

#elif defined(GF_PLATFORM_WIN32)

	void SetIndexBuffer( const gfx::IndexBuffer* pBuffer ){ m_pIndexBuffer = pBuffer; }
	const gfx::IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }

	void SetVertexBuffer( u32 streamNo, const gfx::VertexBuffer* pBuffer ){ m_pVertexBuffer[streamNo] = pBuffer; }
	const gfx::VertexBuffer* GetVertexBuffer( u32 streamNo ) const { return m_pVertexBuffer[streamNo]; }

#endif

	void SetVertexBlendWeight( u32 streamNo, f32 weight ){ m_VertexBlendWeight[streamNo] = weight; }
	f32 GetVertexBlendWeight( u32 streamNo ) const { return m_VertexBlendWeight[streamNo]; }

	void SetJointInstanceNode( const gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pNode, u32 size );//ハードに合わせ変更の可能性あり。

	void SetOcclusionQuery(const gfx::OcclusionQuery *pObj )
  {
    #if defined(GF_PLATFORM_WIN32)
    m_pOcclusionQuery = pObj;
    #endif
  }

	virtual void Draw(RenderState& rRenderState);

	const scenegraph::resource::MaterialResourceNode* GetMaterialResourceNode() const { return m_pMaterialInstanceNode->GetMaterialResourceNode(); }
	//const c8* GetMaterialName() const { return m_pMaterialResourceNode->GetName(); }

	void SetFaceData( const scenegraph::resource::MeshResourceNode::FaceData *pFaceData ){ m_pFaceData = pFaceData; }
	const scenegraph::resource::MeshResourceNode::FaceData* GetFaceData() const { return m_pFaceData; }

	const scenegraph::instance::MaterialInstanceNode* GetMaterialInstanceNode() const { return m_pMaterialInstanceNode; }

protected:
	MeshDrawTag();

  const gfx::Texture																				*m_pTextureList[scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX];
	const gfx::SamplerStateObject															*m_pSamplerList[scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX];
	const IShaderDriver																				*m_pShaderDriver;
	const scenegraph::instance::MaterialInstanceNode					*m_pMaterialInstanceNode;
	const scenegraph::resource::MeshResourceNode::FaceData		*m_pFaceData;


#if defined(GF_PLATFORM_CTR)

	const u8																						*m_pIndexBuffer;
	const u8																						*m_pVertexBuffer[Constant::VERTEX_BUFFER_MAX];

#elif defined(GF_PLATFORM_WIN32)

	const gfx::IndexBuffer															*m_pIndexBuffer;
	const gfx::VertexBuffer															*m_pVertexBuffer[Constant::VERTEX_BUFFER_MAX];

#endif

	f32																									m_VertexBlendWeight[Constant::VERTEX_BUFFER_MAX];

	const scenegraph::instance::JointInstanceNode	*m_JointInstanceNodeList;
	u32																						m_JointNum;

#if defined(GF_PLATFORM_WIN32)
	gfx::SamplerStateObject															*m_DummySamplerStateObject;
	const gfx::OcclusionQuery														*m_pOcclusionQuery;
#endif

	static u32																					s_TextureCache[scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX];

};

}}}

#endif

