#if !defined(POKE_3D_LIB_RENDERER_GFL2_BLOOMRENDERPATHUTIL_H_INCLUDED)
#define POKE_3D_LIB_RENDERER_GFL2_BLOOMRENDERPATHUTIL_H_INCLUDED
#pragma once


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


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(renderingengine)
    GFL_NAMESPACE_BEGIN(scenegraph)
      class DagNode;
      GFL_NAMESPACE_BEGIN(resource)
        class TextureResourceNode;
      GFL_NAMESPACE_END(resource)
    GFL_NAMESPACE_END(scenegraph)
  GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)


//==============================================================================
/**
 @class     BloomRenderPathutil
 @brief     ブルームの描画パス用につくった便利クラス
 */
//==============================================================================
class BloomRenderPathUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomRenderPathUtil);




public:
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
    static gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode);
  
  public:
    TextureResourceNodeGetter(void);
    virtual ~TextureResourceNodeGetter();

  private:
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* getTextureResourceNode(void);

  private:
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_textureResourceNode;
  };




};


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)


#endif  // POKE_3D_LIB_RENDERER_GFL2_BLOOMRENDERPATHUTIL_H_INCLUDED

