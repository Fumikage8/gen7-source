/**
* @brief  フィールドの地形ブロッククラス
* @file   TerrainBlock.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINBLOCK_H_INCLUDED__ )
#define __FIELDTERRAINBLOCK_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Collision/include/gfl2_CollisionModel.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>

// field
///#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodType.h"

class BaseCollisionScene;   /// コリジョン管理シーン
class StaticActor;

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
/**
GFL_NAMESPACE_BEGIN(KusaModel);
class FieldKusaModel;
GFL_NAMESPACE_END(KusaModel);
*/ //@saito_del
GFL_NAMESPACE_BEGIN( Terrain );

class TerrainFactory;
/**
GFL_NAMESPACE_BEGIN( Block );
class LodChangeStaticListener;   // 地形ブロックのLOD変更通知を受け取るオブジェクトの基底クラス
GFL_NAMESPACE_END( Block );
*/ //@saito_del

/**
* @brief フィールドの地形管理
*/
class TerrainBlock
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainBlock );

public:

  struct LodLevel
  {
    enum Value
    {
      HIGH = 0,
      MIDDLE,
      MAX
    };
  };

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::math::Vector3           m_Pos;
    u32                           m_X;
    u32                           m_Z;
    f32                           m_Size;
    u32                           m_LodChangeStaticListenerMax;

    TerrainFactory*               pTerrainFactory;             // 地形生成機能
    BaseCollisionScene*           pCollisionScene;             // 高さコリジョン管理シーン
    BaseCollisionScene*           m_pWallCollisionScene;         // 壁コリジョン管理シーン
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainBlock( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainBlock( void );

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
  * @return 無し
  */
  void Update( void );

  /**
  * @brief LOD変更処理
  *
  * @param  lodLevel 変更したい状態
  *
  * @return 無し
  */
  ///void ChangeLod( Block::LodType::Value lodLevel );

  /**
  * @brief LOD変更通知を受け取るリスナーを登録
  * @note  リスナーの寿命は登録側で管理してください
  *
  * @param  pListener  登録するリスナー
  *
  * @return 成否
  */
  ///b32 RegistListener( Block::LodChangeStaticListener* pListener );

  /**
  * @brief LOD変更通知を受け取るリスナーを登録削除
  * @note  リスナーの寿命は登録側で管理してください
  *
  * @param  pListener  登録削除するリスナー
  *
  * @return 成否
  */
  ///b32 UnRegistListener( Block::LodChangeStaticListener* pListener );

private:

  /**
  * @brief LOD変更をリスナーに通知
  *
  * @param  lodType 変更後の状態
  *
  * @return 無し
  */
  ///void ChangeLodNotificationListener( Block::LodType::Value lodType );

  /**
  * @brief 地形モデルの表示・非表示切り替え
  *
  * @param lodType  対象となるLODモデル
  * @param visible  true 表示 false 非表示
  *
  * @return 無し
  */
  ///void UpdateVisibleImpl( Block::LodType::Value lodType, b32 visible );

  //
  // 制御
  //
  u32                           m_X;
  u32                           m_Z;
  f32                           m_Size;               // ブロックの大きさ(正方形)
  gfl2::math::Vector3           m_Pos;                // ブロックの中心座標
  b32                           m_IsLoading;
  ///b32                           m_IsHighModel;

  ///Block::LodType::Value         m_RequestLodType;    // 出したいモデル
  ///Block::LodType::Value         m_LodType;           // 実際に表示しているモデル


  u8                            m_InvisibleBit;      // 0なら表示、それ以外なら非表示

  //
  // オブジェクト
  //
  // gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelNodeTable[ Block::LodType::MAX ];
  poke_3d::model::BaseModel*                                      m_pModelTable;//[ Block::LodType::MAX ];@saito_change
  ///KusaModel::FieldKusaModel**                                     m_ppEncountGrassModelTable[ Block::LodType::MAX ];
  gfl2::collision::CollisionModel*                                m_pCollisionModel;
  StaticActor*                                                    m_pStaticActor;
  gfl2::collision::CollisionModel*                                m_pWallCollisionModel;
  StaticActor*                                                    m_pWallStaticActor;
  ///Block::LodChangeStaticListener**                                m_ppLodChangeStaticListenerTable;
  u32                                                             m_LodChangeStaticListenerMax;

  //
  // リソースデータ
  //

  //
  // 外部オブジェクト
  //
  TerrainFactory*               m_pTerrainFactory;             // 地形生成機
  BaseCollisionScene*           m_pCollisionScene;             // コリジョン管理シーン
  BaseCollisionScene*           m_pWallCollisionScene;         // 壁コリジョン管理シーン

};  // class TerrainBlock


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBLOCK_H_INCLUDED__