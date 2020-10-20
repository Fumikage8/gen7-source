/**
* @brief  フィールドの地形1ブロック
* @file   FieldTerrainBlock.h
* @author ikuta_junya
* @data   2015.02.07
*/

#if !defined( __FIELDTERRAINBLOCK__H_INCLUDED__ )
#define __FIELDTERRAINBLOCK__H_INCLUDED__ 
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
class FieldTerrainBlock
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldTerrainBlock );


public:

  enum Lod 
  {
    LOD_HIGHT = 0,  // ハイモデル
    LOD_MIDDLE,     // ミドルモデル
    LOD_LOW,        // ローモデル

    LOD_MAX
  };
  
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  FieldTerrainBlock( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldTerrainBlock( void );

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
  * @param  lod     LOD段階
  * @param  pModel  モデル
  *
  * @return 無し
  */
  void SetModel( Lod lod, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModel );

  /**
  * @brief LOD状態を設定
  *
  * @param  lod     LOD段階
  *
  * @return 無し
  */
  void ChangeLod( Lod lod );


  void UpdateCulling( const gfl2::math::Matrix44* pCullingMatrix, const gfl2::math::AABB& rAABB );


private:


  void UpdateVisibleImpl( Lod lod, b32 visible ); 


  // インスタンス
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*  m_pModelInstanceNodeTable[ LOD_MAX ];

  // 制御
  Lod               m_LodState;
  Lod               m_LodStateDraw;   // 本当に描画しているもの
  u8                m_InvisibleBit;

};  // class FieldTerrainBlock


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBLOCK__H_INCLUDED__