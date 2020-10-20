/**
* @brief  フィールドの遠景1ブロック
* @file   FieldTerrainDistantView.h
* @author ikuta_junya
* @data   2015.02.07
*/

#if !defined( __FIELDTERRAINDISTANTVIEW__H_INCLUDED__ )
#define __FIELDTERRAINDISTANTVIEW__H_INCLUDED__ 
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

/**
* @brief フィールドの地形管理
*/
class FieldTerrainDistantView
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldTerrainDistantView );


public:

  
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  FieldTerrainDistantView( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldTerrainDistantView( void );

  /**
  * @brief 初期化処理
  *
  * @return 無し
  */
  void Initialize( void );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Finalize( void );

  /**
  * @brief モデルを設定
  *
  * @param  pModel  モデル
  *
  * @return 無し
  */
  void SetModel( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModel );



  void UpdateCulling( const gfl2::math::Matrix44* pCullingMatrix, const gfl2::math::AABB& rAABB );


private:


  void UpdateVisibleImpl( b32 visible ); 


  // インスタンス
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*  m_pModelInstanceNode;

  // 制御
  u8                m_InvisibleBit;

};  // class FieldTerrainDistantView


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINDISTANTVIEW__H_INCLUDED__
