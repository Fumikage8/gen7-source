//==============================================================================
/**
 @file    gfl2_BloomRenderPathUtil.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.05.27
 @brief   ブルームの描画パス用につくった便利クラス
 @note    ブルーム(bloom)と言ったり、グレア(glare)と言ったりします。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>


// poke_3d_libのインクルード
#include "gfl2_BloomRenderPathUtil.h"


GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)


void BloomRenderPathUtil::TextureResourceNodeGetter::Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  if(m_textureResourceNode == NULL)
  {
    if( dagNode->IsKindOf<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>() )
    {
      m_textureResourceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
    }
  }
}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* BloomRenderPathUtil::TextureResourceNodeGetter::GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>  visitor;
  gfl2::renderingengine::scenegraph::Traverser::Traverse<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>( dagNode, &visitor, 0xFFFFFFFF );
  return visitor.getTextureResourceNode();
}


BloomRenderPathUtil::TextureResourceNodeGetter::TextureResourceNodeGetter(void)
  : m_textureResourceNode(NULL)
{}

BloomRenderPathUtil::TextureResourceNodeGetter::~TextureResourceNodeGetter()
{}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* BloomRenderPathUtil::TextureResourceNodeGetter::getTextureResourceNode(void)
{
  return m_textureResourceNode;
}
 

GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)

