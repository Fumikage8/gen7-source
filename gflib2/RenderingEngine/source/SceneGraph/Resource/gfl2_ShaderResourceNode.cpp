#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

ShaderResourceNode::ShaderResourceNode() : NodeTrait<ShaderResourceNode, ResourceNode>()
{
	for( int i = 0; i < ShaderKind::Max; ++i )
	{
		m_pShader[i] = NULL;
	}
}

ShaderResourceNode::~ShaderResourceNode()
{

	for( int i = 0; i < ShaderKind::Max; ++i )
	{
		if ( m_pShader[i] )
		{
		#if defined(GF_PLATFORM_CTR)
			m_pShader[i] = NULL;
		#elif defined(GF_PLATFORM_WIN32)
			delete m_pShader[i];
			m_pShader[i] = NULL;
		#endif
			
		}
	}

}


void ShaderResourceNode::SetData( const void* data, int size )
{
#if defined(GF_PLATFORM_CTR)
	struct ShaderHeader
	{//16バイトアラインサイズ
		s32						size;
		u32						shaderType;
		u32						dataHash;
		s32						dumy;
		c8						path[64];
	};

	struct CommandHeader
	{//16バイトアラインサイズ
		u32						size;
		u32						dumy[3];
	};

	const u8*								pSeek = reinterpret_cast<const u8*>( data );
	const ShaderHeader*			pShaderHeader = reinterpret_cast<const ShaderHeader*>( data );

	pSeek += sizeof( ShaderHeader );

	{
		switch( pShaderHeader->shaderType ){
		case ShaderKind::Vertex:
			{
				const CommandHeader				*pCommandHeader = reinterpret_cast<const CommandHeader*>( pSeek );
				pSeek += sizeof( CommandHeader );

				m_DataHash[ShaderKind::Vertex] = pShaderHeader->dataHash;
				m_pShader[ShaderKind::Vertex] = pSeek;
				m_ShaderSize[ShaderKind::Vertex] = pCommandHeader->size;
			}
			break;
		case ShaderKind::Fragment:
			{
				m_DataHash[ShaderKind::Fragment] = pShaderHeader->dataHash;
				m_pShader[ShaderKind::Fragment] = pSeek;
				m_ShaderSize[ShaderKind::Fragment] = pShaderHeader->size;
			}
			break;
		}
	}

#elif defined(GF_PLATFORM_WIN32)
	GFL_ASSERT(data);
	struct ShaderHeader
	{//16バイトアラインサイズ
		s32						size;
		u32						shaderType;
		s32						dumy[2];
    c8						path[Constant::RESOURCE_FILE_NAME_MAX];
	};

	const u8*								pSeek = reinterpret_cast<const u8*>( data );
	const u32*							pShaderNum = reinterpret_cast<const u32*>( data );
	u32											shaderNum = *pShaderNum;

	pSeek += sizeof(u32);

	VertexElement element[] = {
		/*{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },

		{ 0, StreamUsage::Color,		0, VertexType::Float, 4 },
		{ 0, StreamUsage::Normal,		0, VertexType::Float, 4 },
		{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
		{ 0, StreamUsage::TexCoord,	1, VertexType::Float, 4 },
		{ 0, StreamUsage::TexCoord,	2, VertexType::Float, 4 },

		{ 0, StreamUsage::Tangent,	0, VertexType::Float, 4 },
		{ 0, StreamUsage::Tangent,	1, VertexType::Float, 4 },
		{ 0, StreamUsage::Tangent,	2, VertexType::Float, 4 },
		{ 0, StreamUsage::BlendIndices,		0, VertexType::Float, 4 },
		{ 0, StreamUsage::BlendWeight,		0, VertexType::Float, 4 },

		{ 0, StreamUsage::Normal,		1, VertexType::Float, 4 },
		{ 0, StreamUsage::Tangent,	3, VertexType::Float, 4 },

		{ 1, StreamUsage::Position,	1, VertexType::Float, 4 },
		{ 2, StreamUsage::Position,	2, VertexType::Float, 4 },
		{ 3, StreamUsage::Position,	3, VertexType::Float, 4 },*/

		{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },
		{ 0, StreamUsage::Normal,		0, VertexType::Float, 4 },
		//{ 0, StreamUsage::BiNormal,	0, VertexType::Float, 4 },
		{ 0, StreamUsage::Tangent,	0, VertexType::Float, 4 },
		{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
		{ 0, StreamUsage::TexCoord,	1, VertexType::Float, 4 },
		{ 0, StreamUsage::TexCoord,	2, VertexType::Float, 4 },
		{ 0, StreamUsage::Color,		0, VertexType::Float, 4 },
		{ 0, StreamUsage::BlendIndices,		0, VertexType::Float, 4 },
		{ 0, StreamUsage::BlendWeight,		0, VertexType::Float, 4 },
		{ 1, StreamUsage::Position,	1, VertexType::Float, 4 },
		{ 2, StreamUsage::Position,	2, VertexType::Float, 4 },
		{ 3, StreamUsage::Position,	3, VertexType::Float, 4 },
		{ 4, StreamUsage::Tangent,	1, VertexType::Float, 4 },
		{ 4, StreamUsage::Tangent,	2, VertexType::Float, 4 },
		{ 4, StreamUsage::Normal,		1, VertexType::Float, 4 },
		{ 4, StreamUsage::Tangent,	3, VertexType::Float, 4 },
		VertexElement::VERTEX_ELEMENT_END()
	};

	for( u32 type = 0; type < shaderNum; ++type )
	{
		const ShaderHeader				*pShaderHeader = reinterpret_cast<const ShaderHeader*>( pSeek );
		pSeek += sizeof(ShaderHeader);

		u32												shaderType = pShaderHeader->shaderType;
		u32												size = pShaderHeader->size;

#if defined(_WIN64)
		switch( shaderType ){
		case ShaderKind::Vertex:
			m_pShader[shaderType] = GFGL::CreateVertexShaderFromMemory( pSeek, size, element );
			break;
		case ShaderKind::Fragment:
			m_pShader[shaderType] = GFGL::CreatePixelShaderFromMemory( pSeek, size );
			break;
		case ShaderKind::Geometry:
			m_pShader[shaderType] = GFGL::CreatePixelShaderFromMemory( pSeek, size );
			break;
		}
#else
		switch( shaderType ){
		case ShaderKind::Vertex:
			m_pShader[shaderType] = GFGL::CreateVertexShaderFromSource( this->GetInstanceAllocator(), (const char*)pSeek, "VsMain", element, NULL );
			break;
		case ShaderKind::Fragment:
			m_pShader[shaderType] = GFGL::CreatePixelShaderFromSource( this->GetInstanceAllocator(), (const char*)pSeek, "PsMain", NULL );
			break;
		case ShaderKind::Geometry:
			m_pShader[shaderType] = GFGL::CreateGeometryShaderFromSource( this->GetInstanceAllocator(), (const char*)pSeek, "GsMain", NULL );
			break;
		}
#endif

		pSeek += size;
	}
#endif
}


}}}}

