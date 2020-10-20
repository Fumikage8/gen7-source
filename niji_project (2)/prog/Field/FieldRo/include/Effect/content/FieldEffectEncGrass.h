/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectEncGrass.h
 *  @brief  フィールドエフェクト：エンカウント草
 *  @author tomoya takahashi
 *  @date   2015.06.11
 */

#if !defined(__FIELD_EFFECT_ENCGRASS_H__)
#define __FIELD_EFFECT_ENCGRASS_H__
#pragma once

// poke_3d
#include <model/include/gfl2_BaseModel.h>
// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldStatic/include/FieldWorldDefine.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class FieldRootNode;

GFL_NAMESPACE_BEGIN(MoveModel);
class FieldMoveModel;
GFL_NAMESPACE_END(MoveModel);

GFL_NAMESPACE_BEGIN(Terrain);
class TerrainManager;
GFL_NAMESPACE_END(Terrain);

GFL_NAMESPACE_END( Field )

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectEncGrass : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectEncGrass );

public:

  enum OnLengthType
  {
    ON_LENGTH_TYPE_NORMAL,  // 通常草タイプ
    ON_LENGTH_TYPE_SHORT,   // 短い草タイプ
    ON_LENGTH_TYPE_YELLOW,  // 黄色い草タイプ
    ON_LENGTH_TYPE_MAX,     // タイプ最大数
  };

  /**
   *  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::heap::HeapBase        *pHeap;
    Field::MyRenderingPipeLine  *pPipeLine;
    FieldRootNode               *pRootNode;
    gfl2::math::Vector3         vPos;
    OnLengthType                onLengthType;
    const void                  *cpColorTable;
    WorldID                     worldID;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectEncGrass( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectEncGrass( void );

  /**
   *  @brief  初期化処理：座標指定
   *  @param  setupData 初期化構造体
   */
  virtual void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );
  
  /**
   *  @brief  エンカウント草のポジションを設定
   */
  void SetEncountGrassPos( s32 BlockX, s32 BlockZ, s32 GridX, s32 GridZ, s32 FloorIndex );

  /**
   *  @brief  エンカウント草のポジションが一致しているかチェック
   */
  bool IsEqualEncountGrassPos( const s32 BlockX, const s32 BlockZ, const s32 GridX, const s32 GridZ, const s32 FloorIndex ) const;

  /**
   *  @brief パラメーターを設定 
   */
  void SetParameter( Terrain::TerrainManager* pTerrain, MoveModel::FieldMoveModel* pPlayer );


  /**
   *  @brief  終了のチェック
   */
  bool IsEnd() const;

protected:
  /**
   *  @brief  初期化
   */
  void setupBallon( u32 index, SetupData &setupData );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  モデルの作成
   *  @param  pModelBinary  モデルバイナリ
   */
  void createModel( u32 index, void *pModelBinary );

  /**
   *  @brief  モーションの作成
   *  @param  pMotionBinary モーションバイナリ
   */
  void createMotion( u32 index, void *pMotionBinary );

  /**
   *  @brief  ONアニメーション開始
   */
  void startOnAnimation( void );

  /**
   *  @brief  OFFアニメーション開始
   */
  void startOffAnimation( void );

  /**
   *  @brief アニメーションフレームを進める
   */
  void updateAnimationFrame( void );

  /**
   *  @brief アニメーションフレームが最終フレームかチェック
   */
  bool isAnimationEndFrame( void ) const;

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

protected:

  enum
  {
    COLOR_IDX_G_R,
    COLOR_IDX_G_G,
    COLOR_IDX_G_B,

    COLOR_IDX_LG_R,
    COLOR_IDX_LG_G,
    COLOR_IDX_LG_B,

    COLOR_IDX_PAD0,
    COLOR_IDX_PAD1,

    COLOR_IDX_NUM,
  };
  struct ColorTable
  {
    u16 worldID;
    s16 padding0;
    u8  color[COLOR_IDX_NUM];
  };

  struct ColorTableArray
  {
    u32 num;
    ColorTable data[];
  };


  enum
  {
    IDX_MODEL,    // モデル
    IDX_EFFECT,   // エフェクト
    IDX_MAX       
  };

  enum
  {
    ST_NULL,
    ST_ON,
    ST_KEEP,
    ST_OFF,
    ST_END_OFF,
    ST_END,
    ST_MAX
  };

  poke_3d::model::BaseModel                                       m_Model[IDX_MAX];

  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode[IDX_MAX];
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode[IDX_MAX];
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode[IDX_MAX];
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード

  Terrain::TerrainManager*                                         m_pTerrainManager;     //!< 地形マネージャー
  MoveModel::FieldMoveModel*                                       m_pPlayer;     //!< プレイヤー
    
  Field::MyRenderingPipeLine  *                                     m_pPipeLine;  //!< パイプライン


  u32 m_Status;         ///< 状態
  f32 m_OldLength;

  bool m_isEffectEnd;   ///< エフェクト表示終了
  bool m_isEnd;         ///< 草の表示終了

  bool m_isAnimeNone; ///< アニメーションデータがない場合

  s32 m_BlockX;              ///< ブロック座標X
  s32 m_BlockZ;              ///< ブロック座標Z
  s32 m_GridX;               ///< ブロック内グリッド座標X
  s32 m_GridZ;               ///< ブロック内グリッド座標Z
  s32 m_FloorIndex;          ///< フロアインデックス

  f32 m_Frame;
  f32 m_StepFrame;
  f32 m_StartFrame;
  f32 m_EndFrame;

  OnLengthType                m_OnLengthType;
  const ColorTable* m_cpColorTable;
}; // class EffectBallonIcon : public IEffectBase

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_EFFECT_ENCGRASS_H__)
