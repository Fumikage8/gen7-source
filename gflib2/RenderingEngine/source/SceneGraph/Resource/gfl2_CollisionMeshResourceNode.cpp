#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CollisionMeshResourceNode::CollisionMeshResourceNode() : NodeTrait<CollisionMeshResourceNode, ResourceNode>(),
	m_pCollisionTriangleList(NULL),
  m_CollisionTriangleCount(0)
{
}

CollisionMeshResourceNode::~CollisionMeshResourceNode()
{
  if (m_pCollisionTriangleList != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pCollisionTriangleList);
    m_pCollisionTriangleList = NULL;
  }
}

void CollisionMeshResourceNode::SetData( const void* data, int size )
{
	GFL_ASSERT(data);

	const u8* pSeek = reinterpret_cast<const u8*>(data);

	struct Header
  {
		u32 triangleCount;
	};

	const Header* pHeader = reinterpret_cast<const Header*>(pSeek);
	pSeek += sizeof( Header );
	
  // トライアングル読み込み
  m_CollisionTriangleCount = pHeader->triangleCount;
  u32 allocSize = sizeof(CollisionTriangle) * m_CollisionTriangleCount;
  m_pCollisionTriangleList = reinterpret_cast<CollisionTriangle*>(this->GetInstanceAllocator()->SystemMemoryMalloc(allocSize));
  std::memcpy(m_pCollisionTriangleList, pSeek, allocSize);
}


}}}}

