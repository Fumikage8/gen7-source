#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_RESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_RESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { 
	
class SceneGraphManager;

namespace resource {

class ResourceManager;

class ResourceNode : public NodeTrait<ResourceNode, DagNode>
{
  friend class ResourceNodeReferenceCountVisitor;

public:

  static ResourceNode* Create( gfx::IGLAllocator* allocator )
  {
    return GLNew(allocator) ResourceNode();
  }

	friend class ResourceManager;
	friend class SceneGraphManager;
	
	virtual ~ResourceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode") ) );
		return hash;
	}

	//! イベントリスナーインターフェイス
	class IEventListener{
	public:
		inline virtual ~IEventListener(){}

		//更新された時
		virtual void OnUpdate( ResourceNode *pThisNode ) = 0;

		//削除された時
		virtual void OnDelete( ResourceNode *pThisNode ) = 0;

	protected:
		IEventListener(){}
	};

	void SetListener( IEventListener *pEventListener ){ m_pEventListener = pEventListener; }

  void SetVersionId(u32 id) { m_VersionId = id; }
  u32 GetVersionId() const { return m_VersionId; }

	inline b32 IsGpuReferenced() const { return m_GpuReferenceCnt ? true : false; }
	inline void ReferenceCntInc(){ ++m_GpuReferenceCnt; }
	inline void ReferenceCntDec(){ --m_GpuReferenceCnt; }
	u32 GetReferenceCnt() const;

	virtual u32 GetNameHash() const { return 0; }

protected:
	ResourceNode();

	IEventListener					*m_pEventListener;

  u32 m_VersionId;      // 読み込んだリソースのバージョンID
	u32											m_GpuReferenceCnt;
};

}}}}

#endif

