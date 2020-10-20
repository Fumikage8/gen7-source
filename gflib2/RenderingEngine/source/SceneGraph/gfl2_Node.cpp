#include <RenderingEngine/include/SceneGraph/gfl2_Node.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

//===============================================================
//! NodeNameを構築します。
//===============================================================
NodeName::NodeName()
{
	std::memset( m_NameBuff, 0, sizeof(m_NameBuff) );
}


//===============================================================
//! NodeNameを破棄します。
//===============================================================
NodeName::~NodeName()
{
	this->SetNodeName("");
}

//===============================================================
//! ノード名を取得します。
//
//! @return ノード名
//===============================================================
const char* NodeName::GetNodeName( void ) const
{
	return m_NameBuff;
}


//===============================================================
//! ノード名を設定します。
//
//! @param name	名前
//===============================================================
void NodeName::SetNodeName( const char *name )
{
	const size_t len = std::strlen(name);

	if( len >= NAME_LENGTH )
		GFL_ASSERT( 0 );

	std::memset(m_NameBuff,0,sizeof(m_NameBuff));
#ifdef WIN32	
	memcpy((char*)&m_NameBuff,name,strlen(name)); // Garbage Data Fix
#else
	std::strcpy( (char*)&m_NameBuff, name );
#endif
}


//===============================================================
//! Nodeを構築します。
//===============================================================
Node::Node()
{
  SceneGraphManager::IncrementNodeCount();

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
  SceneGraphManager::DebugAddNode(this);
#endif
}


//===============================================================
//! Nodeを破棄します。
//===============================================================
Node::~Node()
{
  SceneGraphManager::DecrementNodeCount();

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
  SceneGraphManager::DebugRemoveNode(this);
#endif
}


//===============================================================
//! ノードを作成します。
//
//! @return ノードオブジェクト
//===============================================================
/*Node* Node::CreateNode( void )
{
  return CreateNode( gfx::GLMemory::GetAllocator() );
}*/
Node* Node::CreateNode( gfx::IGLAllocator* pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator );
	Node* instance = new Node();
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}

//===============================================================
//! Transformノードか調べます。
//
//! @return Transformノードならtrue
//===============================================================
bool Node::IsTransformType( void ) const
{
	return false;
}


}}}

