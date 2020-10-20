#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* BlendShapeAnimationResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* BlendShapeAnimationResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
  (void)size;

	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 


	//!@note 適切な場所から参照
	struct BlendShapeAnimationDataHeader
	{
		BlendShapeAnimationDataHeader():frame(0),loop(0),blendShapeNum(0){}
		u32					frame;
    b32         loop;
		u32					blendShapeNum;
	};

	const u8*																pSeek = reinterpret_cast<const u8*>(data);
	const BlendShapeAnimationDataHeader			*pHeader = reinterpret_cast<const BlendShapeAnimationDataHeader*>( pSeek );

	pSeek += sizeof(BlendShapeAnimationDataHeader);

	BlendShapeAnimationResourceNode				*pFastNode = NULL;

	for( u32 no = 0; no < static_cast<u32>(pHeader->blendShapeNum); ++no )
	{
		BlendShapeAnimationResourceNode				*pNode = new BlendShapeAnimationResourceNode();
		u32																		sizeV = 0;

		sizeV += Constant::MESH_NAME_MAX;//TargetName
		sizeV += (Constant::MESH_NAME_MAX * Constant::BLEND_MESH_MAX) * pHeader->frame;//BlendMeshName
		sizeV += (sizeof(f32) * Constant::BLEND_MESH_MAX) * pHeader->frame;//BlendWeight

		pNode->SetData( pSeek, sizeV, pHeader->frame, pHeader->loop );

		if ( pFastNode )
		{
			pFastNode->AddChild( pNode );//まとめるだけ。親子関係解決は行わない。
		}
		else
		{
			pFastNode = pNode;
			SceneGraphManager::AddChild( pFastNode );
		}

		pSeek += sizeV;
	}	
  gfx::GLMemory::ClearInstanceAllocator();

	return pFastNode;
}

/*
void BlendShapeAnimationData::CreateBinary()
{
	struct BlendShapeAnimationDataHeader
	{
		BlendShapeAnimationDataHeader():frame(0),blendShapeNum(0){}
		u32					frame;
		u32					blendShapeNum;
	};

	BinaryMaker											binary;
	BlendShapeAnimationDataHeader		header;

	if ( m_AnimationDataList.size() )
	{
		header.frame = m_AnimationDataList[0].m_FrameNodeDataList.size();
		header.blendShapeNum = static_cast<s32>(m_AnimationDataList.size());
	}

	binary.Write( &header, sizeof(header) );

	for( u32 blendShapeNo = 0; blendShapeNo < m_AnimationDataList.size(); ++blendShapeNo )
	{
		binary.Write( m_AnimationDataList[blendShapeNo].m_Name, sizeof(m_AnimationDataList[blendShapeNo].m_Name) );

		for( u32 frame = 0; frame < m_AnimationDataList[blendShapeNo].m_FrameNodeDataList.size(); ++frame )
		{
			binary.Write( &m_AnimationDataList[blendShapeNo].m_FrameNodeDataList[frame], sizeof(m_AnimationDataList[blendShapeNo].m_FrameNodeDataList[frame]) );			
		}		
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}
*/

b32 BlendShapeAnimationResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("blendshp");
	return (type == supportType);
}

}}}}


