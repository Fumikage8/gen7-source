#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

CullingObjectNode::CullingObjectNode() : NodeTrait<CullingObjectNode, TransformNode>(),
	m_State( State::None ),
	m_pAABBTree( NULL ),
	m_pLinkNext( NULL )
{
	SceneGraphManager::HangOnTheTree( this );
	//m_pOcclusionQuery = gfx::GFGL::CreateOcclusionQuery( this->GetInstanceAllocator() );
}

CullingObjectNode::~CullingObjectNode()
{
	if ( m_pAABBTree )
	{
		m_pAABBTree->RemoveObject( this );
		m_pAABBTree = NULL;
	}
	//GFX_SAFE_DELETE( m_pOcclusionQuery );
}

void CullingObjectNode::SetMoved( const b32 moved )
{
	m_State ^= (( IsMoved() != moved ) ? State::Moved : 0 );

	if ( IsMoved() && m_pAABBTree )
	{
		if ( m_BeforePos != m_WorldMatrix.GetElemPosition() )
		{
			if( m_pAABBTree->GetAreaAABB().IsContain( this->GetWorldAABB().GetCenter() ) )
			{
				m_pAABBTree->SetDirty( true );
			}
			else
			{
				SceneGraphManager::HangOffTheTree( this );
				SceneGraphManager::HangOnTheTree( this );
			}
			m_BeforePos = m_WorldMatrix.GetElemPosition();
		}
	}
}

void CullingObjectNode::SetWorldTransformDirty( b32 f )
{
  TransformNode::SetWorldTransformDirty( f );
	SetMoved( f );
}


}}}}

