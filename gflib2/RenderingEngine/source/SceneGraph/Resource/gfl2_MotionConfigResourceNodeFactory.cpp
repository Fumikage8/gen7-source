#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* MotionConfigResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* MotionConfigResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
  (void)size;

	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator(pAllocator);
	MotionConfigResourceNode* pNode = new MotionConfigResourceNode();

  const u8* u8Data = static_cast<const u8*>(data);
  pNode->SetData(u8Data);
	SceneGraphManager::AddChild(pNode);

  gfx::GLMemory::ClearInstanceAllocator();
	return pNode;
}

b32 MotionConfigResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("mcnf");
	return (type == supportType);
}

}}}}


