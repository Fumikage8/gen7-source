//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		TestEncountGrass.h
 *	@brief		エンカウント草テスト　クラス
 *	@author		tomoya takahashi 
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __TEST_ENCOUNT_GRASS_H__ )
#define __TEST_ENCOUNT_GRASS_H__
#pragma once

namespace gfl2
{
  namespace renderingengine
  {
    namespace scenegraph
    {
      namespace resource
      {
        class ResourceNode;
      }

      namespace instance
      {
        class ModelInstanceNode;
      }
    }
  }

  namespace math
  {
      class Vector3;
  }
}

#include "test/FieldTest/include/FieldTestUtility.h"
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <Animation/include/gfl2_AnimationController.h>
#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h"
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class Effect;
class Grid;

class TestEncountGrass
{
  GFL_FORBID_COPY_AND_ASSIGN(TestEncountGrass); //コピーコンストラクタ＋代入禁止

public:
	// コンストラクタ・デストラクタ
	TestEncountGrass();
	~TestEncountGrass();

	// 初期化
  void Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, Test::FieldTest::MyRenderingPipeLine* pPipeLine );

	// 破棄
	void Finalize( void );

  // 更新処理
  void Update( const gfl2::math::Vector3& pos );

private:
  inline static void ControlVisible( bool flag, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNode, Test::FieldTest::MyRenderingPipeLine* pPipeLine )
  {
    if( flag ){
      if(pNode->IsVisible() == false){
        pNode->SetVisible(true); 
        gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( pNode );
        pPipeLine->AddEdgeRenderingTarget( pNode );
      }
    }else{
      if(pNode->IsVisible() == true){
        pNode->SetVisible(false); 
        gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( pNode );
        pPipeLine->RemoveEdgeRenderingTarget( pNode );
      }
    }
  }

private:
  
  enum{
    RES_GRASS0,   // 1段階目テスト0
    RES_ENCGRASS01, // 計測：高負荷上物草
    RES_ENCGRASS02, // 計測：普通負荷上物草
    RES_GRASS1,
    RES_MAP,
    RES_ENCMAP01, // 計測：高負荷　下草
    RES_ENCMAP02, // 計測：普通負荷　下草
    RES_MOT,
    RES_NUM,

    MDL_HIGH_UPPER = 0,
    MDL_LOW_UPPER,
    MDL_HIGH_LOWER,
    MDL_LOW_LOWER,
    MDL_NUM,


    GRID_NUM = 2,

    EFF_GRASS_MAX = 8, // エフェクトインスタンス数
  };

  gfl2::heap::HeapBase * m_pHeap; // ! ヒープ

  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pResourceRootNode[RES_NUM];  //! リソースルート
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelInstanceNode[MDL_NUM]; //! モデルインスタンス
	gfl2::animation::AnimationController																				m_AnimationControler; //! アニメーションコントローラー
  Test::FieldTest::MyRenderingPipeLine* m_pPipeLine;  //! レンダリングパイプライン

  void * m_pResBuf[RES_NUM];  //! リソースバッファ
  size_t m_ResSize[RES_NUM];  //! リソースサイズ

  // 前方宣言
  Effect * m_pEffectArray;
  u32      m_EffectArrayNum;


  // グリッド判定
  Grid * m_pGrid[GRID_NUM];



};

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif	// __TEST_ENCOUNT_GRASS_H__

