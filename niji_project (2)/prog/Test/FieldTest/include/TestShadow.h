//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		TestShadow.h
 *	@brief  影テスト描画システム
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __TEST_SHADOW_H__ )
#define __TEST_SHADOW_H__
#pragma once

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h"
#include <Animation/include/gfl2_AnimationController.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include "test/FieldTest/include/FieldTestUtility.h"
#include "test/FieldTest/include/movemodel_manager.h"

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

/*
 *  @brief 影テスト描画クラス
 */
class TestShadow
{
  GFL_FORBID_COPY_AND_ASSIGN(TestShadow); //コピーコンストラクタ＋代入禁止

public:
  // コンストラクタ・デストラクタ
  TestShadow();
  ~TestShadow();

  // 初期化
  void Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, Test::FieldTest::MyRenderingPipeLine* pPipeLine );

  // 破棄
	void Finalize( void );

  // 更新処理
  void Update( const fld::mmodel::MoveModelManager * cpMmdl );

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
  
  enum
  {
    RES_SHADOW_0 = 0,
    RES_SHADOW_1,
    RES_SHADOW_2,
    RES_SHADOW_2_MOT,
    RES_NUM,

    MDL_SHADOW_0 = 0,
    MDL_SHADOW_1,
    MDL_NUM,
    
    SHADOW_NUM = 10,

    HIGH_SHADOW_NUM = 10,
  };


  gfl2::heap::HeapBase * m_pHeap; // ! ヒープ

  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pResourceRootNode[RES_NUM];  //! リソースルート
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelInstanceNode[MDL_NUM]; //! モデルインスタンス
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode*   m_pJoint[MDL_NUM][SHADOW_NUM]; //! ジョイントテーブル
  Test::FieldTest::MyRenderingPipeLine* m_pPipeLine;  //! レンダリングパイプライン



  // ハイクオリティ影のワーク
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pHIGHShadowModel[HIGH_SHADOW_NUM]; //! モデルインスタンス
	gfl2::animation::AnimationController															m_AnimationControler[HIGH_SHADOW_NUM]; //! アニメーションコントローラー


  void * m_pResBuf[RES_NUM];  //! リソースバッファ
  size_t m_ResSize[RES_NUM];  //! リソースサイズ


};

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )



#endif	// __TEST_SHADOW_H__

