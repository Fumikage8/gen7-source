//==============================================================================
/**
* @file	TextureResourceNodeGetter.cpp
* @brief	TextureResourceNodeGetter
* @author	munakata_kai
* @date	2017/1/27
*/
// =============================================================================
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/TextureResourceNodeGetter.h"
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

GFL_NAMESPACE_BEGIN( Field )

void TextureResourceNodeGetter::Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  if(m_textureResourceNode == NULL)
  {
    if( dagNode->IsKindOf<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>() )
    {
      m_textureResourceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
    }
  }
}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* TextureResourceNodeGetter::GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>  visitor;
  gfl2::renderingengine::scenegraph::Traverser::Traverse<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>( dagNode, &visitor, 0xFFFFFFFF );
  return visitor.getTextureResourceNode();
}


TextureResourceNodeGetter::TextureResourceNodeGetter(void)
  : m_textureResourceNode(NULL)
  {
  }

TextureResourceNodeGetter::~TextureResourceNodeGetter()
{
}

gfl2::renderingengine::scenegraph::resource::TextureResourceNode* TextureResourceNodeGetter::getTextureResourceNode(void)
{
  return m_textureResourceNode;
}

GFL_NAMESPACE_END( Field )
