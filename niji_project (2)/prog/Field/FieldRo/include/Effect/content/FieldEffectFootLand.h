//======================================================================
/**
 * @file FieldEffectFootLand.h
 * @date 2015/11/13 12:48:49
 * @author miyachi_soichi
 * @brief エフェクト：足跡足跡
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_FOOT_LAND_H_INCLUDED__
#define __FIELD_EFFECT_FOOT_LAND_H_INCLUDED__
#pragma once

// poke_3d
#include <model/include/gfl2_BaseModel.h>
// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

class EffectFootLand : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectFootLand );

public:
  struct SetupData
  {
    void                        *pResource;
    gfl2::heap::HeapBase        *pHeap;
    FieldRootNode               *pRootNode;
    Field::MyRenderingPipeLine  *pPipeLine;
    Field::IField3DObjectNode   *pParentObject;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectFootLand( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectFootLand( void );
  
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
   *  @brief  オフセットを設定
   */
  void SetOffset( b32 isReverse , gfl2::math::Vector3 offset );

  /**
   *  @brief  座標・姿勢を上書き設定
   */
  void SetPosQuaternion( b32 isReverse, gfl2::math::Vector3  pos,gfl2::math::Quaternion qua );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  モデルの作成
   *  @param  pModelBinary  モデルバイナリ
   */
  void createModel( void *pModelBinary );

  /**
   *  @brief  モーションの作成
   *  @param  pMotionBinary モーションバイナリ
   */
  void createMotion( void *pMotionBinary );

protected:
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  gfl2::math::SRT                                                 m_SRT;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_FOOT_LAND_H_INCLUDED__
