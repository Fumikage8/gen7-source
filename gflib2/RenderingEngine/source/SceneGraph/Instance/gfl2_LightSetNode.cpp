#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightSetNode.h>
#include <debug/include/gfl2_DebugPrint.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  LightNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
LightSetNode::LightSetNode() : NodeTrait<LightSetNode, TransformNode>(),
  m_SetNo(0)
{
}

LightSetNode::~LightSetNode()
{
}

void LightSetNode::AddLight( LightNode *pNode )
{
	if ( pNode->GetParent() )
	{
#ifdef WIN32
		GFL_PRINT( "すでに親がいます。複数LightSetに繋げることはできません。" );
#else
		GFL_PRINT( "すでに親がいます。複数LightSetに繋げることはできません。" );
#endif
		pNode->RemoveFromParent();
	}

	if ( m_LightContainer.free_size() )
	{
		m_LightContainer.push_back( pNode );
		this->AddChild( pNode );
	}
}

void LightSetNode::RemoveLight( LightNode *pNode )
{
	m_LightContainer.remove( pNode );
	this->RemoveChild( pNode );
}

void LightSetNode::RemoveAllLight()
{
	m_LightContainer.clear();
}

LightNode* LightSetNode::GetLightNode( s32 no )
{
	if ( no < static_cast<s32>( m_LightContainer.size() ) )
	{
		return m_LightContainer[no];
	}
	return NULL;
}

const LightNode* LightSetNode::GetLightNode( s32 no ) const 
{
	if ( no < static_cast<s32>( m_LightContainer.size() ) )
	{
		return m_LightContainer[no];
	}
	return NULL;
}

}}}}

