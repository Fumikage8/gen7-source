//==============================================================================
/**
 * @file	TextureResourceNodeGetter.h
 * @brief	TextureResourceNodeGetter
 * @author	munakata_kai
 * @date	2017/1/27
 */
// =============================================================================

#if !defined(__TEXTURE_RESOURCE_NODE_GETTER_H__)
#define __TEXTURE_RESOURCE_NODE_GETTER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>

GFL_NAMESPACE_BEGIN(Field)

//ResortIslandDraw::TextureResourceNodeGetter
//#include "App/Resort/source/ResortIslandDraw.h"

//ResourtIslandDrawからそのまま持ってきたもの（そのまま参照すると3DS版で落ちるためコピペ）
//! @brief  テクスチャリソースノードゲッター
//! @note   gfl2::renderingengine::scenegraph::Traverser::VisitorのImplementに渡すクラスとしての役割と
//!         テクスチャリソースノードを取得するクラスとしての役割を持つ。
class TextureResourceNodeGetter
{
  GFL_FORBID_COPY_AND_ASSIGN(TextureResourceNodeGetter);

public:
  //! @brief  gfl2::renderingengine::scenegraph::Traverser::VisitorのImplementに必要なメンバ関数
  void Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode);

public:
  //! @brief  dagNode内に存在するテクスチャリソースノードを取得する(最初の1個を見付けた時点で探すのを止める)
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode);

public:
  TextureResourceNodeGetter(void);
  virtual ~TextureResourceNodeGetter();

private:
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* getTextureResourceNode(void);

private:
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_textureResourceNode;
};

GFL_NAMESPACE_END( Field )

#endif // __TEXTURE_RESOURCE_NODE_GETTER_H__
