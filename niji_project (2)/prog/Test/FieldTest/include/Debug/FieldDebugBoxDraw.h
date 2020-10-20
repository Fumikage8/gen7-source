#if PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugBoxDraw.h
 *	@brief  AABBボックスの表示
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_DEBUG_BOX_DRAW_H__ )
#define __FIELD_DEBUG_BOX_DRAW_H__
#pragma once

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h"

#include "arc_index/FieldChara.gaix"

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
/**
 *  @class AABBボックスをデバッグ表示するクラス
 */
class DebugBoxDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBoxDraw); //コピーコンストラクタ＋代入禁止
public:
  static const int RES_INDEX = GARC_FieldChara_box_GFBMDLP;

  // コンストラクタ/デストラクタ
  DebugBoxDraw();
  ~DebugBoxDraw();

	// 初期化
  void Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode * pResource, Test::FieldTest::MyRenderingPipeLine* pPipeLine );

	// 破棄
	void Terminate( void );

  // 更新処理
  void Update( const gfl2::math::AABB& aabb );
  

  inline bool IsVisible() const { return m_IsVisible; }

private:

  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pResourceRootNode;  //! リソースルート
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelInstanceNode; //! モデルインスタンス

  MyRenderingPipeLine* m_pPipeLine;  //! レンダリングパイプライン
  bool m_IsVisible; // 描画フラグ


  void SetVisible( bool flag );
};

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )



#endif	// __FIELD_DEBUG_FRUSTUM_DRAW_H__


#endif // PM_DEBUG