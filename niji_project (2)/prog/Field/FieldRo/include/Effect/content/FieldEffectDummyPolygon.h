/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectDummyPolygon.h
 *  @brief  フィールドエフェクト：キャプチャー用ダミーポリゴン
 *  @author saita_kazuki
 *  @date   2015.09.11
 */

#if !defined(__FIELD_EFFECT_DUMMY_POLYGON_H__)
#define __FIELD_EFFECT_DUMMY_POLYGON_H__
#pragma once

// poke_3d
#include <model/include/gfl2_BaseModel.h>
// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN( Camera )
class CameraManager;
GFL_NAMESPACE_END( Camera )

GFL_NAMESPACE_BEGIN( Effect )

class EffectDummyPolygon : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectDummyPolygon );

public:
  /**
   *  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::heap::HeapBase        *pHeap;
    FieldRootNode               *pRootNode;
    Field::MyRenderingPipeLine  *pPipeLine;
    Camera::CameraManager       *pCameraManager;
    gfl2::math::Vector3         vPos;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectDummyPolygon( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectDummyPolygon( void );

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
   *  @brief  ベースモデル取得
   */
  poke_3d::model::BaseModel* GetBaseModel( void ) { return &m_Model; }
  
protected:
  /**
   *  @brief  初期化
   */
  void setup( SetupData &setupData );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  モデルの作成
   *  @param  pModelBinary  モデルバイナリ
   */
  void createModel( void *pModelBinary );

protected:
  poke_3d::model::BaseModel                                       m_Model;

  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  Camera::CameraManager                                           *m_pCameraManager;

}; // class EffectDummyPolygon : public IEffectBase

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_EFFECT_DUMMY_POLYGON_H__)
