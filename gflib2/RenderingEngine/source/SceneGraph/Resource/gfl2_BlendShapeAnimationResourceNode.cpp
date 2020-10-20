#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNode.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

BlendShapeAnimationResourceNode::BlendShapeAnimationResourceNode() : NodeTrait<BlendShapeAnimationResourceNode, ResourceNode>(),
	m_FrameSize( 0 ),
  m_IsLoop( false ),
  m_NameHash( 0 ),
	m_pFrameNodeDataList( NULL )
{
	std::memset( m_Name, 0, sizeof(m_Name) );	
}

BlendShapeAnimationResourceNode::~BlendShapeAnimationResourceNode()
{
	if ( m_pFrameNodeDataList )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_pFrameNodeDataList );
		m_pFrameNodeDataList = NULL;
	}
}

void BlendShapeAnimationResourceNode::SetData( const void* data, u32 size, u32 frameSize, b32 isLoop )
{
  (void)size;

	GFL_ASSERT(data);
	const u8*					pSeek = reinterpret_cast<const u8*>(data);

	std::memcpy( m_Name, pSeek, sizeof(m_Name) );
	this->SetNodeName( m_Name );
	pSeek += Constant::MESH_NAME_MAX;

	std::memcpy( &m_NameHash, pSeek, sizeof(m_NameHash) );
	pSeek += sizeof(u32);

  m_FrameSize = frameSize;
  m_IsLoop = isLoop;

	if ( m_pFrameNodeDataList )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_pFrameNodeDataList );
		m_pFrameNodeDataList = NULL;
	}

  u32 frameNodeDataSize =  sizeof(FrameNodeData) * (m_FrameSize);
	m_pFrameNodeDataList = reinterpret_cast<FrameNodeData*>( this->GetInstanceAllocator()->SystemMemoryMalloc(frameNodeDataSize) );
	std::memcpy( m_pFrameNodeDataList, pSeek, frameNodeDataSize );
}

void BlendShapeAnimationResourceNode::GetBlendShape(f32 frame, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMesh, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode) const 
{
  u32 u32Frame = static_cast<u32>(frame);
  if (u32Frame >= this->GetFrameSize()) u32Frame = this->GetFrameSize() - 1;

  u32 u32FrameNext = u32Frame + 1;
  if (u32FrameNext >= this->GetFrameSize()) u32FrameNext = this->GetFrameSize() - 1;

  f32 t = u32FrameNext - frame;
  t = 1.0f - t;

  for (u32 i = 0; i < Constant::BLEND_MESH_MAX; ++i)
  {
    scenegraph::instance::ModelInstanceNode::MeshLinkData* pBlendMeshLinkData = NULL;

    const scenegraph::resource::BlendShapeAnimationResourceNode::FrameNodeData* pFrameNodeData = this->GetFrameNodeData(u32Frame);
    const scenegraph::resource::BlendShapeAnimationResourceNode::FrameNodeData* pFrameNodeDataNext = this->GetFrameNodeData(u32FrameNext);

    const c8* meshName = pFrameNodeData->m_BlendMeshName[i];

    // 対象のブレンドシェイプを探す
    for (u32 j = 0; j < pModelNode->GetMehsNum(); ++j)
    {
      scenegraph::instance::ModelInstanceNode::MeshLinkData* pTmpMeshLinkData = pModelNode->GetMeshLinkData(j);

      if (std::strcmp(meshName, pTmpMeshLinkData->GetName()) == 0)
      {
        pBlendMeshLinkData = pTmpMeshLinkData;
        break;
      }
    }

    f32 blendWeight0 = pFrameNodeData->m_BlendWeight[i];
    f32 blendWeight1 = pFrameNodeDataNext->m_BlendWeight[i];

    bool interpFlag = (pFrameNodeData->m_BlendMeshNameHash[i] == pFrameNodeDataNext->m_BlendMeshNameHash[i]) || 
      (std::strcmp(pFrameNodeDataNext->m_BlendMeshName[i], "") == 0);
    f32 blendWeight = (interpFlag)
      ? blendWeight0 + (blendWeight1 - blendWeight0) * t
      : blendWeight0;

    pMesh->SetBlendMeshData(i, pBlendMeshLinkData, blendWeight);
  }

  // ブレンドシェイプの再設定
  pModelNode->SetUpDrawTag();
}

}}}}

