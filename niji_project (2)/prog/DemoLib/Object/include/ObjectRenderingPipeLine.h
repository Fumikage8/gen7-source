//======================================================================
/**
 * @file	ObjectRenderringPipeLine.h
 * @brief	ObjectをRenderPathに組み込むためのインターフェース
 * @author	ariizumi
 * @data	15/03/16
 */
//======================================================================
#if !defined( __OBJECT_RENDERING_PIPE_LINE_H__ )
#define __OBJECT_RENDERING_PIPE_LINE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <layout/include/gfl2_LayoutRenderPath.h>


GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
    GFL_NAMESPACE_BEGIN( scenegraph )
      GFL_NAMESPACE_BEGIN( resource )
        class ResourceNode;
      GFL_NAMESPACE_END( resource )
      GFL_NAMESPACE_BEGIN( instance )
        class ModelInstanceNode;
      GFL_NAMESPACE_END( instance )
    GFL_NAMESPACE_END( scenegraph )
  GFL_NAMESPACE_END( renderingengine )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
    class CharaModel;
    class DressUpModel;
  GFL_NAMESPACE_END( mdoel )
GFL_NAMESPACE_END( poke_3d )

GFL_NAMESPACE_BEGIN( PokeTool )
  class PokeModel;
GFL_NAMESPACE_END( PokeTool )

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)


class ObjectRenderPipeLine
{
public:
  //モデルを追加
  virtual void AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt ){}
  virtual void RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt ){}
  //ポケモデルを追加
  virtual void AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt ){}
  virtual void RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt ){}
  //キャラモデルを追加
  virtual void AddObject_Chara( poke_3d::model::CharaModel *charaModel , int opt ){}
  virtual void RemoveObject_Chara( poke_3d::model::CharaModel *charaModel , int opt ){}
  //着せ替えキャラモデルを追加
  virtual void AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt ){};
  virtual void RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt ){};

  // Layout:
  virtual gfl2::lyt::LayoutRenderPath* GetLayoutPath( void ) { return NULL; }

  //モデルのビルボードの更新にViewMatrixが必要。
  virtual const gfl2::math::Matrix34* GetViewMatrix(void){return NULL;}

  // Light:
  virtual void AddObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt ){};
  virtual void RemoveObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt ){};

  // Skybox:
  virtual void AddObject_Skybox( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode  ) {}
  virtual void RemoveObject_Skybox( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode  ) {}

  // Draw ENV:
  virtual gfl2::renderingengine::scenegraph::instance::DrawEnvNode *GetDrawEnvNode(int opt) { return(NULL); }


};


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_RENDERING_PIPE_LINE_H__