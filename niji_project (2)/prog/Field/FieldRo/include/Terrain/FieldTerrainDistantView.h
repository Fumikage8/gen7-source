/**
* @brief  フィールドの遠景1つ分クラス
* @file   TerrainDistantView.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINDISTANTVIEW_H_INCLUDED__ )
#define __FIELDTERRAINDISTANTVIEW_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

class TerrainFactory;


/**
* @brief フィールドの遠景1つ分クラス
* @note  カリングするために、遠景を任意のサイズで分割している
*/
class TerrainDistantView
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainDistantView );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    u32                           m_DistantViewNo;             // 遠景番号(エリアローカル番号)
    TerrainFactory*               m_pTerrainFactory;           // 地形生成機能
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainDistantView( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainDistantView( void );

  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  /**
  * @brief 更新処理
  *
  * @param localAnimationFrame;      Localアニメの現在フレーム
  * @param oneDayAnimationFrame;     1日アニメの現在フレーム
  *
  * @return 無し
  */
  void Update( f32 localAnimationFrame, f32 oneDayAnimationFrame, const gfl2::math::Matrix44* pCullingMatrixTable );

  /**
  *  @brief 現在モデルが表示されているか取得する
  */
  b32 IsVisible( void ) const;

  /**
  *  @brief 現在モデルのDrawTag数を取得
  */
  s32 GetDrawTagNum( void ) const;

private:

  /**
  * @brief モデルの表示・非表示切り替え
  *
  * @param visible  true 表示 false 非表示
  *
  * @return 無し
  */
  void UpdateVisibleImpl( b32 visible );

  //
  // 制御
  //
  u32                           m_DistantViewNo;      // 遠景番号(エリアローカル番号)
  u8                            m_InvisibleBit;       // 0なら表示、それ以外なら非表示

  //
  // オブジェクト
  //
  poke_3d::model::BaseModel*    m_pModel;
  
  //
  // リソースデータ
  //

  //
  // 外部オブジェクト
  //
  TerrainFactory*               m_pTerrainFactory;             // 地形生成機

};  // class TerrainDistantView


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINDISTANTVIEW_H_INCLUDED__