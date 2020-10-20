//==============================================================================
/**

 @file    MobiclipRenderPathUtil.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.18
 @brief   mobiclip用の描画パス用につくった便利クラス
 @note    gf内製コード。mobiclipとは無関係。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>


// nijiのインクルード
#include "MobiclipRenderPathUtil.h"


GFL_NAMESPACE_BEGIN(movie)
GFL_NAMESPACE_BEGIN(renderer)


void MobiclipRenderPathUtil::TextureResourceNodeGetter::Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  if(m_textureResourceNode == NULL)
  {
    if( dagNode->IsKindOf<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>() )
    {
      m_textureResourceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
    }
  }
}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* MobiclipRenderPathUtil::TextureResourceNodeGetter::GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>  visitor;
  gfl2::renderingengine::scenegraph::Traverser::Traverse<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>( dagNode, &visitor, 0xFFFFFFFF );
  return visitor.getTextureResourceNode();
}


MobiclipRenderPathUtil::TextureResourceNodeGetter::TextureResourceNodeGetter(void)
  : m_textureResourceNode(NULL)
{}

MobiclipRenderPathUtil::TextureResourceNodeGetter::~TextureResourceNodeGetter()
{}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* MobiclipRenderPathUtil::TextureResourceNodeGetter::getTextureResourceNode(void)
{
  return m_textureResourceNode;
}
 

GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(movie)

